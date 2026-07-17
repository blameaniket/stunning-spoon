#include "raylib.h"
#include "launcher.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <fontconfig/fontconfig.h>
#include <string.h>


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
        item_count++;
    }

    free(line);

    if (items) {
        items[i].text = NULL;
    }
}

char *find_font(const char *family) {
    FcInit();
    FcPattern *pat = FcNameParse((const FcChar8 *)family);
    FcConfigSubstitute(NULL, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    FcResult result;
    FcPattern *font = FcFontMatch(NULL, pat, &result);
    char *path = NULL;
    if (font) {
        FcChar8 *file;
        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
            path = strdup((const char *)file);
        }

        FcPatternDestroy(font);
    }

    FcPatternDestroy(pat);
    return path;
}


void set_color(const char *hex, float alpha, Color *sample_color) {
    unsigned int hex_value = 0;

    if (hex[0] == '#') {
        hex++;
    }

    sscanf(hex, "%x", &hex_value);

    sample_color->r = (hex_value >> 16) & 0xFF;
    sample_color->g = (hex_value >> 8) & 0xFF;
    sample_color->b = hex_value & 0xFF;
    sample_color->a = (unsigned char)(alpha * 255.0f);
}

bool window_should_close() {
    return WindowShouldClose() || launcher->exit_loop;
}


void launcher_init() {
    read_stdin();

    launcher = malloc(sizeof(*launcher));
    char *font_name = "JetBrainsMono Nerd Font Mono";
    int font_size = 15;

    launcher->width = 800;
    launcher->height = (font_size + 2) * 15;
    launcher->exit_loop = false;

    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetTraceLogLevel(LOG_NONE);
    InitWindow(launcher->width, launcher->height, "launcher window");
    SetTargetFPS(0);

    char *font_path = find_font(font_name);
    if (font_path == NULL) {
        fprintf(stderr, "couldnt find font\n");
        exit(EXIT_FAILURE);
    }
    launcher->font = LoadFontEx(font_path, font_size, NULL, 0);
    launcher->selected_index = 0;
    launcher->scroll_offset = 0;
    launcher->repeat_timer = 0.0f;
    launcher->repeat_key = 0;

    set_color("#282828", 1.0, &launcher->background);
    set_color("#ebdbb2", 1.0, &launcher->foreground);
    set_color("#444444", 1.0, &launcher->selection_bg);
    set_color("#ffffff", 1.0, &launcher->selection_fg);
}


void launcher_input(void) {
    if (IsKeyPressed(KEY_DOWN)) {
        launcher->selected_index++;
        launcher->repeat_key = KEY_DOWN;
        launcher->repeat_timer = 0.5f;
    }

    if (IsKeyPressed(KEY_UP)) {
        launcher->selected_index--;
        launcher->repeat_key = KEY_UP;
        launcher->repeat_timer = 0.5f;
    }


    float dt = GetFrameTime();
    if (IsKeyDown(launcher->repeat_key)) {
        launcher->repeat_timer -= dt;
        while (launcher->repeat_timer <= 0.0f) {
            if (launcher->repeat_key == KEY_UP)
                launcher->selected_index--;

            if (launcher->repeat_key == KEY_DOWN)
                launcher->selected_index++;

            launcher->repeat_timer += 0.03f;
        }
    } else {
        launcher->repeat_key = 0;
        launcher->repeat_timer = 0.0f;
    }


    if (launcher->selected_index < 0) launcher->selected_index = 0;
    if (launcher->selected_index >= item_count)
        launcher->selected_index = item_count - 1;

    int line_height = launcher->font.baseSize + 2;
    int visible = launcher->height / line_height;

    if (launcher->selected_index < launcher->scroll_offset)
        launcher->scroll_offset = launcher->selected_index;

    if (launcher->selected_index >= launcher->scroll_offset + visible)
        launcher->scroll_offset = launcher->selected_index - visible + 1;
}

void launcher_update() {
    launcher_input();

    BeginDrawing();
    ClearBackground(launcher->background);
    draw_all_contents();

    DrawFPS(launcher->width - 200, 20);

    EndDrawing();

    if (IsKeyPressed(KEY_ENTER)) {
        puts(items[launcher->selected_index].text);
        fflush(stdout);
        launcher->exit_loop = true;
    }
}

void draw_all_contents(void) {
    int margin = 7;
    int y = 0;
    int line_height = launcher->font.baseSize + 2;
    int visible = launcher->height / line_height;
    int end = launcher->scroll_offset + visible;

    if (end > item_count) 
        end = item_count;

    for (int i = launcher->scroll_offset; i < end; i++) {
        if (i == launcher->selected_index) {
            DrawRectangle(
                0, y - 1,
                launcher->width, line_height + 2,
                launcher->selection_bg
            );

            DrawTextEx(
                launcher->font,
                items[i].text,
                (Vector2){margin, y+1},
                launcher->font.baseSize, 1,
                launcher->selection_fg
            );
        }
        else {
            DrawTextEx(
                launcher->font,
                items[i].text,
                (Vector2){margin, y+1},
                launcher->font.baseSize, 1,
                launcher->foreground
            );
        }

        y += line_height;
    }
}

void cleanup_remaining_bits() {
    UnloadFont(launcher->font);
    for (int i = 0; items[i].text != NULL; i++)
        free(items[i].text);

    free(items);

    free(launcher);

}


