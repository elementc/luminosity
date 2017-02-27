#include "src/c/luminosity.h"

// Triggered when bluetooth connects/disconnects
 void bluetooth_callback(bool connected) {
    // Show icon if disconnected
    layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);

    if(!connected) {
        vibes_double_pulse();
    }
}

// Triggered when the battery has changed level
 void battery_callback(BatteryChargeState state) {
    // Record the new battery level
    s_battery_level = state.charge_percent;

    // Update meter
    layer_mark_dirty(s_analog_layer);

    // update text readout
    snprintf(s_battery_string, sizeof(s_battery_string), "%d%c", s_battery_level, '%');
    APP_LOG(APP_LOG_LEVEL_DEBUG, "battery updated");
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", s_battery_string);
    text_layer_set_text(s_battery_text_layer, s_battery_string);
}

 void health_handler(HealthEventType event, void *context) {
    // Which type of event occurred?
    switch(event) {
        case HealthEventSignificantUpdate:
        case HealthEventMovementUpdate:
        {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Health Event");

            HealthMetric metric = HealthMetricStepCount;
            time_t start = time_start_of_today();
            time_t end = time(NULL);
            HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);

            if(mask & HealthServiceAccessibilityMaskAvailable) {
                int steps = (int)health_service_sum_today(metric);

                if (steps > 999) {
                    snprintf(steps_str, 12, "%d,%03d", steps/1000, steps%1000);
                } else {
                    snprintf(steps_str, 12, "%d", steps);
                }
                text_layer_set_text(s_steps_layer, steps_str);

                APP_LOG(APP_LOG_LEVEL_DEBUG, "steps: %s ", steps_str);
            } else {
                text_layer_set_text(s_steps_layer, "");
                APP_LOG(APP_LOG_LEVEL_DEBUG, "steps unavailable");
            }

        }
        break;
        default:
        break;
    }
}

// Draw the 24 hour lines
 void label_update_proc(Layer* layer, GContext* ctx) {
    GRect bounds = layer_get_unobstructed_bounds(layer);
    #ifdef PBL_RECT
    graphics_context_set_stroke_color(ctx, COLOR_24H_LINES);
    graphics_context_set_stroke_width(ctx, WIDTH_24H_LINES);
    for (int i = 0; i < 24; i++) {
        //GPoint p = hours(i, bounds.size.w, bounds.size.h, 5);
        GPoint p1 = hours(i, bounds.size.w, bounds.size.h, 17);
        GPoint p2 = hours(i, bounds.size.w, bounds.size.h, 0);
        graphics_draw_line(ctx, p1, p2);
    }
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
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
        GPoint inner = gpoint_from_polar(innerBounds, GOvalScaleModeFillCircle, hr_to_a(i));
        GPoint outer = gpoint_from_polar(bounds, GOvalScaleModeFillCircle, hr_to_a(i));
        graphics_draw_line(ctx, inner, outer);
    }
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    int hour = tick_time->tm_hour;
    GRect currentHourBounds = grect_crop(bounds, 20);
    GPoint p1 = gpoint_from_polar(currentHourBounds, GOvalScaleModeFillCircle, hr_to_a(hour));
    GPoint p2 = gpoint_from_polar(bounds, GOvalScaleModeFillCircle, hr_to_a(hour));
    graphics_context_set_stroke_color(ctx, COLOR_24H_CURRENT);
    graphics_context_set_stroke_width(ctx, WIDTH_24H_LINES_CURRENT);
    graphics_draw_line(ctx, p1, p2);
    #endif

}

