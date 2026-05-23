# Swipe GUI Library Documentation

**Swipe** is a lightweight, cross-platform C GUI library featuring an Objective-C-inspired reference-counted API. Built on top of **SDL2** and **SDL2_ttf**, it abstracts windowing, rendering, and input event dispatching into a clean and intuitive object-oriented paradigm in C.

---

## ⚡ Quick Start

Here is a minimal, complete application that initializes Swipe, creates a window with a button, and starts the event loop:

```c
#include <swipe.h>
#include <stdio.h>

void on_click(SwObject *sender, void *context) {
    printf("Button clicked!\n");
}

int main() {
    if (swipe_init() != 0) return 1;
    
    // Create a 400x300 window
    SwWindow *window = sw_window_create("Hello Swipe", sw_rect_make(100, 100, 400, 300));
    
    // Create a button and attach a click handler
    SwButton *button = sw_button_create("Click Me", sw_rect_make(150, 120, 100, 40));
    sw_button_set_on_click(button, on_click, NULL);
    
    // Add button to window & show window
    sw_window_add_view(window, sw_button_to_view(button));
    sw_window_show(window);
    
    // Run the main event loop
    int exit_code = sw_app_run();
    
    // Clean up
    sw_button_release(button);
    sw_window_release(window);
    swipe_shutdown();
    
    return exit_code;
}
```

---

## 🧠 Core Concepts

### 1. Memory Management (Reference Counting)
Swipe uses explicit reference counting to manage memory deterministically.
* **Creation**: All objects (`SwWindow`, `SwView`, widgets) start with a `retain_count = 1`.
* **Retaining**: Call `sw_retain((SwObject *)obj)` when storing a new reference.
* **Releasing**: Call `sw_release((SwObject *)obj)` or widget-specific release functions (e.g., `sw_button_release`) when done. When `retain_count` reaches `0`, the object is automatically freed.

> [!TIP]
> Adding a view to a window or subview **automatically retains it**. You can safely release your local reference immediately after adding it.

### 2. View Hierarchy
All UI components inherit from `SwView`. Views can contain subviews to build complex hierarchies:
```c
SwView *parent = sw_view_create(sw_rect_make(0, 0, 300, 300));
SwView *child = sw_view_create(sw_rect_make(10, 10, 100, 100));

sw_view_add_subview(parent, child);
sw_view_release(child); // Parent holds reference now
```

---

## 📋 API Reference Cheat-Sheet

### Core & Application
| Signature | Description |
| :--- | :--- |
| `int swipe_init(void)` | Initializes SDL2 and font caching subsystems. Returns `0` on success. |
| `void swipe_shutdown(void)` | Closes all resources, cleans font cache, and shuts down SDL. |
| `int sw_app_run(void)` | Starts the block-event loop. Returns application exit code. |
| `void sw_app_exit(int code)` | Interrupts the event loop and exits with `code`. |

### Object Model
| Signature | Description |
| :--- | :--- |
| `SwObject* sw_retain(SwObject *obj)` | Increments the reference count of the object by 1. |
| `void sw_release(SwObject *obj)` | Decrements reference count. Frees object if it hits `0`. |
| `int sw_get_retain_count(SwObject *obj)`| Returns the current reference count. |

### Window Management
| Signature | Description |
| :--- | :--- |
| `SwWindow* sw_window_create(const char* title, SwRect rect)` | Creates a window. |
| `void sw_window_show(SwWindow *win)` \| `sw_window_hide` | Alters window visibility. |
| `void sw_window_set_background_color(SwWindow *win, SwColor col)` | Sets backing color. |
| `void sw_window_add_view(SwWindow *win, SwView *view)` | Adds view to top-level layout. |
| `void sw_window_set_on_close(SwWindow *win, SwCallback cb, void *ctx)`| Fires when closed. |

### Widgets & Customization
Swipe provides three built-in high-level widgets. Remember to cast them via their `*_to_view()` functions to add them to windows or parents.

#### 🔘 Button (`SwButton`)
* `SwButton* sw_button_create(const char *title, SwRect rect)`
* `void sw_button_set_title(SwButton *btn, const char *title)`
* `void sw_button_set_on_click(SwButton *btn, SwCallback cb, void *ctx)`
* `void sw_button_set_enabled(SwButton *btn, int enabled)`
* `SwView* sw_button_to_view(SwButton *btn)`

#### 📝 Label (`SwLabel`)
* `SwLabel* sw_label_create(const char *text, SwRect rect)`
* `void sw_label_set_text(SwLabel *lbl, const char *text)`
* `void sw_label_set_text_color(SwLabel *lbl, SwColor color)`
* `void sw_label_set_font_size(SwLabel *lbl, float size)`
* `void sw_label_set_alignment(SwLabel *lbl, SwAlignment alignment)` (`SW_ALIGN_LEFT`, `_CENTER`, `_RIGHT`)
* `SwView* sw_label_to_view(SwLabel *lbl)`

#### ⌨️ TextField (`SwTextField`)
* `SwTextField* sw_text_field_create(SwRect rect)`
* `void sw_text_field_set_text(SwTextField *tf, const char *text)`
* `const char* sw_text_field_get_text(SwTextField *tf)`
* `void sw_text_field_set_placeholder(SwTextField *tf, const char *ph)`
* `void sw_text_field_set_on_change(SwTextField *tf, SwCallback cb, void *ctx)`
* `SwView* sw_text_field_to_view(SwTextField *tf)`

---

## 🛠️ Build & Run

### 1. Compile Swipe Library & Examples
```bash
# Build both shared (libswipe.so) and static (libswipe.a) libraries
make

# Build and compile the hello_window test example
make example
```

### 2. Run the Example
```bash
./build/hello_window
```

### 3. Clean and Uninstall
```bash
# Clean build artifacts
make clean

# Install system-wide
sudo make install
```
