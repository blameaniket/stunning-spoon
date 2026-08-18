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
    renderer_init();

    ctx.bg_color = launcher->window.background_color;
    ctx.fg_color = launcher->window.foreground_color;
    if (ctx.fg_color.alpha == 0.0f && ctx.fg_color.r == 0.0f && ctx.fg_color.g == 0.0f && ctx.fg_color.b == 0.0f) {
        ctx.fg_color = hex_to_rgb("#ebdbb2");
    }

    const char *font_path = launcher->window.font_family ? launcher->window.font_family : "assets/fonts/IosevkaTermSlab_nerdfont/IosevkaTermSlabNerdFont-Regular.ttf";
    ctx.font = load_font(font_path, 40);
}


void launcher_update() {
    clear_background(ctx.bg_color);

    draw_text(ctx.font, "run:", (Vector2){ 60.0f, 60.0f }, 1.0f, ctx.fg_color);

    float margin = 100.0f;

    const char *list[] = {
        "firefox",
        "spotify",
        "discord",
        "steam",
    };

    float margin_x = 120.0f;
    float margin_y = 60.0f;

    for (int i=0; i < 4; i++) {
        draw_text(ctx.font,
                list[i], 
                (Vector2){ margin_x, margin_y }, 1.0f, 
                ctx.fg_color);

        margin_y += 35;
    }


    window_swap_buffers();
    window_poll_events();
}


void launcher_cleanup() {
    unload_font(ctx.font);
    renderer_shutdown();
}
