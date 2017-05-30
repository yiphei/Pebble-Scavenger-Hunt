/*
 * main module for field agent pebble app
 * See ../README.md for more information
 *
 * Paolo Takagi-Atilano, GREP, May 2017
 */
#include <pebble.h>
#include "my_dialog_window.h"
#include "key_assembly.h"
#include "p_message.h"

// Macros
#define RADIO_BUTTON_WINDOW_RADIO_RADIUS 6
#define MENU_CELL_HEIGHT                 44
#define JOIN_GAME_NUM_ROWS               3
#define SELECT_PLAYER_NUM_ROWS           4
#define SELECT_TEAM_NUM_ROWS             4
#define IN_GAME_NUM_ROWS                 3
#define CLAIM_KRAG_NUM_ROWS              16

#define MESSAGE_LENGTH 8191


// Globals
static Window *claim_krag_window;
static Window *join_game_window;
static Window *select_player_window;
static Window *select_team_window;
static Window *in_game_window;

static MenuLayer *claim_krag_layer;
static MenuLayer *join_game_layer;
static MenuLayer *select_player_layer;
static MenuLayer *select_team_layer;
static MenuLayer *in_game_layer;

static char *hex_digits_str[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
static char hex_digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
static char *player_names[SELECT_PLAYER_NUM_ROWS] = {"paolo", "tony", "michael", "yifei"};
static char *team_names[SELECT_TEAM_NUM_ROWS] = {"one", "two", "three", "four"};

static char curr_hex;
static char curr_krag[5] = {'-', '-', '-', '-'};

static char *opcode;
static char *game_id;
static char *pebble_id;
static char *selected_player;
static char *selected_team;
static char *latitude;
static char *longitude;
static char *curr_msg_str;
static char *guide_id;
static char *hints_log_rec;

static p_message_t *curr_msg;

static int claim_krag_count = 0;
static int s_current_selection = -1;
static int tick_count = 0;
static int end_of_game_tick = 0;
static bool js_running = false; // default assume smartphone JS proxy is not running
static bool end_of_game = false; // default game is not over
static bool u_proxy_up = false; // default assume unix proxy is not running

// static function defintions
static void init();
static void deinit();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void join_game_window_load(Window *window);
static void join_game_window_unload(Window *window);
static uint16_t join_game_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);
static void join_game_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);
static int16_t join_game_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);
static void join_game_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void select_player_window_load(Window *window);
static void select_player_window_unload(Window *window);
static uint16_t select_player_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);
static void select_player_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);
static int16_t select_player_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);
static void select_player_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void select_team_window_load(Window *window);
static void select_team_window_unload(Window *window);
static uint16_t select_team_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);
static void select_team_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);
static int16_t select_team_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);
static void select_team_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void in_game_window_load(Window *window);
static void in_game_window_unload(Window *window);
static uint16_t in_game_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);
static void in_game_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);
static int16_t in_game_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);
static void in_game_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static void claim_krag_window_load(Window *window);
static void claim_krag_window_unload(Window *window);
static uint16_t claim_krag_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);
static void claim_krag_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);
static int16_t claim_krag_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);
static void claim_krag_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static void send_FA_LOCATION(int status);
static void send_FA_CLAIM(char *krag_id);
static void send_FA_LOG(char *text);

static void handle_message();

static void set_location(char *info);
static void set_pebble_id(char *info);
static void set_game_id(char *info);
static void set_guide_id(char *info);
static void set_curr_msg_str(char *info);
static void set_opcode(char *info);
static void set_hints_log_rec(char *info);

// AppMessage functions
static void inbox_received_callback(DictionaryIterator *iterator, void *context);
static void outbox_sent_callback(DictionaryIterator *iterator, void *context);
static void inbox_dropped_callback(AppMessageResult reason, void *context);
static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context);

// my message functions
static void request_pebbleId();
static void request_location();
static void send_message(char *message);




