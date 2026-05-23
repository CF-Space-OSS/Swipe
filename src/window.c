/**
 * Window & View Implementation using SDL2
 */

#include "../include/swipe.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <SDL.h>

/* Native Window Handle structure */
typedef struct {
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
} SwNativeWindow;

/* Window structure definition */
struct SwWindow {
    int retain_count;
    char *title;
    SwRect frame;
    SwColor background_color;
    int visible;
    SwCallback on_close;
    void *on_close_context;
    SwView **views;
    size_t view_count;
    size_t view_capacity;
    void *native_window;  /* Points to SwNativeWindow */
};

/* View structure definition */
struct SwView {
    int retain_count;
    SwRect frame;
    SwColor background_color;
    int hidden;
    SwView **subviews;
    size_t subview_count;
    size_t subview_capacity;
    void *user_data;
    
    /* Drawing and event callbacks for specialized widgets */
    void (*draw)(SwView *view, SDL_Renderer *renderer, SwPoint offset);
    void (*event)(SwView *view, SDL_Event *event, SwPoint offset);
};

/* Application state */
static SwWindow **_windows = NULL;
static size_t _window_count = 0;
static size_t _window_capacity = 0;
static int _app_running = 0;
static int _exit_code = 0;

/* Global keyboard focus state */
static SwView *_focused_view = NULL;

/* Export focus helpers for widgets in widget.c */
SwView* sw_view_get_focused(void) {
    return _focused_view;
}

void sw_view_set_focused(SwView *view) {
    _focused_view = view;
}

/* Helper to find SwWindow by SDL window ID */
static SwWindow* find_window_by_sdl_id(Uint32 windowID) {
    for (size_t i = 0; i < _window_count; i++) {
        if (_windows[i]->native_window) {
            SwNativeWindow *native = (SwNativeWindow *)_windows[i]->native_window;
            if (SDL_GetWindowID(native->sdl_window) == windowID) {
                return _windows[i];
            }
        }
    }
    return NULL;
}

/* ============================================================================
 * WINDOW MANAGEMENT
 * ============================================================================ */

SwWindow* sw_window_create(const char *title, SwRect rect) {
    SwWindow *window = (SwWindow *)malloc(sizeof(SwWindow));
    if (!window) return NULL;
    
    window->title = (char *)malloc(strlen(title) + 1);
    if (!window->title) {
        free(window);
        return NULL;
    }
    strcpy(window->title, title);
    
    window->frame = rect;
    window->background_color = SW_COLOR_WHITE;
    window->visible = 0;
    window->on_close = NULL;
    window->on_close_context = NULL;
    window->views = NULL;
    window->view_count = 0;
    window->view_capacity = 0;
    window->native_window = NULL;
    window->retain_count = 1;
    
    /* Add to global window list */
    if (_window_count >= _window_capacity) {
        _window_capacity = (_window_capacity + 1) * 2;
        SwWindow **new_windows = (SwWindow **)realloc(_windows, _window_capacity * sizeof(SwWindow *));
        if (!new_windows) {
            free(window->title);
            free(window);
            return NULL;
        }
        _windows = new_windows;
    }
    _windows[_window_count++] = window;
    
    return window;
}

void sw_window_release(SwWindow *window) {
    if (!window) return;
    
    window->retain_count--;
    if (window->retain_count > 0) return;
    
    /* Clean up native SDL window and renderer */
    if (window->native_window) {
        SwNativeWindow *native = (SwNativeWindow *)window->native_window;
        SDL_DestroyRenderer(native->sdl_renderer);
        SDL_DestroyWindow(native->sdl_window);
        free(native);
        window->native_window = NULL;
    }
    
    /* Remove from global window list */
    for (size_t i = 0; i < _window_count; i++) {
        if (_windows[i] == window) {
            _windows[i] = _windows[_window_count - 1];
            _window_count--;
            break;
        }
    }
    
    /* Release views */
    for (size_t i = 0; i < window->view_count; i++) {
        sw_view_release(window->views[i]);
    }
    if (window->views) free(window->views);
    
    if (window->title) free(window->title);
    free(window);
}

void sw_window_show(SwWindow *window) {
    if (!window) return;
    window->visible = 1;
}

void sw_window_hide(SwWindow *window) {
    if (!window) return;
    window->visible = 0;
}

void sw_window_set_title(SwWindow *window, const char *title) {
    if (!window || !title) return;
    
    char *new_title = (char *)malloc(strlen(title) + 1);
    if (!new_title) return;
    
    strcpy(new_title, title);
    if (window->title) free(window->title);
    window->title = new_title;
    
    if (window->native_window) {
        SwNativeWindow *native = (SwNativeWindow *)window->native_window;
        SDL_SetWindowTitle(native->sdl_window, title);
    }
}

const char* sw_window_get_title(SwWindow *window) {
    if (!window) return NULL;
    return window->title;
}

void sw_window_set_background_color(SwWindow *window, SwColor color) {
    if (!window) return;
    window->background_color = color;
}

