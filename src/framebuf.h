#pragma once

#include <stdint.h>
#include <stddef.h>

#include <linux/fb.h>

struct framebuf {
    uint16_t *fb;
    struct fb_var_screeninfo screen_info;
    size_t len;
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
};

void fb_init(struct framebuf *self, struct fb_var_screeninfo *info,
             uint16_t *fb);

void fb_set_pixel(struct framebuf *self, size_t x, size_t y, uint32_t rgba);

void fb_set_pixel_rgb(struct framebuf *self, size_t x, size_t y, uint32_t r,
                      uint32_t g, uint32_t b);

void fb_set_pixel_rgba(struct framebuf *self, size_t x, size_t y,
                       uint32_t r, uint32_t g, uint32_t b, uint32_t a);

void fb_save_bitmap(struct framebuf *self, uint16_t *dest);

void fb_load_bitmap(struct framebuf *self, uint16_t *src);