// init
static void init() 
{
  set_game_id("0");

   // join game screen
  join_game_window = window_create();
  WindowHandlers join_game_window_handlers = {
    .load = join_game_window_load,
    .unload = join_game_window_unload
  };
  window_set_window_handlers(join_game_window, (WindowHandlers)join_game_window_handlers);

    // select player screen
  select_player_window = window_create();
  WindowHandlers select_player_window_handlers = {
    .load = select_player_window_load,
    .unload = select_player_window_unload
  };
  window_set_window_handlers(select_player_window, (WindowHandlers)select_player_window_handlers);

  // select team screen
  select_team_window = window_create();
  WindowHandlers select_team_window_handlers = {
    .load = select_team_window_load,
    .unload = select_team_window_unload
  };
  window_set_window_handlers(select_team_window, (WindowHandlers)select_team_window_handlers);

  // in game screen
  in_game_window = window_create();
  WindowHandlers in_game_window_handlers = {
    .load = in_game_window_load,
    .unload = in_game_window_unload
  };
  window_set_window_handlers(in_game_window, (WindowHandlers)in_game_window_handlers);

  // claim krag screen
  claim_krag_window = window_create();
  APP_LOG(APP_LOG_LEVEL_INFO, "created claim_krag_window");
  WindowHandlers claim_krag_window_handlers = {
    .load = claim_krag_window_load,
    .unload = claim_krag_window_unload
  };
  window_set_window_handlers(claim_krag_window, (WindowHandlers)claim_krag_window_handlers);
  

  // Show the Window on the watch, with animated=true.
  window_stack_push(join_game_window, true);

  // Set the handlers for AppMessage inbox/outbox events. Set these  
  //  handlers BEFORE calling open, otherwise you might miss a message.
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);

  // 7. open the app message communication protocol. Request as much space
  //    as possible, because our messages can be quite large at times.     
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

// main
int main(void) {
    init();
    app_event_loop();
    deinit();
}

// deinit
static void deinit() 
{
  // Destroy all the windows!
  if (join_game_window) {
    window_destroy(join_game_window);
    join_game_window = NULL;
  }
  if (select_player_window) {
    window_destroy(select_player_window);
    select_player_window = NULL;
  }
  if (select_team_window) {
    window_destroy(select_team_window);
    select_team_window = NULL;
  }
  if (claim_krag_window) {
    window_destroy(claim_krag_window);
    claim_krag_window = NULL;
  }

  // Unsubscribe from sensors.
  tick_timer_service_unsubscribe();

  // Free allocated static variables and structs
  if (selected_player) {
    free(selected_player);
  }
  if (selected_team) {
    free(selected_team);
  }
  if (pebble_id) {
    free(pebble_id);
  }
  if (latitude) {
    free(latitude);
  }
  if (longitude) {
    free(longitude);
  }
  if (game_id) {
    free(game_id);
  }
  if (curr_msg_str) {
    free(curr_msg_str);
  }
  if (guide_id) {
    free(guide_id);
  }
  if (opcode) {
    free(opcode);
  }
  if (hints_log_rec) {
    free(hints_log_rec);
  }
}

/* join game window */
static void join_game_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  join_game_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(join_game_layer, window);
  menu_layer_set_callbacks(join_game_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = join_game_get_num_rows_callback,
    .draw_row = join_game_draw_row_callback,
    .get_cell_height = join_game_get_cell_height_callback,
    .select_click = join_game_select_callback
  });
  layer_add_child(window_layer, menu_layer_get_layer(join_game_layer));
}

static void join_game_window_unload(Window *window)
{
  menu_layer_destroy(join_game_layer);
}

static uint16_t join_game_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
{
  return JOIN_GAME_NUM_ROWS;
}

static void join_game_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
{
  if (cell_index->row == 0) {
    menu_cell_basic_draw(ctx, cell_layer, "Select Player", NULL, NULL);
  } else if (cell_index->row == 1) {
    menu_cell_basic_draw(ctx, cell_layer, "Select Team", NULL, NULL);
  } else if (cell_index->row == 2) {
    menu_cell_basic_draw(ctx, cell_layer, "Join Game", NULL, NULL);
  }
}

static int16_t join_game_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    MENU_CELL_HEIGHT);
}

static void join_game_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  if (cell_index->row == 0) {
    // Select Player chosen
    s_current_selection = -1;
    window_stack_push(select_player_window, true);
  } else if (cell_index->row == 1) {
    // Select Team chosen
    s_current_selection = -1;
    window_stack_push(select_team_window, true);
  } else if (cell_index->row == 2) {
    // Join Game chosen
    s_current_selection = -1;
    if (selected_player && selected_team && pebble_id && latitude && longitude && js_running) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Everything checks out, attempting to connect to server");
      tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
      window_stack_push(in_game_window, true);
    } else {
      my_dialog_window_push("select team, player, and make sure bluetooth is on");
    }
  }
}