// Analog hands drawing
 GPoint rayFrom(int tri, int radius) {
    GPoint ray = {center.x + sin_lookup(tri)*radius / TRIG_MAX_RATIO , center.y - cos_lookup(tri)*radius / TRIG_MAX_RATIO };
    return ray;
}

 void analog_update_proc(Layer *layer, GContext *ctx) {

    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    int minArmStart = (center.x - 20) * (100-s_battery_level) / 100;
    int hourArmStart = (center.x - 40) * (100-s_battery_level) / 100;

    int minArmEnd = center.x - 20;// - minArmStart;
    int hourArmEnd = center.x - 40;// - minArmStart;


    int hourHand = (tick_time->tm_hour * 60 + tick_time->tm_min) / 2; // 0 to 1440 = 0h tp 24h
    int hourAngle = DEG_TO_TRIGANGLE(hourHand);

    int minHand = (tick_time->tm_min * 6);
    int minAngle = DEG_TO_TRIGANGLE(minHand);

    #ifdef PBL_RECT
    // Draw correct clock markings
    {
        for (int i = 0; i < 12; i++) {
            GPoint pt1 = rayFrom(DEG_TO_TRIGANGLE(i * 30), center.x - 25);
            GPoint pt2 = rayFrom(DEG_TO_TRIGANGLE(i * 30), center.x - 22);
            bool big = i%3==0;

            graphics_context_set_stroke_color(ctx, GColorBlack);
            graphics_context_set_stroke_width(ctx, 2);
            graphics_draw_line(ctx, pt1, pt2);
            graphics_context_set_stroke_color(ctx, big?GColorWhite:GColorLightGray);
            graphics_context_set_stroke_width(ctx, 1);
            graphics_draw_line(ctx, pt1, pt2);
        }
    }
    #endif

    // Draw hub shadow
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, center, 3);

    // Draw minute shadow
    {
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_stroke_width(ctx, 3);
        graphics_draw_line(ctx, center, rayFrom(minAngle, minArmStart));
        graphics_context_set_stroke_width(ctx, 7);
        graphics_draw_line(ctx, rayFrom(minAngle, minArmStart), rayFrom(minAngle, minArmEnd));
    }

    // Draw minute hand
    {
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_stroke_width(ctx, 1);
        graphics_draw_line(ctx, center, rayFrom(minAngle, minArmStart));
        graphics_context_set_stroke_width(ctx, 5);
        graphics_draw_line(ctx, rayFrom(minAngle, minArmStart), rayFrom(minAngle, minArmEnd));
    }

    // Draw hour shadow
    {
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_stroke_width(ctx, 3);
        graphics_draw_line(ctx, center, rayFrom(hourAngle, hourArmStart));
        graphics_context_set_stroke_width(ctx, 9);
        graphics_draw_line(ctx, rayFrom(hourAngle, hourArmStart), rayFrom(hourAngle, hourArmEnd));
    }

    // Draw hour hand
    {
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_stroke_width(ctx, 1);
        graphics_draw_line(ctx, center, rayFrom(hourAngle, hourArmStart));
        graphics_context_set_stroke_width(ctx, 7);
        graphics_draw_line(ctx, rayFrom(hourAngle, hourArmStart), rayFrom(hourAngle, hourArmEnd));
    }

    // Draw hub
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, center, 2);

}

