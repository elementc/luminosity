#include <pebble.h>
#pragma once

#if defined(PBL_COLOR)

    #define COLOR_WINDOW GColorBlack
    #define COLOR_TIME GColorWhite
    #define COLOR_DATE GColorWhite
    #define COLOR_TEMP GColorWhite
    #define COLOR_STEPS GColorWhite
    #define COLOR_BATT GColorWhite

    #define COLOR_24H_LINES GColorBlack
    #define COLOR_24H_CURRENT GColorRed

    #define COLOR_NIGHT GColorIndigo
    #define COLOR_NIGHT_PARTLY GColorLavenderIndigo
    #define COLOR_NIGHT_CLOUDY GColorDarkGray
    #define COLOR_DAY GColorIcterine
    #define COLOR_DAY_PARTLY GColorPastelYellow
    #define COLOR_DAY_CLOUDY GColorLightGray

    #define COLOR_RAINY_DAY GColorBlue
    #define COLOR_SNOWY_DAY GColorWhite
    #define COLOR_SLEETY_DAY GColorCeleste
    #define COLOR_RAINY_NIGHT GColorBlue
    #define COLOR_SNOWY_NIGHT GColorWhite
    #define COLOR_SLEETY_NIGHT GColorCeleste

    #define COLOR_WIND_DAY GColorGreen
    #define COLOR_WIND_NIGHT GColorIslamicGreen

#else

    #define COLOR_WINDOW GColorBlack
    #define COLOR_TIME GColorWhite
    #define COLOR_DATE GColorWhite
    #define COLOR_TEMP GColorWhite
    #define COLOR_STEPS GColorWhite
    #define COLOR_BATT GColorWhite

    #define COLOR_24H_LINES GColorBlack
    #define COLOR_24H_CURRENT GColorWhite

    #define COLOR_NIGHT GColorLightGray
    #define COLOR_NIGHT_PARTLY GColorLightGray
    #define COLOR_NIGHT_CLOUDY GColorLightGray
    #define COLOR_DAY GColorWhite
    #define COLOR_DAY_PARTLY GColorWhite
    #define COLOR_DAY_CLOUDY GColorWhite

    #define COLOR_RAINY_DAY GColorLightGray
    #define COLOR_SNOWY_DAY GColorLightGray
    #define COLOR_SLEETY_DAY GColorLightGray
    #define COLOR_RAINY_NIGHT GColorWhite
    #define COLOR_SNOWY_NIGHT GColorWhite
    #define COLOR_SLEETY_NIGHT GColorWhite

    #define COLOR_WIND_DAY GColorLightGray
    #define COLOR_WIND_NIGHT GColorWhite

#endif
