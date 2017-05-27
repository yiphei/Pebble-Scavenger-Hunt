#include <pebble.h>
#include "pin_window.h"
#include "selection_layer.h"

static char hex_digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static char* selection_handle_get_text(int index, void *context) {
  PinWindow *pin_window = (PinWindow*)context;
  snprintf(
    pin_window->field_buffs[index], 
    sizeof(pin_window->field_buffs[0]), "%c",
    (char)pin_window->pin.digits[index]
  );
  return pin_window->field_buffs[index];
}

static void selection_handle_complete(void *context) {
  PinWindow *pin_window = (PinWindow*)context;
  pin_window->callbacks.pin_complete(pin_window->pin, pin_window);
}

// Paolo wrote this one
static void selection_handle_inc(int index, uint8_t clicks, void *context) {
  PinWindow *pin_window = (PinWindow*)context;

  if (pin_window->pin.digits[index] == hex_digits[MAX_VALUE]) {
    pin_window->pin.digits[index] = hex_digits[0];
  } else {
    for (int i = 0; i < MAX_VALUE; i++) {
      if (pin_window->pin.digits[index] == hex_digits[i]) {
        pin_window->pin.digits[index] = hex_digits[i+1];
        return;
      }
    }
  }
}

// Paolo wrote this one
static void selection_handle_dec(int index, uint8_t clicks, void *context) {
  PinWindow *pin_window = (PinWindow*)context;

  if (pin_window->pin.digits[index] == hex_digits[0]) {
    pin_window->pin.digits[index] = hex_digits[MAX_VALUE];
  } else {
    for (int i = 1; i < MAX_VALUE+1; i++) {
      if (pin_window->pin.digits[index] == hex_digits[i]) {
        pin_window->pin.digits[index] = hex_digits[i-1];
        return;
      }
    }
  }
}

PinWindow* pin_window_create(PinWindowCallbacks callbacks) {
  PinWindow *pin_window = (PinWindow*)malloc(sizeof(PinWindow));
  if (pin_window) {
    pin_window->window = window_create();
    pin_window->callbacks = callbacks;
    if (pin_window->window) {
      pin_window->field_selection = 0;
      for(int i = 0; i < NUM_CELLS; i++) {
        pin_window->pin.digits[i] = '0';
      }
      
      // Get window parameters
      Layer *window_layer = window_get_root_layer(pin_window->window);
      GRect bounds = layer_get_bounds(window_layer);
      
      // Main TextLayer
      pin_window->main_text = text_layer_create(GRect(0, 30, bounds.size.w, 40));

      text_layer_set_text(pin_window->main_text, "Enter Krag Code");
      text_layer_set_font(pin_window->main_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
      text_layer_set_text_alignment(pin_window->main_text, GTextAlignmentCenter);
      layer_add_child(window_layer, text_layer_get_layer(pin_window->main_text));
      
      // Sub TextLayer
      //pin_window->sub_text = text_layer_create(GRect(1, 125, bounds.size.w, 40));

      //text_layer_set_text(pin_window->sub_text, "Enter KRAG Code");
      //text_layer_set_text_alignment(pin_window->sub_text, GTextAlignmentCenter);
      //text_layer_set_font(pin_window->sub_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
      //layer_add_child(window_layer, text_layer_get_layer(pin_window->sub_text));
      
      // Create selection layer
      pin_window->selection = selection_layer_create(GRect(8, 75, 128, 34), NUM_CELLS);

      for (int i = 0; i < NUM_CELLS; i++) {
        selection_layer_set_cell_width(pin_window->selection, i, 28);
      }
      selection_layer_set_cell_padding(pin_window->selection, 4);
      selection_layer_set_active_bg_color(pin_window->selection, GColorRed);
      selection_layer_set_inactive_bg_color(pin_window->selection, GColorDarkGray);

      selection_layer_set_click_config_onto_window(pin_window->selection, pin_window->window);
      selection_layer_set_callbacks(pin_window->selection, pin_window, (SelectionLayerCallbacks) {
        .get_cell_text = selection_handle_get_text,
        .complete = selection_handle_complete,
        .increment = selection_handle_inc,
        .decrement = selection_handle_dec,
      });
      layer_add_child(window_get_root_layer(pin_window->window), pin_window->selection);

      // Create status bar
      //pin_window->status = status_bar_layer_create();
      //status_bar_layer_set_colors(pin_window->status, GColorClear, GColorBlack);
      //layer_add_child(window_layer, status_bar_layer_get_layer(pin_window->status));
      return pin_window;
    }
  }

  APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create PinWindow");
  return NULL;
}

void pin_window_destroy(PinWindow *pin_window) {
  if (pin_window) {
    //status_bar_layer_destroy(pin_window->status);
    selection_layer_destroy(pin_window->selection);
    //text_layer_destroy(pin_window->sub_text);
    text_layer_destroy(pin_window->main_text);
    free(pin_window);
    pin_window = NULL;
    return;
  }
}

void pin_window_push(PinWindow *pin_window, bool animated) {
  window_stack_push(pin_window->window, animated);
}

void pin_window_pop(PinWindow *pin_window, bool animated) {
  window_stack_remove(pin_window->window, animated);
}

bool pin_window_get_topmost_window(PinWindow *pin_window) {
  return window_stack_get_top_window() == pin_window->window;
}

void pin_window_set_highlight_color(PinWindow *pin_window, GColor color) {
  pin_window->highlight_color = color;
  selection_layer_set_active_bg_color(pin_window->selection, color);
}
