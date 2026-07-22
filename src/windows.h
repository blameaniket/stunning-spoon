
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
    int item_count;
    int selected_index;
    int scroll_offset;
    float repeat_timer;
    int repeat_key;

    // add mouse state
    bool mouse_click_item;
} Launcher;
static Launcher *launcher = NULL;


static char *table[] = {
    "hello world 1",
    "hello world 2",
    "hello world 3",
    "hello world 4",
    "hello world 5",
    "hello world 6",
};

void launcher_init();
bool window_should_close();
void launcher_update();
void launcher_cleanup();
void draw_contents();
