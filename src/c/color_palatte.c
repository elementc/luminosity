#include "src/c/luminosity.h"

GColor COLOR_WINDOW;
GColor COLOR_TIME;
GColor COLOR_DATE;
GColor COLOR_TEMP;
GColor COLOR_STEPS;
GColor COLOR_BATT;
GColor COLOR_24H_LINES;
GColor COLOR_24H_CURRENT;
GColor COLOR_NIGHT;
GColor COLOR_NIGHT_PARTLY;
GColor COLOR_NIGHT_CLOUDY;
GColor COLOR_DAY;
GColor COLOR_DAY_PARTLY;
GColor COLOR_DAY_CLOUDY;
GColor COLOR_RAINY_DAY;
GColor COLOR_SNOWY_DAY;
GColor COLOR_SLEETY_DAY;
GColor COLOR_RAINY_NIGHT;
GColor COLOR_SNOWY_NIGHT;
GColor COLOR_SLEETY_NIGHT;
GColor COLOR_WIND_DAY;
GColor COLOR_WIND_NIGHT;
GColor COLOR_CLEAR;


void update_colors(){
  #if defined(PBL_COLOR)
    if (settings.Invert_Colors){
    COLOR_WINDOW  = GColorWhite;
    COLOR_TIME  = GColorBlack;
    COLOR_DATE  = GColorBlack;
    COLOR_TEMP  = GColorBlack;
    COLOR_STEPS  = GColorBlack;
    COLOR_BATT  = GColorBlack;
    COLOR_24H_LINES  = GColorWhite;
    COLOR_24H_CURRENT  = GColorRed;
    COLOR_NIGHT  = GColorIndigo;
    COLOR_NIGHT_PARTLY  = GColorLavenderIndigo;
    COLOR_NIGHT_CLOUDY  = GColorDarkGray;
    COLOR_DAY  = GColorChromeYellow;
    COLOR_DAY_PARTLY  = GColorYellow;
    COLOR_DAY_CLOUDY  = GColorLightGray;
    COLOR_RAINY_DAY  = GColorBlue;
    COLOR_SNOWY_DAY  = GColorShockingPink;
    COLOR_SLEETY_DAY  = GColorCeleste;
    COLOR_RAINY_NIGHT  = GColorBlue;
    COLOR_SNOWY_NIGHT  = GColorShockingPink;
    COLOR_SLEETY_NIGHT  = GColorCeleste;
    COLOR_WIND_DAY  = GColorGreen;
    COLOR_WIND_NIGHT = GColorIslamicGreen;
    COLOR_CLEAR = GColorClear;
  } else{
    COLOR_WINDOW  = GColorBlack;
    COLOR_TIME  = GColorWhite;
    COLOR_DATE  = GColorWhite;
    COLOR_TEMP  = GColorWhite;
    COLOR_STEPS  = GColorWhite;
    COLOR_BATT  = GColorWhite;
    COLOR_24H_LINES  = GColorBlack;
    COLOR_24H_CURRENT  = GColorRed;
    COLOR_NIGHT  = GColorIndigo;
    COLOR_NIGHT_PARTLY  = GColorLavenderIndigo;
    COLOR_NIGHT_CLOUDY  = GColorDarkGray;
    COLOR_DAY  = GColorIcterine;
    COLOR_DAY_PARTLY  = GColorPastelYellow;
    COLOR_DAY_CLOUDY  = GColorLightGray;
    COLOR_RAINY_DAY  = GColorBlue;
    COLOR_SNOWY_DAY  = GColorWhite;
    COLOR_SLEETY_DAY  = GColorCeleste;
    COLOR_RAINY_NIGHT  = GColorBlue;
    COLOR_SNOWY_NIGHT  = GColorWhite;
    COLOR_SLEETY_NIGHT  = GColorCeleste;
    COLOR_WIND_DAY  = GColorGreen;
    COLOR_WIND_NIGHT = GColorIslamicGreen;
    COLOR_CLEAR = GColorClear;
  }

#else
if (settings.Invert_Colors){
  COLOR_WINDOW  = GColorWhite;
  COLOR_TIME  = GColorBlack;
  COLOR_DATE  = GColorBlack;
  COLOR_TEMP  = GColorBlack;
  COLOR_STEPS  = GColorBlack;
  COLOR_BATT  = GColorBlack;
  COLOR_24H_LINES  = GColorWhite;
  COLOR_24H_CURRENT  = GColorBlack;
  COLOR_NIGHT  = GColorBlack;
  COLOR_NIGHT_PARTLY  = GColorBlack;
  COLOR_NIGHT_CLOUDY  = GColorBlack;
  COLOR_DAY  = GColorLightGray;
  COLOR_DAY_PARTLY  = GColorLightGray;
  COLOR_DAY_CLOUDY  = GColorLightGray;
  COLOR_RAINY_DAY  = GColorBlack;
  COLOR_SNOWY_DAY  = GColorBlack;
  COLOR_SLEETY_DAY  = GColorBlack;
  COLOR_RAINY_NIGHT  = GColorDarkGray;
  COLOR_SNOWY_NIGHT  = GColorDarkGray;
  COLOR_SLEETY_NIGHT  = GColorDarkGray;
  COLOR_WIND_DAY  = GColorBlack;
  COLOR_WIND_NIGHT  = GColorLightGray;
  COLOR_CLEAR = GColorClear;
} else{
  COLOR_WINDOW  = GColorBlack;
  COLOR_TIME  = GColorWhite;
  COLOR_DATE  = GColorWhite;
  COLOR_TEMP  = GColorWhite;
  COLOR_STEPS  = GColorWhite;
  COLOR_BATT  = GColorWhite;
  COLOR_24H_LINES  = GColorBlack;
  COLOR_24H_CURRENT  = GColorWhite;
  COLOR_NIGHT  = GColorLightGray;
  COLOR_NIGHT_PARTLY  = GColorLightGray;
  COLOR_NIGHT_CLOUDY  = GColorLightGray;
  COLOR_DAY  = GColorWhite;
  COLOR_DAY_PARTLY  = GColorWhite;
  COLOR_DAY_CLOUDY  = GColorWhite;
  COLOR_RAINY_DAY  = GColorLightGray;
  COLOR_SNOWY_DAY  = GColorLightGray;
  COLOR_SLEETY_DAY  = GColorLightGray;
  COLOR_RAINY_NIGHT  = GColorWhite;
  COLOR_SNOWY_NIGHT  = GColorWhite;
  COLOR_SLEETY_NIGHT  = GColorWhite;
  COLOR_WIND_DAY  = GColorLightGray;
  COLOR_WIND_NIGHT  = GColorWhite;
  COLOR_CLEAR = GColorClear;
  }

#endif
}