// tick has occurred
 void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Time tick occured.");
    update_time();
}

 void main_window_load(Window *window) {

    Layer* window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_unobstructed_bounds(window_layer);

    int w = window_bounds.size.w;
    int h = window_bounds.size.h;

    #ifdef PBL_RECT
        GRect tempRect = GRect(w - 50 - 18, 15, 50, 20);
        GRect forecastHighLowRect = GRect(w - 83, 32, 65, 33);
        GRect timeRect = GRect(0, h/2-30, w, 50);
        GRect stepRect = GRect(20, h-38, w - 40, 20);
        GRect analogRect = GRect(0, 0, w, h);

        GRect conditionRect = GRect(18,18, 26,26);
        GRect btRect = GRect(w/2-10, h-38, 20, 20);
        GRect batteryRect = GRect(23, h-38, 40, 20);
    #else
        GRect tempRect = GRect(w - 100, 25, 50, 20);
        GRect forecastHighLowRect = GRect(w - 108, 39, 65, 33);
        GRect timeRect = GRect(0, h/2-30, w, 50);
        GRect stepRect = GRect(w/2, h-55, 40, 20);
        int targetAnalogWidth = w - 18;
        GRect analogRect = GRect(w/2-targetAnalogWidth/2, h/2-targetAnalogWidth/2, targetAnalogWidth, targetAnalogWidth);

        GRect conditionRect = GRect(48, 28, 26,26);
        GRect btRect = GRect(w/2-10, h-38, 20, 20);
        GRect batteryRect = GRect(45, h-55, 40, 20);

    #endif
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
      calculate_perimiter(s_analog_layer);
    layer_set_update_proc(s_analog_layer, analog_update_proc);


    // Conditions layer
    s_condition_icon_bitmap[0] = gbitmap_create_with_resource(RESOURCE_ID_COND_CLEAR);
    s_condition_icon_bitmap[1] = gbitmap_create_with_resource(RESOURCE_ID_COND_CLEAR_NIGHT);
    s_condition_icon_bitmap[2] = gbitmap_create_with_resource(RESOURCE_ID_COND_RAIN);
    s_condition_icon_bitmap[3] = gbitmap_create_with_resource(RESOURCE_ID_COND_SNOW);
    s_condition_icon_bitmap[4] = gbitmap_create_with_resource(RESOURCE_ID_COND_SLEET);
    s_condition_icon_bitmap[5] = gbitmap_create_with_resource(RESOURCE_ID_COND_WIND);
    s_condition_icon_bitmap[6] = gbitmap_create_with_resource(RESOURCE_ID_COND_FOG);
    s_condition_icon_bitmap[7] = gbitmap_create_with_resource(RESOURCE_ID_COND_CLOUDY);
    s_condition_icon_bitmap[8] = gbitmap_create_with_resource(RESOURCE_ID_COND_PC);
    s_condition_icon_bitmap[9] = gbitmap_create_with_resource(RESOURCE_ID_COND_PC_NIGHT);

    s_conditions_layer = bitmap_layer_create(conditionRect);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_conditions_layer));

    // Create the Bluetooth icon GBitmap
    s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);

    // Create the BitmapLayer to display the GBitmap
    s_bt_icon_layer = bitmap_layer_create(btRect);
    bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
    bitmap_layer_set_background_color(s_bt_icon_layer, GColorBlack);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_icon_layer));

     // Weather forecast layer
    s_forecast_layer = layer_create(window_bounds);
    layer_set_update_proc(s_forecast_layer, forecast_update_proc);
    layer_add_child(window_get_root_layer(window), s_forecast_layer);

    // Create 24 hour layer
    s_24hour_layer = layer_create(window_bounds);
    layer_set_update_proc(s_24hour_layer, label_update_proc);
    layer_add_child(window_get_root_layer(window), s_24hour_layer);


    // Temperature layer
    s_temp_layer = text_layer_create(tempRect);
    text_layer_set_background_color(s_temp_layer, GColorClear);
    text_layer_set_text_color(s_temp_layer, COLOR_TEMP);
    text_layer_set_text(s_temp_layer, "");
    text_layer_set_font(s_temp_layer, s_date_font);
    text_layer_set_text_alignment(s_temp_layer, GTextAlignmentRight);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temp_layer));

    //forecast high/low text layer
    s_forecast_high_low_layer = text_layer_create(forecastHighLowRect);
    text_layer_set_background_color(s_forecast_high_low_layer, GColorClear);
    text_layer_set_text_color(s_forecast_high_low_layer, COLOR_TEMP);
    text_layer_set_text(s_forecast_high_low_layer, "");
    text_layer_set_font(s_forecast_high_low_layer, s_date_font);
    text_layer_set_text_alignment(s_forecast_high_low_layer, GTextAlignmentRight);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_forecast_high_low_layer));

    // Create time TextLayer
    s_time_layer = text_layer_create(timeRect);
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, COLOR_TIME);
    text_layer_set_text(s_time_layer, "--:--");
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);


    // Create date TextLayer
    s_date_layer = text_layer_create(dateRect);
    text_layer_set_text_color(s_date_layer, COLOR_DATE);
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_alignment(s_date_layer, dateAlign);
    text_layer_set_text(s_date_layer, "");
    text_layer_set_font(s_date_layer, s_date_font);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

    // Create battery meter Layer
    s_battery_text_layer = text_layer_create(batteryRect);
    text_layer_set_background_color(s_battery_text_layer, GColorClear);
    text_layer_set_text_color(s_battery_text_layer, COLOR_STEPS);
    text_layer_set_text(s_battery_text_layer, "");
    text_layer_set_font(s_battery_text_layer, s_steps_font);
    text_layer_set_text_alignment(s_battery_text_layer, GTextAlignmentLeft);

    // Steps layer
    s_steps_layer = text_layer_create(stepRect);
    text_layer_set_background_color(s_steps_layer, GColorClear);
    text_layer_set_text_color(s_steps_layer, COLOR_STEPS);
    text_layer_set_text(s_steps_layer, "");
    text_layer_set_font(s_steps_layer, s_steps_font);
    text_layer_set_text_alignment(s_steps_layer, GTextAlignmentRight);
    #if defined(PBL_HEALTH)
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_steps_layer));
    #endif

    // Things that are different if you're analog vs digital
    if (!settings.Analog) layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    if (!settings.Analog) layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_text_layer));
    if (settings.Analog) layer_add_child(window_get_root_layer(window), s_analog_layer);

    // Initialize the display
    update_time_no_update_weather();
    battery_callback(battery_state_service_peek());
    bluetooth_callback(connection_service_peek_pebble_app_connection());

}

 void main_window_unload(Window *window) {

    gbitmap_destroy(s_bt_icon_bitmap);
    bitmap_layer_destroy(s_bt_icon_layer);

    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_steps_layer);
    text_layer_destroy(s_temp_layer);
    text_layer_destroy(s_forecast_high_low_layer);
    text_layer_destroy(s_battery_text_layer);

    layer_destroy(s_analog_layer);
    layer_destroy(s_forecast_layer);
}

 void init() {
    prv_load_settings();

    s_space_ready = true;
    s_weather_ready = false;
   // s_forecast_layer_displaying_wind = true;

    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);

    window_set_background_color(s_main_window, COLOR_WINDOW);

    // Register with Event Services
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    battery_state_service_subscribe(battery_callback);

    #if defined(PBL_HEALTH)
    // Attempt to subscribe
    if(!health_service_events_subscribe(health_handler, NULL)) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    }
    health_handler(HealthEventSignificantUpdate, NULL);
    #else
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    #endif

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    const int inbox_size = 250;
    const int outbox_size = 5;
    app_message_open(inbox_size, outbox_size);

    connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = bluetooth_callback
    });

}

 void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
