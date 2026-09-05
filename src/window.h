


#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

void init_window(int width, int height, const char *window_title);
void close_window();
bool window_should_close();
void window_swap_buffers();
void window_poll_events();



#endif

