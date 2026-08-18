


#include "launcher.h"
#include "window.h"
#include "renderer.h"
#include "keys.h"


typedef struct LauncherContext {
    Color bg_color;
    Color fg_color;

} LauncherContext;


static Launcher launcher;


void launcher_init() {
    renderer_init();

    launcher.should_close = true;


    static const char *items[] = {
        "firefox",
        "spotify",
        "discord",
        "steam",
        "poweroff",
        "reboot",
    };

    launcher.items = items;
    launcher.items_count = sizeof(items) / sizeof(items[0]);


    const char *font_path = launcher.window.font_family ? launcher.window.font_family : 
        "assets/fonts/IosevkaTermSlab_nerdfont/IosevkaTermSlabNerdFont-Regular.ttf";
    launcher.window.font = load_font(font_path, 40);
}


void launcher_update() {
    clear_background(launcher.window.background_color);

    float prompt_x = 60.0f;
    float prompt_y = 60.0f;
    const char *prompt = "run:";
    draw_text(launcher.window.font, launcher.window.prompt, (Vector2){ prompt_x, prompt_y }, 1.0f, launcher.window.foreground_color);


    float spacing = 20.0f;
    float run_width = measure_text_length(launcher.window.font, prompt, 1.0f);
    float margin_x = prompt_x + run_width + spacing;

    float line_height = launcher.window.font.line_height > 0 ? (float)launcher.window.font.line_height : 45.0f;
    float margin_y = line_height + prompt_y;

    for (int i = 0; i < launcher.items_count; i++) {
        draw_text(launcher.window.font,
                launcher.items[i], 
                (Vector2){ margin_x, margin_y }, 1.0f, 
                launcher.window.foreground_color);

        margin_y += line_height;
    }


    window_swap_buffers();
    window_poll_events();
}


void launcher_cleanup() {
    unload_font(launcher.window.font);
    renderer_shutdown();
}


