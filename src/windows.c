
#include "raylib.h"
#include "windows.h"
#include <stdlib.h>

#define BACKGROUND BLACK
#define TEXT_COLOR WHITE

void launcher_init() {
    launcher = malloc(sizeof(*launcher));
    launcher->width = 800;
    launcher->height = 600;
    launcher->window_title = "hello world";
    launcher->close_window = false;

    InitWindow(launcher->width, launcher->height, launcher->window_title);
    SetTargetFPS(0);
}

bool window_should_close() {
    return WindowShouldClose() || launcher->close_window;
}

void launcher_update() {
    BeginDrawing();
    ClearBackground(BACKGROUND);
    draw_contents();

    EndDrawing();
}

void draw_contents() {
    int font_size = 15;
    DrawText("hello world", 10, 10, font_size, TEXT_COLOR);
}

void launcher_cleanup() {
    CloseWindow();
}
