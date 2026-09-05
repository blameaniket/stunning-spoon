


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "renderer.h"
#include "color.h"



void clear_screen(Color color) {
    glClearColor(color.r, color.g, color.b, color.alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}


void renderer_init() {
}

void renderer_shutdown() {
}

Font load_font(const char *file_path, unsigned int font_size) {
    Font font = {0};
    return font;
}

void unload_font(Font font) {
}

void draw_text(Font font, const char *text, Vector2 position, float scale, Color text_color) {
}



