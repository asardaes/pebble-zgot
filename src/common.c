#include <pebble.h>
#include "common.h"

extern TextLayer *date_layer, *temp_layer, *degree_layer, *hour_layer, *min_layer, *second_layer;

extern BitmapLayer *gears_layer;

extern GBitmap *gears_bitmap_a, *gears_bitmap_b;

static Window *s_main_window;

static char temperature[12];
static char date_buffer[8];
static char second_buffer[4];
static char hour_buffer[] = "00";
static char min_buffer[] = "00";

static int anim_freq = 1; // 1 second by default
static int anim_index = 0;
static int counter = 0;
static int which_prev = -1;

static bool weather_flag = true;
static bool shake = true;
static bool anim = false;
static bool pos_dir = false;
static bool increase = false;
static bool rupee = true; // seconds by default

static uint32_t anim_duration = 60;

bool temp_unit = false; // (°C) by default

/* ===================================================================================================================== */

static char *translate_error(AppMessageResult result) {
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
	time_t t = time(NULL);
	struct tm *tick_time = localtime(&t);

	// Write the current hours and minutes into the buffer
	if(clock_is_24h_style() == true) {
		// Use 24 hour format
		strftime(hour_buffer, sizeof("00"), "%H", tick_time);
	} else {
		// Use 12 hour format
		strftime(hour_buffer, sizeof("00"), "%I", tick_time);
	}

	strftime(min_buffer, sizeof("00"), "%M", tick_time);

	// Date + seconds
	if (!anim) {
		if (rupee)
			strftime(second_buffer, sizeof(second_buffer), "%S", tick_time);
		else
			strftime(second_buffer, sizeof(second_buffer), "%j", tick_time);

		text_layer_set_text(second_layer, second_buffer);

		strftime(date_buffer, sizeof(date_buffer), "%a %d", tick_time);
		text_layer_set_text(date_layer, date_buffer);

	} else {
		text_layer_set_text(second_layer, "Hey!");

		strftime(date_buffer, sizeof(date_buffer), "%b '%y", tick_time);
		text_layer_set_text(date_layer, date_buffer);
	}

	// Display this time on the TextLayer
	text_layer_set_text(hour_layer, hour_buffer);
	text_layer_set_text(min_layer, min_buffer);

	// Debug
	/*
	text_layer_set_text(hour_layer, "00");
	text_layer_set_text(min_layer, "00");
	//text_layer_set_text(date_layer, "Wed 08");
	snprintf(second_buffer, sizeof(second_buffer), "0%d", (int)(time(NULL) % 5));
	text_layer_set_text(second_layer, second_buffer);
	*/
}

static int check_freq(struct tm *tick_time) {
	switch (anim_freq) {
		case 0:
		return 0;
		break;

		case 60:
		return tick_time->tm_min % 5;
		break;

		default:
		return tick_time->tm_sec / anim_freq % 5;
		break;
	}
}

/* ===================================================================================================================== */

static void animate() {
	bool end = 0;
	bool stop = 0;
	int anim_target = 0;
	struct tm *tick_time;

	if (counter == 10) {
		increase = !pos_dir;
		anim_duration += 50;
	} else if (counter == 17) {
		anim_duration += 50;
		increase = pos_dir;
	} else if (counter == 22) {
		increase = !pos_dir;
		anim_duration += 50;
	} else if (counter >= 25) {
		end = 1;
	}

	if (counter > 0) {
		GBitmap *temp_bitmap = gears_bitmap_a;
		gears_bitmap_a = gears_bitmap_b;
		bitmap_layer_set_bitmap(gears_layer, gears_bitmap_a);
		gbitmap_destroy(temp_bitmap);

	} else {
		gbitmap_destroy(gears_bitmap_b);
	}

	switch ((increase ? anim_index + 1 : anim_index - 1) % 5) {
		case 0:
		gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_0);
		break;

		case 1:
		gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_1);
		break;

		case 2:
		gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_2);
		break;

		case 3:
		gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_3);
		break;

		case 4:
		gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_4);
		break;
	}

	if (end) {
		stop = true;
		time_t t = time(NULL);
		tick_time = localtime(&t);

		anim_target = check_freq(tick_time);

		if ( (anim_index % 5) < anim_target ) {
			stop = false;
			increase = true;
			anim_index++;

		} else if ( (anim_index % 5) > anim_target ) {
			stop = false;
			increase = false;
			anim_index--;
		}

	} else {
		if (increase)
			anim_index++;
		else
			anim_index--;
	}

	if (stop) {
		anim = false;
		counter = 0;
		anim_duration -= 150;

		gbitmap_destroy(gears_bitmap_b);
		which_case(tick_time, true);
		update_time();

	} else {
		counter++;
		app_timer_register(anim_duration, animate, NULL);
	}
}

/* ===================================================================================================================== */

