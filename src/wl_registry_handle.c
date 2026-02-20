#include "../protocol/xdg-shell-client-protocol.h"
#include "../protocol/wlr-layer-shell-unstable-v1-client-protocol.h"
#include "../protocol/ext-session-lock-v1-client-protocol.h"
#include <wayland-client.h>
#include <string.h>
#include "../include/wayland/wl_registry_handler.h"
#include "../include/app_state.h"

void regis_list(void *data, struct wl_registry *wl_registry, uint32_t name,
                const char *interface, uint32_t version) {

    struct app_state *app = data;

    if (strcmp(interface, "wl_compositor") == 0) {
        app->comp =
            wl_registry_bind(wl_registry, name, &wl_compositor_interface, 6);
    }

    if (strcmp(interface, "wl_shm") == 0) {
        app->shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
    }

    if (strcmp(interface, "xdg_wm_base") == 0) {
        app->xdg_wm_base =
            wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
    }

    if (strcmp(interface, "wl_seat") == 0) {
        app->seat = wl_registry_bind(wl_registry, name, &wl_seat_interface, 9);
    }

    if (strcmp(interface, "zwlr_layer_shell_v1") == 0) {
        app->wlr_layer_shell = wl_registry_bind(
            wl_registry, name, &zwlr_layer_shell_v1_interface, 5);
    }

    if (strcmp(interface, "ext_session_lock_manager_v1") == 0) {
        app->ext_session_lock_manager = wl_registry_bind(
            wl_registry, name, &ext_session_lock_manager_v1_interface, 1);
    }

    printf("interface: '%s', version: %u, name: %u\n", interface, version,
           name);
}

void regis_remove(void *data, struct wl_registry *wl_registry, uint32_t name) {
    (void)data;
    (void)(wl_registry);
    (void)name;
}

struct wl_registry_listener registry_listener = {
    .global = regis_list,
    .global_remove = regis_remove,
};
