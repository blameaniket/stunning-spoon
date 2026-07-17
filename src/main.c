
#include "raylib.h"
#include "launcher.h"


int main() {
    launcher_init();
    while (!window_should_close()) {
        launcher_update();
    }

    cleanup_remaining_bits();
    CloseWindow();
    return 0;
}


