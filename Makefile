SRC_DIR := src
BUILD_DIR := build
CC := gcc

# ---- pkg-config ----
WAYLAND_CFLAGS := $(shell pkg-config --cflags wayland-client)
WAYLAND_LIBS   := $(shell pkg-config --libs wayland-client)

XKB_CFLAGS := $(shell pkg-config --cflags xkbcommon)
XKB_LIBS   := $(shell pkg-config --libs xkbcommon)

# ---- flags ----
CFLAGS_BASE := -Wall -Wextra -Wpedantic
CFLAGS_DEBUG := -O0 -g -fsanitize=address -fno-omit-frame-pointer
CFLAGS_RELEASE := -O3 -DNDEBUG

# ---- sources ----
SRCS := \
	$(wildcard $(SRC_DIR)/*.c) \
	$(wildcard protocol/*.c)

# ---- default: debug ----
CFLAGS := $(CFLAGS_BASE) $(CFLAGS_DEBUG) $(WAYLAND_CFLAGS) $(XKB_CFLAGS)
LIBS   := $(WAYLAND_LIBS) $(XKB_LIBS)
BUILD_SUBDIR := debug

# ---- debug build ----
main: $(SRCS) | $(BUILD_DIR)/debug
	@echo "[DEBUG] Building $@"
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/debug/$@ $(SRCS) $(LIBS)
	@echo "[DEBUG] Running $@"
	@./$(BUILD_DIR)/debug/$@

# ---- release build ----
main-release: CFLAGS := $(CFLAGS_BASE) $(CFLAGS_RELEASE) $(WAYLAND_CFLAGS) $(XKB_CFLAGS)
main-release: LIBS   := $(WAYLAND_LIBS) $(XKB_LIBS)
main-release: $(SRCS) | $(BUILD_DIR)/release
	@echo "[RELEASE] Building main"
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/release/main $(SRCS) $(LIBS)
	@echo "[RELEASE] Running main"
	@./$(BUILD_DIR)/release/main

# ---- directories ----
$(BUILD_DIR)/debug:
	mkdir -p $@

$(BUILD_DIR)/release:
	mkdir -p $@

# ---- clean ----
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

