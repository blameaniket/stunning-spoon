

#include <stdbool.h>

#include "launcher.h"
#include "window.h"
#include "renderer.h"
#include "color.h"


typedef struct LauncherContext {
    bool is_running;
    bool is_visible;

    unsigned int window_height;
    unsigned int window_width;

    Color background_color;
    Color foreground_color;
} LauncherContext;

static LauncherContext launcher = {0};

static void launcher_update();
static void loop();

void clear_screen() {
    launcher.background_color = hex_to_rgb("#111111");
    clear_screen(launcher.background_color);
}

void launcher_run() {
    launcher.window_width = 800;
    launcher.window_height = 600;
    init_window(launcher.window_width, launcher.window_height, "stunning spoon");
    renderer_init();

    launcher.is_visible = false;
    launcher.is_running = true;
    while (!launcher.is_running) {
        while (!window_should_close()) {
            launcher_update();
        }
    }

    renderer_shutdown();
    close_window();
}


static void launcher_update() {
    window_poll_events();
    clear_screen();

    window_swap_buffers();
}



