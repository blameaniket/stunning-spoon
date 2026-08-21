

#include "config.h"

#include <stdio.h>
#include <string.h>


static char *trim(char *str) {
    while (*str == ' ' || *str == '\t') str++;

    char *end = str + strlen(str) - 1;
    while (end >= str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }

    return str;
}


int config_load(Config *config, const char *path) {
    FILE *file = fopen(path, "r");

    if (!file) {
        perror("fopen");
        return -1;
    }

    config->entries_count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Ignore empty lines
        if (line[0] == '\n' || line[0] == '\0') continue;

        // Find ':'
        char *separator = strchr(line, ':');

        if (!separator) continue;

        // Split the line
        *separator = '\0';

        char *name = trim(line);
        char *command = trim(separator + 1);

        // Don't allow too many entries
        if (config->entries_count >= CONFIG_MAX_ENTRIES) break;

        // Don't allow empty entries
        if (*name == '\0' || *command == '\0') continue;

        ConfigEntry *entry = &config->entries[config->entries_count];

        snprintf(entry->name,
            sizeof(entry->name),
            "%s",
            name);

        snprintf(entry->command,
            sizeof(entry->command),
            "%s",
            command);

        config->entries_count++;
    }

    fclose(file);
    return 0;
}




