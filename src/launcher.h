


#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "color.h"
#include "renderer.h"
#include <stdbool.h>



#define LAUNCHER_QUERY_MAX 256

// access the launcher from main()
// for initialization purposes
typedef struct Launcher Launcher;


struct Launcher {
    bool should_close;
    char query[LAUNCHER_QUERY_MAX];
    int query_length;

    struct {
        int width, height;
        const char *title;
        Color background_color;
        Color foreground_color;

        Font font;
        const char *font_family;
        const char *prompt;
    } window;


    // store the items
    const char **items;
    int items_count;

    // track selection
    int selected_index;
    const char *selected_item;
};



void launcher_init(Launcher *launcher);
void launcher_update(Launcher *launcher);
void launcher_cleanup(Launcher *launcher);


#endif // LAUNCHER_H


