#include "../include/wayland/wl_registry_handler.h"
#include "../include/app_state.h"
#include <stdio.h>

struct wl_display *init_wayland(struct app_state *app) {

    struct wl_display *display = wl_display_connect(NULL);

    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return NULL;
    }

    struct wl_registry *regis = wl_display_get_registry(display);
    wl_registry_add_listener(regis, &registry_listener, app);

    wl_display_roundtrip(display);

    wl_registry_destroy(regis);
    return display;
}