void sw_window_set_on_close(SwWindow *window, SwCallback callback, void *context) {
    if (!window) return;
    window->on_close = callback;
    window->on_close_context = context;
}

void sw_window_add_view(SwWindow *window, SwView *view) {
    if (!window || !view) return;
    
    if (window->view_count >= window->view_capacity) {
        window->view_capacity = (window->view_capacity + 1) * 2;
        SwView **new_views = (SwView **)realloc(window->views, window->view_capacity * sizeof(SwView *));
        if (!new_views) return;
        window->views = new_views;
    }
    
    window->views[window->view_count++] = view;
}

void sw_window_remove_view(SwWindow *window, SwView *view) {
    if (!window || !view) return;
    
    for (size_t i = 0; i < window->view_count; i++) {
        if (window->views[i] == view) {
            window->views[i] = window->views[window->view_count - 1];
            window->view_count--;
            break;
        }
    }
}

/* Helper to draw filled rect with opacity */
static void draw_filled_rect(SDL_Renderer *renderer, float x, float y, float w, float h, SwColor color) {
    SDL_SetRenderDrawColor(renderer, 
        (Uint8)(color.red * 255.0f), 
        (Uint8)(color.green * 255.0f), 
        (Uint8)(color.blue * 255.0f), 
        (Uint8)(color.alpha * 255.0f)
    );
    SDL_Rect rect = { (int)x, (int)y, (int)w, (int)h };
    SDL_RenderFillRect(renderer, &rect);
}

/* Recursive view drawing helper */
static void view_render(SwView *view, SDL_Renderer *renderer, SwPoint offset) {
    if (view->hidden) return;
    
    SwPoint local_offset = sw_point_make(offset.x + view->frame.origin.x, offset.y + view->frame.origin.y);
    
    // 1. Draw view background if not transparent
    if (view->background_color.alpha > 0.0f) {
        draw_filled_rect(renderer, local_offset.x, local_offset.y, view->frame.size.width, view->frame.size.height, view->background_color);
    }
    
    // 2. Call custom widget drawing if defined
    if (view->draw) {
        view->draw(view, renderer, local_offset);
    }
    
    // 3. Draw subviews (back-to-front)
    for (size_t i = 0; i < view->subview_count; i++) {
        view_render(view->subviews[i], renderer, local_offset);
    }
}

/* Recursive event dispatching helper (front-to-back hit testing) */
static int view_handle_event(SwView *view, SDL_Event *event, SwPoint offset) {
    if (view->hidden) return 0;
    
    SwPoint local_offset = sw_point_make(offset.x + view->frame.origin.x, offset.y + view->frame.origin.y);
    
    // 1. Check subviews first (front-to-back)
    for (int i = (int)view->subview_count - 1; i >= 0; i--) {
        if (view_handle_event(view->subviews[i], event, local_offset)) {
            return 1; // Event consumed
        }
    }
    
    // 2. Hit-test this view for mouse events
    int inside = 0;
    float x = local_offset.x;
    float y = local_offset.y;
    float w = view->frame.size.width;
    float h = view->frame.size.height;
    
    if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP || event->type == SDL_MOUSEMOTION) {
        float mx = 0, my = 0;
        if (event->type == SDL_MOUSEMOTION) {
            mx = event->motion.x;
            my = event->motion.y;
        } else {
            mx = event->button.x;
            my = event->button.y;
        }
        if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
            inside = 1;
        }
    }
    
    // Dispatch to this view's event callback
    if (view->event) {
        view->event(view, event, offset);
    }
    
    // Consume mouse click events if hit inside bounds
    if ((event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) && inside) {
        return 1;
    }
    
    return 0;
}

