
#include "raylib.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


const char *BACKGROUND = "#222222";
const char *TEXT_COLOR = "#ffffff";
const char *SELECTION_BG = "#333333";


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

        if (line[len - 1] == '\n') 
            line[len - 1] = '\0';

        items[i].text = strdup(line);
    }

    free(line);

    if (items) {
        items[i].text = NULL;
    }
}


int main() {
    if (isatty(STDIN_FILENO)) {
        fprintf(stderr, "no stdin detected\n");
        return 0;
    }
    read_stdin();
    
    int total_items = 0;
    for (int i = 0; items[i].text != NULL; i++) {
        total_items++;
    }


    int font_size = 15;
    int total_lines = 10;
    int line_height = 20;
    int WIDTH = 800;
    int HEIGHT = line_height * (total_lines);
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WIDTH, HEIGHT, "dashlauncher");
    SetTargetFPS(0);

    // keep both the loading font size
    // and the usable font size same
    Font font = LoadFontEx("/usr/share/fonts/TTF/FiraCodeNerdFont-Regular.ttf", font_size, NULL, 0);
    Color background = new_color(BACKGROUND);
    Color listitem_color = new_color(TEXT_COLOR);
    Color selection_bg = new_color(SELECTION_BG);

    int selected = 0;
    int scroll_offset = 0;
    float repeat_timer = 0.0f;
    int repeat_key = 0;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
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

        if (selected < 0) selected = 0;
        if (selected >= total_items)
            selected = total_items - 1;

        // calculate scroll offset

        BeginDrawing();
        ClearBackground(background);

        int margin = 0;
        int y = margin;
        for (int i = scroll_offset; items[i].text != NULL; i++) {
            if (i >= scroll_offset + total_lines) break;

            if (selected == i) {
                DrawRectangle(margin, y, WIDTH - (2 * margin), line_height, selection_bg);
            }

            float text_x = margin + 10;
            float text_y = y + (line_height - font_size) / 2.0f;
            DRAWTEXT(text_x, text_y, items[i].text, listitem_color);

            y += line_height;
        }

        EndDrawing();
    }


    UnloadFont(font);
    CloseWindow();

    for (int i = 0; items[i].text != NULL; i++)
        free(items[i].text);

    free(items);

    return 0;
}



