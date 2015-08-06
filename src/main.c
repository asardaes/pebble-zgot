#include <pebble.h>
	
#define WEATHER_TEMPERATURE 0
#define UNIT_TEMPERATURE 1

static Window *s_main_window;
static TextLayer *s_date_layer, *s_temp_layer, *s_degree_layer, *s_hour_layer, *s_min_layer, *s_second_layer;
static GFont *s_zelda_font, *s_zelda_font_small, *s_degree_font;

static BitmapLayer *s_gears_layer, *s_heartsfill_layer, *s_hearts_layer, *s_buttons_layer, *s_charge_layer, *s_rupee_layer;
static GBitmap *s_heartsfill_bitmap, *s_hearts_bitmap, *s_buttons_bitmap, *s_charge_bitmap, *s_rupee_bitmap;
static GBitmap *s_gears_bitmap_0, *s_gears_bitmap_1, *s_gears_bitmap_2, *s_gears_bitmap_3, *s_gears_bitmap_4;

static char temperature[5];
static char date_buffer[8];
static char second_buffer[3];
static char hour_buffer[] = "00";
static char min_buffer[] = "00";

static int anim_index = 0;
static int counter = 0;
static bool anim = 0;
static bool direction = 0;
static uint32_t anim_duration = 75;

static bool weather_flag = 1; // true initially
static bool t_unit = 0; // false (°C) by default

// static char *test[] = {"Jan 01", "Feb 12", "Mar 31", "Apr 28", "May 15", "Jun 09", "Jul 22", "Aug 10", "Sep 20", "Oct 30", "Nov 26", "Dec 18"};
// static int t = 0;

/* ==================================================================================================================================================================== */

char *translate_error(AppMessageResult result) {
  	switch (result) {
		case APP_MSG_OK: return "APP_MSG_OK";
		case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
		case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
		case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
		case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
		case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
		case APP_MSG_BUSY: return "APP_MSG_BUSY";
		case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
		case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
		case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
		case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
		case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
		case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
		case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
		default: return "UNKNOWN ERROR";
  }
}

/* ==================================================================================================================================================================== */

static void animate() {
	bool end = 0;
	bool stop = 0; 
	bool increase = direction;
	
	if ((counter >= 10) & (counter < 17)) {
		increase = !direction;
		anim_duration = 125;
	} else if ((counter >= 17) & (counter < 22)) {
		anim_duration = 175;
	} else if ((counter >= 22) & (counter < 25)) {
		increase = !direction;
		anim_duration = 225;
	} else if (counter >= 25) {
		end = 1;
	}
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "anim_index = %d, MOD5 = %d", anim_index, (anim_index%5));
	
	switch (anim_index % 5) {
		case 0:
		bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_0);
		
		if (!end) {
			if (increase)
				anim_index++;
			else
				anim_index--;
		}
		
		break;
		// ===================================================================
		case 1:
		bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_1);
		
		if (!end) {
			if (increase)
				anim_index++;
			else
				anim_index--;
		}
		
		break;
		// ===================================================================
		case 2:
		bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_2);
		
		if (!end) {
			if (increase)
				anim_index++;
			else
				anim_index--;
		}
		
		break;
		// ===================================================================
		case 3:
		bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_3);
		
		if (!end) {
			if (increase)
				anim_index++;
			else
				anim_index--;
		}
		
		break;
		// ===================================================================
		case 4:
		bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_4);
		
		if (!end) {
			if (increase)
				anim_index++;
			else
				anim_index--;
		}
		
		break;
	}
	
	if (end) {
		stop = 1;
		
		if ( (anim_index % 5) < (time(NULL) % 5) ) {
			stop = 0;
			anim_index++;
		} else if ( (anim_index % 5) > (time(NULL) % 5) ) {
			stop = 0;
			anim_index--;
		}
	}
	
	if (stop) {
		anim = 0;
		counter = 0;
		anim_duration = 75;
	} else {
		counter++;
		app_timer_register(anim_duration, animate, NULL);
	}
}

/* ==================================================================================================================================================================== */

static void get_weather() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Getting new weather info.");

	// Begin dictionary
	DictionaryIterator *iter;
	
	AppMessageResult res = app_message_outbox_begin(&iter);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMsgBegin: %s", translate_error(res));

	if (iter == NULL) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "NULL iter.");
		dict_write_end(iter);
		return;
	}

	// Add a key-value pair
	dict_write_uint8(iter, 0, 0);
	dict_write_end(iter);

	// Send the message!
	res = app_message_outbox_send();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMsgSend: %s", translate_error(res));
}

