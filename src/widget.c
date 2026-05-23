/**
 * Widget Implementation using SDL2 and SDL2_ttf
 */

#include "../include/swipe.h"
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>

/* Declare external functions exported by window.c and core.c */
extern TTF_Font* sw_get_font(int size);
extern SwView* sw_view_get_focused(void);
extern void sw_view_set_focused(SwView *view);

/* Duplicate SwView structure definition to access fields */
struct SwView {
    int retain_count;
    SwRect frame;
    SwColor background_color;
    int hidden;
    SwView **subviews;
    size_t subview_count;
    size_t subview_capacity;
    void *user_data;
    
    /* Drawing and event callbacks */
    void (*draw)(SwView *view, SDL_Renderer *renderer, SwPoint offset);
    void (*event)(SwView *view, SDL_Event *event, SwPoint offset);
};

/* Button structure */
struct SwButton {
    int retain_count;
    SwView *view;
    char *title;
    int enabled;
    SwCallback on_click;
    void *on_click_context;
    SwState state;
};

/* Label structure */
struct SwLabel {
    int retain_count;
    SwView *view;
    char *text;
    SwColor text_color;
    SwAlignment alignment;
    float font_size;
};

/* TextField structure */
struct SwTextField {
    int retain_count;
    SwView *view;
    char *text;
    char *placeholder;
    SwCallback on_change;
    void *on_change_context;
};

/* ============================================================================
 * BUTTON WIDGET
 * ============================================================================ */

static void button_draw(SwView *view, SDL_Renderer *renderer, SwPoint offset) {
    SwButton *button = (SwButton *)view->user_data;
    if (!button) return;
    
    SwColor bg_color = SW_COLOR_LIGHT_GRAY;
    SwColor text_color = SW_COLOR_BLACK;
    
    if (!button->enabled) {
        bg_color = sw_color_rgb8(180, 180, 180);
        text_color = sw_color_rgb8(120, 120, 120);
    } else if (button->state == SW_STATE_PRESSED) {
        bg_color = sw_color_rgb8(160, 160, 160);
    } else if (button->state == SW_STATE_HOVER) {
        bg_color = sw_color_rgb8(220, 220, 220);
    }
    
    // Draw button background
    SDL_SetRenderDrawColor(renderer, 
        (Uint8)(bg_color.red * 255.0f), 
        (Uint8)(bg_color.green * 255.0f), 
        (Uint8)(bg_color.blue * 255.0f), 
        (Uint8)(bg_color.alpha * 255.0f)
    );
    SDL_Rect rect = { (int)offset.x, (int)offset.y, (int)view->frame.size.width, (int)view->frame.size.height };
    SDL_RenderFillRect(renderer, &rect);
    
    // Draw border
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    // Draw centered title text
    if (button->title && strlen(button->title) > 0) {
        TTF_Font *font = sw_get_font(14);
        if (font) {
            SDL_Color sdl_color = {
                (Uint8)(text_color.red * 255.0f),
                (Uint8)(text_color.green * 255.0f),
                (Uint8)(text_color.blue * 255.0f),
                255
            };
            SDL_Surface *surf = TTF_RenderText_Blended(font, button->title, sdl_color);
            if (surf) {
                SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
                if (tex) {
                    int w = surf->w;
                    int h = surf->h;
                    SDL_Rect dst = {
                        (int)(offset.x + (view->frame.size.width - w) / 2),
                        (int)(offset.y + (view->frame.size.height - h) / 2),
                        w,
                        h
                    };
                    SDL_RenderCopy(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                }
                SDL_FreeSurface(surf);
            }
        }
    }
}

static void button_event(SwView *view, SDL_Event *event, SwPoint offset) {
    SwButton *button = (SwButton *)view->user_data;
    if (!button || !button->enabled) return;
    
    float x = offset.x + view->frame.origin.x;
    float y = offset.y + view->frame.origin.y;
    float w = view->frame.size.width;
    float h = view->frame.size.height;
    
    if (event->type == SDL_MOUSEMOTION) {
        float mx = event->motion.x;
        float my = event->motion.y;
        if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
            if (button->state != SW_STATE_PRESSED) {
                button->state = SW_STATE_HOVER;
            }
        } else {
            button->state = SW_STATE_NORMAL;
        }
    } else if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            float mx = event->button.x;
            float my = event->button.y;
            if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
                button->state = SW_STATE_PRESSED;
            }
        }
    } else if (event->type == SDL_MOUSEBUTTONUP) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            float mx = event->button.x;
            float my = event->button.y;
            if (button->state == SW_STATE_PRESSED) {
                if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
                    button->state = SW_STATE_HOVER;
                    if (button->on_click) {
                        button->on_click((SwObject *)button, button->on_click_context);
                    }
                } else {
                    button->state = SW_STATE_NORMAL;
                }
            }
        }
    }
}

