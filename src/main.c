

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "color.h"
#include "launcher.h"
#include "window.h"
#include "log.h"
#include "config.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>





static void parse_args(int argc, char *argv[]) {
    if (argc > 1 && (strstr(argv[1], "--help") || strstr(argv[1], "-h"))) {
        printf("usage: ./build/app [-v | --version] [-h | --help] [-C <path>] [-c <name>=<value>]\n");
        printf("                   [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n");
        printf("                   [-p | --paginate | -P | --no-pager] [--no-replace-objects] [--no-lazy-fetch]\n");
        printf("                   [--no-optional-locks] [--no-advice] [--bare] [--git-dir=<path>]\n");
        printf("                   [--work-tree=<path>] [--namespace=<name>] [--config-env=<name>=<envvar>]\n");
        printf("                   <command> [<args>]\n");

        exit(EXIT_SUCCESS);


    } else if (argc > 1 && (strstr(argv[1], "--version") || strstr(argv[1], "-v"))) {
        printf("stunning-spoon version v1.0.0 (beta release)\n");
        exit(EXIT_SUCCESS);

    } else if (argc > 1) {
        printf("unknown option: %s\n", argv[1]);
        printf("usage: ./build/app [-v | --version] [-h | --help] [-C <path>] [-c <name>=<value>]\n");
        printf("                   [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n");
        printf("                   [-p | --paginate | -P | --no-pager] [--no-replace-objects] [--no-lazy-fetch]\n");
        printf("                   [--no-optional-locks] [--no-advice] [--bare] [--git-dir=<path>]\n");
        printf("                   [--work-tree=<path>] [--namespace=<name>] [--config-env=<name>=<envvar>]\n");
        printf("                   <command> [<args>]\n");


        exit(EXIT_FAILURE);

    }

}




static void execute_command(const char *command) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);

        // Only reached if execl fails
        perror("execl");
        _exit(EXIT_FAILURE);
    }
}



int main(int argc, char *argv[]) {
    parse_args(argc, argv);

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


    // load config file
    Config config;
    const char *home = getenv("HOME");
    char config_path[512];
    snprintf(config_path, sizeof(config_path), "%s/dev/projects/stunning-spoon/config_entries", home);


    if (config_load(&config, config_path) != 0) {
        log_error("Failed to load config_entries\n");
        return EXIT_FAILURE;
    }


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
    glfwSetWindowPos(launcher_win, x, y + 200);


    // launcher.items = items;
    // launcher.items_count = sizeof(items) / sizeof(items[0]);
    launcher.items = config.entries;
    launcher.items_count = config.entries_count;

    launcher_init(&launcher);
    // everything is initialized, opening the window...


    while (!launcher.should_close) {
        launcher_update(&launcher);
    }

    bool execute = launcher.should_execute;
    const char *command = NULL;
    if (execute &&
            launcher.items_count > 0 &&
            launcher.selected_index >= 0 &&
            launcher.selected_index < launcher.items_count) {

        command = launcher.result.items[launcher.selected_index]->command;
    }

    launcher_cleanup(&launcher);
    destroy_window();

    if (command) execute_command(command);
    return 0;
}