static void update_time() {
	// Get a tm structure
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);

	// Write the current hours and minutes into the buffer
	if(clock_is_24h_style() == true) {
		// Use 24 hour format
		strftime(hour_buffer, sizeof("00"), "%H", tick_time);
	} else {
		// Use 12 hour format
		strftime(hour_buffer, sizeof("00"), "%I", tick_time);
	}
	
	strftime(min_buffer, sizeof("00"), "%M", tick_time);
	strftime(second_buffer, sizeof(second_buffer), "%S", tick_time);

	// Display this time on the TextLayer
	text_layer_set_text(s_hour_layer, hour_buffer);
	text_layer_set_text(s_min_layer, min_buffer);
	text_layer_set_text(s_second_layer, second_buffer);
	//text_layer_set_text(s_hour_layer, "12");
	//text_layer_set_text(s_min_layer, "34");
	//text_layer_set_text(s_second_layer, "04");
	
	// Date
	if (!anim) {
		strftime(date_buffer, sizeof(date_buffer), "%a %d", tick_time);
		text_layer_set_text(s_date_layer, date_buffer);
	} else {
		strftime(date_buffer, sizeof(date_buffer), "%b %y", tick_time);
		text_layer_set_text(s_date_layer, date_buffer);
	}
}

static void rotate_gears(struct tm *tick_time, TimeUnits units_changed) {
	if (!anim) {
		switch (tick_time->tm_sec % 5) {
			case 0:
			bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_0);
			break;

			case 1:
			bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_1);
			break;

			case 2:
			bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_2);
			break;

			case 3:
			bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_3);
			break;

			case 4:
			bitmap_layer_set_bitmap(s_gears_layer, s_gears_bitmap_4);
			break;
		}
	}
	
	update_time();
	
	if (units_changed & MINUTE_UNIT) {
		if (weather_flag) {
			// Get initial weather, hopefully after things have settled down (AppMessage timing issues)...
			// This also means that, if it fails at first, it will keep trying every minute (is that bad for battery?)
			get_weather();
			/*
			anim_index = (int)(time(NULL) % 5) + 100;
			anim = 1;
			//update_time();
			animate();
			*/
		}
	}
	
	// Get weather update every hour
	if (units_changed & HOUR_UNIT) {
		// To mark that temperature should be updated
		weather_flag = 1;
		text_layer_set_text(s_temp_layer, "...");
		
		// Try to get weather
		get_weather();
	}
	
	/*
	snprintf(temperature, sizeof(temperature), "%d", t);
	text_layer_set_text(s_temp_layer, temperature);
	t++;
	*/
}

/* ==================================================================================================================================================================== */

void handle_battery(BatteryChargeState charge_state) {		
	layer_set_frame(bitmap_layer_get_layer(s_heartsfill_layer), GRect(0, 168-20, 
																	 7*charge_state.charge_percent/10, 
																	 20));
	layer_mark_dirty(bitmap_layer_get_layer(s_heartsfill_layer));
	
	if (charge_state.is_charging | charge_state.is_plugged) {
		layer_set_hidden(bitmap_layer_get_layer(s_charge_layer), false);
	} else {
		layer_set_hidden(bitmap_layer_get_layer(s_charge_layer), true);
	}
}

void handle_bt(bool connected) {
	if (s_buttons_bitmap)
		gbitmap_destroy(s_buttons_bitmap);
	
	if (connected) {
		s_buttons_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BUTTONS_BT);
		bitmap_layer_set_bitmap(s_buttons_layer, s_buttons_bitmap);
	} else {
		s_buttons_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BUTTONS_NO_BT);
		bitmap_layer_set_bitmap(s_buttons_layer, s_buttons_bitmap);
	}
}

static void handle_tap(AccelAxisType axis, int32_t dir) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering handle_tap");
	
	if (!anim) {
		if (dir > 0) {
			direction = 1;
			anim_index = (int)(time(NULL) % 5);
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Positive direction.");
		} else {
			direction = 0;
			anim_index = (int)(time(NULL) % 5) + 100; // avoid negative values due to MOD operation
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Negative direction.");
		}

		anim = 1;
		update_time();
		animate();
	}
}

/* ==================================================================================================================================================================== */

