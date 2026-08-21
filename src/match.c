


#include "match.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>



static int score_item(const char *name, const char *query) {
    if (!name || !query) return -1;
    if (*query == '\0') return 0;


    size_t name_len = strlen(name);
    size_t query_len = strlen(query);
    if (query_len > name_len) return -1; 


    for (size_t i = 0; i <= name_len - query_len; i++) {
        int matched = 1;

        for (size_t j = 0; j < query_len; j++) {
            unsigned char a = (unsigned char)name[i + j];
            unsigned char b = (unsigned char)query[j];

            if (tolower(a) != tolower(b)) {
                matched = 0;
                break;
            }
        }


        if (matched) {
            if (name_len == query_len) return 10000;
            if (i == 0) return 5000;

            return 4000 - (int)i;
        }

    }


    return -1;
}


void match_items(
    ConfigEntry *items,
    int total_items,
    const char *query,
    ConfigEntry **results,
    int *result_count
) {
    if (!items || !query || !results || !result_count)
        return;

    *result_count = 0;

    for (int i = 0; i < total_items; i++) {
        int score = score_item(items[i].name, query);

        if (score >= 0) {
            results[*result_count] = &items[i];
            (*result_count)++;
        }
    }
}


