/**
 * Hello Window Example
 * 
 * A simple example demonstrating basic Swipe usage:
 * - Creating a window
 * - Adding labels and buttons
 * - Handling button clicks
 */

#include "../include/swipe.h"
#include <stdio.h>
#include <stdlib.h>

/* Click callback for the button */
void on_button_click(SwObject *sender __attribute__((unused)), void *context) {
    printf("Button clicked!\n");
    SwLabel *label = (SwLabel *)context;
    sw_label_set_text(label, "Button was clicked!");
}

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
    /* Initialize the Swipe library */
    if (swipe_init() != 0) {
        fprintf(stderr, "Failed to initialize Swipe\n");
        return 1;
    }
    
    /* Create a window */
    SwRect window_frame = sw_rect_make(100, 100, 400, 300);
    SwWindow *window = sw_window_create("Hello Swipe", window_frame);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        swipe_shutdown();
        return 1;
    }
    
    /* Create a label */
    SwRect label_frame = sw_rect_make(20, 20, 360, 60);
    SwLabel *label = sw_label_create("Click the button below", label_frame);
    if (!label) {
        fprintf(stderr, "Failed to create label\n");
        sw_window_release(window);
        swipe_shutdown();
        return 1;
    }
    
    sw_label_set_text_color(label, sw_color_rgb(0.0f, 0.0f, 0.0f));
    sw_label_set_font_size(label, 16.0f);
    sw_label_set_alignment(label, SW_ALIGN_CENTER);
    
    /* Create a button */
    SwRect button_frame = sw_rect_make(150, 120, 100, 40);
    SwButton *button = sw_button_create("Click Me", button_frame);
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        sw_label_release(label);
        sw_window_release(window);
        swipe_shutdown();
        return 1;
    }
    
    /* Set up button click handler */
    sw_button_set_on_click(button, on_button_click, (void *)label);
    
    /* Create a text field */
    SwRect text_field_frame = sw_rect_make(50, 200, 300, 40);
    SwTextField *text_field = sw_text_field_create(text_field_frame);
    if (!text_field) {
        fprintf(stderr, "Failed to create text field\n");
        sw_button_release(button);
        sw_label_release(label);
        sw_window_release(window);
        swipe_shutdown();
        return 1;
    }
    
    sw_text_field_set_placeholder(text_field, "Enter text here...");
    
    /* Add views to window */
    sw_window_add_view(window, sw_label_to_view(label));
    sw_window_add_view(window, sw_button_to_view(button));
    sw_window_add_view(window, sw_text_field_to_view(text_field));
    
    /* Show the window */
    sw_window_show(window);
    
    /* Run the application */
    printf("Starting Swipe application...\n");
    int exit_code = sw_app_run();
    
    /* Clean up */
    sw_text_field_release(text_field);
    sw_button_release(button);
    sw_label_release(label);
    sw_window_release(window);
    swipe_shutdown();
    
    printf("Application exited with code: %d\n", exit_code);
    return exit_code;
}
