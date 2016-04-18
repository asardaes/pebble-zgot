#ifdef PBL_PLATFORM_BASALT

#include <pebble.h>
#include "common.h"

Window *s_main_window;
TextLayer *s_date_layer, *s_temp_layer, *s_degree_layer, *s_hour_layer, *s_min_layer, *s_second_layer;
GFont s_zelda_font, s_zelda_font_small, s_degree_font;

BitmapLayer *s_gears_layer, *s_heartsfill_layer, *s_hearts_layer, *s_buttons_layer, *s_charge_layer, *s_rupee_layer, *s_hookshot_layer;
GBitmap *s_heartsfill_bitmap, *s_hearts_bitmap, *s_buttons_bitmap, *s_charge_bitmap, *s_rupee_bitmap, *s_hookshot_bitmap;
GBitmap *s_gears_bitmap_0, *s_gears_bitmap_1, *s_gears_bitmap_2, *s_gears_bitmap_3, *s_gears_bitmap_4;

extern bool temp_unit;

/* ===================================================================================================================== */

void handle_bt(bool connected) {	
	if (s_hookshot_bitmap)
		gbitmap_destroy(s_hookshot_bitmap);
	
	if (connected) {
		s_hookshot_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOOKSHOT);
		bitmap_layer_set_bitmap(s_hookshot_layer, s_hookshot_bitmap);
	} else {
		s_hookshot_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_HOOKSHOT);
		bitmap_layer_set_bitmap(s_hookshot_layer, s_hookshot_bitmap);
	}	
}

/* ===================================================================================================================== */

void main_window_load(Window *window) {
	// Background, 144 × 168 pixels, 176 PPI
	window_set_background_color(window, GColorOxfordBlue);
	
	// Assign GFont
	s_zelda_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ZELDA_16));
	s_zelda_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_16));
	s_degree_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DEGREE_14));
	
	// Gears layers
	s_gears_layer = bitmap_layer_create(GRect(0, 0, 144, 144));
	bitmap_layer_set_background_color(s_gears_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_gears_layer, GCompOpSet);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_gears_layer));
	
	s_gears_bitmap_0 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_0);
	s_gears_bitmap_1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_1);
	s_gears_bitmap_2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_2);
	s_gears_bitmap_3 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_3);
	s_gears_bitmap_4 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GEARS_4);
	
	// Buttons layer
	s_buttons_layer = bitmap_layer_create(GRect(144-58, 168-58, 58, 58));
	bitmap_layer_set_background_color(s_buttons_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_buttons_layer, GCompOpSet);
	s_buttons_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BUTTONS);
	bitmap_layer_set_bitmap(s_buttons_layer, s_buttons_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_buttons_layer));
	
	// Hookshot layer
	s_hookshot_layer = bitmap_layer_create(GRect(144-19, 168-55, 16, 16));
	bitmap_layer_set_background_color(s_hookshot_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_hookshot_layer, GCompOpSet);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_hookshot_layer));
	
	// Hour and Minute layer
	int side = 29;
	
	s_hour_layer = text_layer_create(GRect(0, 15, side, side));
	text_layer_set_background_color(s_hour_layer, GColorClear);
	text_layer_set_text_color(s_hour_layer, GColorWhite);	
	text_layer_set_font(s_hour_layer, s_zelda_font);
	text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
	
	s_min_layer = text_layer_create(GRect(144-side, 15, side, side));
	text_layer_set_background_color(s_min_layer, GColorClear);
	text_layer_set_text_color(s_min_layer, GColorWhite);	
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
	s_heartsfill_layer = bitmap_layer_create(GRect(0, 168-20, 70, 20));
	bitmap_layer_set_background_color(s_heartsfill_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_heartsfill_layer, GCompOpSet);
	s_heartsfill_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEARTS_FILL);
	bitmap_layer_set_bitmap(s_heartsfill_layer, s_heartsfill_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_heartsfill_layer));
	
	s_hearts_layer = bitmap_layer_create(GRect(0, 168-20, 70, 20));
	bitmap_layer_set_background_color(s_hearts_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_hearts_layer, GCompOpSet);
	s_hearts_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEARTS_EDGE);
	bitmap_layer_set_bitmap(s_hearts_layer, s_hearts_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_hearts_layer));
	
	// Charge layer
	s_charge_layer = bitmap_layer_create(GRect(1, 168-20-15-18, 16, 16));
	bitmap_layer_set_background_color(s_charge_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_charge_layer, GCompOpSet);
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
	text_layer_set_text_color(s_degree_layer, GColorWhite);	
	text_layer_set_font(s_degree_layer, s_degree_font);
	text_layer_set_text_alignment(s_degree_layer, GTextAlignmentCenter);	
	
	if (temp_unit)
		text_layer_set_text(s_degree_layer, "\u00B0F");
	else
		text_layer_set_text(s_degree_layer, "\u00B0C");
	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_degree_layer));
	
	// Rupee layer
	s_rupee_layer = bitmap_layer_create(GRect(1, 168-20-15, 13, 13));
	bitmap_layer_set_background_color(s_rupee_layer, GColorClear);
	bitmap_layer_set_compositing_mode(s_rupee_layer, GCompOpSet);
	s_rupee_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RUPEE);
	bitmap_layer_set_bitmap(s_rupee_layer, s_rupee_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_rupee_layer));
	
	// Seconds layer
	s_second_layer = text_layer_create(GRect(17, 168-38, 30, 15));
	text_layer_set_background_color(s_second_layer, GColorClear);
	text_layer_set_text_color(s_second_layer, GColorWhite);
	text_layer_set_font(s_second_layer, s_zelda_font_small);
	text_layer_set_text_alignment(s_second_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_second_layer));
	
	// Get battery percentage when window loads
	handle_battery(battery_state_service_peek());
	
	// Get BT status when window loads;
	handle_bt(bluetooth_connection_service_peek());
}

void main_window_unload(Window *window) {
	// Destroy Bitmap layers
	bitmap_layer_destroy(s_gears_layer);
	bitmap_layer_destroy(s_heartsfill_layer);
	bitmap_layer_destroy(s_hearts_layer);
	bitmap_layer_destroy(s_buttons_layer);
	bitmap_layer_destroy(s_charge_layer);
	bitmap_layer_destroy(s_rupee_layer);
	bitmap_layer_destroy(s_hookshot_layer);
	
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
	gbitmap_destroy(s_hookshot_bitmap);
	
	// Unload GFont
	fonts_unload_custom_font(s_zelda_font);
	fonts_unload_custom_font(s_zelda_font_small);
	fonts_unload_custom_font(s_degree_font);
}

#endif

