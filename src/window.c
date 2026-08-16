


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "window.h"
#include "log.h"

typedef struct WindowContext {
    GLFWwindow *handle;
} WindowContext;

static WindowContext g_win_ctx = { 0 };




static void window_center() {
    if (!g_win_ctx.handle) return;
    int monitor_count = 0;
    GLFWmonitor **monitors = glfwGetMonitors(&monitor_count);

    if (monitor_count == 0) return;

    // For now, use the primary monitor.
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    int monitor_x, monitor_y;
    int monitor_width, monitor_height;

    glfwGetMonitorWorkarea(
        monitor,
        &monitor_x,
        &monitor_y,
        &monitor_width,
        &monitor_height
    );

    int window_width, window_height;
    glfwGetWindowSize(g_win_ctx.handle, &window_width, &window_height);

    int x = monitor_x + (monitor_width - window_width) / 2;
    int y = monitor_y + (monitor_height - window_height) / 2;
    glfwSetWindowPos(g_win_ctx.handle, x, y);
}

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

    window_center();

}


bool window_should_close() {
    if (g_win_ctx.handle == NULL) return true;
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



