

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <windows.h>
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif

#include <stdlib.h>
#include <stdio.h>

#include "window.h"
#include "log.h"


typedef struct WindowContext {
	GLFWwindow *handle;
} WindowContext;

static WindowContext g_win_ctx = {0};


static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "[GLFW ERROR %d] %s\n", error, description);
}

// initialize a glfw window
// parameters: width, height, window_title
void init_window(int width, int height, const char *window_title) {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        log_error("failed to initialize glfw\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_win_ctx.handle = glfwCreateWindow(width, height, window_title, NULL, NULL);
    if (!g_win_ctx.handle) {
        log_error("failed to create glfw window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(g_win_ctx.handle);

    BOOL useDarkMode = TRUE;

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &useDarkMode,
        sizeof(useDarkMode)
    );
#endif

    {
        // center window
        // first get total monitors
        int monitor_count = 0;
        GLFWmonitor **monitors = glfwGetMonitors(&monitor_count);
        if (monitor_count == 0) {
            log_error("could not find any monitor holy fk buy a monitor dude\n");
            exit(EXIT_FAILURE);
        }

        // now get monitor info
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        int monitor_x, monitor_y;
        int monitor_width, monitor_height;
        glfwGetMonitorWorkarea(monitor, &monitor_x, &monitor_y, &monitor_width, &monitor_height);

        // set the position here
        int x = monitor_x + (monitor_width - width) / 2;
        int y = monitor_y + (monitor_height - height) / 2;
        glfwSetWindowPos(g_win_ctx.handle, x, y);

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



void close_window() {
	if (g_win_ctx.handle) {
		glfwDestroyWindow(g_win_ctx.handle);
		g_win_ctx.handle = NULL;
	}
	glfwTerminate();
}


void window_swap_buffers() { glfwSwapBuffers(g_win_ctx.handle); }

void window_poll_events() { glfwPollEvents(); }

