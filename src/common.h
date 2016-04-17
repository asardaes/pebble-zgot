#pragma once

#include <pebble.h>

#ifndef WEATHER_TEMPERATURE
#define WEATHER_TEMPERATURE 0
#endif

#ifndef UNIT_TEMPERATURE
#define UNIT_TEMPERATURE 1
#endif

/* ===================================================================================================================== */

char *translate_error(AppMessageResult result);

void get_weather();

/* ===================================================================================================================== */

void inbox_dropped_callback(AppMessageResult reason, void *context);

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);

void outbox_sent_callback(DictionaryIterator *iterator, void *context);
