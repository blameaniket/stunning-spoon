

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "window.h"
#include "log.h"

typedef struct WindowContext {
    GLFWwindow *handle;
} WindowContext;

// global window context declared here !!!!
// this is available to window.c only
// for storing the state of the window
static WindowContext g_win_ctx = { 0 };


void window_init(int width, int height, const char *window_title) {
    if (!glfwInit()) {
        log_error("failed to initialize glfw\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // for raylib floating window

    g_win_ctx.handle = glfwCreateWindow(width, height, window_title, NULL, NULL);
    if (!g_win_ctx.handle) {
        log_error("failed to create glfw window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(g_win_ctx.handle);
    glfwSwapInterval(1); // Enable V-Sync by default

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log_error("Failed to initialize GLAD OpenGL loader\n");
        exit(EXIT_FAILURE);
    }
}


bool window_should_close() {
    if (g_win_ctx.handle == NULL) return true;

    if (glfwGetKey(g_win_ctx.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(g_win_ctx.handle, true);
    }

    return glfwWindowShouldClose(g_win_ctx.handle);
}



void destroy_window() {

    if (g_win_ctx.handle) {
        glfwDestroyWindow(g_win_ctx.handle);
        g_win_ctx.handle = NULL;
    }
    glfwTerminate();
}


void window_swap_buffers() {
    glfwSwapBuffers(g_win_ctx.handle);
}

void window_poll_events() {
    glfwPollEvents();
}


GLFWwindow* get_current_window() {
    return g_win_ctx.handle;
}


