#include "src/c/render.h"

#include "src/c/callbacks.h"
#include "src/c/color_palette.h"
#include "src/c/declarations.h"
#include "src/c/drawing.h"
#include "src/c/layout_constants.h"



void main_window_load(Window* pw__window_to_load) {

  Layer* window_layer = window_get_root_layer(pw__window_to_load);
  GRect window_bounds = layer_get_unobstructed_bounds(window_layer);

  int w = window_bounds.size.w;
  int h = window_bounds.size.h;
  int targetAnalogWidth = w - 18;

  GRect tempRect = TEMP_RECT;
  GRect forecastHighLowRect = FORECAST_HIGHLOW_RECT;
  GRect timeRect = TIME_RECT;
  GRect stepRect = STEP_RECT;
  GRect analogRect = ANALOG_RECT;
  GRect conditionRect = CONDITION_RECT;
  GRect btRect = BT_RECT;
  GRect batteryRect = BAT_RECT;
  GRect dateRect;
  GTextAlignment dateAlign;
  if (settings.Analog) {
    dateRect = DATE_LAYER_ANALOG;
    dateAlign = DATE_ALIGN_ANALOG;
  } else {
    dateRect = DATE_LAYER_DIGITAL;
    dateAlign = DATE_ALIGN_DIGITAL;
  }

  s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_steps_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  // Analog hands layer
  s_analog_layer = layer_create(analogRect);
  calculate_perimeter(s_analog_layer);
  layer_set_update_proc(s_analog_layer, analog_update_proc);

  // Conditions layer
  s_condition_icon_bitmap[0] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_CLEAR);
  s_condition_icon_bitmap[1] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_CLEAR_NIGHT);
  s_condition_icon_bitmap[2] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_RAIN);
  s_condition_icon_bitmap[3] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_SNOW);
  s_condition_icon_bitmap[4] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_SLEET);
  s_condition_icon_bitmap[5] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_WIND);
  s_condition_icon_bitmap[6] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_FOG);
  s_condition_icon_bitmap[7] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_CLOUDY);
  s_condition_icon_bitmap[8] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_PC);
  s_condition_icon_bitmap[9] =
      gbitmap_create_with_resource(RESOURCE_ID_COND_PC_NIGHT);

  s_conditions_layer = bitmap_layer_create(conditionRect);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  bitmap_layer_get_layer(s_conditions_layer));

  s_wind_bearing_icon = layer_create(conditionRect);
  layer_set_update_proc(s_wind_bearing_icon, bearing_icon_update_proc);
  layer_add_child(window_get_root_layer(pw__window_to_load), s_wind_bearing_icon);

  s_conditions_layer_inverter = effect_layer_create(conditionRect);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  effect_layer_get_layer(s_conditions_layer_inverter));

  // Create the Bluetooth icon GBitmap
  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);

  // Create the BitmapLayer to display the GBitmap
  s_bt_icon_layer = bitmap_layer_create(btRect);
  bitmap_layer_set_background_color(s_bt_icon_layer, COLOR_CLEAR);
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  bitmap_layer_get_layer(s_bt_icon_layer));

  s_bt_icon_layer_inverter = effect_layer_create(btRect);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  effect_layer_get_layer(s_bt_icon_layer_inverter));

  // Weather forecast layer
  s_forecast_layer = layer_create(window_bounds);
  layer_set_update_proc(s_forecast_layer, forecast_update_proc);
  layer_add_child(window_get_root_layer(pw__window_to_load), s_forecast_layer);

  // Create 24 hour layer
  s_24hour_layer = layer_create(window_bounds);
  layer_set_update_proc(s_24hour_layer, label_update_proc);
  layer_add_child(window_get_root_layer(pw__window_to_load), s_24hour_layer);

  // Temperature layer
  window.tl_temperature = text_layer_create(tempRect);
  text_layer_set_background_color(window.tl_temperature, COLOR_CLEAR);
  text_layer_set_text(window.tl_temperature, "");
  text_layer_set_font(window.tl_temperature, s_date_font);
  text_layer_set_text_alignment(window.tl_temperature, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  text_layer_get_layer(window.tl_temperature));

  s_wind_speed_layer = text_layer_create(tempRect);
  text_layer_set_background_color(s_wind_speed_layer, COLOR_CLEAR);
  text_layer_set_text(s_wind_speed_layer, "");
  text_layer_set_font(s_wind_speed_layer, s_date_font);
  text_layer_set_text_alignment(s_wind_speed_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  text_layer_get_layer(s_wind_speed_layer));

  // forecast high/low text layer
  window.tl_high_low_forecast_temperature = text_layer_create(forecastHighLowRect);
  text_layer_set_background_color(window.tl_high_low_forecast_temperature, COLOR_CLEAR);
  text_layer_set_text(window.tl_high_low_forecast_temperature, "");
  text_layer_set_font(window.tl_high_low_forecast_temperature, s_date_font);
  text_layer_set_text_alignment(window.tl_high_low_forecast_temperature, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  text_layer_get_layer(window.tl_high_low_forecast_temperature));

  // forecast high/low text layer
  s_wind_bearing_layer = text_layer_create(forecastHighLowRect);
  text_layer_set_background_color(s_wind_bearing_layer, COLOR_CLEAR);
  text_layer_set_text(s_wind_bearing_layer, "");
  text_layer_set_font(s_wind_bearing_layer, s_date_font);
  text_layer_set_text_alignment(s_wind_bearing_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  text_layer_get_layer(s_wind_bearing_layer));

  // Create time TextLayer
  s_time_layer = text_layer_create(timeRect);
  text_layer_set_background_color(s_time_layer, COLOR_CLEAR);
  text_layer_set_text(s_time_layer, "--:--");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Create date TextLayer
  s_date_layer = text_layer_create(dateRect);
  text_layer_set_background_color(s_date_layer, COLOR_CLEAR);
  text_layer_set_text_alignment(s_date_layer, dateAlign);
  text_layer_set_text(s_date_layer, "");
  text_layer_set_font(s_date_layer, s_date_font);
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  text_layer_get_layer(s_date_layer));

  // Create battery meter Layer
  window.tl_battery = text_layer_create(batteryRect);
  text_layer_set_background_color(window.tl_battery, COLOR_CLEAR);
  text_layer_set_text(window.tl_battery, "");
  text_layer_set_font(window.tl_battery, s_steps_font);
  text_layer_set_text_alignment(window.tl_battery, GTextAlignmentLeft);

  // Steps layer
  s_steps_layer = text_layer_create(stepRect);
  text_layer_set_background_color(s_steps_layer, COLOR_CLEAR);
  text_layer_set_text(s_steps_layer, "");
  text_layer_set_font(s_steps_layer, s_steps_font);
  text_layer_set_text_alignment(s_steps_layer, GTextAlignmentRight);
