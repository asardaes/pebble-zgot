#ifdef PBL_PLATFORM_APLITE

#include <pebble.h>
#include "common.h"

extern bool temp_unit;

static GFont s_zelda_font, s_zelda_font_small, s_degree_font;

static BitmapLayer *s_hearts_layer, *s_buttons_layer, *s_rupee_layer, *s_charge_layer, *s_heartsfill_layer, *s_navi_layer;

static GBitmap *s_heartsfill_bitmap, *s_hearts_bitmap, *s_buttons_bitmap, *s_charge_bitmap, *s_rupee_bitmap, *s_navi_bitmap;

TextLayer *date_layer, *temp_layer, *degree_layer, *hour_layer, *min_layer, *second_layer;

BitmapLayer *gears_layer;

GBitmap *gears_bitmap_a, *gears_bitmap_b;

/* ===================================================================================================================== */

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

void handle_battery(BatteryChargeState charge_state) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Battery percentage: %d.", charge_state.charge_percent);
	
	layer_set_frame(bitmap_layer_get_layer(s_heartsfill_layer), GRect(0, 168-20,
									  7*charge_state.charge_percent/10, 
									  20));
	layer_mark_dirty(bitmap_layer_get_layer(s_heartsfill_layer));
	
	if (charge_state.is_charging) {
		layer_set_hidden(bitmap_layer_get_layer(s_charge_layer), false);
		layer_set_hidden(bitmap_layer_get_layer(s_navi_layer), true);

	} else {
		layer_set_hidden(bitmap_layer_get_layer(s_charge_layer), true);
		layer_set_hidden(bitmap_layer_get_layer(s_navi_layer), false);
	}
}

/* ===================================================================================================================== */

