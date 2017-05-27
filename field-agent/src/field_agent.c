#include <pebble.h>
#include <string.h>
#include "my_dialog_window.h"
#include "key_assembly.h"
#include "pin_window.h"
#include "../../common/message.h"
//#include "../../libcs50/set.h"

#define RADIO_BUTTON_WINDOW_RADIO_RADIUS 6
#define MENU_CELL_HEIGHT                 44

#define JOIN_GAME_NUM_ROWS               4
#define SELECT_PLAYER_NUM_ROWS           4
#define SELECT_TEAM_NUM_ROWS             4
#define IN_GAME_NUM_ROWS                 3


/* Windows */
static Window *join_game_window;
static Window *select_player_window;
static Window *select_team_window;
static Window *in_game_window;
//static Window *hints_log_window;
static PinWindow *claim_krag_window;

/* Layers */
static MenuLayer *join_game_layer;
static MenuLayer *select_player_layer;
static MenuLayer *select_team_layer;
static MenuLayer *in_game_layer;
//static MenuLayer *hints_log_layer;


/* Static Variables */
//static char *timestamp;
static char *guide_id;
static char *game_id = "0";
static char *pebble_id;
static char *latitude;
static char *longitude;
static char *player_names[SELECT_PLAYER_NUM_ROWS] = {"Paolo", "Tony", "Michael", "Yifei"};
static char *team_names[SELECT_TEAM_NUM_ROWS] = {"One", "Two", "Three", "Four"};
static char *selected_player;
static char *selected_team;
static bool js_running = false; // default assume smartphone JS proxy is not running
static int s_current_selection = -1;
//static int opcode_array_length = -1;
//static int status;
//static int hint_count = 0;

/* Static Structs */
static message_t *curr_msg;
//static set_t hints_log;

/* Function Declarations */
static void init();
static void deinit();
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
static void pin_complete_callback(PIN pin, void *context);
//static void hints_log_window_load(Window *window);
//static void hints_log_window_unload(Window *window);
//static void hints_log_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);
//static uint16_t hints_log_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);
//static int16_t hints_log_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);
static void in_game_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void inbox_received_handler(DictionaryIterator *iterator, void *context);
static void set_message(char *info);
static void handle_message();
static void set_location(char *info);
static void set_pebble_id(char *info);
static int inbox_received_identify(DictionaryIterator *iterator, Tuple *recv_tuple);
static void inbox_dropped_callback(AppMessageResult reason, void *context);
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
static void outbox_sent_callback(DictionaryIterator *iterator, void *context);
static void send_FA_LOCATION(int status);
static void send_FA_CLAIM(char *krag_id);
static void send_FA_LOG(char *text);
//static void send_start();
static void get_location(int value);
static void send_string(char *string);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

/* main, init, deinit */
int main(void)
{
  init();
  app_event_loop();
  deinit();
}

static void init()
{
  // Register Bluetooth communication callbacks
  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage - to always get the largest buffer available, follow this best practice:
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

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

  PinWindow *claim_krag_window = pin_window_create((PinWindowCallbacks) {
    .pin_complete = pin_complete_callback
  });
  
  /*// hints log screen
  hints_log_window = window_create();
  WindowHandlers hints_log_window_handlers = {
    .load = hints_log_window_load,
    .unload = hints_log_window_unload
  };
  window_set_window_handlers(hints_log_window, (WindowHandlers)hints_log_window_handlers);*/

  window_stack_push(join_game_window, true);
}

static void deinit()
{
  // destroy all the windows!
  window_destroy(join_game_window);
  window_destroy(select_player_window);
  window_destroy(select_team_window);
  window_destroy(in_game_window);
  pin_window_destroy(claim_krag_window);
  //window_destroy(claim_krag_window);
  //window_destroy(hints_log_window);
}


