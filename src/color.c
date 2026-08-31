


#include "color.h"
#include <stdio.h>

Color hex_to_rgb(const char *hex) {
    unsigned int value = 0;

    if (hex[0] == '#') {
        hex++;
    }


#ifdef _WIN32
    sscanf_s(hex, "%x", &value);
#else
    sscanf(hex, "%x", &value);
#endif

    Color c;
    c.r = ((value >> 16) & 0xFF) / 255.0f;
    c.g = ((value >> 8) & 0xFF) / 255.0f;
    c.b = (value & 0xFF) / 255.0f;
    c.alpha = 1.0;

    return c;
}


