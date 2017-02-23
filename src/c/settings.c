#include "src/c/luminosity.h"

// Initialize the default settings
void prv_default_settings() {
    settings.Analog = false;
    settings.Metric = false;
    settings.Knots = false;
    settings.sunrise = 7;
    settings.sunset = 19;
}

// Read settings from persistent storage
void prv_load_settings() {
    prv_default_settings();
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
    s_sunrise = settings.sunrise;
    s_sunset = settings.sunset;
}

// Save the settings to persistent storage
void prv_save_settings() {
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
    prv_update_display();
}
