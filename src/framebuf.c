#include "framebuf.h"

#include <assert.h>
#include <string.h>
#include <tgmath.h>

#include <stdio.h>

struct color cl_from(uint32_t rgba) {
    struct color ret;
    ret.r = rgba >> 24 & 0xFF;
    ret.g = rgba >> 16 & 0xFF;
    ret.b = rgba >> 8 & 0xFF;
    ret.a = rgba & 0xFF;
    return ret;
}

struct color cl_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    struct color ret;
    ret.r = r;
    ret.g = g;
    ret.b = b;
    ret.a = 0xFF;
    return ret;
}

struct color cl_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    struct color ret;
    ret.r = r;
    ret.g = g;
    ret.b = b;
    ret.a = a;
    return ret;
}

void fb_init(struct framebuf *self, struct fb_var_screeninfo *info,
             uint16_t *fb) {
    assert(self && info && fb);
    self->fb = fb;
    self->screen_info = *info;
    self->len = info->bits_per_pixel / 8 * info->xres * info->yres;
    self->screen_width = info->xres;
    self->r_mask = pow(2, info->red.length) - 1;
    self->g_mask = pow(2, info->green.length) - 1;
    self->b_mask = pow(2, info->blue.length) - 1;
    self->a_mask = pow(2, info->transp.length) - 1;
    self->r_shift = info->red.offset;
    self->g_shift = info->green.offset;
    self->b_shift = info->blue.offset;
    self->a_shift = info->transp.offset;
    self->r_shift_down = 8 - info->red.length;
    self->g_shift_down = 8 - info->green.length;
    self->b_shift_down = 8 - info->blue.length;
    self->a_shift_down = 8 - info->transp.length;
}

uint16_t fb_pack_color(struct framebuf *self, struct color color) {
    assert(self);
    uint8_t r = color.r;
    uint8_t g = color.g;
    uint8_t b = color.b;
    uint8_t a = color.a;

    uint16_t set = ((r >> self->r_shift_down) & self->r_mask)
                   << self->r_shift;
    set |= ((g >> self->g_shift_down) & self->g_mask) << self->g_shift;
    set |= ((b >> self->b_shift_down) & self->b_mask) << self->b_shift;
    set |= ((a >> self->a_shift_down) & self->a_mask) << self->a_shift;
    return set;
}

void fb_clear(struct framebuf *self, struct color color) {
    assert(self);
    uint16_t packed_color = fb_pack_color(self, color);
    for (size_t i = 0; i < self->len / 2; ++i) {
        self->fb[i] = packed_color;
    }
}

void fb_set_pixel(struct framebuf *self, size_t x, size_t y,
                  struct color color) {
    assert(self);
    self->fb[y * self->screen_width + x] = fb_pack_color(self, color);
}

void fb_save_bitmap(struct framebuf *self, uint16_t *dest) {
    assert(self && dest);
    memcpy(dest, self->fb, self->len);
}

void fb_load_bitmap(struct framebuf *self, uint16_t *src) {
    assert(self && src);
    memcpy(self->fb, src, self->len);
}
