#include "../include/wayland/wl_handle_input.h"
#include "../include/app_state.h"
#include <xkbcommon/xkbcommon.h>
#include <unistd.h>
#include <sys/mman.h>

/* ===================== keyboard callbacks ===================== */

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                                   uint32_t format, int32_t fd, uint32_t size) {
    struct wl_handle_input *in = data;

    (void)keyboard;
    (void)format;
    void *map = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);

    in->xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    in->xkb_keymap =
        xkb_keymap_new_from_string(in->xkb_ctx, map, XKB_KEYMAP_FORMAT_TEXT_V1,
                                   XKB_KEYMAP_COMPILE_NO_FLAGS);

    in->xkb_state = xkb_state_new(in->xkb_keymap);

    munmap(map, size);
    close(fd);
}

// Sent when this surface gains keyboard focus.
// After this, key and modifier events become active.
static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface,
                                  struct wl_array *keys) {
    (void)data;
    (void)keyboard;
    (void)serial;
    (void)surface;
    (void)keys;
}

// Sent when this surface loses keyboard focus.
// Clients should clear pressed key state here.
static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface) {
    (void)data;
    (void)keyboard;
    (void)serial;
    (void)surface;
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                                uint32_t serial, uint32_t time, uint32_t key,
                                uint32_t state) {
    struct wl_handle_input *in = data;

    uint32_t xkb_key = key + 8;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(in->xkb_state, xkb_key);

    (void)keyboard;
    (void)serial;
    (void)time;
    (void)state;

    /* process sym here */
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                                      uint32_t serial, uint32_t mods_depressed,
                                      uint32_t mods_latched,
                                      uint32_t mods_locked, uint32_t group) {
    struct wl_handle_input *in = data;

    xkb_state_update_mask(in->xkb_state, mods_depressed, mods_latched,
                          mods_locked, 0, 0, group);

    (void)keyboard;
    (void)serial;
}

static void keyboard_handle_repeat(void *data, struct wl_keyboard *keyboard,
                                   int32_t rate, int32_t delay) {
    (void)data;
    (void)keyboard;
    (void)rate;
    (void)delay;
}

/* ===================== keyboard listener ===================== */

static const struct wl_keyboard_listener keyboard_listener = {
    .keymap = keyboard_handle_keymap,
    .enter = keyboard_handle_enter,
    .leave = keyboard_handle_leave,
    .key = keyboard_handle_key,
    .modifiers = keyboard_handle_modifiers,
    .repeat_info = keyboard_handle_repeat,
};

/* ===================== seat callbacks ===================== */

static void seat_handle_capabilities(void *data, struct wl_seat *seat,
                                     enum wl_seat_capability caps) {
    struct app_state *app = data;
    struct wl_handle_input *in = &app->input;

    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !in->keyboard) {
        in->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(in->keyboard, &keyboard_listener, in);
    }

    if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && in->keyboard) {
        wl_keyboard_destroy(in->keyboard);
        in->keyboard = NULL;
    }
}

static void seat_handle_name(void *data, struct wl_seat *seat,
                             const char *name) {
    (void)data;
    (void)seat;
    (void)name;
}

/* ===================== seat listener ===================== */

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
    .name = seat_handle_name,
};

/* ===================== setup ===================== */

void setup_input(struct app_state *app) {
    wl_seat_add_listener(app->seat, &seat_listener, app);
}
