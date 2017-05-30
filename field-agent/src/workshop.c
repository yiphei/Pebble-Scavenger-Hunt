/*****************************************************************/
/* This program demonstrates how to configure AppMessage handler */
/* functions and use them to send and receive messages.          */
/*****************************************************************/
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
#define HINTS_LOG_NUM_ROWS               5
#define CLAIM_KRAG_NUM_ROWS              16

#define MESSAGE_LENGTH 8191


// Globals
static Window *claim_krag_window;
static Window *join_game_window;
static Window *select_player_window;
static Window *select_team_window;
static Window *in_game_window;
static Window *hints_log_window;

static MenuLayer *claim_krag_layer;
static MenuLayer *join_game_layer;
static MenuLayer *select_player_layer;
static MenuLayer *select_team_layer;
static MenuLayer *in_game_layer;
static MenuLayer *hints_log_layer;

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

static char **hints_log;

static p_message_t *curr_msg;

static int hints_log_count = 0;
static int claim_krag_count = 0;
static int s_current_selection = -1;
static int tick_count = 0;
static bool js_running = false; // default assume smartphone JS proxy is not running


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
static void hints_log_window_load(Window *window);
static void hints_log_window_unload(Window *window);
static void hints_log_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);
static void hints_log_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static uint16_t hints_log_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);
static int16_t hints_log_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);
static void hints_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

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

static void hints_log_add();

static void delete_hints_log();
char *mystrdup(char *str);

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
  //hints_log = NULL;
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

  // hints log screen
  hints_log_window = window_create();
  APP_LOG(APP_LOG_LEVEL_INFO, "created hints_log_window");
  WindowHandlers hints_log_window_handlers = {
    .load = hints_log_window_load,
    .unload = hints_log_window_unload
  };
  window_set_window_handlers(hints_log_window, (WindowHandlers)hints_log_window_handlers);

  // claim krag screen
  claim_krag_window = window_create();
  APP_LOG(APP_LOG_LEVEL_INFO, "created claim_krag_window");
  WindowHandlers claim_krag_window_handlers = {
    .load = claim_krag_window_load,
    .unload = claim_krag_window_unload
  };
  window_set_window_handlers(claim_krag_window, (WindowHandlers)claim_krag_window_handlers);
  

  /* Show the Window on the watch, with animated=true. */
  window_stack_push(join_game_window, true);

  /* Set the handlers for AppMessage inbox/outbox events. Set these    *
   *    handlers BEFORE calling open, otherwise you might miss a message. */
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);

  /* 7. open the app message communication protocol. Request as much space *
   *    as possible, because our messages can be quite large at times.     */
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
  /* 1. Destroy all the windows! */
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
  if (hints_log_window) {
    window_destroy(hints_log_window);
    hints_log_window = NULL;
  }

  /* 2. Unsubscribe from sensors. */
  tick_timer_service_unsubscribe();

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
  /*if (curr_msg) {
    delete_message(curr_msg);
  }*/
  if (hints_log) {
    delete_hints_log();
  }
}

/* join game window */
static void join_game_window_load(Window *window)
{
  //APP_LOG(APP_LOG_LEVEL_INFO, "WE GOT TO JOINGAMEWINDOWLOAD");
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  join_game_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(join_game_layer, window);
  menu_layer_set_callbacks(join_game_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = join_game_get_num_rows_callback,
    .draw_row = join_game_draw_row_callback,
    .get_cell_height = join_game_get_cell_height_callback,
    .select_click = join_game_select_callback//,
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
      //APP_LOG(APP_LOG_LEVEL_INFO, "********%s", game_id);
      APP_LOG(APP_LOG_LEVEL_INFO, "Everything checks out, attempting to connect to server");
      //send_FA_LOCATION(0);
      tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
      window_stack_push(in_game_window, true);
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
  if (cell_index->row == 0) {
    // Claim Krag chosen
    window_stack_push(claim_krag_window, true);

  } else if (cell_index->row == 1) {
    // Hints chosen
    s_current_selection = -1;
    // ***TODO***
    if (hints_log_count > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "hint count %d", hints_log_count);
      window_stack_push(hints_log_window, true);
    } else {
      my_dialog_window_push("no hints");
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
    //for (int i = 0; i < 4)
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


/* Hints Log Menu */
static void hints_log_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  join_game_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(hints_log_layer, window);
  menu_layer_set_callbacks(hints_log_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = hints_log_get_num_rows_callback,
    .draw_row = hints_log_draw_row_callback,
    .get_cell_height = hints_log_get_cell_height_callback,
    .select_click = hints_log_select_callback//,
  });
  layer_add_child(window_layer, menu_layer_get_layer(join_game_layer));
}

static void hints_log_window_unload(Window *window)
{
  menu_layer_destroy(hints_log_layer);
}

static void hints_log_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "attempting to start the draw row");
  if (hints_log_count > 0) {
    APP_LOG(APP_LOG_LEVEL_INFO, "hints_log is null");
    return;
  }
  // finding max value of hints, dont want to draw past that
  if ((int)cell_index->row < hints_log_count) {
    menu_cell_basic_draw(ctx, cell_layer, hints_log[(int)cell_index->row], NULL, NULL);
  }
}

