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
#else
    #define NO_WEATHER_SKY_LINE_WIDTH 8
    #define DATE_LAYER_ANALOG GRect(10, h/2-20, 60, 40)
    #define DATE_LAYER_DIGITAL GRect(0, h/2+15, w, 30)
    #define DATE_ALIGN_ANALOG GTextAlignmentCenter
    #define DATE_ALIGN_DIGITAL GTextAlignmentCenter
#endif
