

#include "launcher.h"
#include "window.h"
#include "renderer.h"
#include "color.h"


typedef struct LauncherContext {
    unsigned int window_height;
    unsigned int window_width;

    Color background_color;
    Color foreground_color;
} LauncherContext;

static LauncherContext launcher = {0};

static void launcher_update();


void launcher_run() {
    launcher.window_width = 800;
    launcher.window_height = 600;
    init_window(launcher.window_width, launcher.window_height, "stunning spoon");
    renderer_init();

    launcher.background_color = hex_to_rgb("#111111");

    while (!window_should_close()) {
        window_poll_events();
        launcher_update();
    }

    renderer_shutdown();
    close_window();
}


static void launcher_update() {
    clear_screen(launcher.background_color);


    window_swap_buffers();
}



