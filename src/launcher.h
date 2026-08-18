


#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "color.h"
#include "renderer.h"
#include <stdbool.h>


// access the launcher from main()
// for initialization purposes
typedef struct Launcher Launcher;


struct Launcher {
    bool should_close;

    struct {
        int width, height;
        const char *title;
        Color background_color;
        Color foreground_color;

        Font font;
        const char *font_family;
        const char *prompt;

    } window;

    struct {
    } repeat;



    const char **items;
    int items_count;
    int selected_index;
    const char *selected_item;
};



void launcher_init();
void launcher_update();
void launcher_cleanup();


#endif // LAUNCHER_H