static uint16_t hints_log_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
{
  return HINTS_LOG_NUM_ROWS;
}

static int16_t hints_log_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    MENU_CELL_HEIGHT);
}

static void hints_log_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  if (hints_log == NULL) {
    return;
  }
  int i = 0;
  while (i < HINTS_LOG_NUM_ROWS && hints_log[i]) {
    i++;
  }
  if (cell_index->row < i) {
    my_dialog_window_push(hints_log[cell_index->row]);
  }
}

// tick_handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "tick %d", tick_count);
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
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send request. Otherwise, log error. */
    if (result == APP_MSG_OK) {
        int value = 1;

        /* construct and send message, outgoing value ignored */
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
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send request. Otherwise, log error. */
    if (result == APP_MSG_OK) {
        int value = 1;

        /* construct and send message, outgoing value ignored */
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
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send message. Otherwise, log error. */
    if (result == APP_MSG_OK) {
      
        /* Construct and send the message */

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
    char *temp = malloc(MESSAGE_LENGTH);//,1);
    snprintf(temp, MESSAGE_LENGTH-1, "opCode=FA_LOCATION|gameId=%s|pebbleId=%s|team=%s|player=%s|latitude=%s|longitude=%s|statusReq=%d",
      game_id, pebble_id, selected_team, selected_player, latitude, longitude, status);
    //APP_LOG(APP_LOG_LEVEL_INFO, "****%s", temp);
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
    snprintf(temp, MESSAGE_LENGTH-1, "opCode=FA_CLAIM|gameId=%s|team=%s|player=%s|latitude=%s|longitude=%s|kragId=%s",
      game_id, selected_team, selected_player, latitude, longitude, krag_id);

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
    snprintf(temp, MESSAGE_LENGTH-1, "opCode=FA_LOG|pebbleID=%s|text=%s", pebble_id, text);
    send_message(temp);
    free(temp);
  }
}

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

static void set_game_id(char *info)
{
  if (game_id) {
    free(game_id);
    game_id = NULL;
  }
  game_id = calloc(sizeof(info), strlen(info));
  if (game_id == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: calloc fail");
    return;
  }
  strcpy(game_id, my_strtok(info, " "));
}

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

static void set_opcode(char *info)
{
  if (opcode) {
    free(opcode);
  }
  opcode = calloc(sizeof(info), strlen(info));

  strcpy(opcode, my_strtok(info, " "));
}

static void delete_hints_log()
{
  if (hints_log != NULL) {
    for (int i = 0; i < HINTS_LOG_NUM_ROWS; i++) {
      if (hints_log[i]) {
        free(hints_log[i]);
      }
    }
  }
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

    /* 1. Check to see if JS environment ready message received. */
    Tuple *ready_tuple = dict_find(iterator, AppKeyJSReady);
    if(ready_tuple) {
        // Log the value sent as part of the received message.
        js_running = true;
        char *ready_str = ready_tuple->value->cstring;
        set_pebble_id(ready_str);
        set_pebble_id("12345678");
        //APP_LOG(APP_LOG_LEVEL_INFO, "GOT HERE");

        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyJSReady: %s\n", pebble_id);
        request_location();
        
    }

    /* 2. Check to see if an opcode message was received received. */
    Tuple *msg_tuple = dict_find(iterator, AppKeyRecvMsg);
    if(msg_tuple) {
        // Log the value sent as part of the received message.
        char *msg = msg_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyrecvMsg: %s\n", msg);
        //APP_LOG(APP_LOG_LEVEL_INFO, "ASDAASDAASDAASDA");
        //set_curr_msg_str(msg);
        //char *temp = malloc(MESSAGE_LENGTH);
        //strcpy(temp, msg);
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyrecvMsg: %s\n", msg);
        curr_msg = parse_message(msg);
        //free(temp);
        if (curr_msg == NULL) {
          //APP_LOG(APP_LOG_LEVEL_ERROR, "NULL parsed message");
          return;
        }
        //APP_LOG(APP_LOG_LEVEL_INFO, "opcode :%s", curr_msg->game_id);
        //APP_LOG(APP_LOG_LEVEL_INFO, "curr_msg:%s", curr_msg);
        if (curr_msg->error_code == 0) {
          //APP_LOG(APP_LOG_LEVEL_INFO, "THIS2");
          handle_message();
        } else {
          APP_LOG(APP_LOG_LEVEL_ERROR, "bad msg");
        }
    }

    /* 3. Check to see if a location message received. */
    Tuple *loc_tuple = dict_find(iterator, AppKeyLocation);
    if(loc_tuple) {
        // Log the value sent as part of the received message.
        char *location = loc_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyLocation: %s\n", location);
        set_location(location);
    }

    /* 4. Check to see if a PebbleId message received. */
    Tuple *id_tuple = dict_find(iterator, AppKeyPebbleId);
    if(id_tuple) {
        // Log the value sent as part of the received message.
        char *id_str = id_tuple->value->cstring;
        set_pebble_id(id_str);
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyPebbleId: %s\n", id_str);
    }

    /* 5. Check to see if an error message was received. */
    Tuple *error_tuple = dict_find(iterator, AppKeyLocation);
    if(error_tuple) {
        // Log the value sent as part of the received message.
        char *error = error_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeySendError: %s\n", error);
    }
}

static void handle_message()
{
  APP_LOG(APP_LOG_LEVEL_INFO, "HANDLING MESSAGE");
  APP_LOG(APP_LOG_LEVEL_INFO, "opcode :%s", curr_msg->op_code);
  if (curr_msg && curr_msg->error_code == 0) {
    set_opcode(curr_msg->op_code);
    //strcpy(op_code, curr_msg->op_code);
    APP_LOG(APP_LOG_LEVEL_INFO, "---%s", opcode);
    if (strcmp(opcode, "GAME_STATUS") == 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "GOT GAME STATUS");
      set_game_id(curr_msg->game_id);
      set_guide_id(curr_msg->guide_id);
      char *temp = malloc(MESSAGE_LENGTH);
      snprintf(temp, MESSAGE_LENGTH-1, "you have claimed %s out of %s krags", curr_msg->num_claimed, curr_msg->num_krags);
      my_dialog_window_push(temp);
      free(temp);
    } else if (strcmp(opcode, "GA_HINT") == 0) {
        vibes_short_pulse();
        my_dialog_window_push(curr_msg->hint);
        hints_log_add(curr_msg->hint);
    } else if (strcmp(opcode, "GS_RESPONSE") == 0) {
      char *respcode = curr_msg->resp_code;
      if (strcmp(respcode, "SH_CLAIMED") == 0) {
        my_dialog_window_push("succesfuly claimed krag!");
      } else if (strcmp(respcode, "SH_CLAIMED_ALREADY") == 0) {
        my_dialog_window_push("krag already claimed");
      } else if (strcmp(respcode, "SH_ERROR_INVALID_MESSAGE") == 0) {
        send_FA_LOG("got SH_ERROR_INVALID_MESSAGE");
      } else if (strcmp(respcode, "SH_ERROR_INVALID_OPCODE") == 0) {
        send_FA_LOG("got SH_ERROR_INVALID_OPCODE");
      } else if (strcmp(respcode, "SH_ERROR_INVALID_TEAMNAME") == 0) {
        window_stack_pop(true);
        my_dialog_window_push("invalid team name");
      } else if (strcmp(respcode, "SH_ERROR_DUPLICATE_PLAYERNAME") == 0) {
        window_stack_pop(true);
        my_dialog_window_push("duplicate player name");
      } else if (strcmp(respcode, "SH_ERROR_INVALID_PLAYERNAME") == 0) {
        window_stack_pop(true);
        my_dialog_window_push("invalid player name");
      } else if (strcmp(respcode, "SH_ERROR_INVALID_ID") == 0) {

      }

    } else if (strcmp(opcode, "GAME_OVER") == 0) {

    } else if (strcmp(opcode, "TEAM_RECORD") == 0) {

    } else {
      char *temp = malloc(MESSAGE_LENGTH);
      snprintf(temp, MESSAGE_LENGTH-1, "got opcode %s", opcode);
      send_FA_LOG(temp);
      free(temp);
    }       
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "the error code is not 0");
  }
}

static void hints_log_add(char *hint)
{
  if (hints_log == NULL) {
    hints_log = calloc(HINTS_LOG_NUM_ROWS, sizeof(char*));
  }
  if (hints_log_count < HINTS_LOG_NUM_ROWS) { // can add more hints easy
    char *temp = malloc(strlen(hint)+1);
    hints_log[hints_log_count] = temp;
    hints_log_count++;
  } else { // hints is already filled up
    for (int i = 4; i > 0; i--) {
      free(hints_log[i]);
      char *temp = malloc(strlen(hints_log[i-1])+1);
      hints_log[i] = temp;
    }
    char *temp2 = malloc(strlen(hint)+1);
    hints_log[0] = temp2;
  }
  
}
