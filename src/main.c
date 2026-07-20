
#include "raylib.h"
#include "windows.h"


int main() {
    launcher_init();
    while (!window_should_close()) {
        launcher_update();
    }

    launcher_cleanup();
    return 0;
}
