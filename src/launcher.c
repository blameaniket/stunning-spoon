

#include "launcher.h"
#include "window.h"



typedef struct LauncherContext {
    unsigned int window_height;
    unsigned int window_width;
} LauncherContext;

static LauncherContext launcher = {0};

static void launcher_init() {
    launcher.window_width = 800;
    launcher.window_height = 600;
}

void launcher_run() {
    launcher_init();
    init_window(launcher.window_width, launcher.window_height, "stunning spoon");

    while (!window_should_close()) {
        window_poll_events();
        window_swap_buffers();
    }

    close_window();
}



