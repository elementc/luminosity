#include <pebble.h>

#define COLOR_TIME GColorWhite
#define COLOR_DATE GColorWhite
#define COLOR_TEMP GColorWhite
#define COLOR_STEPS GColorWhite
#define COLOR_BATT GColorWhite

#define COLOR_24H_LINES GColorBlack
#define COLOR_24H_CURRENT GColorRed

#define COLOR_NIGHT GColorLavenderIndigo
#define COLOR_NIGHT_PARTLY GColorIndigo
#define COLOR_NIGHT_CLOUDY GColorDarkGray
#define COLOR_DAY GColorIcterine
#define COLOR_DAY_PARTLY GColorPastelYellow
#define COLOR_DAY_CLOUDY GColorLightGray

#define COLOR_CLOUDY GColorDarkGray
#define COLOR_RAINY GColorBlue
#define COLOR_SNOWY GColorWhite
#define COLOR_SLEETY GColorCeleste
#define COLOR_PARTLY GColorLightGray

#define NO_WEATHER_SKY_LINE_WIDTH 4

#define TRI_W 8

// Persistent storage key
#define SETTINGS_KEY 1

// Define our settings struct
typedef struct ClaySettings {
    bool Analog;
} ClaySettings;

// An instance of the struct
static ClaySettings settings;

/* Main */
static Window *s_main_window;

/* Battery */
static Layer *s_battery_layer;
static int s_battery_level;
static bool s_charging;

/* Weather */
static TextLayer *s_temp_layer;
static TextLayer *s_forecast_high_low_layer;
static char forecast_clouds_str[25];
static char forecast_precip_type_str[25];
static char forecast_precip_intensity_str[25];
static char forecast_temp_str[25];
static bool s_weather_ready;
static Layer *s_forecast_layer;
static BitmapLayer *s_conditions_layer;

/* Time and date */
static TextLayer *s_time_layer, *s_date_layer;
static GFont s_time_font, s_date_font;
static Layer *s_analog_layer, *s_24hour_layer;

/* Steps */
static TextLayer *s_steps_layer;
static GFont s_steps_font;
static char steps_str[12];

// TODO: needs finishing
/* Bluetooth */
static BitmapLayer *s_bt_icon_layer;
static GBitmap *s_bt_icon_bitmap;

/* Space */
static int s_sunset, s_sunrise;
static bool s_space_ready;
static Layer *s_sky_layer;

static char* condition_icons[] = {
    "clear-day", "clear-night", "rain", "snow", "sleet", "wind", "fog", "cloudy", "partly-cloudy-day", "partly-cloudy-night"
};

static GBitmap* s_condition_icon_bitmap[10];


static void prv_update_display() {
    // hide and display things according to settings
    Layer* winrl = window_get_root_layer(s_main_window);
    Layer* window_layer = window_get_root_layer(s_main_window);
    GRect window_bounds = layer_get_unobstructed_bounds(window_layer);
    int w = window_bounds.size.w;
    int h = window_bounds.size.h;


    if (settings.Analog) {
        layer_remove_from_parent(text_layer_get_layer(s_time_layer));
        layer_remove_from_parent(s_battery_layer);
        layer_add_child(winrl, s_analog_layer);

        layer_set_frame(text_layer_get_layer(s_date_layer), GRect(20, h-38, w, 30));
        text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
    } else {
        layer_add_child(winrl, text_layer_get_layer(s_time_layer));  
        layer_add_child(winrl, s_battery_layer);  
        layer_remove_from_parent(s_analog_layer);

        layer_set_frame(text_layer_get_layer(s_date_layer), GRect(0, h/2+15, w, 30));
        text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    }
}

// Initialize the default settings
static void prv_default_settings() {
    settings.Analog = false;
}

// Read settings from persistent storage
static void prv_load_settings() {
    prv_default_settings();
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
    prv_update_display();
}


static GPoint hours(int hour, int w, int h, int b)
{
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


// Triggered when bluetooth connects/disconnects
static void bluetooth_callback(bool connected) {
    // Show icon if disconnected
    layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);

    if(!connected) {
        vibes_double_pulse();
    }
}