SwButton* sw_button_create(const char *title, SwRect rect) {
    SwButton *button = (SwButton *)malloc(sizeof(SwButton));
    if (!button) return NULL;
    
    button->view = sw_view_create(rect);
    if (!button->view) {
        free(button);
        return NULL;
    }
    
    button->title = (char *)malloc(strlen(title) + 1);
    if (!button->title) {
        sw_view_release(button->view);
        free(button);
        return NULL;
    }
    strcpy(button->title, title);
    
    button->enabled = 1;
    button->on_click = NULL;
    button->on_click_context = NULL;
    button->state = SW_STATE_NORMAL;
    button->retain_count = 1;
    
    /* Connect callbacks and user data to parent SwView */
    button->view->user_data = button;
    button->view->draw = button_draw;
    button->view->event = button_event;
    
    sw_view_set_background_color(button->view, sw_color_rgb8(200, 200, 200));
    
    return button;
}

void sw_button_release(SwButton *button) {
    if (!button) return;
    
    button->retain_count--;
    if (button->retain_count > 0) return;
    
    if (button->title) free(button->title);
    if (button->view) sw_view_release(button->view);
    free(button);
}

void sw_button_set_title(SwButton *button, const char *title) {
    if (!button || !title) return;
    
    char *new_title = (char *)malloc(strlen(title) + 1);
    if (!new_title) return;
    
    strcpy(new_title, title);
    if (button->title) free(button->title);
    button->title = new_title;
}

const char* sw_button_get_title(SwButton *button) {
    if (!button) return NULL;
    return button->title;
}

void sw_button_set_on_click(SwButton *button, SwCallback callback, void *context) {
    if (!button) return;
    button->on_click = callback;
    button->on_click_context = context;
}

void sw_button_set_enabled(SwButton *button, int enabled) {
    if (!button) return;
    button->enabled = enabled;
    button->state = enabled ? SW_STATE_NORMAL : SW_STATE_DISABLED;
}

int sw_button_is_enabled(SwButton *button) {
    if (!button) return 0;
    return button->enabled;
}

SwView* sw_button_to_view(SwButton *button) {
    if (!button) return NULL;
    return button->view;
}

/* ============================================================================
 * LABEL WIDGET
 * ============================================================================ */

