


#include "match.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define SCORE_MIN (-1000000)
#define MAX_STACK_QUERY 512

typedef struct {
	ConfigEntry *entry;
	int score;
	int index;
} MatchResult;

static inline bool is_separator(char c) {
	return (c == ' ' || c == '_' || c == '-' || c == '/' || c == '.' ||
	        c == ':' || c == '\\' || c == '@');
}

static inline int get_char_bonus(const char *name, size_t i) {
	if (i == 0) return 100;
	char prev = name[i - 1];
	char curr = name[i];

	if (is_separator(prev)) {
		return 80;
	}
	if (islower((unsigned char)prev) && isupper((unsigned char)curr)) {
		return 60;
	}
	if (!isalnum((unsigned char)prev) && isalnum((unsigned char)curr)) {
		return 70;
	}
	return 0;
}

static inline bool is_word_boundary(const char *name, size_t i) {
	if (i == 0) return true;
	char prev = name[i - 1];
	char curr = name[i];
	if (is_separator(prev)) return true;
	if (islower((unsigned char)prev) && isupper((unsigned char)curr))
		return true;
	if (!isalnum((unsigned char)prev) && isalnum((unsigned char)curr))
		return true;
	return false;
}

static int strcasecmp_custom(const char *s1, const char *s2) {
	while (*s1 && *s2) {
		int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
		if (diff != 0) return diff;
		s1++;
		s2++;
	}
	return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

static const char *strcasestr_custom(const char *haystack, const char *needle) {
	if (!*needle) return haystack;
	size_t needle_len = strlen(needle);
	for (; *haystack; haystack++) {
		if (tolower((unsigned char)*haystack) ==
		    tolower((unsigned char)*needle)) {
			size_t k;
			for (k = 0; k < needle_len; k++) {
				if (!haystack[k] || tolower((unsigned char)haystack[k]) !=
				                        tolower((unsigned char)needle[k]))
					break;
			}
			if (k == needle_len) return haystack;
		}
	}
	return NULL;
}

static int score_item(const char *name, const char *query) {
	if (!name || !query) return -1;
	if (*query == '\0') return 0;

	size_t name_len = strlen(name);
	size_t query_len = strlen(query);

	if (query_len > name_len) return -1;

	// Quick subsequence check
	const char *h = name;
	const char *q = query;
	while (*h && *q) {
		if (tolower((unsigned char)*h) == tolower((unsigned char)*q) ||
		    (*q == ' ' && is_separator(*h))) {
			q++;
		}
		h++;
	}
	if (*q != '\0') {
		return -1; // Not a subsequence
	}

	// Exact full match bonus
	if (name_len == query_len && strcasecmp_custom(name, query) == 0) {
		int exact_case = (strcmp(name, query) == 0) ? 500 : 0;
		return 20000 + exact_case;
	}

	int stack_d_prev[MAX_STACK_QUERY];
	int stack_d_curr[MAX_STACK_QUERY];
	int stack_s_prev[MAX_STACK_QUERY];
	int stack_s_curr[MAX_STACK_QUERY];

	int *D_prev = stack_d_prev;
	int *D_curr = stack_d_curr;
	int *S_prev = stack_s_prev;
	int *S_curr = stack_s_curr;

	bool allocated = false;
	if (query_len > MAX_STACK_QUERY) {
		D_prev = malloc(sizeof(int) * query_len);
		D_curr = malloc(sizeof(int) * query_len);
		S_prev = malloc(sizeof(int) * query_len);
		S_curr = malloc(sizeof(int) * query_len);
		if (!D_prev || !D_curr || !S_prev || !S_curr) {
			free(D_prev);
			free(D_curr);
			free(S_prev);
			free(S_curr);
			return 100;
		}
		allocated = true;
	}

	for (size_t j = 0; j < query_len; j++) {
		D_prev[j] = SCORE_MIN;
		S_prev[j] = SCORE_MIN;
	}

	for (size_t i = 0; i < name_len; i++) {
		unsigned char nc = (unsigned char)name[i];
		int bonus = get_char_bonus(name, i);

		for (size_t j = 0; j < query_len; j++) {
			unsigned char qc = (unsigned char)query[j];

			bool match =
			    (tolower(nc) == tolower(qc)) || (qc == ' ' && is_separator(nc));

			if (match) {
				int case_bonus = (nc == qc) ? 10 : 0;

				if (j == 0) {
					// Leading gap penalty: -4 per skipped character at start
					int leading_gap = (int)i * 4;
					D_curr[0] = 100 + bonus + case_bonus - leading_gap;
				} else if (i > 0) {
					int score_consecutive = SCORE_MIN;
					if (D_prev[j - 1] != SCORE_MIN) {
						score_consecutive =
						    D_prev[j - 1] + 100 + 50 + case_bonus;
					}

					int score_gap = SCORE_MIN;
					if (S_prev[j - 1] != SCORE_MIN) {
						// Inner gap start penalty: -5
						score_gap =
						    S_prev[j - 1] + 100 + bonus + case_bonus - 5;
					}

					D_curr[j] = (score_consecutive > score_gap)
					                ? score_consecutive
					                : score_gap;
				} else {
					D_curr[j] = SCORE_MIN;
				}
			} else {
				D_curr[j] = SCORE_MIN;
			}

			if (i == 0) {
				S_curr[j] = D_curr[j];
			} else {
				int prev = S_prev[j];
				int prev_penalized = (prev != SCORE_MIN) ? prev - 3 : SCORE_MIN;
				S_curr[j] =
				    (D_curr[j] > prev_penalized) ? D_curr[j] : prev_penalized;
			}
		}

		memcpy(D_prev, D_curr, query_len * sizeof(int));
		memcpy(S_prev, S_curr, query_len * sizeof(int));
	}

	int best_score = S_prev[query_len - 1];

	if (allocated) {
		free(D_prev);
		free(D_curr);
		free(S_prev);
		free(S_curr);
	}

	if (best_score <= SCORE_MIN / 2) {
		return -1;
	}

	// High-level bonuses
	const char *sub = strcasestr_custom(name, query);
	if (sub != NULL) {
		if (sub == name) {
			// Exact prefix substring match
			best_score += 5000;
		} else if (is_word_boundary(name, (size_t)(sub - name))) {
			// Contiguous substring at word boundary (e.g. "term" in
			// "suckless-terminal")
			best_score += 3000;
		} else {
			// Internal contiguous substring
			best_score += 600;
		}
	}

	// Starts with first query character bonus
	if (tolower((unsigned char)name[0]) == tolower((unsigned char)query[0])) {
		best_score += 1500;
	}

	// Length difference penalty
	best_score -= (int)(name_len - query_len) * 2;

	if (best_score < 0) best_score = 0;
	return best_score;
}

static int compare_results(const void *a, const void *b) {
	const MatchResult *ra = (const MatchResult *)a;
	const MatchResult *rb = (const MatchResult *)b;

	if (rb->score != ra->score) {
		return rb->score - ra->score; // Higher score first
	}
	return ra->index - rb->index; // Preserve original order on tie
}

void match_items(ConfigEntry *items, int total_items, const char *query,
                 ConfigEntry **results, int *result_count) {
	if (!items || !query || !results || !result_count) return;

	*result_count = 0;
	if (total_items <= 0) return;

	if (*query == '\0') {
		for (int i = 0; i < total_items; i++) {
			results[i] = &items[i];
		}
		*result_count = total_items;
		return;
	}

	MatchResult *matches = malloc(sizeof(MatchResult) * total_items);
	if (!matches) {
		for (int i = 0; i < total_items; i++) {
			if (score_item(items[i].name, query) >= 0) {
				results[*result_count] = &items[i];
				(*result_count)++;
			}
		}
		return;
	}

	int count = 0;
	for (int i = 0; i < total_items; i++) {
		int score = score_item(items[i].name, query);
		if (score >= 0) {
			matches[count].entry = &items[i];
			matches[count].score = score;
			matches[count].index = i;
			count++;
		}
	}

	qsort(matches, count, sizeof(MatchResult), compare_results);

	for (int i = 0; i < count; i++) {
		results[i] = matches[i].entry;
	}
	*result_count = count;

	free(matches);
}

void sort_items(ConfigEntry *items, int total_items, const char *query) {
	if (!items || total_items <= 1 || !query || *query == '\0') return;

	MatchResult *matches = malloc(sizeof(MatchResult) * total_items);
	if (!matches) return;

	for (int i = 0; i < total_items; i++) {
		matches[i].entry = &items[i];
		matches[i].score = score_item(items[i].name, query);
		matches[i].index = i;
	}

	qsort(matches, total_items, sizeof(MatchResult), compare_results);

	ConfigEntry *temp = malloc(sizeof(ConfigEntry) * total_items);
	if (temp) {
		for (int i = 0; i < total_items; i++) {
			temp[i] = *matches[i].entry;
		}
		memcpy(items, temp, sizeof(ConfigEntry) * total_items);
		free(temp);
	}

	free(matches);
}