// Triggered when the battery has changed level
static void battery_callback(BatteryChargeState state) {
    // Record the new battery level
    s_battery_level = state.charge_percent;
    s_charging = state.is_charging; 

    // Update meter
    layer_mark_dirty(s_battery_layer);
    layer_mark_dirty(s_analog_layer);
}

static void health_handler(HealthEventType event, void *context) {
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
                text_layer_set_text(s_steps_layer, "No Step Data");
                APP_LOG(APP_LOG_LEVEL_DEBUG, "steps unavailable");
            }

        }
        break;
        default:
        break;
    }
}

// send an empty ping to get new weather data
static void send_message() {

    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
}

// Triggered when the time has changed
static void update_time() {
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);

    // Create a long-lived buffer, and show the time
    static char buffer[] = "00:00";
    if(clock_is_24h_style()) {
        strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
    } else {
        strftime(buffer, sizeof(buffer), "%I:%M", tick_time);
    }
    text_layer_set_text(s_time_layer, buffer);

    if (settings.Analog) layer_mark_dirty(s_analog_layer);

    // Show the date
    static char date_buffer[16];
    strftime(date_buffer, sizeof(date_buffer), "%b %e", tick_time);
    text_layer_set_text(s_date_layer, date_buffer);

    if(tick_time->tm_min % 15 == 0) {
        send_message();
    }
}

// battery layer update proc
static void battery_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);

    // Find the width of the bar
    int width = (int)(float)(((float)s_battery_level / 100.0F) * bounds.size.w);

    // Draw the background

    graphics_context_set_stroke_color(ctx, s_charging ? GColorMediumSpringGreen : COLOR_BATT);
    graphics_draw_rect (ctx, GRect(0, 0, bounds.size.w, bounds.size.h));

    // Draw the bar
    graphics_context_set_fill_color(ctx, s_charging ? GColorMediumSpringGreen : COLOR_BATT);
    graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), GCornerNone , 0);
}

static int upperright, lowerright, lowerleft, upperleft, step;
static GRect bounds;
static GPoint center;

// perimiter calc
static void calculate_perimiter(Layer* layer) {
    bounds = layer_get_bounds(layer);    
    center = grect_center_point(&bounds);

    int perimiter = (bounds.size.w + bounds.size.h) * 2;
    step = perimiter / 60;
    int topcount = bounds.size.w / step;
    int sidecount = bounds.size.h / step;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Perimiter: %d, step: %d, topcount: %d, sidecount: %d", perimiter, step, topcount, sidecount);

    int halftop = topcount / 2;

    upperright = halftop;
    lowerright = halftop + sidecount;
    lowerleft = halftop + sidecount + topcount;
    upperleft = halftop + sidecount + topcount + sidecount;
}

// Draw the 24 hour lines
static void label_update_proc(Layer* layer, GContext* ctx) {
    GRect bounds = layer_get_unobstructed_bounds(layer);

    for (int i = 0; i < 24; i++) {
        //GPoint p = hours(i, bounds.size.w, bounds.size.h, 5);
        GPoint p1 = hours(i, bounds.size.w, bounds.size.h, 17);
        GPoint p2 = hours(i, bounds.size.w, bounds.size.h, 0);
        graphics_context_set_stroke_color(ctx, COLOR_24H_LINES);
        graphics_draw_line(ctx, p1, p2);
    }
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);
    int hour = tick_time->tm_hour;
    GPoint p1 = hours(hour, bounds.size.w, bounds.size.h, 17);
    GPoint p2 = hours(hour, bounds.size.w, bounds.size.h, 0);
    graphics_context_set_stroke_color(ctx, COLOR_24H_CURRENT);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_draw_line(ctx, p1, p2);

}

