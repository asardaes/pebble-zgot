#include <pebble.h>
#include "common.h"

int main(void) {	
	init();
	app_event_loop();
	deinit();
}

