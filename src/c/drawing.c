#include "src/c/luminosity.h"
#ifdef PBL_RECT

// angle figurer for square pebbles
 GPoint hours(int hour, int w, int h, int b){
    w -= b*2;
    h -= b*2;
    switch (hour % 24) {
        case 0:
        return GPoint(3*w/6+b,h+b); // 0
        case 1:
        return GPoint(2*w/6+b,h+b); // 1
        case 2:
        return GPoint(1*w/6+b,h+b); // 2
        case 3:
        return GPoint(b,6*h/6+b); // 3
        case 4:
        return GPoint(b,5*h/6+b); // 4
        case 5:
        return GPoint(b,4*h/6+b); // 5
        case 6:
        return GPoint(b,3*h/6+b); // 6
        case 7:
        return GPoint(b,2*h/6+b); // 7
        case 8:
        return GPoint(b,1*h/6+b); // 8
        case 9:
        return GPoint(b,b); // 9
        case 10:
        return GPoint(1*w/6+b,b); // 10
        case 11:
        return GPoint(2*w/6+b,b); // 11
        case 12:
        return GPoint(3*w/6+b,b); // 12
        case 13:
        return GPoint(4*w/6+b,b); // 13
        case 14:
        return GPoint(5*w/6+b,b); // 14

        case 15:
        return GPoint(w+b,b); // 15

        case 16:
        return GPoint(w+b,1*h/6+b); // 16
        case 17:
        return GPoint(w+b,2*h/6+b); // 17
        case 18:
        return GPoint(w+b,3*h/6+b); // 18
        case 19:
        return GPoint(w+b,4*h/6+b); // 19
        case 20:
        return GPoint(w+b,5*h/6+b); // 20
        case 21:
        return GPoint(w+b,h+b); // 21
        case 22:
        return GPoint(5*w/6+b,h+b); // 22
        case 23:
        default:
        return GPoint(4*w/6+b,h+b); // 23
    }
}

#else //round

// for round pebbles, figure out the angle for a particular location
 int hr_to_a(int hour){
    return DEG_TO_TRIGANGLE(
        180 + (15 * hour)
    );
}

#endif

// perimiter calc
 void calculate_perimiter(Layer* layer) {
    bounds = layer_get_bounds(layer);
    center = grect_center_point(&bounds);

    int perimiter = (bounds.size.w + bounds.size.h) * 2;
    step = perimiter / 60;
    int topcount = bounds.size.w / step;
    int sidecount = bounds.size.h / step;

    int halftop = topcount / 2;

    upperright = halftop;
    lowerright = halftop + sidecount;
    lowerleft = halftop + sidecount + topcount;
    upperleft = halftop + sidecount + topcount + sidecount;
}

//set colors to layers and mark all dirty
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
  if (settings.Invert_Colors && s_weather_ready){
    effect_layer_add_effect(s_conditions_layer_inverter, effect_invert_bw_only, NULL);
  }

}
