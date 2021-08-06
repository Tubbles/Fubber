// This is the beginning of a linux framebuffer based graphics library :)

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>

#include "framebuf.h"
#include "keyboard.h"
#include "str.h"

static volatile bool running = true;

static struct framebuf fbuf;
static keyboard kb;

void ctrlc(int _) {
    (void)_;
    running = false;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    kb_init_first_keyboard(&kb);

    // Catch interrupt
    signal(SIGINT, ctrlc);

    // Hide terminal cursor
    printf("\033[?25l");
    fflush(stdout);

    // Turn off terminal echo
    struct termios term_info, term_info_save;
    assert(0 == tcgetattr(0, &term_info));
    term_info_save = term_info;
    term_info.c_lflag &= ~ECHO;
    assert(0 == tcsetattr(0, TCSADRAIN, &term_info));

    while (true) {
        kb_update(&kb);
        sleep(1);
    }

    // // Open the frame buffer
    // struct fb_var_screeninfo info;
    // int fbfd = open("/dev/fb0", O_RDWR);
    // assert(fbfd > 0);
    // assert(0 == ioctl(fbfd, FBIOGET_VSCREENINFO, &info));

    // size_t len = info.bits_per_pixel / 8 * info.xres * info.yres;
    // uint32_t *fb =
    //     mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    // fb_init(&fbuf, &info, fb);
    // uint32_t *start_screen = malloc(sizeof(*fb) * len);
    // fb_save_bitmap(&fbuf, start_screen);

    // while (running) {
    //     fb_clear(&fbuf, cl_from_rgb(0xFF, 0xAA, 0x33));
    // }

    // fb_load_bitmap(&fbuf, start_screen); // Reset the old frame buffer
    // state free(start_screen); start_screen = NULL; close(fbfd);

    // Reset the terminal
    assert(0 == tcsetattr(0, TCSADRAIN, &term_info_save));
    printf("\033[?25h"); // show cursor
    fflush(stdout);

    return 0;
}
