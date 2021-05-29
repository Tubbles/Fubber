// This is the beginning of a linux framebuffer based graphics library :)

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

#include <fcntl.h>
#include <linux/fb.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>

static volatile bool running = true;

struct framebuf {
    uint16_t *fb;
    struct fb_var_screeninfo screen_info;
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

static struct framebuf fbuf;

void ctrlc(int _) {
    (void)_;
    running = false;
}

void init_framebuf(struct framebuf *self, struct fb_var_screeninfo *info,
                   uint16_t *fb) {
    self->fb = fb;
    self->screen_info = *info;
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

void set_pixel(struct framebuf *fb, size_t x, size_t y, uint32_t rgba) {
    uint8_t r = rgba >> 24 & 0xFF;
    uint8_t g = rgba >> 16 & 0xFF;
    uint8_t b = rgba >> 8 & 0xFF;
    uint8_t a = rgba >> 0 & 0xFF;
    // printf("b: 0x%X\n", b);

    uint16_t set = ((r >> fb->r_shift_down) & fb->r_mask) << fb->r_shift;
    set |= ((g >> fb->g_shift_down) & fb->g_mask) << fb->g_shift;
    set |= ((b >> fb->b_shift_down) & fb->b_mask) << fb->b_shift;
    set |= ((a >> fb->a_shift_down) & fb->a_mask) << fb->a_shift;

    // printf("b_shift_down: %i\n", b_shift_down);
    // printf("b_mask: 0x%X\n", b_mask);
    // printf("b_shift: %i\n", b_shift);
    // printf("setting to 0x%X\n", set);
    fb->fb[y * fb->screen_width + x] = set;
}

void set_pixel_rgb(struct framebuf *fb, size_t x, size_t y, uint32_t r,
                   uint32_t g, uint32_t b) {
    set_pixel(fb, x, y, r << 24 | g << 16 | b << 8 | 0xFF);
}

void set_pixel_rgba(struct framebuf *fb, size_t x, size_t y, uint32_t r,
                    uint32_t g, uint32_t b, uint32_t a) {
    set_pixel(fb, x, y, r << 24 | g << 16 | b << 8 | a);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Hide terminal cursor
    printf("\033[?25l");
    fflush(stdout);
    signal(SIGINT, ctrlc);

    // Turn off terminal echo
    struct termios term_info, term_info_save;
    assert(0 == tcgetattr(0, &term_info));
    term_info_save = term_info;
    term_info.c_lflag &= ~ECHO;
    assert(0 == tcsetattr(0, TCSADRAIN, &term_info));

    // Open the frame buffer
    struct fb_var_screeninfo screen_info;
    int fbfd = open("/dev/fb0", O_RDWR);
    assert(fbfd > 0);
    assert(0 == ioctl(fbfd, FBIOGET_VSCREENINFO, &screen_info));
    size_t len = screen_info.bits_per_pixel / 8 * screen_info.xres *
                 screen_info.yres;

    uint16_t *fb =
        mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    init_framebuf(&fbuf, &screen_info, fb);

    while (running) {
        for (uint64_t x = 0; x < screen_info.xres; ++x) {
            for (uint64_t y = 0; y < screen_info.yres; ++y) {
                set_pixel_rgb(&fbuf, x, y, 0xFF, 0xAA, 0x33);
            }
        }
    }

    close(fbfd);
    assert(0 == tcsetattr(0, TCSADRAIN, &term_info_save));
    printf("\033[?25h"); // show cursor
    fflush(stdout);
    // printf("\014"); // clear screen
    // fflush(stdout);
    system("clear");

    return 0;
}
