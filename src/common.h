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

#ifndef RUPEE
#define RUPEE 4
#endif

void handle_battery(BatteryChargeState charge_state);

void init();
void deinit();

