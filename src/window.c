

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "window.h"
#include "log.h"


#define MAX_KEYS 512
#define CHAR_PRESSED_QUEUE_SIZE 32


typedef struct WindowContext {
    GLFWwindow *handle;
    int window_height;
    int window_width;

    int char_pressed_queue[CHAR_PRESSED_QUEUE_SIZE];
    int char_pressed_queue_count;

    // input handling
    bool keys_down[MAX_KEYS];
    bool keys_pressed[MAX_KEYS];
    bool keys_released[MAX_KEYS];
} WindowContext;

// global window context declared here !!!!
// this is available to window.c only
// for storing the state of the window
static WindowContext g_win_ctx = { 0 };


static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void char_callback(GLFWwindow *window, unsigned int codepoint);


void window_init(int width, int height, const char *window_title) {
    g_win_ctx.window_width = width;
    g_win_ctx.window_height = height;

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

    // for input handling
    glfwSetKeyCallback(g_win_ctx.handle, key_callback);
    glfwSetCharCallback(g_win_ctx.handle, char_callback);
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
    for (int i = 0; i < MAX_KEYS; i++) {
        g_win_ctx.keys_pressed[i] = false;
        g_win_ctx.keys_released[i] = false;
    }
    glfwPollEvents();
}


GLFWwindow* get_current_window() {
    return g_win_ctx.handle;
}

int get_window_height() {
    return g_win_ctx.window_height;
}


int get_window_width() {
    return g_win_ctx.window_width;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key < 0 || key >= MAX_KEYS) return;

    if (action == GLFW_PRESS) {
        g_win_ctx.keys_down[key] = true;
        g_win_ctx.keys_pressed[key] = true;
    }
    // GLFW_REPEAT gets the keyboard repeat events
    // so it is not necessary to manually configure them
    else if (action == GLFW_REPEAT) {
        // Key is still held.
        // Do NOT set keys_pressed again.
        g_win_ctx.keys_down[key] = true;
        g_win_ctx.keys_pressed[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        g_win_ctx.keys_down[key] = false;
        g_win_ctx.keys_released[key] = true;
    }
}



static void char_callback(GLFWwindow *window, unsigned int codepoint) {
    (void)window;
    if (g_win_ctx.char_pressed_queue_count < CHAR_PRESSED_QUEUE_SIZE) {
        g_win_ctx.char_pressed_queue[g_win_ctx.char_pressed_queue_count++] = (int)codepoint;
    }
}


bool is_key_pressed(int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    return g_win_ctx.keys_pressed[key];
}



bool is_key_down(int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    return g_win_ctx.keys_down[key];
}


bool is_key_released(int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    return g_win_ctx.keys_released[key];
}



int get_char_pressed(void) {
    if (g_win_ctx.char_pressed_queue_count <= 0) return 0;

    int value = g_win_ctx.char_pressed_queue[0];
    for (int i = 0; i < g_win_ctx.char_pressed_queue_count - 1; i++) {
        g_win_ctx.char_pressed_queue[i] = g_win_ctx.char_pressed_queue[i + 1];
    }

    g_win_ctx.char_pressed_queue[g_win_ctx.char_pressed_queue_count - 1] = 0;
    g_win_ctx.char_pressed_queue_count--;

    return value;
}


