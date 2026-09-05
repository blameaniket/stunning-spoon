



#ifndef MATCH_H
#define MATCH_H

#include "config.h"


void sort_items(ConfigEntry *items, int total_items, const char *query);
void match_items(ConfigEntry *items, int total_items, const char *query, ConfigEntry **results, int *result_count);


#endif // MATCH_H




