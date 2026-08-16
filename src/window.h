

#ifndef WINDOW_H
#define WINDOW_H


#include <stdbool.h>


void window_init(int width, int height, const char *window_title);
bool window_should_close();
void destroy_window();
void window_swap_buffers();
void window_poll_events();



#endif // WINDOW_H