#if defined(PBL_HEALTH)
  layer_add_child(window_get_root_layer(pw__window_to_load),
                  text_layer_get_layer(s_steps_layer));
#endif

  // Things that are different if you're analog vs digital
  if (!settings.Analog)
    layer_add_child(window_get_root_layer(pw__window_to_load),
                    text_layer_get_layer(s_time_layer));
  if (!settings.Analog)
    layer_add_child(window_get_root_layer(pw__window_to_load),
                    text_layer_get_layer(window.tl_battery));
  if (settings.Analog)
    layer_add_child(window_get_root_layer(pw__window_to_load), s_analog_layer);

  // Initialize the display
  apply_colors_to_layers();
  update_time_no_update_weather();
  battery_callback(battery_state_service_peek());
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  UnobstructedAreaHandlers handlers = {
      .will_change = prv_unobstructed_will_change,
      .did_change = prv_unobstructed_did_change};
  unobstructed_area_service_subscribe(handlers, NULL);
}

void main_window_unload(Window* _) {

  gbitmap_destroy(s_bt_icon_bitmap);
  bitmap_layer_destroy(s_bt_icon_layer);

  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_steps_layer);
  text_layer_destroy(window.tl_temperature);
  text_layer_destroy(s_wind_speed_layer);
  text_layer_destroy(window.tl_high_low_forecast_temperature);
  text_layer_destroy(s_wind_bearing_layer);
  text_layer_destroy(window.tl_battery);
  layer_destroy(s_analog_layer);
  layer_destroy(s_forecast_layer);
  effect_layer_destroy(s_bt_icon_layer_inverter);
  effect_layer_destroy(s_conditions_layer_inverter);
}

