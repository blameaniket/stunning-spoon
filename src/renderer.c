
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "renderer.h"


typedef struct RenderBatch {
} RenderBatch;


void clear_color(Color color) {
    glClearColor(color.r, color.b, color.g, color.alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

