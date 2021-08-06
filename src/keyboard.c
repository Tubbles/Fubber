#include "keyboard.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <unistd.h>

#include "str.h"

#include <linux/input.h>

void kb_init_first_keyboard(keyboard *self) {
    assert(self);
    // Open the backend file
    str devdir = str_lit("/dev/input/by-path");
    DIR *dir;
    assert(dir = opendir(str_ptr(devdir)));
    struct dirent *de;
    while ((de = readdir(dir)) != NULL) {
        if (strstr(de->d_name, "kbd") != NULL) {
            printf("%s\n", de->d_name);
            break;
        }
    }
    assert(de);
    str filename = str_null;
    str_cat(&filename, devdir, str_lit("/"), str_ref((char *)de->d_name));
    char *file = str_ptr(filename);
    assert(self->backend_fd = open(file, O_RDONLY));
    // if (errno) {
    //     printf("%s\n", strerror(errno));
    // }
    closedir(dir);
    str_free(devdir);
    str_free(filename);

    fcntl(self->backend_fd, F_SETFL,
          fcntl(self->backend_fd, F_GETFL, 0) | O_NONBLOCK);

    memset(self->key, 0, KEY_MAX);
}

void kb_free(keyboard *self) {
    assert(self);
    //
    close(self->backend_fd);
}

void kb_update(keyboard *self) {
    assert(self);
    int n;
    struct input_event ev;
    int i = 0;
    do {
        n = read(self->backend_fd, &ev, sizeof(ev));
        if ((n == (ssize_t)-1) && errno == EINTR) {
            continue;
        } else if (n != sizeof ev) {
            printf("wrong size or error: %i\n", n);
            printf("%s\n", strerror(errno));
            break;
        }
        i += 1;
    } while (errno != EWOULDBLOCK);
    printf("read %i times\n", i);
    fflush(stdout);
}
