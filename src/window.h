


#ifndef WINDOW_H
#define WINDOW_H


#include <GLFW/glfw3.h>

void init_window();
bool window_should_close();
void close_window();
void window_swap_buffers();
void window_poll_events();
void window_hide();
void window_show();
void window_focus();
GLFWwindow *get_current_window();



#endif // WINDOW_H

