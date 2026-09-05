

#include "launcher.h"
#include "window.h"

void launcher_run() {
    init_window(800, 600, "stunning spoon launcher");
    while (!window_should_close()) {
        window_poll_events();
        window_swap_buffers();
    }
    close_window();
}



