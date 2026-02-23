#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>

#include "../protocol/xdg-shell-client-protocol.h"
#include "../include/wayland/wl_xdg_handle.h"
#include "../include/wayland/wl_handle_input.h"
#include "../include/wayland/init_wayland.h"

struct app_state app = {
    .running = 1,
};

int main() {

    struct wl_display *display = init_wayland(&app);

    if (!display) {
        fprintf(stderr, "Failed to Initialize wayland");
        abort();
    }

    fprintf(stderr, "Connection established!\n");

    if (!app.comp || !app.shm) {
        fprintf(stderr, "Missing required Wayland globals\n");
        abort();
    }

    app.surface = wl_compositor_create_surface(app.comp);

    // Setup Xdg_Window
    setup_xdg_surface(&app, app.surface);

    // Setup Keyboard Input
    setup_input(&app);

    while (app.running) {
        wl_display_dispatch(display);
    }

    wl_display_roundtrip(display);
    xdg_toplevel_destroy(app.win.xdg_top_level);
    xdg_surface_destroy(app.win.xdg_surface);
    wl_surface_destroy(app.surface);

    xdg_wm_base_destroy(app.xdg_wm_base);
    wl_compositor_destroy(app.comp);
    wl_shm_destroy(app.shm);

    wl_display_disconnect(display);

    return 0;
}