/* Select Player Name Menu */
static void select_player_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  select_player_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(select_player_layer, window);
  menu_layer_set_callbacks(select_player_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = select_player_get_num_rows_callback,
    .draw_row = select_player_draw_row_callback,
    .get_cell_height = select_player_get_cell_height_callback,
    .select_click = select_player_select_callback
  });
  layer_add_child(window_layer, menu_layer_get_layer(select_player_layer));
}

static void select_player_window_unload(Window *window)
{
  menu_layer_destroy(select_player_layer);
  s_current_selection = -1;
}

static uint16_t select_player_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
{
  return SELECT_PLAYER_NUM_ROWS + 1;
}

static void select_player_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
{
  if (cell_index->row == SELECT_PLAYER_NUM_ROWS) {
    // This is the submit item
    menu_cell_basic_draw(ctx, cell_layer, "Submit", NULL, NULL);
  } else {
    // This is a team name choice item
    menu_cell_basic_draw(ctx, cell_layer, player_names[(int)cell_index->row], NULL, NULL);

    GRect bounds = layer_get_bounds(cell_layer);
    GPoint p = GPoint(bounds.size.w - (3*RADIO_BUTTON_WINDOW_RADIO_RADIUS), (bounds.size.h / 2));
    
    // Selected?
    if (menu_cell_layer_is_highlighted(cell_layer)) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    } else {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }

    // Draw radio filled/empty
    graphics_draw_circle(ctx, p, RADIO_BUTTON_WINDOW_RADIO_RADIUS);
    if (cell_index->row == s_current_selection) {
      // This is the selection
      graphics_fill_circle(ctx, p, RADIO_BUTTON_WINDOW_RADIO_RADIUS - 3);
    }
  }
}

static int16_t select_player_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    MENU_CELL_HEIGHT);
}

static void select_player_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  if (cell_index->row == SELECT_PLAYER_NUM_ROWS) {
    // Select pressed
    if (selected_player) {
      window_stack_pop(true);
      s_current_selection = -1;
    }
  } else {
    // Player name selected
    s_current_selection = cell_index->row;
    selected_player = calloc(1,sizeof(player_names[s_current_selection])+1);
    strcpy(selected_player, player_names[s_current_selection]);
    menu_layer_reload_data(menu_layer);
  }
}

/* Select Team Menu */
static void select_team_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  select_team_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(select_team_layer, window);
  menu_layer_set_callbacks(select_team_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = select_team_get_num_rows_callback,
    .draw_row = select_team_draw_row_callback,
    .get_cell_height = select_team_get_cell_height_callback,
    .select_click = select_team_select_callback
  });
  layer_add_child(window_layer, menu_layer_get_layer(select_team_layer));
}

static void select_team_window_unload(Window *window)
{
  menu_layer_destroy(select_team_layer);
  s_current_selection = -1;
}

static uint16_t select_team_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
{
  return SELECT_TEAM_NUM_ROWS + 1;
}

static void select_team_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
{
  if (cell_index->row == SELECT_TEAM_NUM_ROWS) {
    // This is the submit item
    menu_cell_basic_draw(ctx, cell_layer, "Submit", NULL, NULL);
  } else {
    // This is a team name choice item
    menu_cell_basic_draw(ctx, cell_layer, team_names[(int)cell_index->row], NULL, NULL);

    GRect bounds = layer_get_bounds(cell_layer);
    GPoint p = GPoint(bounds.size.w - (3*RADIO_BUTTON_WINDOW_RADIO_RADIUS), (bounds.size.h / 2));
    
    // Selected?
    if (menu_cell_layer_is_highlighted(cell_layer)) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    } else {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }

    // Draw radio filled/empty
    graphics_draw_circle(ctx, p, RADIO_BUTTON_WINDOW_RADIO_RADIUS);
    if (cell_index->row == s_current_selection) {
      // This is the selection
      graphics_fill_circle(ctx, p, RADIO_BUTTON_WINDOW_RADIO_RADIUS - 3);
    }
  }
}

static int16_t select_team_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    MENU_CELL_HEIGHT);
}

static void select_team_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  if (cell_index->row == SELECT_TEAM_NUM_ROWS) {
    // Select pressed
    if (selected_team) {
      window_stack_pop(true);
      s_current_selection = -1;
    }
  } else {
    // Player name selected
    s_current_selection = cell_index->row;
    selected_team = calloc(1,sizeof(team_names[s_current_selection])+1);
    strcpy(selected_team, team_names[s_current_selection]);
    menu_layer_reload_data(menu_layer);
  }
}

