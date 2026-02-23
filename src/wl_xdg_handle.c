#include "../protocol/xdg-shell-client-protocol.h"
#include "../include/wayland/wl_xdg_handle.h"
#include "../include/app_state.h"
#include "../include/wayland/render.h"

/* =========================
   xdg_wm_base (ping/pong)
   ========================= */

static void xdg_wm_base_handle_ping(void *data, struct xdg_wm_base *xdg_wm_base,
                                    uint32_t serial) {
    (void)data;
    xdg_wm_base_pong(xdg_wm_base, serial);
}

struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_handle_ping,
};

/* =========================
   xdg_surface
   ========================= */

static void xdg_surface_handle_configure(void *data,
                                         struct xdg_surface *xdg_surface,
                                         uint32_t serial) {
    struct app_state *state = data;
    (void)xdg_surface;

    state->win.pending_serial = serial;
    state->win.got_configure = 1;

    xdg_surface_ack_configure(xdg_surface, serial);
    submit_frame();
}

struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

/* =========================
   xdg_toplevel
   ========================= */

static void xdg_toplevel_handle_configure(void *data,
                                          struct xdg_toplevel *xdg_toplevel,
                                          int32_t width, int32_t height,
                                          struct wl_array *states) {

    struct app_state *state = data;
    (void)xdg_toplevel;
    (void)states;

    if (width > 0 && height > 0) {
        state->win.win_width = width;
        state->win.win_height = height;
    }
}

static void xdg_toplevel_handle_close(void *data,
                                      struct xdg_toplevel *xdg_toplevel) {

    struct app_state *state = data;
    (void)xdg_toplevel;

    state->running = 0;
}

struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_handle_configure,
    .close = xdg_toplevel_handle_close,
};

/* =========================
   Setup
   ========================= */

void setup_xdg_surface(struct app_state *app, struct wl_surface *surface) {
    app->win.xdg_surface =
        xdg_wm_base_get_xdg_surface(app->xdg_wm_base, surface);

    app->win.xdg_top_level = xdg_surface_get_toplevel(app->win.xdg_surface);

    /* Initial commit (required by protocol) */
    wl_surface_commit(surface);

    xdg_surface_add_listener(app->win.xdg_surface, &xdg_surface_listener, app);

    xdg_toplevel_add_listener(app->win.xdg_top_level, &xdg_toplevel_listener,
                              app);

    xdg_wm_base_add_listener(app->xdg_wm_base, &xdg_wm_base_listener, app);
}
