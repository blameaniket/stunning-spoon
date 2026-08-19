

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "color.h"
#include "launcher.h"
#include "window.h"
#include "log.h"



static const char *items[] = {
    "firefox",
    "spotify",
    "discord",
    "steam",
    "poweroff",
    "reboot",
};


int main(int argc, char *argv[]) {
    struct Launcher launcher = {
        .window = {
            .width = 800,
            .height = 450,
            .title = "stunning-spoon launcher",
            .background_color = hex_to_rgb("#282828"),
            .foreground_color = hex_to_rgb("#ebdbb2"),
            .selected_item_color = hex_to_rgb("#ff2c2c"),
            .font_family = "assets/fonts/IosevkaTermSlab_nerdfont/IosevkaTermSlabNerdFont-Regular.ttf",
            .prompt = "run:",
        },
    };


    // initialize a glfw window
    window_init(launcher.window.width, launcher.window.height, launcher.window.title);

    // center the window directly from main
    GLFWwindow *launcher_win = get_current_window();
    if (!launcher_win) {
        log_error("cannot find launcher window\n");
        exit(EXIT_FAILURE);
    };

    int monitor_count = 0;
    GLFWmonitor **monitors = glfwGetMonitors(&monitor_count);
    if (monitor_count == 0) {
        log_error("could not find any monitor\n");
        log_error("buy a monitor dude\n");
        exit(EXIT_FAILURE);
    };

    // for now use the primary monitor.
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    int monitor_x, monitor_y;
    int monitor_width, monitor_height;

    glfwGetMonitorWorkarea(monitor, 
            &monitor_x, &monitor_y, 
            &monitor_width, &monitor_height);

    int window_width, window_height;
    glfwGetWindowSize(launcher_win, &window_width, &window_height);

    // launcher position here
    // change it however you like
    // currently it is a bit higher than center
    int x = monitor_x + (monitor_width - window_width) / 2;
    int y = monitor_y + (monitor_height - window_height) / 2;
    glfwSetWindowPos(launcher_win, x, y);


    launcher.items = items;
    launcher.items_count = sizeof(items) / sizeof(items[0]);

    launcher_init(&launcher);
    // everything is initialized, opening the window...


    while (!launcher.should_close) {
        launcher_update(&launcher);
    }

    // window is closed performing cleanup...
    launcher_cleanup(&launcher);
    destroy_window();
    return 0;
}



