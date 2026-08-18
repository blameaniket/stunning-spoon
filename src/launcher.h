


#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "color.h"
#include <stdbool.h>


// access the launcher from main()
// for initialization purposes
typedef struct Launcher Launcher;


struct Launcher {
    bool is_closed;

    struct {
        int width, height;
        const char *title;
        Color background_color;
        Color foreground_color;

        const char *font_family;
    } window;
};



void launcher_init(Launcher *launcher);
void launcher_update();
void launcher_cleanup();


#endif // LAUNCHER_H


