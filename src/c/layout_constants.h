#include <pebble.h>
#pragma once
#define WIDTH_24H_LINES 1
#define WIDTH_24H_LINES_CURRENT 3
#define TRI_W 8

//layout info
#ifdef PBL_RECT
    #define NO_WEATHER_SKY_LINE_WIDTH 4
    #define DATE_LAYER_ANALOG GRect(15, h-55, 60, 40)
    #define DATE_LAYER_DIGITAL GRect(0, h/2+15, w, 30)
    #define DATE_ALIGN_ANALOG GTextAlignmentLeft
    #define DATE_ALIGN_DIGITAL GTextAlignmentCenter
    #define TEMP_RECT GRect(w - 50 - 23, 15, 50, 20)
    #define FORECAST_HIGHLOW_RECT GRect(w - 88, 32, 65, 33)
    #define TIME_RECT GRect(0, h/2-30, w, 50)
    #define STEP_RECT GRect(20, h-38, w - 40, 20)
    #define ANALOG_RECT GRect(0, 0, w, h)
    #define CONDITION_RECT GRect(18,18, 26,26)
    #define BAT_RECT  GRect(23, h-38, 40, 20)
#else
    #define NO_WEATHER_SKY_LINE_WIDTH 8
    #define DATE_LAYER_ANALOG GRect(10, h/2-20, 60, 40)
    #define DATE_LAYER_DIGITAL GRect(0, h/2+15, w, 30)
    #define DATE_ALIGN_ANALOG GTextAlignmentCenter
    #define DATE_ALIGN_DIGITAL GTextAlignmentCenter
    #define TEMP_RECT GRect(w - 105, 25, 50, 20)
    #define FORECAST_HIGHLOW_RECT GRect(w - 107, 42, 69, 33)
    #define TIME_RECT GRect(0, h/2-30, w, 50)
    #define STEP_RECT GRect(w/2, h-55, 40, 20)
    #define ANALOG_RECT GRect(w/2-targetAnalogWidth/2, h/2-targetAnalogWidth/2, targetAnalogWidth, targetAnalogWidth)
    #define CONDITION_RECT GRect(43, 28, 35, 35)
    #define BAT_RECT  GRect(45, h-55, 40, 20)
#endif

#define BT_RECT  GRect(w/2-10, h-38, 20, 20)
