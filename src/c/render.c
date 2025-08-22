#include "src/c/render.h"

#include "src/c/color_palette.h"
#include "src/c/declarations.h"
#include "src/c/drawing.h"
#include "src/c/layout_constants.h"

// Analog hands drawing
GPoint rayFrom(int tri, int radius) {
  GPoint ray = {center.x + sin_lookup(tri) * radius / TRIG_MAX_RATIO,
                center.y - cos_lookup(tri) * radius / TRIG_MAX_RATIO};
  return ray;
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
  window_set_background_color(s_main_window, COLOR_WINDOW);
  text_layer_set_text_color(s_temp_layer, COLOR_TEMP);
  text_layer_set_text_color(s_wind_speed_layer, COLOR_TEMP);
  text_layer_set_text_color(s_forecast_high_low_layer, COLOR_TEMP);
  text_layer_set_text_color(s_wind_bearing_layer, COLOR_TEMP);
  text_layer_set_text_color(s_time_layer, COLOR_TIME);
  text_layer_set_text_color(s_date_layer, COLOR_DATE);
  text_layer_set_text_color(s_battery_text_layer, COLOR_STEPS);
  text_layer_set_text_color(s_steps_layer, COLOR_STEPS);
  layer_mark_dirty(window_get_root_layer(s_main_window));
  layer_mark_dirty(text_layer_get_layer(s_temp_layer));
  layer_mark_dirty(bitmap_layer_get_layer(s_bt_icon_layer));
  layer_mark_dirty(bitmap_layer_get_layer(s_conditions_layer));

  effect_layer_remove_effect(s_conditions_layer_inverter);
  if (settings.Invert_Colors && s_weather_ready) {
    effect_layer_add_effect(s_conditions_layer_inverter, effect_invert_bw_only,
                            NULL);
  }
}