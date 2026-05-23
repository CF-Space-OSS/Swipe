/**
 * Swipe - A Simple C GUI Library
 * 
 * A lightweight, Objective-C-inspired GUI library for creating desktop applications in C.
 * Provides an easy-to-use API for window management, event handling, and UI widgets.
 */

#ifndef SWIPE_H
#define SWIPE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * CORE TYPES AND STRUCTURES
 * ============================================================================ */

typedef struct SwObject SwObject;
typedef struct SwWindow SwWindow;
typedef struct SwView SwView;
typedef struct SwButton SwButton;
typedef struct SwLabel SwLabel;
typedef struct SwTextField SwTextField;
typedef struct SwEvent SwEvent;

/* Callback function types */
typedef void (*SwCallback)(SwObject *sender, void *context);
typedef void (*SwEventHandler)(SwEvent *event, void *context);

/* Color structure */
typedef struct {
    float red;
    float green;
    float blue;
    float alpha;
} SwColor;

/* Size structure */
typedef struct {
    float width;
    float height;
} SwSize;

/* Point structure */
typedef struct {
    float x;
    float y;
} SwPoint;

/* Rectangle structure */
typedef struct {
    SwPoint origin;
    SwSize size;
} SwRect;

/* Event types */
typedef enum {
    SW_EVENT_BUTTON_CLICK,
    SW_EVENT_TEXT_CHANGED,
    SW_EVENT_WINDOW_CLOSE,
    SW_EVENT_WINDOW_RESIZE,
    SW_EVENT_MOUSE_ENTER,
    SW_EVENT_MOUSE_EXIT,
    SW_EVENT_KEYBOARD_INPUT
} SwEventType;

/* Button states */
typedef enum {
    SW_STATE_NORMAL,
    SW_STATE_HOVER,
    SW_STATE_PRESSED,
    SW_STATE_DISABLED
} SwState;

/* Text alignment */
typedef enum {
    SW_ALIGN_LEFT,
    SW_ALIGN_CENTER,
    SW_ALIGN_RIGHT
} SwAlignment;

/* ============================================================================
 * INITIALIZATION AND CLEANUP
 * ============================================================================ */

/**
 * Initialize the Swipe library
 * Must be called before using any Swipe functions
 * Returns 0 on success, -1 on failure
 */
int swipe_init(void);

/**
 * Shutdown the Swipe library and clean up resources
 */
void swipe_shutdown(void);

/**
 * Check if Swipe is initialized
 */
int swipe_is_initialized(void);

/* ============================================================================
 * REFERENCE COUNTING (Memory Management)
 * ============================================================================ */

/**
 * Increment the reference count of an object
 * Objects are created with retain_count = 1
 * Use this when storing references to objects
 * @return The object pointer for chaining
 */
SwObject* sw_retain(SwObject *object);

/**
 * Decrement the reference count and release if count reaches 0
 * Always safe to call with NULL
 * Use this when you're done with a reference to an object
 */
void sw_release(SwObject *object);

/**
 * Get the current reference count of an object
 * Returns 0 if object is NULL
 */
int sw_get_retain_count(SwObject *object);

/* ============================================================================
 * WINDOW MANAGEMENT
 * ============================================================================ */

/**
 * Create a new window
 * @param title Window title
 * @param rect Window position and size
 * @return Pointer to SwWindow, or NULL on failure
 */
SwWindow* sw_window_create(const char *title, SwRect rect);

/**
 * Release a window and its resources
 */
void sw_window_release(SwWindow *window);

/**
 * Show a window
 */
void sw_window_show(SwWindow *window);

/**
 * Hide a window
 */
void sw_window_hide(SwWindow *window);

/**
 * Set window title
 */
void sw_window_set_title(SwWindow *window, const char *title);

/**
 * Get window title
 */
const char* sw_window_get_title(SwWindow *window);

/**
 * Set window background color
 */
void sw_window_set_background_color(SwWindow *window, SwColor color);

/**
 * Set window close callback
 */
void sw_window_set_on_close(SwWindow *window, SwCallback callback, void *context);

/**
 * Add a view to the window
 */
void sw_window_add_view(SwWindow *window, SwView *view);

/**
 * Remove a view from the window
 */
void sw_window_remove_view(SwWindow *window, SwView *view);

/**
 * Start the application event loop
 */
int sw_app_run(void);

/**
 * Exit the application event loop
 */
void sw_app_exit(int exit_code);

/* ============================================================================
 * VIEW MANAGEMENT
 * ============================================================================ */

/**
 * Create a base view
 * @param rect View position and size
 * @return Pointer to SwView, or NULL on failure
 */
SwView* sw_view_create(SwRect rect);

/**
 * Release a view and its resources
 */
void sw_view_release(SwView *view);

/**
 * Set view background color
 */
void sw_view_set_background_color(SwView *view, SwColor color);

/**
 * Set view frame (position and size)
 */