static void label_draw(SwView *view, SDL_Renderer *renderer, SwPoint offset) {
    SwLabel *label = (SwLabel *)view->user_data;
    if (!label || !label->text || strlen(label->text) == 0) return;
    
    TTF_Font *font = sw_get_font((int)label->font_size);
    if (!font) return;
    
    SDL_Color sdl_color = {
        (Uint8)(label->text_color.red * 255.0f),
        (Uint8)(label->text_color.green * 255.0f),
        (Uint8)(label->text_color.blue * 255.0f),
        (Uint8)(label->text_color.alpha * 255.0f)
    };
    
    SDL_Surface *surf = TTF_RenderText_Blended(font, label->text, sdl_color);
    if (!surf) return;
    
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (tex) {
        int w = surf->w;
        int h = surf->h;
        
        int text_x = (int)offset.x;
        if (label->alignment == SW_ALIGN_CENTER) {
            text_x = (int)(offset.x + (view->frame.size.width - w) / 2);
        } else if (label->alignment == SW_ALIGN_RIGHT) {
            text_x = (int)(offset.x + view->frame.size.width - w);
        }
        
        int text_y = (int)(offset.y + (view->frame.size.height - h) / 2);
        
        SDL_Rect dst = { text_x, text_y, w, h };
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

SwLabel* sw_label_create(const char *text, SwRect rect) {
    SwLabel *label = (SwLabel *)malloc(sizeof(SwLabel));
    if (!label) return NULL;
    
    label->view = sw_view_create(rect);
    if (!label->view) {
        free(label);
        return NULL;
    }
    
    label->text = (char *)malloc(strlen(text) + 1);
    if (!label->text) {
        sw_view_release(label->view);
        free(label);
        return NULL;
    }
    strcpy(label->text, text);
    
    label->text_color = SW_COLOR_BLACK;
    label->alignment = SW_ALIGN_LEFT;
    label->font_size = 14.0f;
    label->retain_count = 1;
    
    label->view->user_data = label;
    label->view->draw = label_draw;
    
    sw_view_set_background_color(label->view, SW_COLOR_TRANSPARENT);
    
    return label;
}

void sw_label_release(SwLabel *label) {
    if (!label) return;
    
    label->retain_count--;
    if (label->retain_count > 0) return;
    
    if (label->text) free(label->text);
    if (label->view) sw_view_release(label->view);
    free(label);
}

void sw_label_set_text(SwLabel *label, const char *text) {
    if (!label || !text) return;
    
    char *new_text = (char *)malloc(strlen(text) + 1);
    if (!new_text) return;
    
    strcpy(new_text, text);
    if (label->text) free(label->text);
    label->text = new_text;
}

const char* sw_label_get_text(SwLabel *label) {
    if (!label) return NULL;
    return label->text;
}

void sw_label_set_text_color(SwLabel *label, SwColor color) {
    if (!label) return;
    label->text_color = color;
}

void sw_label_set_alignment(SwLabel *label, SwAlignment alignment) {
    if (!label) return;
    label->alignment = alignment;
}

void sw_label_set_font_size(SwLabel *label, float size) {
    if (!label) return;
    if (size > 0) {
        label->font_size = size;
    }
}

SwView* sw_label_to_view(SwLabel *label) {
    if (!label) return NULL;
    return label->view;
}

/* ============================================================================
 * TEXT FIELD WIDGET
 * ============================================================================ */

static void text_field_draw(SwView *view, SDL_Renderer *renderer, SwPoint offset) {
    SwTextField *tf = (SwTextField *)view->user_data;
    if (!tf) return;
    
    int is_focused = (sw_view_get_focused() == view);
    
    // Draw background (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rect = { (int)offset.x, (int)offset.y, (int)view->frame.size.width, (int)view->frame.size.height };
    SDL_RenderFillRect(renderer, &rect);
    
    // Draw border (blue if focused, light gray if not)
    if (is_focused) {
        SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    }
    SDL_RenderDrawRect(renderer, &rect);
    
    TTF_Font *font = sw_get_font(14);
    if (!font) return;
    
    int draw_placeholder = (strlen(tf->text) == 0);
    const char *display_text = draw_placeholder ? tf->placeholder : tf->text;
    
    SDL_Color text_color;
    if (draw_placeholder) {
        text_color = (SDL_Color){ 160, 160, 160, 255 }; // Gray
    } else {
        text_color = (SDL_Color){ 0, 0, 0, 255 }; // Black
    }
    
    int text_w = 0;
    int text_h = 0;
    
    if (display_text && strlen(display_text) > 0) {
        SDL_Surface *surf = TTF_RenderText_Blended(font, display_text, text_color);
        if (surf) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
            if (tex) {
                text_w = surf->w;
                text_h = surf->h;
                
                SDL_Rect dst = {
                    (int)(offset.x + 8),
                    (int)(offset.y + (view->frame.size.height - text_h) / 2),
                    text_w,
                    text_h
                };
                SDL_RenderCopy(renderer, tex, NULL, &dst);
                SDL_DestroyTexture(tex);
            }
            SDL_FreeSurface(surf);
        }
    } else {
        TTF_SizeText(font, " ", NULL, &text_h);
    }
    
    // Draw blinking vertical text cursor if focused
    if (is_focused) {
        int cursor_visible = (SDL_GetTicks() / 500) % 2;
        if (cursor_visible) {
            int actual_text_w = 0;
            if (strlen(tf->text) > 0) {
                TTF_SizeText(font, tf->text, &actual_text_w, NULL);
            }
            
            int cursor_x = (int)(offset.x + 8 + actual_text_w);
            int cursor_y1 = (int)(offset.y + (view->frame.size.height - text_h) / 2);
            int cursor_y2 = cursor_y1 + text_h;
            
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawLine(renderer, cursor_x, cursor_y1, cursor_x, cursor_y2);
        }
    }
}

static void text_field_event(SwView *view, SDL_Event *event, SwPoint offset) {
    SwTextField *tf = (SwTextField *)view->user_data;
    if (!tf) return;
    
    float x = offset.x + view->frame.origin.x;
    float y = offset.y + view->frame.origin.y;
    float w = view->frame.size.width;
    float h = view->frame.size.height;
    
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            float mx = event->button.x;
            float my = event->button.y;
            if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
                sw_view_set_focused(view);
            }
        }
    } else if (event->type == SDL_TEXTINPUT) {
        if (sw_view_get_focused() == view) {
            size_t current_len = strlen(tf->text);
            size_t input_len = strlen(event->text.text);
            if (current_len + input_len < 256) {
                strcat(tf->text, event->text.text);
                if (tf->on_change) {
                    tf->on_change((SwObject *)tf, tf->on_change_context);
                }
            }
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (sw_view_get_focused() == view) {
            if (event->key.keysym.sym == SDLK_BACKSPACE) {
                size_t len = strlen(tf->text);
                if (len > 0) {
                    tf->text[len - 1] = '\0';
                    if (tf->on_change) {
                        tf->on_change((SwObject *)tf, tf->on_change_context);
                    }
                }
            }
        }
    }
}