int sw_app_run(void) {
    _app_running = 1;
    _exit_code = 0;
    
    SDL_Event event;
    while (_app_running && _window_count > 0) {
        int visible_windows = 0;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                _app_running = 0;
                break;
            }
            
            // Handle window events (closing and resizing)
            if (event.type == SDL_WINDOWEVENT) {
                SwWindow *win = find_window_by_sdl_id(event.window.windowID);
                if (win) {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        if (win->on_close) {
                            win->on_close((SwObject *)win, win->on_close_context);
                        } else {
                            sw_window_hide(win);
                        }
                    } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        win->frame.size.width = event.window.data1;
                        win->frame.size.height = event.window.data2;
                    }
                }
            }
            
            // Handle mouse inputs
            if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEMOTION) {
                Uint32 windowID = (event.type == SDL_MOUSEMOTION) ? event.motion.windowID : event.button.windowID;
                SwWindow *win = find_window_by_sdl_id(windowID);
                if (win) {
                    int hit = 0;
                    for (int j = (int)win->view_count - 1; j >= 0; j--) {
                        if (view_handle_event(win->views[j], &event, sw_point_make(0, 0))) {
                            hit = 1;
                            break;
                        }
                    }
                    if (!hit && event.type == SDL_MOUSEBUTTONDOWN) {
                        // Clicked empty window area, clear focus
                        _focused_view = NULL;
                    }
                }
            }
            
            // Handle keyboard inputs (directed to focused widget)
            if (event.type == SDL_TEXTINPUT || event.type == SDL_KEYDOWN) {
                if (_focused_view && _focused_view->event) {
                    _focused_view->event(_focused_view, &event, sw_point_make(0, 0));
                }
            }
        }
        
        // Render all active windows
        for (size_t i = 0; i < _window_count; i++) {
            SwWindow *window = _windows[i];
            if (window->visible) {
                visible_windows++;
                
                if (!window->native_window) {
                    // Create window dynamically
                    SDL_Window *sdl_win = SDL_CreateWindow(
                        window->title,
                        (int)window->frame.origin.x,
                        (int)window->frame.origin.y,
                        (int)window->frame.size.width,
                        (int)window->frame.size.height,
                        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
                    );
                    SDL_Renderer *sdl_rend = SDL_CreateRenderer(
                        sdl_win,
                        -1,
                        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                    );
                    
                    SDL_StartTextInput();
                    
                    SwNativeWindow *native = (SwNativeWindow *)malloc(sizeof(SwNativeWindow));
                    native->sdl_window = sdl_win;
                    native->sdl_renderer = sdl_rend;
                    window->native_window = native;
                }
                
                SwNativeWindow *native = (SwNativeWindow *)window->native_window;
                
                // Set window background
                SDL_SetRenderDrawColor(
                    native->sdl_renderer,
                    (Uint8)(window->background_color.red * 255.0f),
                    (Uint8)(window->background_color.green * 255.0f),
                    (Uint8)(window->background_color.blue * 255.0f),
                    (Uint8)(window->background_color.alpha * 255.0f)
                );
                SDL_RenderClear(native->sdl_renderer);
                
                // Draw views
                for (size_t j = 0; j < window->view_count; j++) {
                    view_render(window->views[j], native->sdl_renderer, sw_point_make(0, 0));
                }
                
                SDL_RenderPresent(native->sdl_renderer);
            } else {
                // Destroy hidden window resources
                if (window->native_window) {
                    SwNativeWindow *native = (SwNativeWindow *)window->native_window;
                    SDL_DestroyRenderer(native->sdl_renderer);
                    SDL_DestroyWindow(native->sdl_window);
                    free(native);
                    window->native_window = NULL;
                }
            }
        }
        
        if (visible_windows == 0) {
            _app_running = 0;
        }
        
        SDL_Delay(16); // Cap at 60 FPS
    }
    
    return _exit_code;
}

void sw_app_exit(int exit_code) {
    _app_running = 0;
    _exit_code = exit_code;
}

/* ============================================================================
 * VIEW MANAGEMENT
 * ============================================================================ */

SwView* sw_view_create(SwRect rect) {
    SwView *view = (SwView *)malloc(sizeof(SwView));
    if (!view) return NULL;
    
    view->frame = rect;
    view->background_color = SW_COLOR_WHITE;
    view->hidden = 0;
    view->subviews = NULL;
    view->subview_count = 0;
    view->subview_capacity = 0;
    view->user_data = NULL;
    view->draw = NULL;
    view->event = NULL;
    view->retain_count = 1;
    
    return view;
}

void sw_view_release(SwView *view) {
    if (!view) return;
    
    view->retain_count--;
    if (view->retain_count > 0) return;
    
    /* Clean up global keyboard focus if this view is being deleted */
    if (_focused_view == view) {
        _focused_view = NULL;
    }
    
    /* Release subviews */
    for (size_t i = 0; i < view->subview_count; i++) {
        sw_view_release(view->subviews[i]);
    }
    if (view->subviews) free(view->subviews);
    
    free(view);
}

void sw_view_set_background_color(SwView *view, SwColor color) {
    if (!view) return;
    view->background_color = color;
}

void sw_view_set_frame(SwView *view, SwRect frame) {
    if (!view) return;
    view->frame = frame;
}

SwRect sw_view_get_frame(SwView *view) {
    if (!view) return sw_rect_make(0, 0, 0, 0);
    return view->frame;
}

void sw_view_set_hidden(SwView *view, int hidden) {
    if (!view) return;
    view->hidden = hidden;
}

void sw_view_add_subview(SwView *parent, SwView *child) {
    if (!parent || !child) return;
    
    if (parent->subview_count >= parent->subview_capacity) {
        parent->subview_capacity = (parent->subview_capacity + 1) * 2;
        SwView **new_subviews = (SwView **)realloc(parent->subviews, parent->subview_capacity * sizeof(SwView *));
        if (!new_subviews) return;
        parent->subviews = new_subviews;
    }
    
    parent->subviews[parent->subview_count++] = child;
}

void sw_view_remove_subview(SwView *parent, SwView *child) {
    if (!parent || !child) return;
    
    for (size_t i = 0; i < parent->subview_count; i++) {
        if (parent->subviews[i] == child) {
            parent->subviews[i] = parent->subviews[parent->subview_count - 1];
            parent->subview_count--;
            break;
        }
    }
}