static void main_window_load(Window *window) {	
	// Black background, 144 × 168 pixels, 176 PPI
	window_set_background_color(window, GColorBlack);
	
	// Assign GFont
	s_zelda_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ZELDA_16));
	s_zelda_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_16));
	s_degree_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DEGREE_14));
	
	// Gears layers
	s_gears_layer = bitmap_layer_create(GRect(0, 0, 144, 144));
	bitmap_layer_set_compositing_mode(s_gears_layer, GCompOpAssign);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_gears_layer));
	
	s_gears_bitmap_0 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_0);
	s_gears_bitmap_1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_1);
	s_gears_bitmap_2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_2);
	s_gears_bitmap_3 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_3);
	s_gears_bitmap_4 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_4);
	
	// Buttons layer
	s_buttons_layer = bitmap_layer_create(GRect(144-58, 168-58, 58, 58));
	bitmap_layer_set_background_color(s_buttons_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_buttons_layer, GCompOpOr);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_buttons_layer));
	
	// Hour and Minute layer
	int side = 29;
	
	s_hour_layer = text_layer_create(GRect(0, 15, side, side));
	text_layer_set_background_color(s_hour_layer, GColorClear);
	text_layer_set_text_color(s_hour_layer, GColorBlack);	
	text_layer_set_font(s_hour_layer, s_zelda_font);
	text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
	
	s_min_layer = text_layer_create(GRect(144-side, 15, side, side));
	text_layer_set_background_color(s_min_layer, GColorClear);
	text_layer_set_text_color(s_min_layer, GColorBlack);	
	text_layer_set_font(s_min_layer, s_zelda_font);
	text_layer_set_text_alignment(s_min_layer, GTextAlignmentCenter);	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_min_layer));
		
	// Date layer
	s_date_layer = text_layer_create(GRect(0, 0, 144, 50));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, GColorWhite);	
	text_layer_set_font(s_date_layer, s_zelda_font_small);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	
	// Hearts layers
	s_hearts_layer = bitmap_layer_create(GRect(0, 168-20, 70, 20));
	bitmap_layer_set_background_color(s_hearts_layer, GColorClear);
	s_hearts_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEARTS_EDGE);
	bitmap_layer_set_bitmap(s_hearts_layer, s_hearts_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_hearts_layer));
	
	s_heartsfill_layer = bitmap_layer_create(GRect(0, 168-20, 70, 20));
	bitmap_layer_set_background_color(s_heartsfill_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_heartsfill_layer, GCompOpOr);
	s_heartsfill_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEARTS_FILL);
	bitmap_layer_set_bitmap(s_heartsfill_layer, s_heartsfill_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_heartsfill_layer));
	
	// Charge layer
	s_charge_layer = bitmap_layer_create(GRect(1, 168-20-15-18, 16, 16));
	bitmap_layer_set_background_color(s_charge_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_charge_layer, GCompOpOr);
	s_charge_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGE);
	bitmap_layer_set_bitmap(s_charge_layer, s_charge_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_charge_layer));
	
	// Temperature layers
	s_temp_layer = text_layer_create(GRect(144-34, 168-30, 34, 30));
	text_layer_set_background_color(s_temp_layer, GColorClear);
	text_layer_set_text_color(s_temp_layer, GColorWhite);	
	text_layer_set_font(s_temp_layer, s_zelda_font);
	text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);	
	text_layer_set_text(s_temp_layer, "...");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temp_layer));
	
	s_degree_layer = text_layer_create(GRect(144-58, 168-19, 20, 20));
	text_layer_set_background_color(s_degree_layer, GColorClear);
	text_layer_set_text_color(s_degree_layer, GColorBlack);	
	text_layer_set_font(s_degree_layer, s_degree_font);
	text_layer_set_text_alignment(s_degree_layer, GTextAlignmentCenter);	
	
	if (t_unit)
		text_layer_set_text(s_degree_layer, "\u00B0F");
	else
		text_layer_set_text(s_degree_layer, "\u00B0C");
	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_degree_layer));
	
	// Rupee layer
	s_rupee_layer = bitmap_layer_create(GRect(1, 168-20-15, 13, 13));
	bitmap_layer_set_background_color(s_rupee_layer, GColorClear);
	s_rupee_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RUPEE);
	bitmap_layer_set_bitmap(s_rupee_layer, s_rupee_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_rupee_layer));
	
	// Seconds layer
	s_second_layer = text_layer_create(GRect(17, 168-38, 15, 15));
	text_layer_set_background_color(s_second_layer, GColorClear);
	text_layer_set_text_color(s_second_layer, GColorWhite);
	text_layer_set_font(s_second_layer, s_zelda_font_small);
	text_layer_set_text_alignment(s_second_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_second_layer));
	
	// Get battery percentage when window loads
	BatteryChargeState batt = battery_state_service_peek();
	handle_battery(batt);
	
	// Get BT status when window loads
	bool bt = bluetooth_connection_service_peek();
	handle_bt(bt);
}

