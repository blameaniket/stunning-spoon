

#ifndef WINDOW_H
#define WINDOW_H


#include <stdbool.h>
#include <GLFW/glfw3.h>


void window_init(int width, int height, const char *window_title);
bool window_should_close();
void destroy_window();
void window_swap_buffers();
void window_poll_events();
GLFWwindow* get_current_window();



#endif // WINDOW_H




