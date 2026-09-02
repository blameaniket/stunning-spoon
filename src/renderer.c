#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "renderer.h"
#include "window.h"

static const char *rect_vs_source =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "uniform mat4 projection;\n"
    "uniform vec4 rect;\n"
    "void main()\n"
    "{\n"
    "    vec2 pos = rect.xy + aPos * rect.zw;\n"
    "    gl_Position = projection * vec4(pos, 0.0, 1.0);\n"
    "}\n";

static const char *rect_fs_source = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "uniform vec4 color;\n"
                                    "void main()\n"
                                    "{\n"
                                    "    FragColor = color;\n"
                                    "}\n";

static const char *text_vs_source =
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
    "    TexCoords = vertex.zw;\n"
    "}\n";

static const char *text_fs_source =
    "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D text;\n"
    "uniform vec4 textColor;\n"
    "void main()\n"
    "{\n"
    "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
    "    FragColor = textColor * sampled;\n"
    "}\n";

typedef struct RenderContext {
	// Rectangle pipeline
	GLuint rect_shader;
	GLuint rect_vao;
	GLuint rect_vbo;
	GLint rect_proj_loc;
	GLint rect_bounds_loc;
	GLint rect_color_loc;

	// Text pipeline
	GLuint text_shader;
	GLuint text_vao;
	GLuint text_vbo;
	GLint text_proj_loc;
	GLint text_color_loc;
	GLint text_sampler_loc;

	// FreeType library handle
	FT_Library ft_lib;
	bool ft_initialized;
} RenderContext;

static RenderContext g_renderer_ctx = {0};

static GLuint compile_shader(GLenum type, const char *src) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
		log_error("SHADER ERROR: Shader compilation failed:\n%s\n", info_log);
	}
	return shader;
}

static GLuint create_program(const char *vs, const char *fs) {
	GLuint v_shader = compile_shader(GL_VERTEX_SHADER, vs);
	GLuint f_shader = compile_shader(GL_FRAGMENT_SHADER, fs);

	GLuint program = glCreateProgram();
	glAttachShader(program, v_shader);
	glAttachShader(program, f_shader);
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
		log_error("SHADER ERROR: Shader linking failed:\n%s\n", info_log);
	}

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	return program;
}

static void get_ortho_projection(float left, float right, float bottom,
                                 float top, float near_val, float far_val,
                                 float *mat) {
	for (int i = 0; i < 16; i++) {
		mat[i] = 0.0f;
	}
	mat[0] = 2.0f / (right - left);
	mat[5] = 2.0f / (top - bottom);
	mat[10] = -2.0f / (far_val - near_val);
	mat[12] = -(right + left) / (right - left);
	mat[13] = -(top + bottom) / (top - bottom);
	mat[14] = -(far_val + near_val) / (far_val - near_val);
	mat[15] = 1.0f;
}

static void update_viewport_and_projection(GLint proj_loc) {
	int w = 800;
	int h = 600;
	GLFWwindow *win = get_current_window();
	if (win) {
		glfwGetFramebufferSize(win, &w, &h);
	}
	if (w <= 0) w = 1;
	if (h <= 0) h = 1;

	glViewport(0, 0, w, h);

	float proj[16];
	get_ortho_projection(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f, proj);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, proj);
}

