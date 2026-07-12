#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "config.h"
#include "utils.h"


static char query[256] = {0};
static int qlen = 0;
static int selected = 0;
static int *visible_indices = NULL;
static int visible_count = 0;
static int total_items = 0;

Color new_color(const char *hex) {
    unsigned int value;
    if (*hex == '#') hex++;
    sscanf(hex, "%x", &value);

    return (Color){
        (value >> 16) & 0xFF,
        (value >> 8) & 0xFF,
        value & 0xFF,
        255
    };
}

void read_stdin(void) {
    char *line = NULL;
    size_t i = 0, itemsiz = 0, linesiz = 0;
    ssize_t len;

    for (; (len = getline(&line, &linesiz, stdin)) != -1; i++) {
        if (i + 1 >= itemsiz) {
            itemsiz += 256;
            item_t *tmp = realloc(items, itemsiz * sizeof(*items));
            if (!tmp) {
                printf("cannot allocate %zu bytes\n", itemsiz * sizeof(*items));
                exit(1);
            }
            items = tmp;
        }

        if (line[len - 1] == '\n') line[len - 1] = '\0';

        items[i].text = strdup(line);
    }

    free(line);

    if (items) {
        items[i].text = NULL;
    }
}

void filter_items(void) {
    visible_count = 0;
    if (!items || total_items == 0) return;

    for (int i = 0; i < total_items; i++) {
        if (query[0] && !strstr(items[i].text, query))
            continue;

        visible_indices[visible_count++] = i;
    }
}


int main() {
    if (isatty(STDIN_FILENO)) {
        fprintf(stderr, "no stdin detected\n");
        return 0;
    }
    read_stdin();

    while (items && items[total_items].text != NULL) {
        total_items++;
    }

    if (total_items > 0) {
        visible_indices = malloc(total_items * sizeof(int));

        for (int i = 0; i < total_items; i++) {
            visible_indices[visible_count++] = i;
        }
    }

    SetTraceLogLevel(LOG_NONE);
    InitWindow(menu_width, menu_height, "dashlauncher");
    SetTargetFPS(0);

    // define all colors
    Color bg = new_color(MENU_BGCOLOR);
    Color fg = new_color("#ffffff");
    Color selected_bg = new_color("#525e54");
    Color inputbox_bgcolor = new_color(INPUTBOX_BGCOLOR);
    Color entry_text_color = WHITE;


    // font loading
    // use jetbrains mono nerd font
    Font font = LoadFontEx("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf", 20, NULL, 0);


    static float repeat_timer = 0.0f;
    static int repeat_key = 0;
    while (!WindowShouldClose()) {
        bool query_changed = false;
        int key = GetCharPressed();

        while (key > 0) {
            if (qlen < 255) {
                query[qlen++] = (char)key;
                query[qlen] = '\0';
                query_changed = true;
            }
            key = GetCharPressed();
        }


        float dt = GetFrameTime();
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (qlen > 0) {
                    query[--qlen] = '\0';
                    query_changed = true;
                }
                repeat_key = KEY_BACKSPACE;
            }

            if (IsKeyPressed(KEY_UP)) {
                selected--;
                repeat_key = KEY_UP;
            }
            if (IsKeyPressed(KEY_DOWN)) {
                selected++;
                repeat_key = KEY_DOWN;
            }

            repeat_timer = 0.40f;
        }

        if (IsKeyDown(repeat_key)) {
            repeat_timer -= dt;
            while (repeat_timer <= 0.0f) {
                if (repeat_key == KEY_BACKSPACE && qlen > 0) {
                    query[--qlen] = '\0';
                    query_changed = true;
                }

                if (repeat_key == KEY_UP)
                    selected--;

                if (repeat_key == KEY_DOWN)
                    selected++;

                repeat_timer += 0.03f;
            }
        } else {
            repeat_key = 0;
            repeat_timer = 0.0f;
        }

        if (query_changed) {
            filter_items();
        }

        if (visible_count <= 0) {
            selected = 0;
        } else {
            if (selected < 0) selected = 0;
            if (selected >= visible_count)
                selected = visible_count - 1;
        }

        BeginDrawing();
            ClearBackground(bg);


            inputbox.xoffset = inputbox_xoffset;
            inputbox.yoffset = inputbox_yoffset;
            inputbox.width = inputbox_width;
            inputbox.height = inputbox_height;
            inputbox.bg_color = inputbox_bgcolor;
            draw_box(inputbox);
            // draw text
            DrawTextEx(font, query, 
                       (Vector2){inputbox_text_xoffset, inputbox_text_yoffset}, 
                       inputbox_text_fontsize, inputbox_text_letterspacing, WHITE);

            // entries part
            int draw_y = inputbox_height;
            for (int index = 0; index < visible_count; index++) {
                // dont draw the items which are not visible
                // very big performance jump
                if (draw_y >= menu_height) break;

                int item_idx = visible_indices[index];

                // draw rectangle
                // if current item being drawn in the loop (index)
                // is the currently selected item
                if (selected == index) {
                    DrawRectangle(0, draw_y, menu_width, 20, selected_bg);
                }

                // draw text
                DrawTextEx(font, items[item_idx].text, (Vector2){10, draw_y}, 20, 1, entry_text_color);

                draw_y += 20;
            }

            DrawFPS(menu_width - 80, 2);
        EndDrawing();

        if (IsKeyPressed(KEY_ENTER)) {
            if (visible_count > 0 && selected >= 0 && selected < visible_count) {
                int item_idx = visible_indices[selected];
                printf("%s\n", items[item_idx].text);
            }
            break;
        }
    }


    // cleanup all the shit
    UnloadFont(font);
    CloseWindow();

    for (int i = 0; items[i].text != NULL; i++)
        free(items[i].text);

    free(items);
    free(visible_indices);

    return 0;
}
