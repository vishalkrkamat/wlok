#include "../protocol/xdg-shell-client-protocol.h"
#include "../include/wayland/xdg_handle.h"
#include "../include/app_state.h"

extern int run;
//         Xdg Base           //

void xdg_wm_base_handle_listener(void *data, struct xdg_wm_base *xdg_wm_base,
                                 uint32_t serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_handle_listener,
};

// Function for xdg surface configure //
void xdg_surface_handle_configure(void *data, struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
    xdg_window.pending_serial = serial;
    xdg_window.got_configure = 1;
}

struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

// Xdg Top level //
void xdg_toplevel_handle_configure(void *data,
                                   struct xdg_toplevel *xdg_toplevel,
                                   int32_t width, int32_t height,
                                   struct wl_array *states) {
    if (width > 0 && height > 0) {
        xdg_window.win_width = width;
        xdg_window.win_height = height;
    }
}

void xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel) {
    run = 0;
}

struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_handle_configure,
    .close = xdg_toplevel_handle_close,
};

//         Setup            //

void setup_xdg_surface(struct app_state app, struct wl_surface *surface) {

    xdg_window.xdg_surface =
        xdg_wm_base_get_xdg_surface(app.xdg_wm_base, surface);
    xdg_window.xdg_top_level = xdg_surface_get_toplevel(xdg_window.xdg_surface);

    // Commit with no buffer
    wl_surface_commit(surface);

    xdg_surface_add_listener(xdg_window.xdg_surface, &xdg_surface_listener,
                             NULL);

    xdg_toplevel_add_listener(xdg_window.xdg_top_level, &xdg_toplevel_listener,
                              NULL);

    xdg_wm_base_add_listener(app.xdg_wm_base, &xdg_wm_base_listener, NULL);
}
