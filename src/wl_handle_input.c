#include <wayland-client.h>
#include <stdio.h>
#include "../include/wayland/wl_handle_input.h"
#include "../include/app_state.h"
#include <xkbcommon/xkbcommon.h>
#include <unistd.h>

struct handle_keyboard_input keyb = {0};

// Input Handling of Keyboard
static void keymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format,
                   int32_t fd, uint32_t size) {
    printf("keymap received (fd=%d size=%u format=%u)\n", fd, size, format);
    close(fd); // IMPORTANT: compositor expects you to close it
}

// Sent when this surface gains keyboard focus.
// After this, key and modifier events become active.
static void enter(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
                  struct wl_surface *surface, struct wl_array *keys) {
    printf("keyboard focus entered surface\n");
}

// Sent when this surface loses keyboard focus.
// Clients should clear pressed key state here.
static void leave(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
                  struct wl_surface *surface) {

    // printf("keyboard focus left surface\n");
}

static void key(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
                uint32_t time, uint32_t key, uint32_t state) {

    // printf("key event: key=%u state=%u\n", key, state);
}

static void modifiers(void *data, struct wl_keyboard *wl_keyboard,
                      uint32_t serial, uint32_t mods_depressed,
                      uint32_t mods_latched, uint32_t mods_locked,
                      uint32_t group) {
    // printf("modifiers changed\n");
}

static void repeat_info(void *data, struct wl_keyboard *wl_keyboard,
                        int32_t rate, int32_t delay) {
    // printf("repeat rate=%d delay=%dms\n", rate, delay);
}

//

const struct wl_keyboard_listener key_listener = {
    .keymap = keymap,
    .enter = enter,
    .leave = leave,
    .key = key,
    .modifiers = modifiers,
    .repeat_info = repeat_info,
};

//       Wl seat       //
static void handle_capabilities(void *data, struct wl_seat *seat,
                                enum wl_seat_capability caps) {
    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {

        keyb.keyboard = wl_seat_get_keyboard(app.seat);

        wl_keyboard_add_listener(keyb.keyboard, &key_listener, NULL);
    }
};

static void name(void *data, struct wl_seat *seat, const char *name) {}

static struct wl_seat_listener wl_seat_listener = {
    .capabilities = handle_capabilities,
    .name = name,
};

void setup_input(struct wl_seat *seat) {
    wl_seat_add_listener(seat, &wl_seat_listener, NULL);
}
