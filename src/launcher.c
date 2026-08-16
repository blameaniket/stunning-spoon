

#include "launcher.h"
#include "renderer.h"
#include "window.h"

typedef struct LauncherContext {
    Color bg_color;
    Color fg_color;
} LauncherContext;

static LauncherContext ctx = { 0 };



void launcher_init(Launcher *launcher) {
    ctx.bg_color = launcher->window.background_color;
}


void launcher_update() {
    clear_color(ctx.bg_color);

    window_swap_buffers();
    window_poll_events();
}




