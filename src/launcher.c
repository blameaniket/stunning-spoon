


#include "launcher.h"
#include "window.h"
#include "renderer.h"
#include "keys.h"


typedef struct LauncherContext {
    Color bg_color;
    Color fg_color;
    Font font;

    const char **items;
    int items_count;
    int selected_index;
    const char *selected_item;
} LauncherContext;


static LauncherContext ctx = { 0 };


void launcher_init(Launcher *launcher) {
    renderer_init();

    ctx.bg_color = launcher->window.background_color;
    ctx.fg_color = launcher->window.foreground_color;
    if (ctx.fg_color.alpha == 0.0f && ctx.fg_color.r == 0.0f && ctx.fg_color.g == 0.0f && ctx.fg_color.b == 0.0f) {
        ctx.fg_color = hex_to_rgb("#ebdbb2");
    }



    static const char *items[] = {
        "firefox",
        "spotify",
        "discord",
        "steam",
        "poweroff",
        "reboot",
    };

    ctx.items = items;
    ctx.items_count = sizeof(items) / sizeof(items[0]);


    const char *font_path = launcher->window.font_family ? launcher->window.font_family : 
        "assets/fonts/IosevkaTermSlab_nerdfont/IosevkaTermSlabNerdFont-Regular.ttf";
    ctx.font = load_font(font_path, 40);
}


void launcher_update() {
    clear_background(ctx.bg_color);

    float prompt_x = 60.0f;
    float prompt_y = 60.0f;
    const char *prompt = "run:";
    draw_text(ctx.font, prompt, (Vector2){ prompt_x, prompt_y }, 1.0f, ctx.fg_color);


    float spacing = 20.0f;
    float run_width = measure_text_length(ctx.font, prompt, 1.0f);
    float margin_x = prompt_x + run_width + spacing;

    float line_height = ctx.font.line_height > 0 ? (float)ctx.font.line_height : 45.0f;
    float margin_y = line_height + prompt_y;

    for (int i = 0; i < ctx.items_count; i++) {
        draw_text(ctx.font,
                ctx.items[i], 
                (Vector2){ margin_x, margin_y }, 1.0f, 
                ctx.fg_color);

        margin_y += line_height;
    }


    window_swap_buffers();
    window_poll_events();
}


void launcher_cleanup() {
    unload_font(ctx.font);
    renderer_shutdown();
}