/* Join Game Menu */
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
    if (selected_player && selected_team) {
      //send_start();
      get_location(1);
      if (js_running && latitude && longitude) {
        send_FA_LOCATION(1);
        if (guide_id && strcmp(game_id, "0") == 0) {
          //hints_log = set_new();
          tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
          window_stack_push(in_game_window, true);
        }
      }
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
    selected_player = player_names[s_current_selection];
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
    selected_team = team_names[s_current_selection];
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
    pin_window_push(claim_krag_window, true);
  } else if (cell_index->row == 1) {
    // Hints chosen
    s_current_selection = -1;
    // ***TODO***
    window_stack_push(select_team_window, true);
  } else if (cell_index->row == 2) {
    // Status chosen
    send_FA_LOCATION(1);
  }
}

/* Claim Krag Pin Entry */
static void pin_complete_callback(PIN pin, void *context) {
  char temp[5];
  snprintf(temp, "%c%c%c%c", pin.digits[0], pin.digits[1], pin.digits[2], pin.digits[3]);
  pin_window_pop((PinWindow*)context, true);
}


/* Hints Log Menu */
/*static void hints_log_window_load(Window *window)
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
  // write a set_iterate function to put in here
}

static uint16_t hints_log_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context)
{
  return hint_count;
}

static int16_t hints_log_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    MENU_CELL_HEIGHT);
}

static void in_game_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  // write a set_iterate function to go here
}*/

/* Messaging */
static void set_message(char *info)
{
  if (curr_msg) {
    deleteMessage(curr_msg);
    curr_msg = NULL;
  }
  curr_msg = parseMessage(info);
}

