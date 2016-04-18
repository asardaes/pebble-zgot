#pragma once

#include <pebble.h>

#ifndef WEATHER_TEMPERATURE
#define WEATHER_TEMPERATURE 0
#endif

#ifndef UNIT_TEMPERATURE
#define UNIT_TEMPERATURE 1
#endif

#ifndef ANIM_FREQ
#define ANIM_FREQ 2
#endif

#ifndef SHAKE
#define SHAKE 3
#endif

/* ===================================================================================================================== */

void animate();

/* ===================================================================================================================== */

void update_time();
void tick_handler(struct tm *tick_time, TimeUnits units_changed);

/* ===================================================================================================================== */

void handle_battery(BatteryChargeState charge_state);
void handle_tap(AccelAxisType axis, int32_t dir);

/* ===================================================================================================================== */

char *translate_error(AppMessageResult result);
void get_weather();

/* ===================================================================================================================== */

void inbox_received_callback(DictionaryIterator *iterator, void *context);
void inbox_dropped_callback(AppMessageResult reason, void *context);
void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
void outbox_sent_callback(DictionaryIterator *iterator, void *context);

/* ===================================================================================================================== */

void init();
void deinit();