/* In Game Menu */
static void in_game_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  in_game_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(in_game_layer, window);
  menu_layer_set_callbacks(in_game_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = in_game_get_num_rows_callback,
    .draw_row = in_game_draw_row_callback,
    .get_cell_height = in_game_get_cell_height_callback,
    .select_click = in_game_select_callback//,
  });
  layer_add_child(window_layer, menu_layer_get_layer(in_game_layer));
}

static void in_game_window_unload(Window *window)
{
  menu_layer_destroy(in_game_layer);
}

static uint16_t in_game_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
{
  return IN_GAME_NUM_ROWS;
}

static void in_game_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
{
  if (cell_index->row == 0) {
    menu_cell_basic_draw(ctx, cell_layer, "Claim Krag", NULL, NULL);
  } else if (cell_index->row == 1) {
    menu_cell_basic_draw(ctx, cell_layer, "Hints", NULL, NULL);
  } else if (cell_index->row == 2) {
    menu_cell_basic_draw(ctx, cell_layer, "Status", NULL, NULL);
  }
}

static int16_t in_game_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    MENU_CELL_HEIGHT);
}

static void in_game_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  if (!u_proxy_up) {
    window_stack_pop(true);
    my_dialog_window_push("connection problem, try again");
    return;
  }
  if (cell_index->row == 0) {
    // Claim Krag chosen
    window_stack_push(claim_krag_window, true);

  } else if (cell_index->row == 1) {
    // Hints chosen
    s_current_selection = -1;
    if (hints_log_rec != NULL) {
      my_dialog_window_push(hints_log_rec);
    } else {
      my_dialog_window_push("no hint");
    }
  } else if (cell_index->row == 2) {
    // Status chosen
    send_FA_LOCATION(1);
  }
}


/* Claim Krag Menu */
static void claim_krag_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  claim_krag_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(claim_krag_layer, window);
  menu_layer_set_callbacks(claim_krag_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = claim_krag_get_num_rows_callback,
    .draw_row = claim_krag_draw_row_callback,
    .get_cell_height = claim_krag_get_cell_height_callback,
    .select_click = claim_krag_select_callback
  });
  layer_add_child(window_layer, menu_layer_get_layer(claim_krag_layer));
}

static void claim_krag_window_unload(Window *window)
{
  menu_layer_destroy(claim_krag_layer);
  s_current_selection = -1;
  claim_krag_count = 0;
  for (int i = 0; i < 4; i++) {
    curr_krag[i] = '-';
  }
}

static uint16_t claim_krag_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
{
  return CLAIM_KRAG_NUM_ROWS + 1;
}

static void claim_krag_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
{
  if (cell_index->row == CLAIM_KRAG_NUM_ROWS) {
    // This is the submit item
    menu_cell_basic_draw(ctx, cell_layer, "Submit", NULL, NULL);
  } else {
    // This is a hex choice item
    menu_cell_basic_draw(ctx, cell_layer, hex_digits_str[(int)cell_index->row], NULL, NULL);

    GRect bounds = layer_get_bounds(cell_layer);
    GPoint p = GPoint(bounds.size.w - (3*RADIO_BUTTON_WINDOW_RADIO_RADIUS), (bounds.size.h / 2));
    
    // Selected?
    if (menu_cell_layer_is_highlighted(cell_layer)) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    } else {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }

    // Draw radio filled/empty
    graphics_draw_circle(ctx, p, RADIO_BUTTON_WINDOW_RADIO_RADIUS);
    if (cell_index->row == s_current_selection) {
      // This is the selection
      graphics_fill_circle(ctx, p, RADIO_BUTTON_WINDOW_RADIO_RADIUS - 3);
    }
  }
}

static int16_t claim_krag_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    MENU_CELL_HEIGHT);
}