static void inbox_received_handler(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Info: inbox received");

  Tuple *recv_tuple;
  int i = inbox_received_identify(iterator, recv_tuple);

  if (recv_tuple) {
    if (i > 0) {
      char *info = recv_tuple->value->cstring;
      switch(i) {
        case 0: // recv_tuple is AppKeyJSReady
          APP_LOG(APP_LOG_LEVEL_INFO, "Info: inbox is AppKeyJSReady");
          js_running = true;
          set_pebble_id(info);
          break;
        case 1: // recv_tuple is AppKeyRecvMsg
          APP_LOG(APP_LOG_LEVEL_INFO, "Info: inbox is AppKeyRecvMsg");
          set_message(info);
          handle_message();
          break;
        case 2: // recv_tuple is AppKeyLocation
          APP_LOG(APP_LOG_LEVEL_INFO, "Info: inbox is AppKeyLocation");
          set_location(info);
          break;
        case 3: // recv_tuple is AppKeyPebbleId, update current pebble_id
          APP_LOG(APP_LOG_LEVEL_INFO, "Info: inbox is AppKeyPebbleId");
          set_pebble_id(info);
          break;
        case 4: // recv_tuple is AppKeySendError, document error
          APP_LOG(APP_LOG_LEVEL_INFO, "Info: inbox is AppKeySendError");
          APP_LOG(APP_LOG_LEVEL_ERROR, "Error: AppKeySendError is %s", info);
          break;
      }
    } else {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error: could not identify AppKey");
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: could not identify inbox");
  }
}

static void handle_message()
{
  char *op_code = curr_msg->opCode;
  if (strcmp(op_code, "GAME_STATUS") == 0) {
    //case "GAME_STATUS":
      int i = (int)(curr_msg->numKrags - curr_msg->numClaimed);
      game_id = curr_msg->gameId;
      guide_id = curr_msg->guideId;
      char temp[80];
      snprintf(temp, "%d krags remaining", i);
      my_dialog_window_push("status", temp);
      //break;
    } else if (strcmp(op_code, "GA_HINT") == 0) {
    //case "GA_HINT":
      //char *temp_hint = curr_msg->hint;
      //hint_count++;
      //set_insert(hints_log, timestamp, curr_msg->hint);
      //my_dialog_window_push("hint", curr_msg->hint);
      //break;
    } else if (strcmp(op_code, "SH_ERROR_DUPLICATE_PLAYERNAME") == 0) {
    //case "SH_ERROR_DUPLICATE_PLAYERNAME":
      my_dialog_window_push("error", "select new player name");
      //break;
    } else if (strcmp(op_code, "SH_CLAIMED") == 0) {
    //case "SH_CLAIMED":
      my_dialog_window_push("krag","succesfuly claimed crag!");
      
      // reset hint log
      //hint_count = 0;
      //if(hints_log) {
      //  set_delete(hints_log);
      //}
      //break;
    } else if (strcmp(op_code, "SH_CLAIMED_ALREADY") == 0) {
    //case "SH_CLAIMED_ALREADY":
      my_dialog_window_push("krag", "krag already claimed");
      //break;
    } else if (strcmp(op_code, "GS_RESPONSE") == 0) {
    //case "GS_RESPONSE":
      // *** TODO *** figure out what this is
      //break;
    } else if (strcmp(op_code, "GAME_OVER") == 0) {
    //case "GAME_OVER":
      // *** TODO *** figure out what to do here
      //break;
    } else if (strcmp(op_code, "TEAM_RECORD") == 0) {
    //case "TEAM_RECORD":
      // *** TODO *** figure out what to do here
      //break;
    } else if (strcmp(op_code, "SH_ERROR_INVALID_MESSAGE") == 0) {
    //case "SH_ERROR_INVALID_MESSAGE":
      my_dialog_window_push("error", "invalid message");
      //break;
    } else if (strcmp(op_code, "SH_ERROR_INVALID_OPCODE") == 0) {
    //case "SH_ERROR_INVALID_OPCODE":
      my_dialog_window_push("error", "invalid opcode");
      //break;
    } else if (strcmp(op_code, "SH_ERROR_INVALID_TEAMNAME") == 0) {
    //case "SH_ERROR_INVALID_TEAMNAME":
      my_dialog_window_push("error","select new team");
      //break;
    } else if (strcmp(op_code, "SH_ERROR_INVALID_PLAYERNAME") == 0) {
    //case "SH_ERROR_INVALID_PLAYERNAME":
      my_dialog_window_push("error","select new player");
      //break;
    } else if (strcmp(op_code, "SH_ERROR_INVALID_ID") == 0) {
    //case "SH_ERROR_INVALID_ID":
      my_dialog_window_push("error", "invalid id");
      //break;
    } else {

    }
    //default:
    // *** TODO *** after send_FA_LOG is written

  
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

  latitude = malloc(sizeof(info));
  longitude = malloc(sizeof(info));
  if (latitude == NULL || longitude == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: malloc fail");
    return;
  }

  strcpy(latitude, myStrtok(info, "|"));
  strcpy(longitude, myStrtok(NULL, "|"));
}

static void set_pebble_id(char *info)
{
  if (pebble_id) { // if already exists, get rid of it
    free(pebble_id);
    pebble_id = NULL;
  }
  pebble_id = malloc(sizeof(info)+1);
  
  if (pebble_id == NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: malloc fail");
    return;
  }
  strcpy(pebble_id, info);
}

// Determines which AppKey message is, returns corresponding code:
//  0: AppKeyJSReady
//  1: AppKeyRecvMsg
//  2: AppKeyLocation
//  3: AppKeyPebbleId
//  4: AppKeySendError
// -1: Something Else (Error)
// (Don't Have to worry about AppKeySendMsg for this Function)
static int inbox_received_identify(DictionaryIterator *iterator, Tuple *recv_tuple)
{
  recv_tuple = dict_find(iterator, AppKeyJSReady);
  if (recv_tuple) { // inbox was AppKeyJSReady
    return 0;
  }
  
  recv_tuple = dict_find(iterator, AppKeyRecvMsg);
  if (recv_tuple) { // inbox was AppKeyRecvMsg
    return 1;
  }

  recv_tuple = dict_find(iterator, AppKeyLocation);
  if (recv_tuple) { // inbox was AppKeyLocation
    return 2;
  }

  recv_tuple = dict_find(iterator, AppKeyPebbleId);
  if (recv_tuple) { // inbox was AppKeyPebbleId
    return 3;
  }

  recv_tuple = dict_find(iterator, AppKeySendError);
  if (recv_tuple) { // inbox was AppKeySendError
    return 4;
  }
  return -1;
}

// dropped inbox
static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error: inbox dropped");
}

