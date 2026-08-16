

#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

#include "color.h"
#include "launcher.h"
#include "window.h"
#include "log.h"



int main(int argc, char *argv[]) {
    log_debug("stunning spoon\n");
    log_debug("opengl launcher and dmenu replacement\n");
    log_debug("-------------------------------------\n\n");

    struct Launcher launcher = {
        .window = {
            .width = 800,
            .height = 600,
            .title = "stunning spoon",
            .background_color = hex_to_rgb("#282828"),
        },
    };


    if (argc > 1 && (strstr(argv[1], "--help") || strstr(argv[1], "-h"))) {
        printf("usage: ./build/app [-v | --version] [-h | --help] [-C <path>] [-c <name>=<value>]\n");
        printf("                   [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n");
        printf("                   [-p | --paginate | -P | --no-pager] [--no-replace-objects] [--no-lazy-fetch]\n");
        printf("                   [--no-optional-locks] [--no-advice] [--bare] [--git-dir=<path>]\n");
        printf("                   [--work-tree=<path>] [--namespace=<name>] [--config-env=<name>=<envvar>]\n");
        printf("                   <command> [<args>]\n");


    } else if (argc > 1 && (strstr(argv[1], "--version") || strstr(argv[1], "-v"))) {
        printf("stunning-spoon version 0.1 (beta release)\n");

    } else if (argc > 1) {
        printf("unknown option: %s\n", argv[1]);
        printf("usage: ./build/app [-v | --version] [-h | --help] [-C <path>] [-c <name>=<value>]\n");
        printf("                   [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n");
        printf("                   [-p | --paginate | -P | --no-pager] [--no-replace-objects] [--no-lazy-fetch]\n");
        printf("                   [--no-optional-locks] [--no-advice] [--bare] [--git-dir=<path>]\n");
        printf("                   [--work-tree=<path>] [--namespace=<name>] [--config-env=<name>=<envvar>]\n");
        printf("                   <command> [<args>]\n");

    }

    log_debug("initializing window...\n");
    window_init(launcher.window.width, launcher.window.height, launcher.window.title);
    launcher_init(&launcher);

    log_debug("window is initialized\n\n");


    log_debug("opening window ...\n\n");
    while (!window_should_close()) {
        launcher_update();
    }


    log_debug("window closed, performing cleanup ...\n");
    destroy_window();

    log_debug("finished cleanup, exiting ...\n");


    return 0;
}



