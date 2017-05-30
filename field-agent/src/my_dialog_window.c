/*
 * my_dialog_window.c - see my_dialog_window.h for more information
 * 
 * much of this code was inpired from: (I changed parts to suit my needs)
 * https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/dialog_message_window.c
 * 
 * Paolo Takagi-Atilano
 */

// figure out "type" TextLayer for title purposes
// may have fixed the memory leaks, check out later

#include <pebble.h>
#include "my_dialog_window.h"

// windows and layers
static Window *my_dialog_window;
static TextLayer *my_dialog_text_layer;
//static TextLayer *my_dialog_type_layer;
static Layer *s_background_layer;

// text and type strings
//static char *type;
static char *text;

// See link
static Animation *s_appear_anim = NULL;

// See link
static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
  s_appear_anim = NULL;
}

// See link
static void background_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorYellow, GColorWhite));
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, 0);
}

// See link
static void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // background layer
  const GEdgeInsets background_insets = {.top = bounds.size.h  /* Start hidden */};
  s_background_layer = layer_create(grect_inset(bounds, background_insets));
  layer_set_update_proc(s_background_layer, background_update_proc);
  layer_add_child(window_layer, s_background_layer);

  // type layer
  /*my_dialog_type_layer = text_layer_create(GRect(TYPE_MESSAGE_WINDOW_MARGIN, bounds.size.h + TYPE_MESSAGE_WINDOW_MARGIN, bounds.size.w - (2 * TYPE_MESSAGE_WINDOW_MARGIN), bounds.size.h));
  text_layer_set_text(my_dialog_type_layer, type);
  text_layer_set_background_color(my_dialog_type_layer, GColorClear);
  text_layer_set_text_alignment(my_dialog_type_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  text_layer_set_font(my_dialog_type_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(my_dialog_type_layer));*/

  // text layer
  my_dialog_text_layer = text_layer_create(GRect(DIALOG_MESSAGE_WINDOW_MARGIN, bounds.size.h + DIALOG_MESSAGE_WINDOW_MARGIN, bounds.size.w - (2 * DIALOG_MESSAGE_WINDOW_MARGIN), bounds.size.h));
  text_layer_set_text(my_dialog_text_layer, text);
  text_layer_set_background_color(my_dialog_text_layer, GColorClear);
  text_layer_set_text_alignment(my_dialog_text_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  text_layer_set_font(my_dialog_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_layer, text_layer_get_layer(my_dialog_text_layer));
}

// See link
static void window_unload(Window *window)
{
  layer_destroy(s_background_layer);
  text_layer_destroy(my_dialog_text_layer);

  window_destroy(window);
  my_dialog_window = NULL;

  //free(type);
  if (text) {
    free(text);
  }
  //free(text);
  //type = NULL;
  //text = NULL;
}

// See link
static void window_appear(Window *window)
{
  if(s_appear_anim) {
    // In progress, cancel
    animation_unschedule(s_appear_anim);
  }
  GRect bounds = layer_get_bounds(window_get_root_layer(window));
  Layer *label_layer = text_layer_get_layer(my_dialog_text_layer);
  //Layer *header_layer = text_layer_get_layer

  GRect start = layer_get_frame(s_background_layer);
  GRect finish = bounds;
  Animation *background_anim = (Animation*)property_animation_create_layer_frame(s_background_layer, &start, &finish);

  // Animate label_layer
  start = layer_get_frame(label_layer);
  const GEdgeInsets finish_insets = {
    .top = DIALOG_MESSAGE_WINDOW_MARGIN + 5 /* small adjustment */,
    .right = DIALOG_MESSAGE_WINDOW_MARGIN, .left = DIALOG_MESSAGE_WINDOW_MARGIN};
  finish = grect_inset(bounds, finish_insets);
  Animation *label_anim = (Animation*)property_animation_create_layer_frame(label_layer, &start, &finish);

  s_appear_anim = animation_spawn_create(background_anim, label_anim, NULL);
  animation_set_handlers(s_appear_anim, (AnimationHandlers) {
    .stopped = anim_stopped_handler
  }, NULL);
  animation_set_delay(s_appear_anim, 700);
  animation_schedule(s_appear_anim);
}

void my_dialog_window_push(/*char *p_type, */char *p_text)
{
  // only pushes window if p_type and p_text are not null
  if (/*p_type != NULL && */p_text != NULL) {
    //type = calloc(sizeof(p_type), strlen(p_type));
    text = calloc(sizeof(p_text), strlen(p_text));
    if (/*type == NULL || */text == NULL) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Malloc error");
      return;
    }
    //strcpy(type, p_type);
    strcpy(text, p_text);

    if(!my_dialog_window) {
      my_dialog_window = window_create();
      window_set_background_color(my_dialog_window, GColorBlack);
      window_set_window_handlers(my_dialog_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
        .appear = window_appear
    });
  }
  window_stack_push(my_dialog_window, true);
  }
}

void my_dialog_window_pop()
{
  //window_unload(my_dialog_window);
  window_stack_pop(my_dialog_window);
}