

#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_MAX_ENTRIES 128
#define CONFIG_ENTRY_MAX 256


typedef struct {
    char name[CONFIG_ENTRY_MAX];
    char command[CONFIG_ENTRY_MAX];
} ConfigEntry;


typedef struct {
    ConfigEntry entries[CONFIG_MAX_ENTRIES];
    int entries_count;
} Config;


int config_load(Config *config, const char *path);


#endif // CONFIG_H
