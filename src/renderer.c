


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "renderer.h"
#include "color.h"


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
	GLuint text_shader;
	GLuint text_vao;
	GLuint text_vbo;
	GLint text_proj_loc;
	GLint text_color_loc;
	GLint text_sampler_loc;

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

void clear_color(Color color) {
    glClearColor(color.r, color.g, color.b, color.alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}


void renderer_init() {
	g_renderer_ctx.text_shader = create_program(text_vs_source, text_fs_source);
	g_renderer_ctx.text_proj_loc = glGetUniformLocation(g_renderer_ctx.text_shader, "projection");
	g_renderer_ctx.text_color_loc = glGetUniformLocation(g_renderer_ctx.text_shader, "textColor");
	g_renderer_ctx.text_sampler_loc = glGetUniformLocation(g_renderer_ctx.text_shader, "text");


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


	if (FT_Init_FreeType(&g_renderer_ctx.ft_lib)) {
		log_error("failed to initialize freetype library\n");
		g_renderer_ctx.ft_initialized = false;
	} else {
		g_renderer_ctx.ft_initialized = true;
	}

}

void renderer_shutdown() {
    if (g_renderer_ctx.text_vao) {
		glDeleteVertexArrays(1, &g_renderer_ctx.text_vao);
		glDeleteBuffers(1, &g_renderer_ctx.text_vbo);
		glDeleteProgram(g_renderer_ctx.text_shader);
    }

	if (g_renderer_ctx.ft_initialized) {
		FT_Done_FreeType(g_renderer_ctx.ft_lib);
		g_renderer_ctx.ft_initialized = false;
	}
}

Font load_font(const char *file_path, unsigned int font_size) {
    Font font = {0};
    font.size = font_size;


    return font;
}

void unload_font(Font font) {
}

void draw_text(Font font, const char *text, Vector2 position, float scale, Color text_color) {
}



