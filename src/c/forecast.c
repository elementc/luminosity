#include "src/c/luminosity.h"

// Draw forecast ring
 void forecast_update_proc(Layer* layer, GContext* ctx) {
    GRect fcst_bounds = layer_get_unobstructed_bounds(layer);

    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    int hour = tick_time->tm_hour;

    #ifdef PBL_RECT

    int w = fcst_bounds.size.w;
    int h = fcst_bounds.size.h;
    if (!s_space_ready) //if we have no space, we can do neither
        return;
    else if (!s_weather_ready && s_space_ready){ //if we have space only, we can draw just the sunrise/sunset ring
        for (int i = 0; i< 24; i++){
            int width = NO_WEATHER_SKY_LINE_WIDTH;
            if (i >= s_sunset || i < s_sunrise){
                //night color pick
                graphics_context_set_fill_color(ctx, COLOR_NIGHT);
            }else{
                //day color pick
                graphics_context_set_fill_color(ctx, COLOR_DAY);
            }
            GPoint p1 = hours(i, w, h, 0);
            GPoint p2 = hours(i+1, w, h, 0);
            GRect r1;
            if (i < 3 || i >= 21) //bottom
               r1 = GRect(p2.x, p1.y-width, p1.x - p2.x, width);
            else if (i >=3 && i < 9) //left side
                r1 = GRect(p1.x, p1.y, width, p2.y - p1.y);
            else if (i >= 9 && i < 15) //top
                r1 = GRect(p1.x, p1.y, p2.x-p1.x, width);
            else //right side
                r1 = GRect(p1.x - width, p1.y, width, p2.y - p1.y);

            graphics_fill_rect(ctx, r1, 0, GCornerNone);
        }
        return;
    }
    else{ // full set of rings
        for (int i = 0; i< 24; i++){

            //outer (light + temp + clouds) ring
            int temp = (forecast_temp_str[((24-hour) + i) % 24] - '0') + 2;
            graphics_context_set_stroke_width(ctx, temp);
            if (i >= s_sunset || i < s_sunrise){
                //night color pick
                if (forecast_clouds_str[((24-hour) + i) % 24] == '0')
                    graphics_context_set_fill_color(ctx, COLOR_NIGHT);
                else if (forecast_clouds_str[((24-hour) + i) % 24] == '1')
                    graphics_context_set_fill_color(ctx, COLOR_NIGHT_PARTLY);
                else
                    graphics_context_set_fill_color(ctx, COLOR_NIGHT_CLOUDY);
            }else{
                //day color pick
                if (forecast_clouds_str[((24-hour) + i) % 24] == '0')
                    graphics_context_set_fill_color(ctx, COLOR_DAY);
                else if (forecast_clouds_str[((24-hour) + i) % 24] == '1')
                    graphics_context_set_fill_color(ctx, COLOR_DAY_PARTLY);
                else
                    graphics_context_set_fill_color(ctx, COLOR_DAY_CLOUDY);
            }
            GPoint p1 = hours(i, w, h, 0);
            GPoint p2 = hours(i+1, w, h, 0);

            GRect r1;
            if (i < 3 || i >= 21) //bottom
                r1 = GRect(p2.x, p1.y-temp, p1.x - p2.x, temp);
            else if (i >=3 && i < 9) //left side
                r1 = GRect(p1.x, p1.y, temp, p2.y - p1.y);
            else if (i >= 9 && i < 15) //top
                r1 = GRect(p1.x, p1.y, p2.x-p1.x, temp);
            else //right side
                r1 = GRect(p1.x - temp, p1.y, temp, p2.y - p1.y);

            graphics_fill_rect(ctx, r1, 0, GCornerNone);

            // inner (precip) ring
            if (s_forecast_layer_displaying_wind){
                // write method for displaying wind data
                if (i >= s_sunset || i < s_sunrise){
                  graphics_context_set_fill_color(ctx, COLOR_WIND_NIGHT);
                }
                else {
                  graphics_context_set_fill_color(ctx, COLOR_WIND_DAY);
                }
                int width = (forecast_wind_intensity_str[((24-hour) + i) % 24] - '0')+2;
                p1 = hours(i, w, h, temp);
                p2 = hours(i+1, w, h, temp);
                if (i < 3 || i >= 21) //bottom
                    r1 = GRect(p2.x, (p1.y - width), p1.x - p2.x, width);
                else if (i >=3 && i < 9) //left side
                    r1 = GRect(p1.x , p1.y, width, p2.y - p1.y);
                else if (i >= 9 && i < 15) //top
                    r1 = GRect(p1.x, p1.y , p2.x-p1.x, width);
                else //right side
                    r1 = GRect(p1.x - width, p1.y, width, p2.y - p1.y);

                graphics_fill_rect(ctx, r1, 0, GCornerNone);
            }
            else {
                //c, r, s, p, _, ? = cloudy, rain, snow, partly cloudy, clear, unknown
                if (i >= s_sunset || i < s_sunrise){
                    switch (forecast_precip_type_str[((24-hour) + i) % 24]) {
                        case 'r':
                            graphics_context_set_fill_color(ctx, COLOR_RAINY_NIGHT);
                            break;
                        case 's':
                            graphics_context_set_fill_color(ctx, COLOR_SNOWY_NIGHT);
                            break;
                        case 'l':
                            graphics_context_set_fill_color(ctx, COLOR_SLEETY_NIGHT);
                            break;
                        case '_':
                        case '?':
                        default:
                            continue; // don't draw clear segments!
                    }
                }
                else {
                    switch (forecast_precip_type_str[((24-hour) + i) % 24]) {
                        case 'r':
                            graphics_context_set_fill_color(ctx, COLOR_RAINY_DAY);
                            break;
                        case 's':
                            graphics_context_set_fill_color(ctx, COLOR_SNOWY_DAY);
                            break;
                        case 'l':
                            graphics_context_set_fill_color(ctx, COLOR_SLEETY_DAY);
                            break;
                        case '_':
                        case '?':
                        default:
                            continue; // don't draw clear segments!
                    }
                }
                int width = (forecast_precip_intensity_str[((24-hour) + i) % 24] - '0')+2;
                p1 = hours(i, w, h, temp);
                p2 = hours(i+1, w, h, temp);
                if (i < 3 || i >= 21) //bottom
                    r1 = GRect(p2.x, (p1.y - width), p1.x - p2.x, width);
                else if (i >=3 && i < 9) //left side
                    r1 = GRect(p1.x , p1.y, width, p2.y - p1.y);
                else if (i >= 9 && i < 15) //top
                    r1 = GRect(p1.x, p1.y , p2.x-p1.x, width);
                else //right side
                    r1 = GRect(p1.x - width, p1.y, width, p2.y - p1.y);

                graphics_fill_rect(ctx, r1, 0, GCornerNone);
            }
        }
        return;
    }

    #else

    if (!s_space_ready) //if we have no space, we can do neither
        return;
    else if (!s_weather_ready && s_space_ready){ //if we have space only, we can draw just the sunrise/sunset ring
        GRect bounds = grect_crop(fcst_bounds, NO_WEATHER_SKY_LINE_WIDTH / 2);
        for (int i = 0; i < 24; i++){
             if (i >= s_sunset || i < s_sunrise){
                //night color pick
                graphics_context_set_stroke_color(ctx, COLOR_NIGHT);
            }
            else{
                //day color pick
                graphics_context_set_stroke_color(ctx, COLOR_DAY);
            }
            graphics_context_set_stroke_width(ctx, NO_WEATHER_SKY_LINE_WIDTH);
            graphics_draw_arc(ctx, bounds, GOvalScaleModeFitCircle, hr_to_a(i), hr_to_a(i+1));
        }

        return;
    }
    else{ //full set of rings
        for (int i = 0; i< 24; i++){
        //outer (light + temp + clouds) ring
            int temp = (forecast_temp_str[((24-hour) + i) % 24] - '0') + 6;
            graphics_context_set_stroke_width(ctx, temp);
            if (i >= s_sunset || i < s_sunrise){
                //night color pick
                if (forecast_clouds_str[((24-hour) + i) % 24] == '0')
                    graphics_context_set_stroke_color(ctx, COLOR_NIGHT);
                else if (forecast_clouds_str[((24-hour) + i) % 24] == '1')
                    graphics_context_set_stroke_color(ctx, COLOR_NIGHT_PARTLY);
                else
                    graphics_context_set_stroke_color(ctx, COLOR_NIGHT_CLOUDY);
            }
            else{
                //day color pick
                if (forecast_clouds_str[((24-hour) + i) % 24] == '0')
                    graphics_context_set_stroke_color(ctx, COLOR_DAY);
                else if (forecast_clouds_str[((24-hour) + i) % 24] == '1')
                    graphics_context_set_stroke_color(ctx, COLOR_DAY_PARTLY);
                else
                    graphics_context_set_stroke_color(ctx, COLOR_DAY_CLOUDY);
            }
            int p1 = hr_to_a(i);
            int p2 = hr_to_a(i+1);
            GRect r1 = grect_crop(fcst_bounds, temp/2);
            graphics_draw_arc(ctx, r1, GOvalScaleModeFitCircle, p1, p2);

            // inner ring
            if (s_forecast_layer_displaying_wind){
                //write method for displaying wind data
                if (i >= s_sunset || i < s_sunrise){
                    graphics_context_set_stroke_color(ctx, COLOR_WIND_NIGHT);
                }
                else {
                    graphics_context_set_stroke_color(ctx, COLOR_WIND_DAY);
                }
                int width = (forecast_wind_intensity_str[((24-hour)+i) % 24] - '0');
                graphics_context_set_stroke_width(ctx, width);
                GRect r2 = grect_crop(fcst_bounds, temp + width/2);
                graphics_draw_arc(ctx, r2, GOvalScaleModeFitCircle, p1, p2);
            }
            else{
                //precip data
                //c, r, s, p, _, ? = cloudy, rain, snow, partly cloudy, clear, unknown
                if (i >= s_sunset || i < s_sunrise){
                    switch (forecast_precip_type_str[((24-hour) + i) % 24]) {
                        case 'r':
                            graphics_context_set_stroke_color(ctx, COLOR_RAINY_NIGHT);
                            break;
                        case 's':
                            graphics_context_set_stroke_color(ctx, COLOR_SNOWY_NIGHT);
                            break;
                        case 'l':
                            graphics_context_set_stroke_color(ctx, COLOR_SLEETY_NIGHT);
                            break;
                        case '_':
                        case '?':
                        default:
                            continue; // don't draw clear segments!
                    }
                }
                else {
                    switch (forecast_precip_type_str[((24-hour) + i) % 24]) {
                        case 'r':
                            graphics_context_set_stroke_color(ctx, COLOR_RAINY_DAY);
                            break;
                        case 's':
                            graphics_context_set_stroke_color(ctx, COLOR_SNOWY_DAY);
                            break;
                        case 'l':
                            graphics_context_set_stroke_color(ctx, COLOR_SLEETY_DAY);
                            break;
                        case '_':
                        case '?':
                        default:
                            continue; // don't draw clear segments!
                    }
                }
                int width = (forecast_precip_intensity_str[((24-hour) + i) % 24] - '0')+4;
                graphics_context_set_stroke_width(ctx, width);
                GRect r2 = grect_crop(fcst_bounds, temp + width/2);
                graphics_draw_arc(ctx, r2, GOvalScaleModeFitCircle, p1, p2);
            }
        }
        return;
    }

    #endif
}
