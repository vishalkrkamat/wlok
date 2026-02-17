#ifndef APP_STATE
#define APP_STATE

#include <stdint.h>
#include <string.h>
#include <wayland-client.h>

struct xdg_window {
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_top_level;

    int32_t win_width;
    int32_t win_height;

    uint32_t pending_serial;
    int got_configure;
};

// Wayland globals
struct app_state {

    // wl global objects
    struct wl_shm *shm;
    struct xdg_wm_base *xdg_wm_base;
    struct wl_compositor *comp;

    int buffer_busy;
    int running;
    struct wl_surface *surface;
};

struct shm_buffer {
    uint8_t *addr;
    size_t size;
    int fd;
    struct wl_buffer *wl_buffer;
};

extern struct app_state app;
extern struct xdg_window xdg_window;

#endif // !APP_STATE
