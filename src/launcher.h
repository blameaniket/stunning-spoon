


#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <stdbool.h>
#include <GLFW/glfw3.h>

#include "color.h"
#include "renderer.h"

typedef struct LauncherContext {
    bool is_running;
    bool is_visible;

    unsigned int window_height;
    unsigned int window_width;
    GLFWwindow *window_handle;

    Color background_color;
    Color foreground_color;

    Font font;
} LauncherContext;

LauncherContext *launcher_get_context();

void launcher_run();


#endif // LAUNCHER_H

