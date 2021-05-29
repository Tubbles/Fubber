#include "framebuf.h"

#include <string.h>
#include <tgmath.h>

void fb_init(struct framebuf *self, struct fb_var_screeninfo *info,
             uint16_t *fb) {
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

void fb_set_pixel(struct framebuf *self, size_t x, size_t y,
                  uint32_t rgba) {
    uint8_t r = rgba >> 24 & 0xFF;
    uint8_t g = rgba >> 16 & 0xFF;
    uint8_t b = rgba >> 8 & 0xFF;
    uint8_t a = rgba >> 0 & 0xFF;
    // printf("b: 0x%X\n", b);

    uint16_t set = ((r >> self->r_shift_down) & self->r_mask)
                   << self->r_shift;
    set |= ((g >> self->g_shift_down) & self->g_mask) << self->g_shift;
    set |= ((b >> self->b_shift_down) & self->b_mask) << self->b_shift;
    set |= ((a >> self->a_shift_down) & self->a_mask) << self->a_shift;

    // printf("b_shift_down: %i\n", b_shift_down);
    // printf("b_mask: 0x%X\n", b_mask);
    // printf("b_shift: %i\n", b_shift);
    // printf("setting to 0x%X\n", set);
    self->fb[y * self->screen_width + x] = set;
}

void fb_set_pixel_rgb(struct framebuf *self, size_t x, size_t y, uint32_t r,
                      uint32_t g, uint32_t b) {
    fb_set_pixel(self, x, y, r << 24 | g << 16 | b << 8 | 0xFF);
}

void fb_set_pixel_rgba(struct framebuf *self, size_t x, size_t y,
                       uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    fb_set_pixel(self, x, y, r << 24 | g << 16 | b << 8 | a);
}

void fb_save_bitmap(struct framebuf *self, uint16_t *dest) {
    memcpy(dest, self->fb, self->len);
}

void fb_load_bitmap(struct framebuf *self, uint16_t *src) {
    memcpy(self->fb, src, self->len);
}
