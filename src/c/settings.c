#include "src/c/luminosity.h"
#include "src/c/render.h"

// Initialize the default settings
void prv_default_settings() {
  settings.Analog = false;
  settings.Metric = false;
  settings.Knots = false;
  settings.Invert_Colors = false;
  settings.sunrise = 7;
  settings.sunset = 19;
  settings.enable_wind_ring = true;
}

// Read settings from persistent storage
void prv_load_settings() {
  prv_default_settings();
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
  s_sunrise = settings.sunrise;
  s_sunset = settings.sunset;
  update_colors();
}

// Save the settings to persistent storage
void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  update_colors();
  apply_colors_to_layers();
  prv_update_display();
}

#define CURRENT_WEATHER_STRUCT_VERSION 2

void cache_weather_data() {
  if (!s_weather_ready) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Requested to cache weather data but weather "
                                 "data is not available, aborting!");
    return;
  }
  _Static_assert(sizeof(weather_cache) <= PERSIST_DATA_MAX_LENGTH);
  APP_LOG(APP_LOG_LEVEL_INFO, "Caching weather data...");
  weather_cache.struct_version = CURRENT_WEATHER_STRUCT_VERSION;
  persist_write_data(WEATHER_CACHE_KEY, &weather_cache, sizeof(weather_cache));
}

void load_weather_data_from_cache() {
  if (s_weather_ready) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Requested to load cached weather data but we "
                                 "have fresh data already, aborting!");
    return;
  }

  if (!persist_exists(WEATHER_CACHE_KEY)) {
    APP_LOG(APP_LOG_LEVEL_INFO,
            "Requested access to cached weather data, but none exists.");
    return;
  }

  _Static_assert(sizeof(weather_cache) <= PERSIST_DATA_MAX_LENGTH);
  if (persist_get_size(WEATHER_CACHE_KEY) != sizeof(weather_cache)) {
    APP_LOG(
        APP_LOG_LEVEL_INFO,
        "Requested access to cached weather data, but cached data was a "
        "different length than the expected struct. Must be stale. Purging.");
    persist_delete(WEATHER_CACHE_KEY);
    return;
  }

  size_t bytes_read = persist_read_data(WEATHER_CACHE_KEY, &weather_cache,
                                        sizeof(weather_cache));

  if (bytes_read != sizeof(weather_cache)) {
    APP_LOG(APP_LOG_LEVEL_WARNING,
            "Read an unexpected size from weather cache. Purging.");
    persist_delete(WEATHER_CACHE_KEY);
    memset(&weather_cache, 0, sizeof(weather_cache));
    return;
  }

  if (weather_cache.struct_version != CURRENT_WEATHER_STRUCT_VERSION) {
    APP_LOG(APP_LOG_LEVEL_INFO,
            "Old structure version in weather cache. Purging.");
    persist_delete(WEATHER_CACHE_KEY);
    memset(&weather_cache, 0, sizeof(weather_cache));
    return;
  }

  // TODO: verify that weather_fetched_hour is recent enough, discard if not.

  APP_LOG(APP_LOG_LEVEL_INFO, "Loaded data from weather cache!");
  s_weather_ready = true;

  update_strings_from_weather_cache();
  prv_update_display();
  return;
}