void renderer_init(void) {
	// 1. Setup OpenGL State
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// 2. Setup Rectangle pipeline
	g_renderer_ctx.rect_shader = create_program(rect_vs_source, rect_fs_source);
	g_renderer_ctx.rect_proj_loc =
	    glGetUniformLocation(g_renderer_ctx.rect_shader, "projection");
	g_renderer_ctx.rect_bounds_loc =
	    glGetUniformLocation(g_renderer_ctx.rect_shader, "rect");
	g_renderer_ctx.rect_color_loc =
	    glGetUniformLocation(g_renderer_ctx.rect_shader, "color");

	static const float unit_quad[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	                                  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};

	glGenVertexArrays(1, &g_renderer_ctx.rect_vao);
	glGenBuffers(1, &g_renderer_ctx.rect_vbo);

	glBindVertexArray(g_renderer_ctx.rect_vao);
	glBindBuffer(GL_ARRAY_BUFFER, g_renderer_ctx.rect_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unit_quad), unit_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
	                      (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 3. Setup Text pipeline
	g_renderer_ctx.text_shader = create_program(text_vs_source, text_fs_source);
	g_renderer_ctx.text_proj_loc =
	    glGetUniformLocation(g_renderer_ctx.text_shader, "projection");
	g_renderer_ctx.text_color_loc =
	    glGetUniformLocation(g_renderer_ctx.text_shader, "textColor");
	g_renderer_ctx.text_sampler_loc =
	    glGetUniformLocation(g_renderer_ctx.text_shader, "text");

	glGenVertexArrays(1, &g_renderer_ctx.text_vao);
	glGenBuffers(1, &g_renderer_ctx.text_vbo);

	glBindVertexArray(g_renderer_ctx.text_vao);
	glBindBuffer(GL_ARRAY_BUFFER, g_renderer_ctx.text_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 4. Initialize FreeType
	if (FT_Init_FreeType(&g_renderer_ctx.ft_lib)) {
		log_error("Failed to initialize FreeType library\n");
		g_renderer_ctx.ft_initialized = false;
	} else {
		g_renderer_ctx.ft_initialized = true;
	}
}

void renderer_shutdown(void) {
	if (g_renderer_ctx.rect_vao) {
		glDeleteVertexArrays(1, &g_renderer_ctx.rect_vao);
		glDeleteBuffers(1, &g_renderer_ctx.rect_vbo);
		glDeleteProgram(g_renderer_ctx.rect_shader);
	}

	if (g_renderer_ctx.text_vao) {
		glDeleteVertexArrays(1, &g_renderer_ctx.text_vao);
		glDeleteBuffers(1, &g_renderer_ctx.text_vbo);
		glDeleteProgram(g_renderer_ctx.text_shader);
	}

	if (g_renderer_ctx.ft_initialized) {
		FT_Done_FreeType(g_renderer_ctx.ft_lib);
		g_renderer_ctx.ft_initialized = false;
	}

	memset(&g_renderer_ctx, 0, sizeof(g_renderer_ctx));
}

void clear_background(Color color) {
	glClearColor(color.r, color.g, color.b, color.alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void draw_rectangle(RendererRectangle rectangle) {
	if (rectangle.width <= 0.0f || rectangle.height <= 0.0f) return;

	glUseProgram(g_renderer_ctx.rect_shader);
	update_viewport_and_projection(g_renderer_ctx.rect_proj_loc);

	glUniform4f(g_renderer_ctx.rect_bounds_loc, rectangle.x, rectangle.y,
	            rectangle.width, rectangle.height);
	glUniform4f(g_renderer_ctx.rect_color_loc, rectangle.color.r,
	            rectangle.color.g, rectangle.color.b, rectangle.color.alpha);

	glBindVertexArray(g_renderer_ctx.rect_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}

Font load_font(const char *file_path, unsigned int font_size) {
	Font font = {0};
	font.size = font_size;

	if (!g_renderer_ctx.ft_initialized) {
		if (FT_Init_FreeType(&g_renderer_ctx.ft_lib)) {
			log_error(
			    "FreeType not initialized and failed to init on font load\n");
			return font;
		}
		g_renderer_ctx.ft_initialized = true;
	}

	FT_Face face;
	if (FT_New_Face(g_renderer_ctx.ft_lib, file_path, 0, &face)) {
		log_error("Failed to load font from: %s\n", file_path);
		return font;
	}

	FT_Set_Pixel_Sizes(face, 0, font_size);

	font.ascent = (int)(face->size->metrics.ascender >> 6);
	font.descent = (int)(face->size->metrics.descender >> 6);
	font.line_height = (int)(face->size->metrics.height >> 6);
	if (font.ascent == 0) font.ascent = (int)font_size;
	if (font.line_height == 0) font.line_height = (int)(font_size * 1.2f);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			log_warning("Failed to load glyph for '%c' (%u)\n",
			            (c >= 32 && c <= 126) ? c : '?', c);
			continue;
		}

		GLuint texture = 0;
		if (face->glyph->bitmap.width > 0 && face->glyph->bitmap.rows > 0) {
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
			             face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
			             face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		font.glyphs[c].texture_id = texture;
		font.glyphs[c].width = face->glyph->bitmap.width;
		font.glyphs[c].height = face->glyph->bitmap.rows;
		font.glyphs[c].bearing_x = face->glyph->bitmap_left;
		font.glyphs[c].bearing_y = face->glyph->bitmap_top;
		font.glyphs[c].advance = face->glyph->advance.x;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	FT_Done_Face(face);
	font.loaded = true;
	return font;
}

void unload_font(Font font) {
	for (int i = 0; i < 128; i++) {
		if (font.glyphs[i].texture_id != 0) {
			glDeleteTextures(1, &font.glyphs[i].texture_id);
		}
	}
}

void draw_text(Font font, const char *text, Vector2 position, float scale,
               Color text_color) {
	if (!text || !font.loaded) return;

	glUseProgram(g_renderer_ctx.text_shader);
	update_viewport_and_projection(g_renderer_ctx.text_proj_loc);

	glUniform4f(g_renderer_ctx.text_color_loc, text_color.r, text_color.g,
	            text_color.b, text_color.alpha);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(g_renderer_ctx.text_sampler_loc, 0);

	glBindVertexArray(g_renderer_ctx.text_vao);
	glBindBuffer(GL_ARRAY_BUFFER, g_renderer_ctx.text_vbo);

	float x = position.x;
	float y = position.y;

	for (const char *p = text; *p; p++) {
		unsigned char c = (unsigned char)*p;

		if (c == '\n') {
			x = position.x;
			y += (font.line_height > 0 ? font.line_height : (int)font.size) *
			     scale;
			continue;
		}

		if (c == '\t') {
			unsigned int space_adv = font.glyphs[' '].advance
			                             ? font.glyphs[' '].advance
			                             : (font.size << 6) / 2;
			x += ((space_adv >> 6) * 4) * scale;
			continue;
		}

		if (c >= 128) continue;

		Glyph ch = font.glyphs[c];

		float xpos = x + ch.bearing_x * scale;
		float ypos = y + (font.ascent - ch.bearing_y) * scale;
		float w = ch.width * scale;
		float h = ch.height * scale;

		if (ch.texture_id != 0 && w > 0.0f && h > 0.0f) {
			float vertices[6][4] = {{xpos, ypos, 0.0f, 0.0f},
			                        {xpos, ypos + h, 0.0f, 1.0f},
			                        {xpos + w, ypos + h, 1.0f, 1.0f},

			                        {xpos, ypos, 0.0f, 0.0f},
			                        {xpos + w, ypos + h, 1.0f, 1.0f},
			                        {xpos + w, ypos, 1.0f, 0.0f}};

			glBindTexture(GL_TEXTURE_2D, ch.texture_id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		x += (ch.advance >> 6) * scale;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

Vector2 measure_text(Font font, const char *text, float scale) {
	Vector2 size = {0.0f, 0.0f};
	if (!text || !font.loaded) return size;

	float current_line_width = 0.0f;
	float max_width = 0.0f;
	float line_h =
	    (font.line_height > 0 ? (float)font.line_height : (float)font.size) *
	    scale;
	float total_height = line_h;

	for (const char *p = text; *p; p++) {
		unsigned char c = (unsigned char)*p;

		if (c == '\n') {
			if (current_line_width > max_width) {
				max_width = current_line_width;
			}
			current_line_width = 0.0f;
			total_height += line_h;
			continue;
		}

		if (c == '\t') {
			unsigned int space_adv = font.glyphs[' '].advance
			                             ? font.glyphs[' '].advance
			                             : (font.size << 6) / 2;
			current_line_width += ((space_adv >> 6) * 4) * scale;
			continue;
		}

		if (c >= 128) continue;

		Glyph ch = font.glyphs[c];
		current_line_width += (ch.advance >> 6) * scale;
	}

	if (current_line_width > max_width) {
		max_width = current_line_width;
	}

	size.x = max_width;
	size.y = total_height;
	return size;
}

float measure_text_length(Font font, const char *text, float scale) {
	return measure_text(font, text, scale).x;
}