static void main_window_unload(Window *window) {
	// Destroy Bitmap layers
	bitmap_layer_destroy(s_gears_layer);
	bitmap_layer_destroy(s_heartsfill_layer);
	bitmap_layer_destroy(s_hearts_layer);
	bitmap_layer_destroy(s_buttons_layer);
	bitmap_layer_destroy(s_charge_layer);
	bitmap_layer_destroy(s_rupee_layer);
	
	// Destroy TextLayer
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_temp_layer);
	text_layer_destroy(s_hour_layer);
	text_layer_destroy(s_min_layer);
	text_layer_destroy(s_degree_layer);
	text_layer_destroy(s_second_layer);
	
	// Destroy GBitmap
	gbitmap_destroy(s_gears_bitmap_0);
	gbitmap_destroy(s_gears_bitmap_1);
	gbitmap_destroy(s_gears_bitmap_2);
	gbitmap_destroy(s_gears_bitmap_3);
	gbitmap_destroy(s_gears_bitmap_4);
	gbitmap_destroy(s_hearts_bitmap);
	gbitmap_destroy(s_heartsfill_bitmap);
	gbitmap_destroy(s_buttons_bitmap);
	gbitmap_destroy(s_charge_bitmap);
	gbitmap_destroy(s_rupee_bitmap);
	
	// Unload GFont
	fonts_unload_custom_font(s_zelda_font);
	fonts_unload_custom_font(s_zelda_font_small);
	fonts_unload_custom_font(s_degree_font);
}

/* ==================================================================================================================================================================== */

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	// Read first item
	Tuple *t = dict_read_first(iterator);
	
	// For all items
	while(t != NULL) {
		// Which key was received?
		switch(t->key) {
			case WEATHER_TEMPERATURE:
			// true = Fahrenheit
			if (t_unit) {
				snprintf(temperature, sizeof(temperature), "%d",  ((int)t->value->int32) * 9/5 + 32 );
				//APP_LOG(APP_LOG_LEVEL_DEBUG, "Temperature in Fahrenheit: %d", ((int)t->value->int32) * 9/5 + 32);
			} else {
				snprintf(temperature, sizeof(temperature), "%d", (int)t->value->int32);
			}
			text_layer_set_text(s_temp_layer, temperature);
			
			// Mark that weather shouldn't be updated any more
			weather_flag = 0; 
			break;
			
			case UNIT_TEMPERATURE:
			if (strcmp(t->value->cstring, "C") == 0) {
				persist_write_bool(UNIT_TEMPERATURE, false);
				text_layer_set_text(s_degree_layer, "\u00B0C");
				t_unit = 0;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting temperature units to celsius");
			} else if (strcmp(t->value->cstring, "F") == 0) {
				persist_write_bool(UNIT_TEMPERATURE, true);
				text_layer_set_text(s_degree_layer, "\u00B0F");
				t_unit = 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting temperature units to fahrenheit");
			}
			
			text_layer_set_text(s_temp_layer, "...");
			weather_flag = 1;
			get_weather();
			break;
			
			default:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
			break;
		}

    // Look for next item
    t = dict_read_next(iterator);
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!: %s", translate_error(reason));
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!: %s", translate_error(reason));
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


/* ==================================================================================================================================================================== */

static void init() {
	if (persist_exists(UNIT_TEMPERATURE))
		t_unit = persist_read_bool(UNIT_TEMPERATURE);
	
	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	
	// Make window fullscreen
	window_set_fullscreen(s_main_window, true);

	// Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);
	
	// Register with TickTimerService
	tick_timer_service_subscribe(SECOND_UNIT, rotate_gears);
	
	// Register battery service
	battery_state_service_subscribe(&handle_battery);
	
	// Register bluetooth service
	bluetooth_connection_service_subscribe(&handle_bt);
	
	// Subscribe to the accelerometer tap service
	accel_tap_service_subscribe(handle_tap);
	
	// Make sure the time is displayed from the start
	update_time();
	
	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	
	// Open AppMessage
	//AppMessageResult res = app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	AppMessageResult res = app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMsgOpen: %s", translate_error(res));
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	accel_tap_service_unsubscribe();
}

/* ==================================================================================================================================================================== */

int main(void) {
	init();
	app_event_loop();
	deinit();
}