static void claim_krag_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  if (cell_index->row == CLAIM_KRAG_NUM_ROWS) {
    // Select pressed
    if (claim_krag_count == 4) {
      APP_LOG(APP_LOG_LEVEL_INFO, "%s", curr_krag);
      send_FA_CLAIM(curr_krag);
      window_stack_pop(true);
      s_current_selection = -1;
      claim_krag_count = 0;
      for (int i = 0; i < 4; i++) {
        curr_krag[i] = '-';
      }
    }
  } else {
    if (claim_krag_count <= 3) {
      s_current_selection = cell_index->row;
      curr_hex = hex_digits[s_current_selection];
      curr_krag[claim_krag_count] = curr_hex;
      menu_layer_reload_data(menu_layer);
      claim_krag_count++;
      my_dialog_window_push(curr_krag);
    }
  }
}

// tick_handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (end_of_game) {
    end_of_game_tick++;
    if (end_of_game_tick >= 10) { // end of game windows displays for 10 seconds
      window_stack_pop_all(true);
    }
  }
  if (tick_count % 15 == 0) {
    send_FA_LOCATION(0);
  }
  tick_count++;
}

// outbox_sent_callback
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message successfully sent.\n");
}

// inbox_dropped_callback
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message dropped from inbox.\n");
}

// outbox_failed_callback
static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message failed to send.\n");
}

