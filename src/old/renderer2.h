#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include "color.h"
#include "math.h"

typedef struct {
	float x;
	float y;
	float width;
	float height;
	Color color;
} RendererRectangle;

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

void renderer_init(void);
void renderer_shutdown(void);
void clear_background(Color color);

void draw_rectangle(RendererRectangle rectangle);

Font load_font(const char *file_path, unsigned int font_size);
void unload_font(Font font);
void draw_text(Font font, const char *text, Vector2 position, float scale,
               Color text_color);
Vector2 measure_text(Font font, const char *text, float scale);
float measure_text_length(Font font, const char *text, float scale);

#endif // RENDERER_H
