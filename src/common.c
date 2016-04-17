#include <pebble.h>
#include "common.h"

/* ===================================================================================================================== */

char *translate_error(AppMessageResult result) {
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

void get_weather() {
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

/* ===================================================================================================================== */

void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!: %s", translate_error(reason));
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!: %s", translate_error(reason));
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
