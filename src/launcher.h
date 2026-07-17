

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>


typedef struct item_t {
    char *text;
} item_t;
static item_t *items = NULL;
static int item_count = 0;

typedef struct {
    int width, height;

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

    bool exit_loop;
} Launcher;
static Launcher *launcher = NULL;

char *find_font(const char *family);
void read_stdin();
void launcher_init();
void launcher_update();
void draw_all_contents();
void cleanup_remaining_bits();
bool window_should_close();