// Draw forecast ring
static void forecast_update_proc(Layer* layer, GContext* ctx) {
    GRect fcst_bounds = layer_get_unobstructed_bounds(layer);
    int w = fcst_bounds.size.w;
    int h = fcst_bounds.size.h;

    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);
    int hour = tick_time->tm_hour;
    if (!s_space_ready && !s_weather_ready) //if we have neither, we can do neither
        return;
    else if (!s_weather_ready && s_space_ready){ //if we have space only, we can draw just the sunrise/sunset ring
        for (int i = 0; i< 24; i++){
            graphics_context_set_stroke_width(ctx, NO_WEATHER_SKY_LINE_WIDTH);
            if (i > s_sunset || i <= s_sunrise){
                //night color pick
                graphics_context_set_stroke_color(ctx, COLOR_NIGHT);
            }else{
                //day color pick
                graphics_context_set_stroke_color(ctx, COLOR_DAY);
            }
            int center = NO_WEATHER_SKY_LINE_WIDTH / 2;
            GPoint p1 = hours(i, w, h, center);
            GPoint p2 = hours(i+1, w, h, center);
            graphics_draw_line(ctx, p1, p2);     
        }
        return;
    }
    else if (!s_space_ready) //need both space and weather to do the whole set of rings...
        return;
    else{
        //outer (light + temp + clouds) ring
        for (int i = 0; i< 24; i++){
            
            int temp = (forecast_temp_str[((24-hour) + i) % 24] - '0') + 2;
            graphics_context_set_stroke_width(ctx, temp);
            int center = temp / 2;
            if (i > s_sunset || i <= s_sunrise){
                //night color pick
                if (forecast_clouds_str[((24-hour) + i) % 24] == '0')
                    graphics_context_set_stroke_color(ctx, COLOR_NIGHT);
                else if (forecast_clouds_str[((24-hour) + i) % 24] == '1')
                    graphics_context_set_stroke_color(ctx, COLOR_NIGHT_PARTLY);
                else
                    graphics_context_set_stroke_color(ctx, COLOR_NIGHT_CLOUDY);
            }else{
                //day color pick
                if (forecast_clouds_str[((24-hour) + i) % 24] == '0')
                    graphics_context_set_stroke_color(ctx, COLOR_DAY);
                else if (forecast_clouds_str[((24-hour) + i) % 24] == '1')
                    graphics_context_set_stroke_color(ctx, COLOR_DAY_PARTLY);
                else
                    graphics_context_set_stroke_color(ctx, COLOR_DAY_CLOUDY);
            }
            GPoint p1 = hours(i, w, h, center);
            GPoint p2 = hours(i+1, w, h, center);
            graphics_draw_line(ctx, p1, p2);     
        }
        
        graphics_context_set_stroke_width(ctx, 1); //reset stroke width
        
        for (int i = 0; i < 24; i++) {
            //c, r, s, p, _, ? = cloudy, rain, snow, partly cloudy, clear, unknown
            switch (forecast_precip_type_str[((24-hour) + i) % 24]) {
                case 'r':
                    graphics_context_set_stroke_color(ctx, COLOR_RAINY);
                    break;
                case 's':
                    graphics_context_set_stroke_color(ctx, COLOR_SNOWY);
                    break;
                case 'l':
                    graphics_context_set_stroke_color(ctx, COLOR_SLEETY);
                    break;
                case '_':
                case '?':
                default:
                    continue; // don't draw clear segments!
    
            }
            int temp = (forecast_temp_str[((24-hour) + i) % 24] - '0') + 2;
            int width = (forecast_precip_intensity_str[((24-hour) + i) % 24] - '0')+2;
            int center = (width/2) + temp;
            graphics_context_set_stroke_width(ctx, 1); //reset stroke width in case the next op comes out to 0
            graphics_context_set_stroke_width(ctx, width);
            GPoint p1 = hours(i ,w,h,center);
            GPoint p2 = hours(i + 1,w,h,center);
            graphics_draw_line(ctx, p1, p2);
        }
    }
}

// Analog hands drawing
static GPoint rayFrom(int tri, int radius) {
    GPoint ray = {center.x + sin_lookup(tri)*radius / TRIG_MAX_RATIO , center.y - cos_lookup(tri)*radius / TRIG_MAX_RATIO };
    return ray;
}

static void analog_update_proc(Layer *layer, GContext *ctx) {

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


// Space drawing
static void sky_update_proc(Layer *layer, GContext *ctx) {

    // Draw background
    for (int i = 0; i < 24; i++) {    
    }
}


// tick has occurred 
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();

}

