/**
 * Swipe Core Implementation using SDL2
 */

#include "../include/swipe.h"
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>

/* Global state */
static int _swipe_initialized = 0;

/* Color definitions */
const SwColor SW_COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
const SwColor SW_COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
const SwColor SW_COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
const SwColor SW_COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
const SwColor SW_COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
const SwColor SW_COLOR_GRAY = {0.5f, 0.5f, 0.5f, 1.0f};
const SwColor SW_COLOR_LIGHT_GRAY = {0.8f, 0.8f, 0.8f, 1.0f};
const SwColor SW_COLOR_DARK_GRAY = {0.2f, 0.2f, 0.2f, 1.0f};
const SwColor SW_COLOR_TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};

#define FONT_CACHE_SIZE 10
static struct {
    int size;
    TTF_Font *font;
} _font_cache[FONT_CACHE_SIZE];

/* Font cache retrieval helper - exported for widget.c */
TTF_Font* sw_get_font(int size) {
    if (size <= 0) size = 14;
    for (int i = 0; i < FONT_CACHE_SIZE; i++) {
        if (_font_cache[i].size == size && _font_cache[i].font) {
            return _font_cache[i].font;
        }
    }
    
    int slot = -1;
    for (int i = 0; i < FONT_CACHE_SIZE; i++) {
        if (!_font_cache[i].font) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        TTF_CloseFont(_font_cache[0].font);
        slot = 0;
    }
    
    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", size);
    if (!font) {
        font = TTF_OpenFont("/System/Library/Fonts/Geneva.ttf", size);
    }
    _font_cache[slot].size = size;
    _font_cache[slot].font = font;
    return font;
}

/* ============================================================================
 * INITIALIZATION AND CLEANUP
 * ============================================================================ */

int swipe_init(void) {
    if (_swipe_initialized) {
        return 0;
    }
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        return -1;
    }
    
    if (TTF_Init() < 0) {
        SDL_Quit();
        return -1;
    }
    
    // Clear font cache
    for (int i = 0; i < FONT_CACHE_SIZE; i++) {
        _font_cache[i].size = 0;
        _font_cache[i].font = NULL;
    }
    
    _swipe_initialized = 1;
    return 0;
}

void swipe_shutdown(void) {
    if (_swipe_initialized) {
        // Close all cached fonts
        for (int i = 0; i < FONT_CACHE_SIZE; i++) {
            if (_font_cache[i].font) {
                TTF_CloseFont(_font_cache[i].font);
                _font_cache[i].font = NULL;
                _font_cache[i].size = 0;
            }
        }
        
        TTF_Quit();
        SDL_Quit();
        _swipe_initialized = 0;
    }
}

int swipe_is_initialized(void) {
    return _swipe_initialized;
}

/* ============================================================================
 * COLOR UTILITIES
 * ============================================================================ */

SwColor sw_color_rgb(float red, float green, float blue) {
    return sw_color_rgba(red, green, blue, 1.0f);
}

SwColor sw_color_rgba(float red, float green, float blue, float alpha) {
    SwColor color;
    color.red = red;
    color.green = green;
    color.blue = blue;
    color.alpha = alpha;
    return color;
}

SwColor sw_color_rgb8(uint8_t red, uint8_t green, uint8_t blue) {
    return sw_color_rgba8(red, green, blue, 255);
}

SwColor sw_color_rgba8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    SwColor color;
    color.red = red / 255.0f;
    color.green = green / 255.0f;
    color.blue = blue / 255.0f;
    color.alpha = alpha / 255.0f;
    return color;
}

/* ============================================================================
 * RECT AND SIZE UTILITIES
 * ============================================================================ */

SwRect sw_rect_make(float x, float y, float width, float height) {
    SwRect rect;
    rect.origin.x = x;
    rect.origin.y = y;
    rect.size.width = width;
    rect.size.height = height;
    return rect;
}

SwSize sw_size_make(float width, float height) {
    SwSize size;
    size.width = width;
    size.height = height;
    return size;
}

SwPoint sw_point_make(float x, float y) {
    SwPoint point;
    point.x = x;
    point.y = y;
    return point;
}

/* ============================================================================
 * REFERENCE COUNTING
 * ============================================================================ */

SwObject* sw_retain(SwObject *object) {
    if (!object) return NULL;
    
    int *retain_count = (int *)object;
    (*retain_count)++;
    
    return object;
}

void sw_release(SwObject *object) {
    if (!object) return;
    
    int *retain_count = (int *)object;
    (*retain_count)--;
}

int sw_get_retain_count(SwObject *object) {
    if (!object) return 0;
    
    int *retain_count = (int *)object;
    return *retain_count;
}
