#ifndef WL_REGISTRY_H
#define WL_REGISTRY_H

#include <stdio.h>
#include <wayland-client.h>
#include <stdint.h>

extern struct wl_registry_listener registry_listener;

void regis_list(void *data, struct wl_registry *wl_registry, uint32_t name,
                const char *interface, uint32_t version);

void regis_remove(void *data, struct wl_registry *wl_registry, uint32_t name);

#endif // !WL_REGISTRY_H
