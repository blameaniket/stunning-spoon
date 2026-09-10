


#include <stdbool.h>
#include "launcher.h"
#include "window.h"
#include "renderer.h"
#include "color.h"
#include "log.h"

static LauncherContext launcher = {0};

static void launcher_update();

LauncherContext *launcher_get_context() {
    return &launcher;
}

void clear_screen() {
    clear_color(launcher.background_color);
}

void launcher_run() {
    init_window();
    renderer_init();

    int font_size = 20;
    launcher.font = load_font("assets/fonts/IosevkaTermSlab_nerdfont/IosevkaTermSlabNerdFont-Regular.ttf", font_size);
    launcher.background_color = hex_to_rgb("#111111");
    launcher.foreground_color = hex_to_rgb("#ffffff");

    launcher.is_running = true;
    launcher.is_visible = false;

    window_hide();

    while (launcher.is_running) {

        /*
         * When hidden, don't continuously render.
         * Sleep until something happens, e.g. Ctrl+Space.
         */
        if (!launcher.is_visible) {
            glfwWaitEvents();
            continue;
        }

        launcher_update();
    }

    renderer_shutdown();
    close_window();
}

static void launcher_update() {
    window_poll_events();

    if (window_should_close()) {
        launcher.is_running = false;
        return;
    }

    clear_screen();
    draw_text(launcher.font, "hello world", (Vector2){100,100}, 1.0, launcher.foreground_color);
    window_swap_buffers();
}