// request_pebbleId
static void request_pebbleId() {
    // Declare dictionary iterator
    DictionaryIterator *out_iter;

    // Prepare the outbox 
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    // If outbox was prepared, send request. Otherwise, log error.
    if (result == APP_MSG_OK) {
        int value = 1;

        // construct and send message, outgoing value ignored
        dict_write_int(out_iter, AppKeyPebbleId, &value, sizeof(value), true);

        result = app_message_outbox_send();

        if (result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending pebbleId request from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for pebbleId request.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Requested pebbleId.\n");
}

// request_location
static void request_location() {
    // Declare dictionary iterator
    DictionaryIterator *out_iter;

    // Prepare the outbox
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    // If outbox was prepared, send request. Otherwise, log error.
    if (result == APP_MSG_OK) {
        int value = 1;

        // construct and send message, outgoing value ignored 
        dict_write_int(out_iter, AppKeyLocation, &value, sizeof(value), true);

        result = app_message_outbox_send();

        if (result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending locationrequest from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for location request.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Requested location.\n");
}

// send_message
static void send_message(char *message) {
    // Declare dictionary iterator
    DictionaryIterator *out_iter;

    // Prepare the outbox
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    // If outbox was prepared, send message. Otherwise, log error.
    if (result == APP_MSG_OK) {
      
        // Construct and send the message */

        dict_write_cstring(out_iter, AppKeySendMsg, message);
        result = app_message_outbox_send();
        if(result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending message from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for send_message.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Sent message.\n");
}

static void send_FA_LOCATION(int status)
{
  if (game_id && pebble_id && selected_team && selected_player && latitude && longitude) {
    char *temp = malloc(MESSAGE_LENGTH);
    snprintf(temp, MESSAGE_LENGTH-1, "opCode=FA_LOCATION|gameId=%s|pebbleId=%s|team=%s|player=%s|latitude=%s|longitude=%s|statusReq=%d",
      game_id, pebble_id, selected_team, selected_player, latitude, longitude, status);
    send_message(temp);
    free(temp);
  }
}

// sends FA_CLAIM to Game Server
// opCode=FA_CLAIM|gameId=|pebbleId=|team=|player=|latitude=|longitude=|kragId=
static void send_FA_CLAIM(char *krag_id)
{
  if (game_id && pebble_id && selected_team && selected_player && latitude && longitude && krag_id) {
    char *temp = calloc(MESSAGE_LENGTH,1);
    snprintf(temp, MESSAGE_LENGTH-1, "opCode=FA_CLAIM|gameId=%s|pebbleId=%s|team=%s|player=%s|latitude=%s|longitude=%s|kragId=%s",
      game_id, pebble_id, selected_team, selected_player, latitude, longitude, krag_id);

    send_message(temp);
    free(temp);
  }
}

// sends FA_LOG to Game Server
// opCode=FA_LOG|pebbleId=|text=
static void send_FA_LOG(char *text)
{
  if (pebble_id && text) {
    char *temp = calloc(MESSAGE_LENGTH,1);
    snprintf(temp, MESSAGE_LENGTH-1, "opCode=FA_LOG|pebbleId=%s|text=%s", pebble_id, text);
    send_message(temp);
    free(temp);
  }
}

/* Setters */
// sets value of curr_msg_str
static void set_curr_msg_str(char *info)
{
  if (curr_msg_str) {
    free(curr_msg_str);
    curr_msg_str = NULL;
  }
  curr_msg_str = calloc(sizeof(info), strlen(info));
  if (curr_msg_str == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: calloc fail");
    return;
  }
  strcpy(curr_msg_str, my_strtok(info, " "));
}

// sets value of pebble_id
static void set_pebble_id(char *info)
{
  if (pebble_id) {
    free(pebble_id);
    pebble_id = NULL;
  }
  pebble_id = calloc(sizeof(info), strlen(info));
  if (pebble_id == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: calloc fail");
    return;
  }
  strcpy(pebble_id, my_strtok(info, " "));
}

// sets value of set_game_id
static void set_game_id(char *info)
{
  if (game_id) {
    free(game_id);
    game_id = NULL;
  }
  APP_LOG(APP_LOG_LEVEL_INFO, "%s", info);
  game_id = calloc(sizeof(info), strlen(info));
  if (game_id == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: calloc fail");
    return;
  }
  strcpy(game_id, my_strtok(info, " "));
}

// sets guide id
static void set_guide_id(char *info)
{
  if (guide_id) {
    free(guide_id);
    guide_id = NULL;
  }

  guide_id = calloc(sizeof(info), strlen(info));
  if (guide_id == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: calloc fail");
    return;
  }
  strcpy(guide_id, my_strtok(info, " "));
}

// sets hints log rec
static void set_hints_log_rec(char *info)
{
  if (hints_log_rec) {
    free(hints_log_rec);
    hints_log_rec = NULL;
  }

  hints_log_rec = calloc(sizeof(info), strlen(info));
  if (hints_log_rec == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: calloc fail");
    return;
  }
  strcpy(hints_log_rec, my_strtok(info, "~"));
}

// sets location (latitude and longitude strings)
static void set_location(char *info)
{
  // get rid of past location if it exists
  if (latitude) {
    free(latitude);
    latitude = NULL;
  }
  if (longitude) {
    free(longitude);
    longitude = NULL;
  }

  latitude = calloc(sizeof(info), strlen(info));
  longitude = calloc(sizeof(info), strlen(info));
  if (latitude == NULL || longitude == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: calloc fail");
    return;
  }

  strcpy(latitude, my_strtok(info, "|"));
  strcpy(longitude, my_strtok(NULL, "|"));
}

// sets opcode
static void set_opcode(char *info)
{
  if (opcode) {
    free(opcode);
  }
  opcode = calloc(sizeof(info), strlen(info));

  strcpy(opcode, my_strtok(info, " "));
}

// inbox_received_callback
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    /* Possible message types defined in key_assembly.h
    enum {
      AppKeyJSReady = 0,      // The JS environment on the companion smartphone is up and running!
      AppKeySendMsg,          // Send a message over Bluetooth to the companion smartphone and then on to the Game Server
      AppKeyRecvMsg,          // A message from the Game Server is available (arrived over Bluetooth)
      AppKeyLocation,         // Request your GPS location from the companion smartphone
      AppKeyPebbleId,         // Request your unique pebble ID from the companion smartphone
      AppKeySendError         // Error: companion app can't connect to the Proxy (and ultimately the Game Server).
    };
    */

    // Check to see if JS environment ready message received.
    Tuple *ready_tuple = dict_find(iterator, AppKeyJSReady);
    if(ready_tuple) {
        // Log the value sent as part of the received message.
        js_running = true;
        char *ready_str = ready_tuple->value->cstring;
        set_pebble_id(ready_str);

        // used for emulator:
        set_pebble_id("12345678");

        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyJSReady: %s\n", pebble_id);
        request_location();
        
    }

    // Check to see if an opcode message was received received.
    Tuple *msg_tuple = dict_find(iterator, AppKeyRecvMsg);
    if(msg_tuple) {
        // Log the value sent as part of the received message.
        char *msg = msg_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyRecvMsg: %s\n", msg);
        curr_msg = parse_message(msg);
        if (curr_msg == NULL) {
          return;
        }
        if (curr_msg->error_code == 0) {
          handle_message();
        } else {
          APP_LOG(APP_LOG_LEVEL_ERROR, "bad msg");
        }
    }

    // Check to see if a location message received.
    Tuple *loc_tuple = dict_find(iterator, AppKeyLocation);
    if(loc_tuple) {
        // Log the value sent as part of the received message.
        char *location = loc_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyLocation: %s\n", location);
        set_location(location);
    }

    // Check to see if a PebbleId message received. 
    Tuple *id_tuple = dict_find(iterator, AppKeyPebbleId);
    if(id_tuple) {
        // Log the value sent as part of the received message.
        char *id_str = id_tuple->value->cstring;
        set_pebble_id(id_str);
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyPebbleId: %s\n", id_str);
    }

    // Check to see if an error message was received.
    Tuple *error_tuple = dict_find(iterator, AppKeyLocation);
    if(error_tuple) {
        // Log the value sent as part of the received message.
        char *error = error_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeySendError: %s\n", error);
    }
}

// handles message, called by inbox_received_callback for AppKeyrecyMsg
static void handle_message()
{
  u_proxy_up = true;
  APP_LOG(APP_LOG_LEVEL_INFO, "HANDLING MESSAGE, opcode: %s", curr_msg->op_code);
  if (curr_msg && curr_msg->error_code == 0) {
    set_opcode(curr_msg->op_code);

    // opcode is game status
    if (strcmp(opcode, "GAME_STATUS") == 0) {
      set_game_id(curr_msg->game_id);
      set_guide_id(curr_msg->guide_id);
      char *temp = malloc(MESSAGE_LENGTH);
      snprintf(temp, MESSAGE_LENGTH-1, "you have claimed %s out of %s krags", curr_msg->num_claimed, curr_msg->num_krags);
      my_dialog_window_push(temp);
      free(temp);

    // opcode is ga hint
    } else if (strcmp(opcode, "GA_HINT") == 0) {
        vibes_short_pulse();
        my_dialog_window_push(curr_msg->hint);
        set_hints_log_rec(curr_msg->hint);

    // opcode is gs response    
    } else if (strcmp(opcode, "GS_RESPONSE") == 0) {
      char *respcode = curr_msg->resp_code;

      // respcode is sh claimed
      if (strcmp(respcode, "SH_CLAIMED") == 0) {
        my_dialog_window_push("succesfully claimed krag!");

      // respcode is sh claimed already
      } else if (strcmp(respcode, "SH_CLAIMED_ALREADY") == 0) {
        my_dialog_window_push("krag already claimed");

      // respcode is sh error invalid message  
      } else if (strcmp(respcode, "SH_ERROR_INVALID_MESSAGE") == 0) {
        send_FA_LOG("got SH_ERROR_INVALID_MESSAGE");

      // respcode is sh error invalid opcode  
      } else if (strcmp(respcode, "SH_ERROR_INVALID_OPCODE") == 0) {
        send_FA_LOG("got SH_ERROR_INVALID_OPCODE");

      // respcode is sh error invalid team name  
      } else if (strcmp(respcode, "SH_ERROR_INVALID_TEAMNAME") == 0) {
        // take user back to join game menu
        window_stack_pop(true);
        my_dialog_window_push("invalid team name");

      // respcode is sh error duplicate playername  
      } else if (strcmp(respcode, "SH_ERROR_DUPLICATE_PLAYERNAME") == 0) {
        // take user back to join game menu
        window_stack_pop(true);
        my_dialog_window_push("duplicate player name");

      // respcode is sh error invalid playername  
      } else if (strcmp(respcode, "SH_ERROR_INVALID_PLAYERNAME") == 0) {
        // take user back to join game menu
        window_stack_pop(true);
        my_dialog_window_push("invalid player name");

       // respcode is sh error invalid id 
      } else if (strcmp(respcode, "SH_ERROR_INVALID_ID") == 0) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "got SH_ERRPR_INVALID_ID");
      }

     // opcode is game over 
    } else if (strcmp(opcode, "GAME_OVER") == 0) {
      // start end of game sequence
      end_of_game = true;
      vibes_short_pulse();
      my_dialog_window_push("Game Over!");
      my_dialog_window_push("Game Over!");
    // opcode is team record  
    } else if (strcmp(opcode, "TEAM_RECORD") == 0) {
      // do nothing, wait for gane over mesasge to start end of game sequence
    } else {
      // log getting a weird message
      char *temp = malloc(MESSAGE_LENGTH);
      snprintf(temp, MESSAGE_LENGTH-1, "got opcode %s", opcode);
      send_FA_LOG(temp);
      free(temp);
    }       
  } else {
    // bad message
    APP_LOG(APP_LOG_LEVEL_ERROR, "the error code is not 0");
  }
}