// Draw the 24 hour lines
void label_update_proc(Layer* layer, GContext* ctx) {
  GRect bounds = layer_get_unobstructed_bounds(layer);
#ifdef PBL_RECT
  graphics_context_set_stroke_color(ctx, COLOR_24H_LINES);
  graphics_context_set_stroke_width(ctx, WIDTH_24H_LINES);
  for (int i = 0; i < 24; i++) {
    // GPoint p = hours(i, bounds.size.w, bounds.size.h, 5);
    GPoint p1 = hours(i, bounds.size.w, bounds.size.h, 17);
    GPoint p2 = hours(i, bounds.size.w, bounds.size.h, 0);
    graphics_draw_line(ctx, p1, p2);
  }
  time_t temp = time(NULL);
  struct tm* tick_time = localtime(&temp);
  int hour = tick_time->tm_hour;
  GPoint p1 = hours(hour, bounds.size.w, bounds.size.h, 17);
  GPoint p2 = hours(hour, bounds.size.w, bounds.size.h, 0);
  graphics_context_set_stroke_color(ctx, COLOR_24H_CURRENT);
  graphics_context_set_stroke_width(ctx, WIDTH_24H_LINES_CURRENT);
  graphics_draw_line(ctx, p1, p2);
#else
  GRect innerBounds = grect_crop(bounds, 18);
  graphics_context_set_stroke_color(ctx, COLOR_24H_LINES);
  graphics_context_set_stroke_width(ctx, WIDTH_24H_LINES);
  for (int i = 0; i < 24; i++) {
    GPoint inner =
        gpoint_from_polar(innerBounds, GOvalScaleModeFillCircle, hr_to_a(i));
    GPoint outer =
        gpoint_from_polar(bounds, GOvalScaleModeFillCircle, hr_to_a(i));
    graphics_draw_line(ctx, inner, outer);
  }
  time_t temp = time(NULL);
  struct tm* tick_time = localtime(&temp);
  int hour = tick_time->tm_hour;
  GRect currentHourBounds = grect_crop(bounds, 20);
  GPoint p1 = gpoint_from_polar(currentHourBounds, GOvalScaleModeFillCircle,
                                hr_to_a(hour));
  GPoint p2 =
      gpoint_from_polar(bounds, GOvalScaleModeFillCircle, hr_to_a(hour));
  graphics_context_set_stroke_color(ctx, COLOR_24H_CURRENT);
  graphics_context_set_stroke_width(ctx, WIDTH_24H_LINES_CURRENT);
  graphics_draw_line(ctx, p1, p2);
#endif
}


