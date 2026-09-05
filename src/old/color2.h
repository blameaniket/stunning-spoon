


#ifndef COLOR_H
#define COLOR_H


typedef struct {
    float r, g, b;
    float alpha;
} Color;


Color hex_to_rgb(const char *hex);

#endif // COLOR_H
