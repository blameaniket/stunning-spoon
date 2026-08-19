


#include "launcher.h"
#include "window.h"
#include "renderer.h"
#include "keys.h"
#include <stdio.h>


void launcher_init(Launcher *launcher) {
    renderer_init();
    launcher->should_close = false;
    launcher->should_execute = false;


    launcher->query[0] = '\0';
    launcher->query_length = 0;
    launcher->selected_index = 0;


    // loading font on gpu
    int font_size = 37;
    const char *font_path = launcher->window.font_family ? launcher->window.font_family : 
        "assets/fonts/IosevkaTermSlab_nerdfont/IosevkaTermSlabNerdFont-Regular.ttf";
    launcher->window.font = load_font(font_path, 35);
}





void launcher_update(Launcher *launcher) {
    window_poll_events();
    if (window_should_close()) launcher->should_close = true;
    // open the window even if there is no items in the list
    // so no other conditions

    // handle all keyboard inputs here
    int ch;
    while ((ch = get_char_pressed()) != 0) {
        // printf("CHAR: %d '%c'\n", ch, (char)ch);
        if (ch >= 32 && ch <= 126) {
            if (launcher->query_length < LAUNCHER_QUERY_MAX - 1) {
                launcher->query[launcher->query_length++] = (char)ch;
                launcher->query[launcher->query_length] = '\0';
            }
        }
    }

    if (is_key_pressed(KEY_BACKSPACE)) {
        if (launcher->query_length > 0) {
            launcher->query_length--;
            launcher->query[launcher->query_length] = '\0';
        }
    }

    if (is_key_pressed(KEY_DOWN)) launcher->selected_index++;
    if (is_key_pressed(KEY_UP)) launcher->selected_index--;

    // selected index gets out of bound
    // so calculate them and set them accurately
    if (launcher->items_count > 0) {
        if (launcher->selected_index < 0)
            launcher->selected_index = 0;

        if (launcher->selected_index >= launcher->items_count)
            launcher->selected_index = launcher->items_count - 1;
    } else {
        launcher->selected_index = 0;
    }



    if (is_key_pressed(KEY_ENTER)) {
        if (launcher->items_count > 0 &&
                launcher->selected_index >= 0 &&
                launcher->selected_index < launcher->items_count) {

            launcher->should_execute = true;
            launcher->should_close = true;
        }
    }


    clear_background(launcher->window.background_color);

    // draw prompt
    float prompt_x = 60.0f;
    float prompt_y = 60.0f;
    draw_text(launcher->window.font, 
            launcher->window.prompt, 
            (Vector2){ prompt_x, prompt_y }, 1.0f, 
            launcher->window.foreground_color);


    float spacing = 20.0f;
    float prompt_width = measure_text_length(launcher->window.font, launcher->window.prompt, 1.0f);

    // draw query
    float query_x = prompt_x + prompt_width + spacing;
    draw_text(launcher->window.font, 
            launcher->query, 
            (Vector2){ query_x, prompt_y }, 1.0f, 
            launcher->window.foreground_color);



    float margin_x = prompt_x + prompt_width + spacing;
    float line_height = launcher->window.font.line_height > 0 ? (float)launcher->window.font.line_height : 45.0f;
    float margin_y = line_height + prompt_y;
    for (int i = 0; i < launcher->items_count; i++) {
        int next_item = margin_y + line_height;
        if (next_item >= get_window_height()) break;

        Color font_color = (launcher->selected_index == i) ? 
            launcher->window.selected_item_color : launcher->window.foreground_color;

        draw_text(launcher->window.font,
                launcher->items[i].name, 
                (Vector2){ margin_x, margin_y }, 1.0f, 
                font_color);

        margin_y += line_height;
    }


    window_swap_buffers();
}


void launcher_cleanup(Launcher *launcher) {
    unload_font(launcher->window.font);
    renderer_shutdown();
}