void sw_view_set_frame(SwView *view, SwRect frame);

/**
 * Get view frame
 */
SwRect sw_view_get_frame(SwView *view);

/**
 * Hide or show a view
 */
void sw_view_set_hidden(SwView *view, int hidden);

/**
 * Add subview
 */
void sw_view_add_subview(SwView *parent, SwView *child);

/**
 * Remove subview
 */
void sw_view_remove_subview(SwView *parent, SwView *child);

/* ============================================================================
 * BUTTON WIDGET
 * ============================================================================ */

/**
 * Create a button
 * @param title Button title
 * @param rect Button position and size
 * @return Pointer to SwButton, or NULL on failure
 */
SwButton* sw_button_create(const char *title, SwRect rect);

/**
 * Release a button
 */
void sw_button_release(SwButton *button);

/**
 * Set button title
 */
void sw_button_set_title(SwButton *button, const char *title);

/**
 * Get button title
 */
const char* sw_button_get_title(SwButton *button);

/**
 * Set button click callback
 */
void sw_button_set_on_click(SwButton *button, SwCallback callback, void *context);

/**
 * Set button enabled state
 */
void sw_button_set_enabled(SwButton *button, int enabled);

/**
 * Get button state
 */
int sw_button_is_enabled(SwButton *button);

/**
 * Cast button to view
 */
SwView* sw_button_to_view(SwButton *button);

/* ============================================================================
 * LABEL WIDGET
 * ============================================================================ */

/**
 * Create a label
 * @param text Label text
 * @param rect Label position and size
 * @return Pointer to SwLabel, or NULL on failure
 */
SwLabel* sw_label_create(const char *text, SwRect rect);

/**
 * Release a label
 */
void sw_label_release(SwLabel *label);

/**
 * Set label text
 */
void sw_label_set_text(SwLabel *label, const char *text);

/**
 * Get label text
 */
const char* sw_label_get_text(SwLabel *label);

/**
 * Set label text color
 */
void sw_label_set_text_color(SwLabel *label, SwColor color);

/**
 * Set label text alignment
 */
void sw_label_set_alignment(SwLabel *label, SwAlignment alignment);

/**
 * Set label font size
 */
void sw_label_set_font_size(SwLabel *label, float size);

/**
 * Cast label to view
 */
SwView* sw_label_to_view(SwLabel *label);

/* ============================================================================
 * TEXT FIELD WIDGET
 * ============================================================================ */

/**
 * Create a text field
 * @param rect Text field position and size
 * @return Pointer to SwTextField, or NULL on failure
 */
SwTextField* sw_text_field_create(SwRect rect);

/**
 * Release a text field
 */
void sw_text_field_release(SwTextField *text_field);

/**
 * Set text field content
 */
void sw_text_field_set_text(SwTextField *text_field, const char *text);

/**
 * Get text field content
 */
const char* sw_text_field_get_text(SwTextField *text_field);

/**
 * Set placeholder text
 */
void sw_text_field_set_placeholder(SwTextField *text_field, const char *placeholder);

/**
 * Set text change callback
 */
void sw_text_field_set_on_change(SwTextField *text_field, SwCallback callback, void *context);

/**
 * Cast text field to view
 */
SwView* sw_text_field_to_view(SwTextField *text_field);

/* ============================================================================
 * COLOR UTILITIES
 * ============================================================================ */

/**
 * Create a color from RGB values (0.0 to 1.0)
 */
SwColor sw_color_rgb(float red, float green, float blue);

/**
 * Create a color from RGBA values (0.0 to 1.0)
 */
SwColor sw_color_rgba(float red, float green, float blue, float alpha);

/**
 * Create a color from 8-bit RGB values (0 to 255)
 */
SwColor sw_color_rgb8(uint8_t red, uint8_t green, uint8_t blue);

/**
 * Create a color from 8-bit RGBA values (0 to 255)
 */
SwColor sw_color_rgba8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

/* Predefined colors */
extern const SwColor SW_COLOR_WHITE;
extern const SwColor SW_COLOR_BLACK;
extern const SwColor SW_COLOR_RED;
extern const SwColor SW_COLOR_GREEN;
extern const SwColor SW_COLOR_BLUE;
extern const SwColor SW_COLOR_GRAY;
extern const SwColor SW_COLOR_LIGHT_GRAY;
extern const SwColor SW_COLOR_DARK_GRAY;
extern const SwColor SW_COLOR_TRANSPARENT;

/* ============================================================================
 * RECT AND SIZE UTILITIES
 * ============================================================================ */

/**
 * Create a rectangle
 */
SwRect sw_rect_make(float x, float y, float width, float height);

/**
 * Create a size
 */
SwSize sw_size_make(float width, float height);

/**
 * Create a point
 */
SwPoint sw_point_make(float x, float y);

#ifdef __cplusplus
}
#endif

#endif /* SWIPE_H */