void which_case(struct tm *tick_time, bool offset) {
	int which = 0;

	which = check_freq(tick_time);

	if (offset) {
		if (which == 0) which = 4;
		else which -= 1;
	}

	if (offset || which != which_prev) {
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick case: %d", which);

		if (!offset) {
			GBitmap *temp_bitmap = gears_bitmap_a;
			gears_bitmap_a = gears_bitmap_b;
			bitmap_layer_set_bitmap(gears_layer, gears_bitmap_a);
			gbitmap_destroy(temp_bitmap);

			which_prev = which;
		}

		// Preload
		switch (which) {
			case 0:
			gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_1);
			break;

			case 1:
			gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_2);
			break;

			case 2:
			gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_3);
			break;

			case 3:
			gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_4);
			break;

			case 4:
			gears_bitmap_b = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_0);
			break;
		}

		// Call again to preload next. This assumes gears_bitmap_a != NULL
		if (offset) which_case(tick_time, false);
	}
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	if (!shake | !anim) {
		which_case(tick_time, false);
	}

	update_time();

	if (units_changed & MINUTE_UNIT) {
		if (connection_service_peek_pebble_app_connection() && weather_flag) {
			// Get initial weather, hopefully after things have settled down (AppMessage timing issues)...
			// This also means that, if it fails at first, it will keep trying every minute (is that bad for battery?)
			get_weather();
		}
	}

	// Get weather update every hour
	if (units_changed & HOUR_UNIT) {
		// To mark that temperature should be updated
		weather_flag = 1;
		text_layer_set_text(temp_layer, "...");

		// Try to get weather
		get_weather();
	}
}

/* ===================================================================================================================== */

static void unshake() {
	anim = false;
}

static void handle_tap(AccelAxisType axis, int32_t dir) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering handle_tap");

	if (!anim) {
		// Get a tm structure
		time_t t = time(NULL);
		struct tm *tick_time = localtime(&t);

		anim_index = check_freq(tick_time);

		if (dir > 0) {
			pos_dir = true;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Positive pos_dir.");
		} else {
			pos_dir = false;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Negative pos_dir.");
		}

		anim_index += 100; // avoid negative values due to MOD operation
		anim = true;
		update_time();
		increase = pos_dir;

		if (shake)
			animate();
		else
			app_timer_register(5000, unshake, NULL);
	}
}

/* ===================================================================================================================== */

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	// Read first item
	Tuple *t = dict_read_first(iterator);

	// For all items
	while(t != NULL) {
		// Which key was received?
		switch(t->key) {
			case WEATHER_TEMPERATURE:
			// true = Fahrenheit
			if (temp_unit) {
				snprintf(temperature, sizeof(temperature), "%d",  ((int)t->value->int32) * 9/5 + 32 );
			} else {
				snprintf(temperature, sizeof(temperature), "%d", (int)t->value->int32);
			}
			text_layer_set_text(temp_layer, temperature);

			// Mark that weather shouldn't be updated any more
			weather_flag = 0;
			break;

			case UNIT_TEMPERATURE:
			if (strcmp(t->value->cstring, "C") == 0) {
				persist_write_bool(UNIT_TEMPERATURE, false);
				text_layer_set_text(degree_layer, "\u00B0C");
				temp_unit = 0;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting temperature units to celsius");
			} else if (strcmp(t->value->cstring, "F") == 0) {
				persist_write_bool(UNIT_TEMPERATURE, true);
				text_layer_set_text(degree_layer, "\u00B0F");
				temp_unit = 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting temperature units to fahrenheit");
			}

			text_layer_set_text(temp_layer, "...");
			weather_flag = 1;
			get_weather();
			break;

			case ANIM_FREQ:
			anim_freq = t->value->int32;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Animation frequency: %d", anim_freq);
			persist_write_int(ANIM_FREQ, anim_freq);
			break;

			case SHAKE:
			shake = t->value->int32;
			persist_write_bool(SHAKE, shake);
			break;

			case RUPEE:
			rupee = strcmp(t->value->cstring, "S") == 0;
			persist_write_bool(RUPEE, rupee);
			break;

			default:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
			break;
		}

		// Look for next item
		t = dict_read_next(iterator);
	}

	tick_timer_service_unsubscribe();

	if ((anim_freq % 60 == 0) & !rupee)
		tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	else
		tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
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

/* ===================================================================================================================== */

void init() {
	if (persist_exists(UNIT_TEMPERATURE))
		temp_unit = persist_read_bool(UNIT_TEMPERATURE);

	if (persist_exists(ANIM_FREQ))
		anim_freq = persist_read_int(ANIM_FREQ);

	if (persist_exists(SHAKE))
		shake = persist_read_bool(SHAKE);

	if (persist_exists(RUPEE))
		rupee = persist_read_bool(RUPEE);

	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});

	// Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);

	// Register with TickTimerService
	if ((anim_freq % 60 == 0) & !rupee)
		tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	else
		tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

	// Register battery service
	battery_state_service_subscribe(handle_battery);

	// Register bluetooth service
	connection_service_subscribe((ConnectionHandlers) {
		.pebble_app_connection_handler = handle_bt,
		NULL
	});

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
	AppMessageResult res = app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMsgOpen: %s", translate_error(res));
}

void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();
	connection_service_unsubscribe();
	accel_tap_service_unsubscribe();
}

