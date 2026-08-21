


#include "launcher.h"
#include "window.h"
#include "renderer.h"
#include "match.h"
#include "keys.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>


void launcher_init(Launcher *launcher) {
    renderer_init();
    launcher->should_close = false;
    launcher->should_execute = false;


    launcher->query[0] = '\0';
    launcher->query_length = 0;
    launcher->selected_index = 0;

    launcher->result.items = malloc(sizeof(ConfigEntry *) * launcher->items_count);
    launcher->result.items_count = 0;

    launcher->cursor_pos = 0;

    if (launcher->result.items == NULL && launcher->items_count > 0) {
        log_error("failed to allocate result items\n");
        launcher->should_close = true;
        return;
    }


    const char *home = getenv("HOME");
    char font_path[512];

    int font_size = 37;
    snprintf(font_path, sizeof(font_path), 
            "%s/dev/projects/stunning-spoon/assets/fonts/IosevkaTermSlab_nerdfont/IosevkaTermSlabNerdFont-Regular.ttf", home);
    launcher->window.font = load_font(font_path, 35);
}





void launcher_update(Launcher *launcher) {
    window_poll_events();
    if (window_should_close()) launcher->should_close = true;


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


    bool ctrl = is_key_down(KEY_LEFT_CONTROL) || is_key_down(KEY_RIGHT_CONTROL);

    if (is_key_pressed(KEY_BACKSPACE)) {
        if (ctrl) {
            while (launcher->query_length > 0 &&
                    launcher->query[launcher->query_length - 1] == ' ') {
                launcher->query_length--;
            }

            while (launcher->query_length > 0 &&
                    launcher->query[launcher->query_length - 1] != ' ') {
                launcher->query_length--;
            }

            launcher->query[launcher->query_length] = '\0';

        } else {
            // normal Backspace delete one character
            if (launcher->query_length > 0) {
                launcher->query_length--;
                launcher->query[launcher->query_length] = '\0';
            }
        }
    }


    // Ctrl-W: delete word
    if (ctrl && is_key_pressed(KEY_W)) {
        while (launcher->query_length > 0 &&
                launcher->query[launcher->query_length - 1] == ' ') {
            launcher->query_length--;
        }

        while (launcher->query_length > 0 &&
                launcher->query[launcher->query_length - 1] != ' ') {
            launcher->query_length--;
        }

        launcher->query[launcher->query_length] = '\0';
    }



    // this puts all the values for 
    // result items and resultant items count
    // all in one go, so no need to worry about resultant items count
    match_items(launcher->items, 
            launcher->items_count, 
            launcher->query, 
            launcher->result.items, 
            &launcher->result.items_count);



    if (is_key_pressed(KEY_DOWN) || (ctrl && is_key_pressed(KEY_N))) launcher->selected_index++;
    if (is_key_pressed(KEY_UP)   || (ctrl && is_key_pressed(KEY_P))) launcher->selected_index--;


    // selected index gets out of bound
    // so calculate them and set them accurately
    if (launcher->result.items_count > 0) {
        if (launcher->selected_index < 0)
            launcher->selected_index = 0;

        if (launcher->selected_index >= launcher->result.items_count)
            launcher->selected_index = launcher->result.items_count - 1;
    } else {
        launcher->selected_index = 0;
    }



    if (is_key_pressed(KEY_ENTER)) {
        if (launcher->result.items_count > 0 &&
                launcher->selected_index >= 0 &&
                launcher->selected_index < launcher->result.items_count) {

            launcher->should_execute = true;
            launcher->should_close = true;
        }
    }



    // set launcher background
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


    for (int i = 0; i < launcher->result.items_count; i++) {
        int next_item = margin_y + 2*line_height;
        if (next_item >= get_window_height()) break;

        Color font_color = (launcher->selected_index == i) ? 
            launcher->window.selected_item_color : launcher->window.foreground_color;

        draw_text(launcher->window.font,
                launcher->result.items[i]->name, 
                (Vector2){ margin_x, margin_y }, 1.0f, 
                font_color);

        margin_y += line_height;
    }


    window_swap_buffers();
}


void launcher_cleanup(Launcher *launcher) {
    free(launcher->result.items);

    unload_font(launcher->window.font);
    renderer_shutdown();
}


