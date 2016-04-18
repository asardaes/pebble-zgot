#pragma once

#ifdef PBL_PLATFORM_APLITE

#include <pebble.h>
#include "common.h"

/* ===================================================================================================================== */

void handle_bt(bool connected);

/* ===================================================================================================================== */

void main_window_load(Window *window);
void main_window_unload(Window *window);

#endif
