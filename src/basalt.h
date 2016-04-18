#pragma once

#ifdef PBL_PLATFORM_BASALT

#include <pebble.h>

/* ===================================================================================================================== */

void animate();

/* ===================================================================================================================== */

void update_time();

void tick_handler(struct tm *tick_time, TimeUnits units_changed);

/* ===================================================================================================================== */

void handle_battery(BatteryChargeState charge_state);

void handle_bt(bool connected);

void handle_tap(AccelAxisType axis, int32_t dir);

/* ===================================================================================================================== */

void main_window_load(Window *window);

void main_window_unload(Window *window);

/* ===================================================================================================================== */

void inbox_received_callback(DictionaryIterator *iterator, void *context);

/* ===================================================================================================================== */

void init();

void deinit();

#endif
