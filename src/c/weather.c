#include "src/c/luminosity.h"

const char* bearing_to_cardinal(int bearing) {
  if (bearing >= 348.75 || bearing < 11.25) {
    return "N";
  } else if (bearing >= 11.25 && bearing < 33.75) {
    return "NNE";
  } else if (bearing >= 33.75 && bearing < 56.25) {
    return "NE";
  } else if (bearing >= 56.25 && bearing < 78.75) {
    return "ENE";
  } else if (bearing >= 78.75 && bearing < 101.25) {
    return "E";
  } else if (bearing >= 101.25 && bearing < 123.75) {
    return "ESE";
  } else if (bearing >= 123.75 && bearing < 146.25) {
    return "SE";
  } else if (bearing >= 146.25 && bearing < 168.75) {
    return "SSE";
  } else if (bearing >= 168.75 && bearing < 191.25) {
    return "S";
  } else if (bearing >= 191.25 && bearing < 213.75) {
    return "SSW";
  } else if (bearing >= 213.75 && bearing < 236.25) {
    return "SW";
  } else if (bearing >= 236.25 && bearing < 258.75) {
    return "WSW";
  } else if (bearing >= 258.75 && bearing < 281.25) {
    return "W";
  } else if (bearing >= 281.25 && bearing < 303.75) {
    return "WNW";
  } else if (bearing >= 303.75 && bearing < 326.25) {
    return "NW";
  } else if (bearing >= 326.25 && bearing < 348.75) {
    return "NNW";
  } else
    return "UNK";
}

void bearing_icon_update_proc(Layer* layer, GContext* ctx) {
  if (s_weather_ready) {
    GRect bounds = layer_get_bounds(layer);
    int radius = 0.5 * (bounds.size.w) - 3;
    GPoint center =
        GPoint(bounds.origin.x + radius + 3, bounds.origin.y + radius + 3);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_context_set_stroke_color(ctx, COLOR_TIME);
    graphics_draw_circle(ctx, center, radius);
    GPoint edge =
        gpoint_from_polar(bounds, GOvalScaleModeFillCircle,
                          DEG_TO_TRIGANGLE(weather_cache.wind_bearing));
    graphics_context_set_stroke_color(ctx, COLOR_WIND_DAY);
    graphics_draw_line(ctx, center, edge);
  }
}

void update_strings_from_weather_cache() {
  if (s_weather_ready) {

    snprintf(forecast_high_low_buffer, sizeof(forecast_high_low_buffer),
             "%d˚/%d˚", weather_cache.temp_high, weather_cache.temp_low);
    if (window.tl_high_low_forecast_temperature) {
      text_layer_set_text(window.tl_high_low_forecast_temperature,
                          forecast_high_low_buffer);
    }

    snprintf(
        temperature_buffer, sizeof(temperature_buffer), "%d˚",
        weather_cache.temperature); // TODO optionally display units someday
    if (window.tl_current_temperature) {
      text_layer_set_text(window.tl_current_temperature, temperature_buffer);
    }

    snprintf(wind_speed_buffer, sizeof(wind_speed_buffer), "%d%s",
             weather_cache.wind_speed,
             (settings.Knots ? "kts" : (settings.Metric ? "m/s" : "mph")));
    if (window.tl_wind_speed) {
      text_layer_set_text(window.tl_wind_speed, wind_speed_buffer);
    }

    snprintf(wind_bearing_buffer, sizeof(wind_bearing_buffer), "%d˚ %s",
             weather_cache.wind_bearing,
             bearing_to_cardinal(weather_cache.wind_bearing));
    if (window.tl_wind_bearing) {
      text_layer_set_text(window.tl_wind_bearing, wind_bearing_buffer);
    }

    if (window.bl_conditions) {
      for (int i = 0; i < 10; i++) {
        if (strcmp(weather_cache.conditions, condition_icons[i]) == 0) {
          bitmap_layer_set_bitmap(window.bl_conditions,
                                  s_condition_icon_bitmap[i]);
        }
      }
    }
  }
}