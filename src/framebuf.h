#pragma once

#include <stddef.h>
#include <stdint.h>

#include <linux/fb.h>

struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct color cl_from(uint32_t rgba);
struct color cl_from_rgb(uint8_t r, uint8_t g, uint8_t b);
struct color cl_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

struct framebuf {
    void *fb;
    struct fb_var_screeninfo screen_info;
    size_t bytes_per_pixel;
    size_t len; // in bytes
    size_t num_pixels;
    size_t screen_width;
    uint32_t r_mask;
    uint32_t g_mask;
    uint32_t b_mask;
    uint32_t a_mask;
    int r_shift;
    int g_shift;
    int b_shift;
    int a_shift;
    int r_shift_down;
    int g_shift_down;
    int b_shift_down;
    int a_shift_down;
    void (*set_pixel)(struct framebuf *self, size_t index,
                      uint32_t packed_color);
};

void fb_init(struct framebuf *self, struct fb_var_screeninfo *info,
             uint32_t *fb);

uint32_t fb_pack_color(struct framebuf *self, struct color color);

void fb_clear(struct framebuf *self, struct color color);

void fb_set_pixel(struct framebuf *self, size_t x, size_t y,
                  struct color color);

void fb_save_bitmap(struct framebuf *self, uint32_t *dest);

void fb_load_bitmap(struct framebuf *self, uint32_t *src);
