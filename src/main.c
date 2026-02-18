#define _GNU_SOURCE
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "../protocol/xdg-shell-client-protocol.h"
#include "../include/wayland/wl_registry_handler.h"
#include "../include/wayland/wl_xdg_handle.h"
#include "../include/wayland/wl_handle_input.h"

struct app_state app = {
    .running = 1,
};

struct xdg_window xdg_window = {0};

struct shm_buffer *draw_frame(int width, int height);
void draw_and_commit();

// Wl Buffer

void wl_buffer_release(void *data, struct wl_buffer *buffer) {
    wl_buffer_destroy(buffer);
    struct shm_buffer *b = data;

    munmap(b->addr, b->size);
    close(b->fd);
    free(b);
};

struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
};

// wl callback
void wlbac(void *data, struct wl_callback *wl_callback,
           uint32_t callback_data) {

    wl_callback_destroy(wl_callback);
    if (!app.buffer_busy) {
        draw_and_commit();
    }
}

struct wl_callback_listener frame_listener = {
    .done = wlbac,
};

int main() {

    struct wl_display *display = wl_display_connect(NULL);
    struct wl_registry *regis = wl_display_get_registry(display);
    wl_registry_add_listener(regis, &registry_listener, &app);

    wl_display_roundtrip(display);

    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return 1;
    }

    fprintf(stderr, "Connection established!\n");

    if (app.comp && app.shm) {
        printf("Found them\n");
    }

    app.surface = wl_compositor_create_surface(app.comp);
    setup_xdg_surface(app, app.surface);
    // Setup Keyboard Input

    setup_input(app.seat);

    while (app.running) {
        wl_display_dispatch(display);
        if (xdg_window.got_configure && xdg_window.win_width > 0 &&
            xdg_window.win_height > 0) {

            xdg_surface_ack_configure(xdg_window.xdg_surface,
                                      xdg_window.pending_serial);
            draw_and_commit();
            xdg_window.got_configure = 0;
        }
    }

    xdg_toplevel_destroy(xdg_window.xdg_top_level);
    xdg_surface_destroy(xdg_window.xdg_surface);
    wl_surface_destroy(app.surface);

    wl_registry_destroy(regis);
    xdg_wm_base_destroy(app.xdg_wm_base);
    wl_compositor_destroy(app.comp);
    wl_shm_destroy(app.shm);

    wl_display_disconnect(display);

    return 0;
}

// Create a new SHM buffer and draw a frame into it.
// The returned buffer is submitted to the compositor and will be freed
// asynchronously when wl_buffer.release is received.
struct shm_buffer *draw_frame(int width, int height) {

    // struct buffer_data *buf;
    struct shm_buffer *buf = malloc(sizeof *buf);
    int stride = width * 4;
    buf->size = stride * height;

    // Memory from the ram
    buf->fd = memfd_create("wayland-buffer", MFD_CLOEXEC);
    ftruncate(buf->fd, buf->size);

    // Make address out of the memory we got from ram so using its address to
    // perform actions on the address
    buf->addr =
        mmap(NULL, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED, buf->fd, 0);

    struct wl_shm_pool *pool = wl_shm_create_pool(app.shm, buf->fd, buf->size);

    buf->wl_buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride,
                                               WL_SHM_FORMAT_XRGB8888);

    wl_shm_pool_destroy(pool);
    wl_buffer_add_listener(buf->wl_buffer, &wl_buffer_listener, buf);

    // memset(addr, 0xff, size);
    /* ===== DRAW HERE ===== */

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            int o = y * stride + x * 4;
            buf->addr[o + 0] = 0;
            buf->addr[o + 1] = 0;
            buf->addr[o + 2] = 255;
            buf->addr[o + 3] = 0;
        }

    /* ==================== */

    return buf;
}

void draw_and_commit() {
    struct wl_callback *cb = wl_surface_frame(app.surface);
    wl_callback_add_listener(cb, &frame_listener, NULL);

    struct shm_buffer *buf =
        draw_frame(xdg_window.win_width, xdg_window.win_height);

    wl_surface_attach(app.surface, buf->wl_buffer, 0, 0);
    wl_surface_commit(app.surface);
    app.buffer_busy = 1;
}
