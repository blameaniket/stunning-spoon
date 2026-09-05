


#ifndef COLOR_H
#define COLOR_H


typedef struct Color {
    float r, g, b, alpha;
} Color;

Color hex_to_rgb(const char *hex);


#endif // COLOR_H