// failed outbox
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error: outbox failed");
}

// outbox sent
static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Info: outbox sent");
}

// sends FA_LOCATION to Game Server
// opCode=FA_LOCATION|gameId=|pebbleId=|team=|player=|latitude=|longitude=|statusReq=
static void send_FA_LOCATION(int status)
{
  if (game_id && pebble_id && selected_team && selected_player && latitude && longitude && status) {
    char temp [4098];
    snprintf(temp, "opCode=FA_LOCATION|gameId=%s|pebbleId=%s|team=%s|player=%s|latitude=%s|longitude=%s|statReq=%d",
      game_id, pebble_id, selected_team, selected_player, latitude, longitude, status);
    send_string(temp);
  }
}

// sends FA_CLAIM to Game Server
// opCode=FA_CLAIM|gameId=|pebbleId=|team=|player=|latitude=|longitude=|kragId=
static void send_FA_CLAIM(char *krag_id)
{
  if (game_id && pebble_id && selected_team && selected_player && latitude && longitude && krag_id) {
    char temp[4098];
    snprintf(temp, "opCode=FA_CLAIM|gameId=%s|team=%s|player=%s|latitude=%s|longitude=%s|kragId=%s",
      game_id, selected_team, selected_player, latitude, longitude, krag_id);
    send_string(temp);
  }
}

// sends FA_LOG to Game Server
// opCode=FA_LOG|pebbleId=|text=
static void send_FA_LOG(char *text)
{
  if (pebble_id && text) {
    char temp[4098];
    snprintf(temp, "opCode=FA_LOG|pebbleID=%s|text=%s", pebble_id, text);
    send_string(temp);
  }
}

// for setting bools and getting pebble_id when join game is presesd in join game menu
/*static void send_start()
{
  DictionaryIterator *iterator;
  AppMessageResult app_msg_result = app_message_outbox_begin(&iterator);

  if (app_msg_result == APP_MSG_OK) {
    dict_write_cstring(iterator, AppKeyJSReady);
    app_msg_result = app_message_outbox_send();
    if (app_msg_result == APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Info: sent start");
    } else {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error: sending start fail");
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: preparing start fail");
  }
}*/

static void get_location(int value)
{
  DictionaryIterator *iterator;
  AppMessageResult app_msg_result = app_message_outbox_begin(&iterator);

  if (app_msg_result == APP_MSG_OK) {
    dict_write_int(iterator, AppKeyLocation, &value, sizeof(int), true);
    app_msg_result = app_message_outbox_send();
    if (app_msg_result == APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Info: sent location request");
    } else {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error: sending location request fail");
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: preparing location fail");
  }
}

static void send_string(char *string)
{
  DictionaryIterator *iterator;
  AppMessageResult app_msg_result = app_message_outbox_begin(&iterator);

  if (app_msg_result == APP_MSG_OK) {
    dict_write_cstring(iterator, AppKeySendMsg, string);
    app_msg_result = app_message_outbox_send();
    if (app_msg_result == APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Info: sent string");
    } else {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error: sending string fail");
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error: preparing string fail");
  }
}

/* Other */
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  //int sec = tick_time->tm_sec;
  // update timestamp
  //char temp[80];
  // format is day:hr:minute:second
  //snprintf(temp, "%d:%d:%d:%d",
  //  tick_time->tm_mday, tick_time->tm_hour, tick_time->tm_min, sec);

  // send location every 15 seconds
  if (tick_time->tm_sec % 15 == 0) {
    send_FA_LOCATION(false);
  }
}