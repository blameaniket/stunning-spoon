


#include "launcher.h"
#include "renderer.h"
#include "window.h"

typedef struct LauncherContext {
    Color bg_color;
    Color fg_color;

    Font font;
} LauncherContext;

static LauncherContext ctx = { 0 };



void launcher_init(Launcher *launcher) {
    ctx.bg_color = launcher->window.background_color;
    ctx.font = launcher->window.font_family;

    load_font(launcher->window.font_family, );
    renderer_init();
}


void launcher_update() {
    clear_background(ctx.bg_color);

    window_swap_buffers();
    window_poll_events();
}


void launcher_cleanup() {
    renderer_shutdown();
}


