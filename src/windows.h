
#include "raylib.h"
#include <stdlib.h>

typedef struct {
    int width, height;
    const char *window_title;
    bool close_window;

    Font font;
    Color background;
    Color foreground;
    Color selection_bg;
    Color selection_fg;

    // selection and scrolling part
    int selected_index;
    int scroll_offset;
    float repeat_timer;
    int repeat_key;
} Launcher;
static Launcher *launcher = NULL;


void launcher_init();
bool window_should_close();
void launcher_update();
void launcher_cleanup();
void draw_contents();