// appmessage
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Inbox received message");

    // Check for settings tuples
    bool changedSettings = false;

    Tuple *cfg_analog_tuple = dict_find(iterator, MESSAGE_KEY_CFG_ANALOG);
    if (cfg_analog_tuple) {
        settings.Analog = cfg_analog_tuple->value->int32 == 1;
        changedSettings = true;
    }

    if (changedSettings) {
        prv_save_settings();
        send_message();
    }


    // Weather data
    static char temperature_buffer[8];
    static char forecast_high_low_buffer[16];

    // Read tuples for data
    Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
    Tuple *forecast_high_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_HIGH);
    Tuple *forecast_low_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_LOW);
    Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
    Tuple *forecast_clouds_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_CLOUDS);
    Tuple *forecast_precip_type_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_PRECIP_TYPE);
    Tuple *forecast_precip_intensity_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_PRECIP_INTENSITY);
    Tuple *forecast_temp_tuple = dict_find(iterator, MESSAGE_KEY_FORECAST_TEMP);

    // If all data is available, use it
    if(temp_tuple) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Inbox received weather data");

        snprintf(temperature_buffer, sizeof(temperature_buffer), "%d˚", (int)temp_tuple->value->int32); // TODO units
        snprintf(forecast_high_low_buffer, sizeof(forecast_high_low_buffer), "%d˚/%d˚", (int)forecast_high_tuple->value->int32, (int)forecast_low_tuple->value->int32);
        snprintf(forecast_clouds_str, sizeof(forecast_clouds_str), "%s", forecast_clouds_tuple->value->cstring);
        snprintf(forecast_precip_type_str, sizeof(forecast_precip_type_str), "%s", forecast_precip_type_tuple->value->cstring);
        snprintf(forecast_precip_intensity_str, sizeof(forecast_precip_intensity_str), "%s", forecast_precip_intensity_tuple->value->cstring);
        snprintf(forecast_temp_str, sizeof(forecast_temp_str), "%s", forecast_temp_tuple->value->cstring);
        text_layer_set_text(s_temp_layer, temperature_buffer);
        text_layer_set_text(s_forecast_high_low_layer, forecast_high_low_buffer);

        
        char* conditions = conditions_tuple->value->cstring;
        for (int i = 0; i < 10; i++) {
            if (strcmp(conditions, condition_icons[i]) == 0) {
                bitmap_layer_set_bitmap(s_conditions_layer, s_condition_icon_bitmap[i]);
            }
        }
        
        layer_mark_dirty(text_layer_get_layer(s_temp_layer));
        layer_mark_dirty(text_layer_get_layer(s_forecast_high_low_layer));
        layer_mark_dirty(bitmap_layer_get_layer(s_conditions_layer));
        s_weather_ready = true;
        layer_mark_dirty(s_forecast_layer);

    }
    // space data
    // Read tuples for data
    Tuple *sunrise_tuple = dict_find(iterator, MESSAGE_KEY_SPACE_SUNRISE);
    Tuple *sunset_tuple = dict_find(iterator, MESSAGE_KEY_SPACE_SUNSET);

    if(sunrise_tuple) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Inbox recieved space data.");
        s_sunset = sunset_tuple->value->int32;
        s_sunrise = sunrise_tuple->value->int32;
        s_space_ready = true;
        layer_mark_dirty(s_forecast_layer);
    }

}

