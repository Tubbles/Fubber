// This is the beginning of a linux framebuffer based graphics library :)

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>

#include "framebuf.h"

static volatile bool running = true;

static struct framebuf fbuf;

void ctrlc(int _) {
    (void)_;
    running = false;
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
    struct fb_var_screeninfo info;
    int fbfd = open("/dev/fb0", O_RDWR);
    assert(fbfd > 0);
    assert(0 == ioctl(fbfd, FBIOGET_VSCREENINFO, &info));

    size_t len = info.bits_per_pixel / 8 * info.xres * info.yres;
    uint16_t *fb =
        mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    init_framebuf(&fbuf, &info, fb);
    uint16_t *start_screen = malloc(sizeof(*fb) * len);
    print_screen(&fbuf, start_screen);

    while (running) {
        for (uint64_t x = 0; x < info.xres; ++x) {
            for (uint64_t y = 0; y < info.yres; ++y) {
                set_pixel_rgb(&fbuf, x, y, 0xFF, 0xAA, 0x33);
            }
        }
    }

    load_bitmap(&fbuf, start_screen); // Reset the old frame buffer state
    close(fbfd);
    assert(0 == tcsetattr(0, TCSADRAIN, &term_info_save));
    printf("\033[?25h"); // show cursor
    fflush(stdout);

    return 0;
}
