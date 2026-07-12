#include "utils.h"
#include "raylib.h"

void draw_box(box_t box) {
    DrawRectangle(box.xoffset, box.yoffset, 
                  box.width, box.height, 
                  box.bg_color);
}

