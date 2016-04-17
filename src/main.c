#include <pebble.h>

#ifdef PBL_PLATFORM_APLITE
	#include "aplite.h"
#elif PBL_PLATFORM_BASALT
	#include "basalt.h"
#endif

int main(void) {	
	init();
	app_event_loop();
	deinit();
}
