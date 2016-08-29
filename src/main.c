#include <pebble.h>

#include <pebble-generic-weather/pebble-generic-weather.h>
#include <pebble-events/pebble-events.h>

static Window *s_window;
static TextLayer *s_text_layer, *s_time_layer;
static char api_key[50];

static void read_persist()
{
	if(persist_exists(MESSAGE_KEY_OWMAPIKEY))
	{
		persist_read_string(MESSAGE_KEY_OWMAPIKEY, api_key, sizeof(api_key));
	}
}

static void store_persist()
{
	persist_write_string(MESSAGE_KEY_OWMAPIKEY, api_key);
}

static void weather_callback(GenericWeatherInfo *info, GenericWeatherStatus status) {
  switch(status) {
    case GenericWeatherStatusAvailable:
    {
      static char s_buffer[256];
      snprintf(s_buffer, sizeof(s_buffer),
        "Temperature (K/C/F): %d/%d/%d\n\nName:\n%s\n\nDescription:\n%s",
        info->temp_k, info->temp_c, info->temp_f, info->name, info->description);
      text_layer_set_text(s_text_layer, s_buffer);
    }
      break;
    case GenericWeatherStatusNotYetFetched:
      text_layer_set_text(s_text_layer, "GenericWeatherStatusNotYetFetched");
      break;
    case GenericWeatherStatusBluetoothDisconnected:
      text_layer_set_text(s_text_layer, "GenericWeatherStatusBluetoothDisconnected");
      break;
    case GenericWeatherStatusPending:
      text_layer_set_text(s_text_layer, "GenericWeatherStatusPending");
      break;
    case GenericWeatherStatusFailed:
      text_layer_set_text(s_text_layer, "GenericWeatherStatusFailed");
      break;
    case GenericWeatherStatusBadKey:
      text_layer_set_text(s_text_layer, "GenericWeatherStatusBadKey");
      break;
    case GenericWeatherStatusLocationUnavailable:
      text_layer_set_text(s_text_layer, "GenericWeatherStatusLocationUnavailable");
      break;
  }
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

   // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
	Tuple *data = dict_find(iterator, MESSAGE_KEY_READY);
	if(data)
	{
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Ready Received! Requesting weather.");
		generic_weather_fetch(weather_callback);
	}
	
	data = dict_find(iterator, MESSAGE_KEY_OWMAPIKEY);
	if(data)
	{
		strcpy(api_key, data->value->cstring);
		generic_weather_set_api_key(api_key);
		generic_weather_fetch(weather_callback);
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 40, bounds.size.w, bounds.size.h-30));
  text_layer_set_text(s_text_layer, "Ready.");
  text_layer_set_text_alignment(s_text_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  
  s_time_layer = text_layer_create(GRect(0, 0, bounds.size.w, 20));
  // Apply a font to the TextLayer
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  text_layer_destroy(s_time_layer);
  window_destroy(window);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  if(tick_time->tm_min % 30 == 0) {  generic_weather_fetch(weather_callback);}
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  read_persist();
    // Register with TickTimerService
  events_tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Make sure the time is displayed from the start
  update_time();

  // Replace this with your own API key from OpenWeatherMap.org
  generic_weather_init();
  generic_weather_set_api_key(api_key);
  generic_weather_set_provider(GenericWeatherProviderOpenWeatherMap);
  //this will listen for the Ready event from Clay
  events_app_message_request_inbox_size(1024);
	events_app_message_register_inbox_received(inbox_received_callback, NULL);
	events_app_message_register_inbox_dropped(inbox_dropped_callback, NULL);
	events_app_message_register_outbox_failed(outbox_failed_callback, NULL);
	events_app_message_register_outbox_sent(outbox_sent_callback, NULL);
  events_app_message_open();

  //app_timer_register(3000, js_ready_handler, NULL);
  //rather than assume it takes 3 seconds to start js why not just watch for an event
}

static void deinit() {
  generic_weather_deinit();
  store_persist();
}

int main() {
  init();
  app_event_loop();
  deinit();
}