void analog_update_proc(Layer* layer, GContext* ctx) {

  time_t temp = time(NULL);
  struct tm* tick_time = localtime(&temp);

  int minArmStart = (center.x - 20) * (100 - s_battery_level) / 100;
  int hourArmStart = (center.x - 40) * (100 - s_battery_level) / 100;

  int minArmEnd = center.x - 20;  // - minArmStart;
  int hourArmEnd = center.x - 40; // - minArmStart;

  int hourHand = (tick_time->tm_hour * 60 + tick_time->tm_min) /
                 2; // 0 to 1440 = 0h to 24h
  int hourAngle = DEG_TO_TRIGANGLE(hourHand);

  int minHand = (tick_time->tm_min * 6);
  int minAngle = DEG_TO_TRIGANGLE(minHand);

#ifdef PBL_RECT
  // Draw correct clock markings
  {
    for (int i = 0; i < 12; i++) {
      GPoint pt1 = rayFrom(DEG_TO_TRIGANGLE(i * 30), center.x - 25);
      GPoint pt2 = rayFrom(DEG_TO_TRIGANGLE(i * 30), center.x - 22);
      bool big = i % 3 == 0;
      // todo replace colors with palette
      graphics_context_set_stroke_color(ctx, COLOR_WINDOW);
      graphics_context_set_stroke_width(ctx, 2);
      graphics_draw_line(ctx, pt1, pt2);
      graphics_context_set_stroke_color(ctx,
                                        big ? COLOR_TIME : COLOR_DAY_CLOUDY);
      graphics_context_set_stroke_width(ctx, 1);
      graphics_draw_line(ctx, pt1, pt2);
    }
  }
#endif

  // Draw hub shadow
  graphics_context_set_fill_color(ctx, COLOR_WINDOW);
  graphics_fill_circle(ctx, center, 3);

  // Draw minute shadow
  {
    graphics_context_set_stroke_color(ctx, COLOR_TIME);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_line(ctx, center, rayFrom(minAngle, minArmStart));
    graphics_context_set_stroke_width(ctx, 7);
    graphics_draw_line(ctx, rayFrom(minAngle, minArmStart),
                       rayFrom(minAngle, minArmEnd));
  }

  // Draw minute hand
  {
    graphics_context_set_stroke_color(ctx, COLOR_TIME);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, center, rayFrom(minAngle, minArmStart));
    graphics_context_set_stroke_width(ctx, 5);
    graphics_draw_line(ctx, rayFrom(minAngle, minArmStart),
                       rayFrom(minAngle, minArmEnd));
  }

  // Draw hour shadow
  {
    graphics_context_set_stroke_color(ctx, COLOR_WINDOW);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_line(ctx, center, rayFrom(hourAngle, hourArmStart));
    graphics_context_set_stroke_width(ctx, 9);
    graphics_draw_line(ctx, rayFrom(hourAngle, hourArmStart),
                       rayFrom(hourAngle, hourArmEnd));
  }

  // Draw hour hand
  {
    graphics_context_set_stroke_color(ctx, COLOR_TIME);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, center, rayFrom(hourAngle, hourArmStart));
    graphics_context_set_stroke_width(ctx, 7);
    graphics_draw_line(ctx, rayFrom(hourAngle, hourArmStart),
                       rayFrom(hourAngle, hourArmEnd));
  }

  // Draw hub
  graphics_context_set_fill_color(ctx, COLOR_TIME);
  graphics_fill_circle(ctx, center, 2);
}

// set colors to layers and mark all dirty
void apply_colors_to_layers() {
  window_set_background_color(window.w_main, COLOR_WINDOW);
  text_layer_set_text_color(window.tl_temperature, COLOR_TEMP);
  text_layer_set_text_color(s_wind_speed_layer, COLOR_TEMP);
  text_layer_set_text_color(window.tl_high_low_forecast_temperature, COLOR_TEMP);
  text_layer_set_text_color(s_wind_bearing_layer, COLOR_TEMP);
  text_layer_set_text_color(s_time_layer, COLOR_TIME);
  text_layer_set_text_color(s_date_layer, COLOR_DATE);
  text_layer_set_text_color(window.tl_battery, COLOR_STEPS);
  text_layer_set_text_color(s_steps_layer, COLOR_STEPS);
  layer_mark_dirty(window_get_root_layer(window.w_main));
  layer_mark_dirty(text_layer_get_layer(window.tl_temperature));
  layer_mark_dirty(bitmap_layer_get_layer(s_bt_icon_layer));
  layer_mark_dirty(bitmap_layer_get_layer(s_conditions_layer));

  effect_layer_remove_effect(s_conditions_layer_inverter);
  if (settings.Invert_Colors && s_weather_ready) {
    effect_layer_add_effect(s_conditions_layer_inverter, effect_invert_bw_only,
                            NULL);
  }
}