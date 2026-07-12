#include <stddef.h>
#include "raylib.h"

#define DRAWTEXT(x, y, z, color)  DrawTextEx(font, z, (Vector2){ x, y }, font_size, 1, color);

typedef struct item_t {
    char *text;
} item_t;
static item_t *items = NULL;


Color new_color(const char *hex);
void read_stdin(void);
void filter_items(void);

