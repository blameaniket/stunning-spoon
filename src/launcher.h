



#ifndef LAUNCHER_H
#define LAUNCHER_H

typedef struct Launcher Launcher;

typedef struct LauncherContext {
} LauncherContext;

struct Launcher {
    struct {
        int width, height;
        const char *title;
    } window;
};



void launcher_init(Launcher *launcher);
LauncherContext get_launcher_context(Launcher *launcher);


#endif // LAUNCHER_H
