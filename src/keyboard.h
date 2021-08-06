#pragma once

#include <stdbool.h>
#include <stdio.h>

// In order to easy refer to the keycodes, we include this here
#include <linux/input-event-codes.h>

typedef struct {
    int backend_fd;
    int key[KEY_MAX];
} keyboard;

void kb_init_first_keyboard(keyboard *self);
void kb_free(keyboard *self);
void kb_update(keyboard *self);
bool kb_get(keyboard *self);