SwTextField* sw_text_field_create(SwRect rect) {
    SwTextField *text_field = (SwTextField *)malloc(sizeof(SwTextField));
    if (!text_field) return NULL;
    
    text_field->view = sw_view_create(rect);
    if (!text_field->view) {
        free(text_field);
        return NULL;
    }
    
    text_field->text = (char *)malloc(256);
    if (!text_field->text) {
        sw_view_release(text_field->view);
        free(text_field);
        return NULL;
    }
    text_field->text[0] = '\0';
    
    text_field->placeholder = (char *)malloc(256);
    if (!text_field->placeholder) {
        free(text_field->text);
        sw_view_release(text_field->view);
        free(text_field);
        return NULL;
    }
    text_field->placeholder[0] = '\0';
    
    text_field->on_change = NULL;
    text_field->on_change_context = NULL;
    text_field->retain_count = 1;
    
    text_field->view->user_data = text_field;
    text_field->view->draw = text_field_draw;
    text_field->view->event = text_field_event;
    
    sw_view_set_background_color(text_field->view, sw_color_rgb8(255, 255, 255));
    
    return text_field;
}

void sw_text_field_release(SwTextField *text_field) {
    if (!text_field) return;
    
    text_field->retain_count--;
    if (text_field->retain_count > 0) return;
    
    if (text_field->text) free(text_field->text);
    if (text_field->placeholder) free(text_field->placeholder);
    if (text_field->view) sw_view_release(text_field->view);
    free(text_field);
}

void sw_text_field_set_text(SwTextField *text_field, const char *text) {
    if (!text_field || !text) return;
    
    if (strlen(text) < 256) {
        strcpy(text_field->text, text);
    }
}

const char* sw_text_field_get_text(SwTextField *text_field) {
    if (!text_field) return NULL;
    return text_field->text;
}

void sw_text_field_set_placeholder(SwTextField *text_field, const char *placeholder) {
    if (!text_field || !placeholder) return;
    
    if (strlen(placeholder) < 256) {
        strcpy(text_field->placeholder, placeholder);
    }
}

void sw_text_field_set_on_change(SwTextField *text_field, SwCallback callback, void *context) {
    if (!text_field) return;
    text_field->on_change = callback;
    text_field->on_change_context = context;
}

SwView* sw_text_field_to_view(SwTextField *text_field) {
    if (!text_field) return NULL;
    return text_field->view;
}
