#ifndef XDG_HANDLE
#include "../app_state.h"

extern struct xdg_wm_base_listener xdg_wm_base_listener;
extern struct xdg_surface_listener xdg_surface_listener;
extern struct xdg_toplevel_listener xdg_toplevel_listener;

void setup_xdg_surface(struct app_state app, struct wl_surface *surface);

#endif // !XDG_HANDLE
