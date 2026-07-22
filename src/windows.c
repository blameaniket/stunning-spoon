#include "raylib.h"
#include "windows.h"
#include <stdlib.h>
#include <stdio.h>


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

void launcher_init(void) {
    launcher = malloc(sizeof(*launcher));
    launcher->width = 800;
    launcher->height = 600;
    launcher->window_title = "hello world";
    launcher->close_window = false;

    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetTraceLogLevel(LOG_NONE);
    InitWindow(launcher->width, launcher->height, launcher->window_title);
    SetTargetFPS(60);

    int font_size = 25;
    launcher->font = LoadFontEx("C:\\Windows\\Fonts\\JetBrainsMonoNerdFont-Regular.ttf", font_size, NULL, 0);
    launcher->selected_index = 0;
    launcher->item_count = sizeof(table)/sizeof(table[0]);

    set_color("#282828", 1.0, &launcher->background);
    set_color("#ebdbb2", 1.0, &launcher->foreground);
    set_color("#444444", 1.0, &launcher->selection_bg);
    set_color("#ffffff", 1.0, &launcher->selection_fg);
}

bool window_should_close(void) {
    return WindowShouldClose() || launcher->close_window;
}


void handle_all_inputs() {
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
    if (launcher->selected_index >= launcher->item_count)
        launcher->selected_index = launcher->item_count - 1;

    int line_height = launcher->font.baseSize + 2;
    int visible = launcher->height / line_height;
    if (launcher->selected_index < launcher->scroll_offset)
        launcher->scroll_offset = launcher->selected_index;

    if (launcher->selected_index >= launcher->scroll_offset + visible)
        launcher->scroll_offset = launcher->selected_index - visible + 1;
}


void launcher_update(void) {
    handle_all_inputs();

    BeginDrawing();
    ClearBackground(launcher->background);
    draw_contents();

    DrawFPS(launcher->width - 50, 20);
    EndDrawing();

    bool close_window_trigger = IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (close_window_trigger) {
        puts(table[launcher->selected_index]);
        fflush(stdout);
        launcher->close_window = true;
    }
}

void draw_contents() {
    int margin = 10;
    int line_height = launcher->font.baseSize + margin;
    Vector2 mouse_position = GetMousePosition();
    int y = margin;

    for (int i = 0; i < launcher->item_count; i++) {
        if (y + line_height > launcher->height) break;
        Rectangle rectangle_dimension = {
            10.0f,
            (float)y,
            (float)launcher->width - 20.0f,
            (float)line_height
        };

        bool is_hovered = CheckCollisionPointRec(mouse_position, rectangle_dimension);
        bool is_selected = (i == launcher->selected_index);
        if (is_selected || is_hovered) {
            DrawRectangleRec(rectangle_dimension, launcher->selection_bg);
        }

        Vector2 text_pos = {
            rectangle_dimension.x + 10.0f,
            rectangle_dimension.y + ((float)line_height - launcher->font.baseSize) / 2.0f
        };
        DrawTextEx(
            launcher->font,
            table[i],
            text_pos,
            (float)launcher->font.baseSize, 1.0f,
            launcher->foreground
        );

        y += line_height;

        if (!is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            continue;
        }
    }
}

void launcher_cleanup(void) {
    UnloadFont(launcher->font);
    CloseWindow();

    free(launcher);
}
