# Swipe GUI Library Makefile
# 
# Build targets:
#   make          - Build the library
#   make example  - Build the example
#   make clean    - Remove build artifacts
#   make docs     - Generate documentation

CC := gcc
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 sdl2_ttf 2>/dev/null || echo "-I/opt/homebrew/opt/sdl2/include -I/opt/homebrew/opt/sdl2/include/SDL2 -I/opt/homebrew/opt/sdl2_ttf/include -I/opt/homebrew/opt/sdl2_ttf/include/SDL2")
SDL2_LIBS := $(shell pkg-config --libs sdl2 sdl2_ttf 2>/dev/null || echo "-L/opt/homebrew/opt/sdl2/lib -L/opt/homebrew/opt/sdl2_ttf/lib -lSDL2 -lSDL2_ttf")

CFLAGS := -Wall -Wextra -Werror -pedantic -fPIC -Iinclude $(SDL2_CFLAGS)
LDFLAGS := -shared $(SDL2_LIBS)

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
EXAMPLE_DIR := examples
DIST_DIR := dist

# Source files
SOURCES := $(SRC_DIR)/core.c $(SRC_DIR)/window.c $(SRC_DIR)/widget.c
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Library name
LIBRARY := $(DIST_DIR)/libswipe.so
LIBRARY_STATIC := $(DIST_DIR)/libswipe.a

# Example
EXAMPLE := $(BUILD_DIR)/hello_window
EXAMPLE_SOURCE := $(EXAMPLE_DIR)/hello_window.c

CALC_EXAMPLE := $(BUILD_DIR)/calculator
CALC_SOURCE := $(EXAMPLE_DIR)/calculator.c

# Targets
.PHONY: all library library-static example calculator clean

all: library library-static

library: $(LIBRARY)

library-static: $(LIBRARY_STATIC)

$(LIBRARY): $(OBJECTS) | $(DIST_DIR)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "✓ Built shared library: $@"

$(LIBRARY_STATIC): $(OBJECTS) | $(DIST_DIR)
	ar rcs $@ $^
	@echo "✓ Built static library: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

$(DIST_DIR):
	mkdir -p $@

example: $(EXAMPLE)

$(EXAMPLE): $(EXAMPLE_SOURCE) $(LIBRARY_STATIC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -L$(DIST_DIR) -o $@ $< -lswipe $(SDL2_LIBS)
	@echo "✓ Built example: $@"

calculator: $(CALC_EXAMPLE)

$(CALC_EXAMPLE): $(CALC_SOURCE) $(LIBRARY_STATIC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -L$(DIST_DIR) -o $@ $< -lswipe $(SDL2_LIBS)
	@echo "✓ Built calculator: $@"

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)
	@echo "✓ Cleaned build artifacts"

install: library library-static
	@echo "Installing Swipe library..."
	mkdir -p /usr/local/include/swipe
	mkdir -p /usr/local/lib
	cp $(INCLUDE_DIR)/swipe.h /usr/local/include/swipe/
	cp $(LIBRARY) /usr/local/lib/
	cp $(LIBRARY_STATIC) /usr/local/lib/
	@if [ "$$(uname)" = "Linux" ]; then \
		if command -v ldconfig >/dev/null 2>&1; then \
			ldconfig; \
		fi; \
	fi
	@echo "✓ Swipe installed successfully"

uninstall:
	@echo "Uninstalling Swipe library..."
	rm -rf /usr/local/include/swipe
	rm -f /usr/local/lib/libswipe.so
	rm -f /usr/local/lib/libswipe.a
	@if [ "$$(uname)" = "Linux" ]; then \
		if command -v ldconfig >/dev/null 2>&1; then \
			ldconfig; \
		fi; \
	fi
	@echo "✓ Swipe uninstalled"

.PHONY: install uninstall
