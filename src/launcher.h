



#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "color.h"
typedef struct Launcher Launcher;


struct Launcher {
    struct {
        int width, height;
        const char *title;
        Color background_color;
        Color foreground_color;
    } window;
};



void launcher_init(Launcher *launcher);
void launcher_update();


#endif // LAUNCHER_H
