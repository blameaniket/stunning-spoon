


#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "color.h"
#include "config.h"
#include "renderer.h"
#include <stdbool.h>



#define LAUNCHER_QUERY_MAX 256

// access the launcher from main()
// for initialization purposes
typedef struct Launcher Launcher;


struct Launcher {
    bool should_close;
    bool should_execute;

    struct {
        int width, height;
        const char *title;
        Color background_color;
        Color foreground_color;
        Color selected_item_color;

        Font font;
        const char *font_family;
        const char *prompt;
    } window;


    // store the items
    // const char **items;
    ConfigEntry *items;
    int items_count;
    struct {
        ConfigEntry **items;
        int items_count;
    } result;



    // query
    char query[LAUNCHER_QUERY_MAX];
    int query_length;
    float single_letter_width;

    int cursor_pos;
    float cursor_width;
    float cursor_height;

    // track selection
    int selected_index;
    const char *selected_item;
};



void launcher_init(Launcher *launcher);
void launcher_update(Launcher *launcher);
void launcher_cleanup(Launcher *launcher);


#endif // LAUNCHER_H


