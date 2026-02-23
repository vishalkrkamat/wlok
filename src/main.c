#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>

#include "../protocol/xdg-shell-client-protocol.h"
#include "../include/wayland/wl_xdg_handle.h"
#include "../include/wayland/wl_handle_input.h"
#include "../include/wayland/init_wayland.h"
#include "../include/wayland/render.h"

struct app_state app = {
    .running = 1,
};

// Wl Buffer
void wl_buffer_release(void *data, struct wl_buffer *buffer) {
    wl_buffer_destroy(buffer);
    struct shm_buffer *b = data;

    munmap(b->addr, b->size);
    close(b->fd);
    free(b);
    app.buffer_busy = 0;
}

struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
};

// wl callback
void wlbac(void *data, struct wl_callback *wl_callback,
           uint32_t callback_data) {

    (void)data;
    (void)callback_data;

    wl_callback_destroy(wl_callback);
    if (!app.buffer_busy) {
        submit_frame();
    }
}

struct wl_callback_listener frame_listener = {
    .done = wlbac,
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

    xdg_toplevel_destroy(app.win.xdg_top_level);
    xdg_surface_destroy(app.win.xdg_surface);
    wl_surface_destroy(app.surface);

    xdg_wm_base_destroy(app.xdg_wm_base);
    wl_compositor_destroy(app.comp);
    wl_shm_destroy(app.shm);

    wl_display_disconnect(display);

    return 0;
}
