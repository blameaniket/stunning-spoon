

#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include "color.h"


typedef struct Vector2 {
    float x;
    float y;
} Vector2;


typedef struct {
	unsigned int texture_id;
	int width;
	int height;
	int bearing_x;
	int bearing_y;
	unsigned int advance;
} Glyph;

typedef struct {
	Glyph glyphs[128];
	unsigned int size;
	float scale;

	int ascent;
	int descent;
	int line_height;
	bool loaded;
} Font;


void renderer_init();
void renderer_shutdown();
void clear_screen(Color color);
Font load_font(const char *file_path, unsigned int font_size);
void unload_font(Font font);
void draw_text(Font font, const char *text, Vector2 position, float scale, Color text_color);

#endif // RENDERER_H


