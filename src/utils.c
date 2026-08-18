
#include <string.h>
#include <stdio.h>

static void parse_args(int argc, char *argv[]) {
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

}
