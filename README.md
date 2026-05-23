# Swipe - Simple C GUI Library

A lightweight, cross-platform C library for creating GUI applications with a simple, Objective-C-inspired API.

## Features

✨ **Simple API** - Easy-to-use functions for GUI development
🎨 **Lightweight** - Minimal dependencies, small footprint  
🖥️ **Cross-platform** - Designed for Windows, macOS, and Linux
📦 **Object-oriented in C** - Clear object model with explicit memory management
⚡ **Reference Counting** - Automatic memory management like Objective-C
🎯 **Built-in Widgets** - Buttons, labels, text fields
⚡ **Event-driven** - Callback-based event system
🎨 **Color Management** - Built-in color utilities with predefined colors

## Quick Start

### Building

```bash
# Using Make
make
sudo make install

# Or using CMake
mkdir build
cd build
cmake ..
make
sudo make install
```

### Minimal Example

```c
#include <swipe.h>

int main() {
    swipe_init();
    
    SwWindow *window = sw_window_create(
        "My App",
        sw_rect_make(100, 100, 400, 300)
    );
    
    sw_window_show(window);
    int exit_code = sw_app_run();
    
    sw_window_release(window);
    swipe_shutdown();
    
    return exit_code;
}
```

## Project Structure

```
swipe/
├── include/              # Public API headers
│   └── swipe.h
├── src/                  # Implementation
│   ├── core.c           # Core functionality
│   ├── window.c         # Window and view management
│   └── widget.c         # UI widgets
├── examples/            # Example programs
│   └── hello_window.c
├── Makefile             # GNU Make build system
├── CMakeLists.txt       # CMake build system
├── doc.md               # Complete documentation
└── README.md            # This file
```

## API Overview

### Initialization
```c
swipe_init();
// ... create and use widgets ...
swipe_shutdown();
```

### Reference Counting
```c
// Create object (count = 1)
SwButton *button = sw_button_create("Click me", rect);

// Store reference
my_app.button = button;
sw_retain((SwObject *)button);  // count = 2

// Release when done
sw_release((SwObject *)button);  // count = 1
sw_button_release(button);       // count = 0, freed
```

### Windows
```c
SwWindow *window = sw_window_create("Title", sw_rect_make(x, y, w, h));
sw_window_show(window);
sw_window_release(window);
```

### Widgets
```c
// Button
SwButton *button = sw_button_create("Click me", rect);
sw_button_set_on_click(button, callback, context);

// Label
SwLabel *label = sw_label_create("Hello", rect);
sw_label_set_text_color(label, SW_COLOR_RED);

// Text Field
SwTextField *field = sw_text_field_create(rect);
sw_text_field_set_placeholder(field, "Enter text...");
```

### Event Loop
```c
int exit_code = sw_app_run();
```

## Building

### Make
```bash
make              # Build libraries
make example      # Build example
make clean        # Remove artifacts
make install      # Install system-wide
```

### CMake
```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

## Documentation

Complete API documentation is available in [doc.md](doc.md)

## Examples

See [examples/hello_window.c](examples/hello_window.c) for a complete example demonstrating:
- Window creation
- Widget creation (button, label, text field)
- Event handling
- Running the event loop

Build and run:
```bash
make example
./build/hello_window
```

## System Requirements

- C99 compatible compiler (GCC, Clang, MSVC)
- POSIX-compliant system or Windows
- CMake 3.10+ (for CMake build)
- GNU Make (for Make build)

## Installation

### From Source

```bash
git clone https://github.com/yourname/swipe.git
cd swipe
make
sudo make install
```

### Using in Your Project

**With pkg-config:**
```bash
gcc `pkg-config --cflags libswipe` app.c -o app `pkg-config --libs libswipe`
```

**With Manual Linking:**
```bash
gcc -I/usr/local/include app.c -o app -L/usr/local/lib -lswipe
```

## API Highlights

### Color Utilities
```c
SwColor red = sw_color_rgb(1.0f, 0.0f, 0.0f);
SwColor blue = sw_color_rgb8(0, 0, 255);
SwColor white = SW_COLOR_WHITE;
```

### Geometry
```c
SwRect rect = sw_rect_make(10, 20, 300, 200);
SwSize size = sw_size_make(100, 50);
SwPoint point = sw_point_make(10, 15);
```

### View Hierarchy
```c
SwView *parent = sw_view_create(rect1);
SwView *child = sw_view_create(rect2);
sw_view_add_subview(parent, child);
```

### Event Callbacks
```c
void on_button_click(SwObject *sender, void *context) {
    printf("Clicked!\n");
}

sw_button_set_on_click(button, on_button_click, NULL);
```

## Limitations

- Platform-specific implementation required
- Single-threaded event loop
- Text field limited to 256 characters
- No built-in layout manager

## Future Plans

- Layout managers (grid, flex)
- Additional widgets (checkbox, radio, table)
- Custom drawing API
- Drag and drop
- Accessibility features
- Multi-threaded rendering

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

## License

MIT License - See LICENSE file for details

## Author

Created by Swipe Development Team

## Support

For issues and questions, please open an issue on the project repository.
