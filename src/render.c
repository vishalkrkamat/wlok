#define _GNU_SOURCE
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "../include/app_state.h"
#include <stdio.h>

extern struct app_state app;
extern struct wl_buffer_listener wl_buffer_listener;
extern struct wl_callback_listener frame_listener;

struct shm_buffer *render_frame_buffer(int width, int height);
void submit_frame();

// Create a new SHM buffer and draw a frame into it.
// The returned buffer is submitted to the compositor and will be freed
// asynchronously when wl_buffer.release is received.
struct shm_buffer *render_frame_buffer(int width, int height) {

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
    fprintf(stderr, "Are you running\n");
    return buf;
}

void submit_frame() {
    struct wl_callback *cb = wl_surface_frame(app.surface);
    wl_callback_add_listener(cb, &frame_listener, NULL);

    struct shm_buffer *buf =
        render_frame_buffer(app.win.win_width, app.win.win_height);

    wl_surface_attach(app.surface, buf->wl_buffer, 0, 0);
    wl_surface_commit(app.surface);
    app.buffer_busy = 1;
}