static char reasonStr[20];
static void getAppMessageResult(AppMessageResult reason){
    switch(reason){
    case APP_MSG_OK:
    snprintf(reasonStr,20,"%s","APP_MSG_OK");
    break;
    case APP_MSG_SEND_TIMEOUT:
    snprintf(reasonStr,20,"%s","SEND TIMEOUT");
    break;
    case APP_MSG_SEND_REJECTED:
    snprintf(reasonStr,20,"%s","SEND REJECTED");
    break;
    case APP_MSG_NOT_CONNECTED:
    snprintf(reasonStr,20,"%s","NOT CONNECTED");
    break;
    case APP_MSG_APP_NOT_RUNNING:
    snprintf(reasonStr,20,"%s","NOT RUNNING");
    break;
    case APP_MSG_INVALID_ARGS:
    snprintf(reasonStr,20,"%s","INVALID ARGS");
    break;
    case APP_MSG_BUSY:
    snprintf(reasonStr,20,"%s","BUSY");
    break;
    case APP_MSG_BUFFER_OVERFLOW:
    snprintf(reasonStr,20,"%s","BUFFER OVERFLOW");
    break;
    case APP_MSG_ALREADY_RELEASED:
    snprintf(reasonStr,20,"%s","ALRDY RELEASED");
    break;
    case APP_MSG_CALLBACK_ALREADY_REGISTERED:
    snprintf(reasonStr,20,"%s","CLB ALR REG");
    break;
    case APP_MSG_CALLBACK_NOT_REGISTERED:
    snprintf(reasonStr,20,"%s","CLB NOT REG");
    break;
    case APP_MSG_OUT_OF_MEMORY:
    snprintf(reasonStr,20,"%s","OUT OF MEM");
    break;   
    case APP_MSG_CLOSED:
        snprintf(reasonStr,20,"%s","MSG CLOSED");
        break;  
    case APP_MSG_INTERNAL_ERROR:
        snprintf(reasonStr,20,"%s","INTERNAL ERR");
        break;  
    case APP_MSG_INVALID_STATE:
        snprintf(reasonStr,20,"%s","INVALID STATE");
        break;  
    }

}


static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
    getAppMessageResult(reason);
    APP_LOG(APP_LOG_LEVEL_ERROR, reasonStr);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *window) {

    Layer* window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_unobstructed_bounds(window_layer);

    int w = window_bounds.size.w;
    int h = window_bounds.size.h;

    //GRect cityRect = GRect(20, 15, 100, 25);
    GRect tempRect = GRect(w - 50 - 18, 15, 50, 20);
    GRect forecastHighLowRect = GRect(w - 83, 32, 65, 33);
    GRect timeRect = GRect(0, h/2-30, w, 50);
    GRect stepRect = GRect(20, h-38, w - 40, 20);

    GRect conditionRect = GRect(18,18, 26,26);    
    GRect btRect = GRect(w/2-10, 20, 20, 20);
    GRect batteryRect = GRect(23, h-28, 40, 5);

    GRect dateRect;
    GTextAlignment dateAlign;

    if (settings.Analog) {
        dateRect = GRect(20, h-38, w, 30);
        dateAlign = GTextAlignmentLeft;
    } else {
        dateRect = GRect(0, h/2+15, w, 30);
        dateAlign = GTextAlignmentCenter;
    }

    s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
    s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    s_steps_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);


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
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_icon_layer));

    // Create sky layer
    s_sky_layer = layer_create(window_bounds);
    layer_set_update_proc(s_sky_layer, sky_update_proc);
    layer_add_child(window_get_root_layer(window), s_sky_layer);
    
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
    s_battery_layer = layer_create(batteryRect);
    layer_set_update_proc(s_battery_layer, battery_update_proc);

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

    // Analog hands layer
    s_analog_layer = layer_create(window_bounds);
    layer_set_update_proc(s_analog_layer, analog_update_proc);

    // Things that are different if you're analog vs digital
    if (!settings.Analog) layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    if (!settings.Analog) layer_add_child(window_get_root_layer(window), s_battery_layer);
    if (settings.Analog) layer_add_child(window_get_root_layer(window), s_analog_layer);

    // Initialize the display
    update_time();
    battery_callback(battery_state_service_peek());
    bluetooth_callback(connection_service_peek_pebble_app_connection());

}

static void main_window_unload(Window *window) {

    gbitmap_destroy(s_bt_icon_bitmap);
    bitmap_layer_destroy(s_bt_icon_layer);

    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_steps_layer);
    text_layer_destroy(s_temp_layer);

    layer_destroy(s_battery_layer);
    layer_destroy(s_sky_layer);
    layer_destroy(s_analog_layer);
    layer_destroy(s_forecast_layer);
}

static void init() {
    prv_load_settings();

    s_space_ready = false;
    s_weather_ready = false;

    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);

    window_set_background_color(s_main_window, GColorBlack);

    Layer* window_layer = window_get_root_layer(s_main_window);
    calculate_perimiter(window_layer);

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

    const int inbox_size = 170;
    const int outbox_size = 150;
    app_message_open(inbox_size, outbox_size);

    connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = bluetooth_callback
    });

}

static void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
