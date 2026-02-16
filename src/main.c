#define _GNU_SOURCE
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "../protocol/xdg-shell-client-protocol.h"
#include "../include/wayland/wl_registry_handler.h"
#include "../include/wayland/xdg_handle.h"

struct wl_buffer *bufr;

struct app_state app = {0};
struct xdg_window xdg_window;

struct wl_surface *surface = NULL;
int run = 1;
struct wl_buffer *draw_frame(int width, int height);

// Wl Buffer

void wl_buffer_release(void *data, struct wl_buffer *buffer) {
    wl_buffer_destroy(buffer);
};

struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
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

    surface = wl_compositor_create_surface(app.comp);
    setup_xdg_surface(app, surface);

    while (run) {
        wl_display_dispatch(display);

        if (xdg_window.got_configure && xdg_window.win_width > 0 &&
            xdg_window.win_height > 0) {

            xdg_surface_ack_configure(xdg_window.xdg_surface,
                                      xdg_window.pending_serial);

            bufr = draw_frame(xdg_window.win_width, xdg_window.win_height);

            wl_surface_attach(surface, bufr, 0, 0);
            wl_surface_commit(surface);

            xdg_window.got_configure = 0;
        }
    }

    xdg_toplevel_destroy(xdg_window.xdg_top_level);
    xdg_surface_destroy(xdg_window.xdg_surface);
    wl_surface_destroy(surface);

    wl_registry_destroy(regis);
    xdg_wm_base_destroy(app.xdg_wm_base);
    wl_compositor_destroy(app.comp);
    wl_shm_destroy(app.shm);

    wl_display_disconnect(display);

    return 0;
}

struct wl_buffer *draw_frame(int width, int height) {
    int stride = width * 4;
    size_t size = stride * height;

    // Memory from the ram
    int fd = memfd_create("wayland-buffer", MFD_CLOEXEC);
    ftruncate(fd, size);

    // Make address out of the memory we got from ram so using its address to
    // perform actions on the address
    uint8_t *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct wl_shm_pool *pool = wl_shm_create_pool(app.shm, fd, size);

    bufr = wl_shm_pool_create_buffer(pool, 0, width, height, stride,
                                     WL_SHM_FORMAT_XRGB8888);

    wl_shm_pool_destroy(pool);
    wl_buffer_add_listener(bufr, &wl_buffer_listener, NULL);

    // memset(addr, 0xff, size);
    /* ===== DRAW HERE ===== */

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            int o = y * stride + x * 4;
            addr[o + 0] = 0;
            addr[o + 1] = 0;
            addr[o + 2] = 255;
            addr[o + 3] = 0;
        }

    /* ==================== */

    return bufr;
}