void main_window_load(Window *window) {
	// Black background, 144 × 168 pixels, 176 PPI
	window_set_background_color(window, GColorBlack);
	
	// Assign GFont
	s_zelda_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ZELDA_16));
	s_zelda_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_16));
	s_degree_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DEGREE_14));
	
	// Gears layers
	gears_layer = bitmap_layer_create(GRect(0, 0, 144, 144));
	bitmap_layer_set_compositing_mode(gears_layer, GCompOpAssign);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(gears_layer));

	// Initialize bitmap_a
	gears_bitmap_a = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_0);
	// Get a tm structure
	time_t t = time(NULL); 
	struct tm *tick_time = localtime(&t);
	// Initialize bitmap_b
	which_case(tick_time, true);
	
	// Buttons layer
	s_buttons_layer = bitmap_layer_create(GRect(144-58, 168-58, 58, 58));
	bitmap_layer_set_background_color(s_buttons_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_buttons_layer, GCompOpOr);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_buttons_layer));
	
	// Hour and Minute layer
	int side = 29;
	
	hour_layer = text_layer_create(GRect(0, 15, side, side));
	text_layer_set_background_color(hour_layer, GColorClear);
	text_layer_set_text_color(hour_layer, GColorBlack);	
	text_layer_set_font(hour_layer, s_zelda_font);
	text_layer_set_text_alignment(hour_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(hour_layer));
	
	min_layer = text_layer_create(GRect(144-side, 15, side, side));
	text_layer_set_background_color(min_layer, GColorClear);
	text_layer_set_text_color(min_layer, GColorBlack);	
	text_layer_set_font(min_layer, s_zelda_font);
	text_layer_set_text_alignment(min_layer, GTextAlignmentCenter);	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(min_layer));
		
	// Date layer
	date_layer = text_layer_create(GRect(0, 0, 144, 50));
	text_layer_set_background_color(date_layer, GColorClear);
	text_layer_set_text_color(date_layer, GColorWhite);	
	text_layer_set_font(date_layer, s_zelda_font_small);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
	
	// Hearts layers
	s_hearts_layer = bitmap_layer_create(GRect(0, 168-20, 70, 20));
	bitmap_layer_set_background_color(s_hearts_layer, GColorClear);
	s_hearts_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEARTS_EDGE);
	bitmap_layer_set_bitmap(s_hearts_layer, s_hearts_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_hearts_layer));
	
	s_heartsfill_layer = bitmap_layer_create(GRect(0, 168-20, 70, 20));
	bitmap_layer_set_background_color(s_heartsfill_layer, GColorClear);
	s_heartsfill_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEARTS_FILL);
	bitmap_layer_set_bitmap(s_heartsfill_layer, s_heartsfill_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_heartsfill_layer));
	
	// Charge layer
	s_charge_layer = bitmap_layer_create(GRect(1, 168-20-15-18, 16, 16));
	bitmap_layer_set_background_color(s_charge_layer, GColorClear);
	s_charge_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGE);
	bitmap_layer_set_bitmap(s_charge_layer, s_charge_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_charge_layer));

	// Navi layer
	s_navi_layer = bitmap_layer_create(GRect(1, 168-20-15-18, 18, 15));
	bitmap_layer_set_background_color(s_navi_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_navi_layer, GCompOpAssign);
	s_navi_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NAVI);
	bitmap_layer_set_bitmap(s_navi_layer, s_navi_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_navi_layer));
	
	// Temperature layers
	temp_layer = text_layer_create(GRect(144-34, 168-30, 34, 30));
	text_layer_set_background_color(temp_layer, GColorClear);
	text_layer_set_text_color(temp_layer, GColorWhite);	
	text_layer_set_font(temp_layer, s_zelda_font);
	text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);	
	text_layer_set_text(temp_layer, "...");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(temp_layer));
	
	degree_layer = text_layer_create(GRect(144-58, 168-19, 20, 20));
	text_layer_set_background_color(degree_layer, GColorClear);
	text_layer_set_text_color(degree_layer, GColorBlack);	
	text_layer_set_font(degree_layer, s_degree_font);
	text_layer_set_text_alignment(degree_layer, GTextAlignmentCenter);	
	
	if (temp_unit)
		text_layer_set_text(degree_layer, "\u00B0F");
	else
		text_layer_set_text(degree_layer, "\u00B0C");
	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(degree_layer));
	
	// Rupee layer
	s_rupee_layer = bitmap_layer_create(GRect(1, 168-20-15, 13, 13));
	bitmap_layer_set_background_color(s_rupee_layer, GColorClear);
	s_rupee_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RUPEE);
	bitmap_layer_set_bitmap(s_rupee_layer, s_rupee_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_rupee_layer));
	
	// Seconds layer
	second_layer = text_layer_create(GRect(17, 168-38, 30, 15));
	text_layer_set_background_color(second_layer, GColorClear);
	text_layer_set_text_color(second_layer, GColorWhite);
	text_layer_set_font(second_layer, s_zelda_font_small);
	text_layer_set_text_alignment(second_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(second_layer));
	
	// Get battery percentage when window loads
	handle_battery(battery_state_service_peek());
	
	// Get BT status when window loads;
	handle_bt(connection_service_peek_pebble_app_connection());
}

void main_window_unload(Window *window) {
	// Destroy Bitmap layers
	bitmap_layer_destroy(gears_layer);
	bitmap_layer_destroy(s_heartsfill_layer);
	bitmap_layer_destroy(s_hearts_layer);
	bitmap_layer_destroy(s_buttons_layer);
	bitmap_layer_destroy(s_charge_layer);
	bitmap_layer_destroy(s_rupee_layer);
	bitmap_layer_destroy(s_navi_layer);
	
	// Destroy TextLayer
	text_layer_destroy(date_layer);
	text_layer_destroy(temp_layer);
	text_layer_destroy(hour_layer);
	text_layer_destroy(min_layer);
	text_layer_destroy(degree_layer);
	text_layer_destroy(second_layer);
	
	// Destroy GBitmap
	gbitmap_destroy(gears_bitmap_a);
	gbitmap_destroy(gears_bitmap_b);
	gbitmap_destroy(s_hearts_bitmap);
	gbitmap_destroy(s_heartsfill_bitmap);
	gbitmap_destroy(s_buttons_bitmap);
	gbitmap_destroy(s_rupee_bitmap);
	gbitmap_destroy(s_charge_bitmap);
	gbitmap_destroy(s_navi_bitmap);
	
	// Unload GFont
	fonts_unload_custom_font(s_zelda_font);
	fonts_unload_custom_font(s_zelda_font_small);
	fonts_unload_custom_font(s_degree_font);
}

#endif

