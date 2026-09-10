


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
#include <stdbool.h>

#include "launcher.h"
#include "renderer.h"
#include "color.h"
#include "log.h"


typedef struct LauncherContext {
    bool is_running;
    bool is_visible;

    unsigned int window_height;
    unsigned int window_width;
    GLFWwindow *window_handle;

    Color background_color;
} LauncherContext;

static LauncherContext launcher = {0};


#ifdef _WIN32
static HWND launcher_hwnd = NULL;
static WNDPROC original_wndproc = NULL;


#endif


static void launcher_update();
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);



#ifdef _WIN32

static LRESULT CALLBACK launcher_wndproc(
        HWND hwnd, 
        UINT msg, 
        WPARAM wParam, 
        LPARAM lParam)
{
    switch (msg) {
        case WM_HOTKEY:
            if (wParam == 1 && launcher.is_running) {
                if (launcher.is_visible) {
                    launcher.is_visible = false;
                    glfwHideWindow(launcher.window_handle);
                } else {
                    launcher.is_visible = true;
                    glfwShowWindow(launcher.window_handle);
                    glfwFocusWindow(launcher.window_handle);
                }

                return 0;
            }

            break;
    }

    return CallWindowProc(original_wndproc, hwnd, msg, wParam, lParam);

}

#endif


static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "[GLFW ERROR %d] %s\n", error, description);
}

void init_window() {
    launcher.window_width = 800;
    launcher.window_height = 600;
    
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        log_error("failed to initialize glfw\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    launcher.window_handle = glfwCreateWindow(
            launcher.window_width, 
            launcher.window_height, 
            "stunning spoon", 
            NULL, 
            NULL);

    if (!launcher.window_handle) {
        log_error("failed to create glfw window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(launcher.window_handle);
    BOOL useDarkMode = TRUE;

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &useDarkMode,
        sizeof(useDarkMode)
    );


    launcher_hwnd = hwnd;
    original_wndproc = (WNDPROC)SetWindowLongPtr(
            hwnd,
            GWLP_WNDPROC,
            (LONG_PTR)launcher_wndproc);

    if (!RegisterHotKey(hwnd, 1, MOD_CONTROL, VK_SPACE)) {
        log_error("failed to register global hotkey\n");
    }

#endif

    int monitor_count = 0;
    GLFWmonitor **monitors = glfwGetMonitors(&monitor_count);
    if (monitor_count == 0) {
        log_error("could not find any monitor holy fk buy a monitor dude\n");
        exit(EXIT_FAILURE);
    }

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    int monitor_x, monitor_y;
    int monitor_width, monitor_height;
    glfwGetMonitorWorkarea(monitor, &monitor_x, &monitor_y, &monitor_width, &monitor_height);

    int x = monitor_x + (monitor_width - launcher.window_width) / 2;
    int y = monitor_y + (monitor_height - launcher.window_height) / 2;
    glfwSetWindowPos(launcher.window_handle, x, y);


    glfwMakeContextCurrent(launcher.window_handle);
    glfwSwapInterval(1); // Enable V-Sync by default


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log_error("Failed to initialize GLAD OpenGL loader\n");
        exit(EXIT_FAILURE);
    }


    glfwSetKeyCallback(launcher.window_handle, key_callback);

}

bool window_should_close() {
	if (launcher.window_handle == NULL) return true;
	return glfwWindowShouldClose(launcher.window_handle);
}

void close_window() {
#ifdef _WIN32
    if (launcher_hwnd) {
        UnregisterHotKey(launcher_hwnd, 1);

        if (original_wndproc) {
            SetWindowLongPtr(
                launcher_hwnd,
                GWLP_WNDPROC,
                (LONG_PTR)original_wndproc
            );
        }

        launcher_hwnd = NULL;
        original_wndproc = NULL;
    }
#endif

    if (launcher.window_handle) {
        glfwDestroyWindow(launcher.window_handle);
        launcher.window_handle = NULL;
    }

    glfwTerminate();
}


static void key_callback(
        GLFWwindow *window,
        int key,
        int scancode,
        int action,
        int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        launcher.is_visible = false;
        glfwHideWindow(window);
    }
}


void window_swap_buffers() { 
    glfwSwapBuffers(launcher.window_handle); 
}

void window_poll_events() {
    glfwPollEvents(); 
}


void window_hide() {
    glfwHideWindow(launcher.window_handle);
}

void window_show() {
    glfwShowWindow(launcher.window_handle);
}

void window_focus() {
    glfwFocusWindow(launcher.window_handle);
}


void clear_screen() {
    clear_color(launcher.background_color);
}


void launcher_run() {
    init_window();
    renderer_init();

    launcher.background_color = hex_to_rgb("#111111");

    // daemon state
    launcher.is_running = true;
    launcher.is_visible = false;

    window_hide();

    while (launcher.is_running) {
        if (!launcher.is_visible) {
            glfwWaitEvents();
            continue;
        }

        launcher_update();
    }

    renderer_shutdown();
    close_window();
}


static void launcher_update() {
    window_poll_events();

    if (window_should_close()) {
        launcher.is_running = false;
        return;
    }

    clear_screen();
    window_swap_buffers();
}


