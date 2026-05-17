var space = require('./space');

var openmeteo_wmo_to_icon_day = {
    0: "clear-day", // clear
    1: "clear-day", // mainly clear
    2: "partly-cloudy-day", // partly cloudy
    3: "cloudy", // overcast
    45: "fog", // fog
    48: "fog", // "depositing rime fog" whatever that means
    51: "rain", // drizzle, slight
    53: "rain", // drizzle: moderate 
    55: "rain", // drizzle: heavy
    56: "sleet", // light freezing drizzle
    57: "sleet", // heavy freezing drizzle
    61: "rain", // rain: slight
    63: "rain", // rain: moderate
    65: "rain", // rain: heavy
    66: "sleet", // freezing rain: light
    67: "sleet", // freezing rain: heavy
    71: "snow", // snow: slight TODO: icon needs rework
    73: "snow", // snow: moderate
    75: "snow", // snow: heavy
    77: "snow", // "snow grains" I had to google it
    80: "rain", // rain showers: slight
    81: "rain", // rain showers: moderate
    82: "rain", // rain showers: violent :D
    85: "snow", // snow showers: slight
    86: "snow", // snow showers: heavy
    95: "rain", // thunderstorm slight or moderate TODO: icon?
    96: "rain", // thunderstorm with slight hail TODO: icon?
    99: "rain" // thunderstorm with heavy hail TODO: icon?

};

var openmeteo_wmo_to_icon_night = {
    0: "clear-night", // clear
    1: "clear-night", // mainly clear
    2: "partly-cloudy-night", // partly cloudy
    3: "cloudy", // overcast
    45: "fog", // fog
    48: "fog", // "depositing rime fog" whatever that means
    51: "rain", // drizzle, slight
    53: "rain", // drizzle: moderate 
    55: "rain", // drizzle: heavy
    56: "sleet", // light freezing drizzle
    57: "sleet", // heavy freezing drizzle
    61: "rain", // rain: slight
    63: "rain", // rain: moderate
    65: "rain", // rain: heavy
    66: "sleet", // freezing rain: light
    67: "sleet", // freezing rain: heavy
    71: "snow", // snow: slight
    73: "snow", // snow: moderate
    75: "snow", // snow: heavy
    77: "snow", // "snow grains" I had to google it
    80: "rain", // rain showers: slight
    81: "rain", // rain showers: moderate
    82: "rain", // rain showers: violent :D
    85: "snow", // snow showers: slight
    86: "snow", // snow showers: heavy
    95: "rain", // thunderstorm slight or moderate TODO: icon?
    96: "rain", // thunderstorm with slight hail TODO: icon?
    99: "rain" // thunderstorm with heavy hail TODO: icon?

};

var openmeteo_wmo_to_precip_type = {
    51: "r", // drizzle, slight
    53: "r", // drizzle: moderate 
    55: "r", // drizzle: heavy
    56: "l", // light freezing drizzle
    57: "l", // heavy freezing drizzle
    61: "r", // rain: slight
    63: "r", // rain: moderate
    65: "r", // rain: heavy
    66: "l", // freezing rain: light
    67: "l", // freezing rain: heavy
    71: "s", // snow: slight
    73: "s", // snow: moderate
    75: "s", // snow: heavy
    77: "s", // "snow grains"
    80: "r", // rain showers: slight
    81: "r", // rain showers: moderate
    82: "r", // rain showers: violent :D
    85: "s", // snow showers: slight
    86: "s", // snow showers: heavy
    95: "r", // thunderstorm slight or moderate
    96: "r", // thunderstorm with slight hail
    99: "r"  // thunderstorm with heavy hail 

};

function get_icon_for_weather(json) {
    var hour = new Date().getHours();

    var wmo_code = json.current.weather_code;

    if (hour >= space.sunset || hour < space.sunrise) {
        return openmeteo_wmo_to_icon_night[wmo_code] || "clear-night"; // TODO: need a "unknown code" icon
    } else {
        return openmeteo_wmo_to_icon_day[wmo_code] || "clear-day";
    }

}

function get_precip_type_for_wmo_code(wmo_code) {
    return openmeteo_wmo_to_precip_type[wmo_code] || "_";

}

function get_current_temperature(json) {
    return Math.round(json.current.temperature_2m);
}

function get_current_wind_speed(json) {
    return Math.round(json.current.wind_speed_10m);
}

function get_current_wind_bearing(json) {
    return Math.round(json.current.wind_direction_10m);
}

function get_high_temperature(json, start_offset) {
    var high_temperature = -999999.0;
    for (var i = start_offset; i < start_offset + 24; i++) {
        if (json.hourly.temperature_2m[i] > high_temperature) {
            high_temperature = json.hourly.temperature_2m[i];
        }
    }

    return Math.round(high_temperature);
}

function get_low_temperature(json, start_offset) {
    var low_temperature = 999999.0;

    for (var i = start_offset; i < start_offset + 24; i++) {
        if (json.hourly.temperature_2m[i] < low_temperature) {
            low_temperature = json.hourly.temperature_2m[i];
        }
    }
    return Math.round(low_temperature);
}

function build_temperature_forecast_str(json, start_offset) {
    var high_temperature = get_high_temperature(json, start_offset);
    var low_temperature = get_low_temperature(json, start_offset);

    var delta = high_temperature - low_temperature;

    var temp_str = "";

    for (var i = start_offset; i < start_offset + 24; i++) {
        var scale = json.hourly.temperature_2m[i] - low_temperature;
        var intensity = Math.round(scale / delta * 9);

        intensity = Math.min(9, intensity);
        intensity = Math.max(0, intensity);

        temp_str += ("" + intensity);
    }
    console.log("calculated temp_str: " + temp_str);
    return temp_str;
}

function build_cloud_cover_forecast_str(json, start_offset) {
    var cloud_str = "";

    for (var i = start_offset; i < start_offset + 24; i++) {
        var coverage = json.hourly.cloud_cover[i];
        if (coverage < 40) {
            cloud_str += '0';
        } else if (coverage < 75) {
            cloud_str += '1';
        } else {
            cloud_str += '2';
        }
    }
    console.log("calculated cloud_str: " + cloud_str);
    return cloud_str;
}

function get_high_wind_speed(json, start_offset) {
    var high_speed = -999999.9;
    for (var i = start_offset; i < start_offset + 24; i++) {
        if (json.hourly.wind_speed_10m[i] > high_speed) {
            high_speed = json.hourly.wind_speed_10m[i];
        }
    }

    return Math.round(high_speed);
}

function get_low_wind_speed(json, start_offset) {
    var low_speed = 999999.9;
    for (var i = start_offset; i < start_offset + 24; i++) {
        if (json.hourly.wind_speed_10m[i] < low_speed) {
            low_speed = json.hourly.wind_speed_10m[i];
        }
    }

    return Math.round(low_speed);
}

function build_wind_intensity_forecast_string(json, start_offset) {
    var high_speed = get_high_wind_speed(json, start_offset);
    var low_speed = get_low_wind_speed(json, start_offset);

    var delta = high_speed - low_speed;

    var wind_str = "";

    for (var i = start_offset; i < start_offset + 24; i++) {
        var scale = json.hourly.wind_speed_10m[i] - low_speed;
        var intensity = Math.round(scale / delta * 9);

        intensity = Math.min(9, intensity);
        intensity = Math.max(0, intensity);

        wind_str += ("" + intensity);
    }

    console.log("calculated wind_str: ", wind_str);
    return wind_str;
}

function build_precip_type_forecast_string(json, start_offset) {
    var precip_type_str = "";

    for (var i = start_offset; i < start_offset + 24; i++) {
        var wmo_code = json.hourly.weather_code[i];
        precip_type_str += get_precip_type_for_wmo_code(wmo_code);
    }

    console.log("calculated precip_type_str: ", precip_type_str);
    return precip_type_str;
}

function build_precip_intensity_forecast_string(json, start_offset) {
    var precip_intensity_str = "";

    for (var i = start_offset; i < start_offset + 24; i++) {
        var probability = json.hourly.precipitation_probability[i];
        if (probability != 0) {
            var intensity = json.hourly.precipitation[i];
            var score = Math.min(Math.round(3 * probability / 100) + Math.round(2 * intensity), 9);
            precip_intensity_str += ("" + score);
        } else {
            precip_intensity_str += "0";
        }
    }

    console.log("calculated precip_intensity_str: " + precip_intensity_str);

    return precip_intensity_str;

}

function parseWeatherData(json) {
    var start_offset = new Date().getHours(); // Start counting from the current hour (TODO god i hope this is 24h instead of 12h)

    console.log("Assembling data for offset " + start_offset + " - "  + json.hourly.time[start_offset] + " through " + json.hourly.time[start_offset + 24] + "...");
    console.log("Sunrise: " + space.sunrise + " Sunset: " + space.sunset);

    // Assemble dictionary using our keys
    var dictionary = {
        'TEMPERATURE': get_current_temperature(json),
        'CONDITIONS': get_icon_for_weather(json),
        'WIND_SPEED': get_current_wind_speed(json),
        'WIND_BEARING': get_current_wind_bearing(json),
        'FORECAST_CLOUDS': build_cloud_cover_forecast_str(json, start_offset),
        'FORECAST_PRECIP_TYPE': build_precip_type_forecast_string(json, start_offset),
        'FORECAST_PRECIP_INTENSITY': build_precip_intensity_forecast_string(json, start_offset),
        'FORECAST_WIND_INTENSITY': build_wind_intensity_forecast_string(json, start_offset),
        'FORECAST_TEMP': build_temperature_forecast_str(json, start_offset),
        'FORECAST_HIGH': get_high_temperature(json, start_offset),
        'FORECAST_LOW': get_low_temperature(json, start_offset),
        'WEATHER_FETCHED_HOUR': start_offset
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
        function (e) {
            console.log('Weather info sent to Pebble successfully!');
        },
        function (e) {
            console.log('Error sending weather info to Pebble!');
        }
    );
}

function handleWeatherData() {
    // responseText contains a JSON object with weather info
    var json = JSON.parse(this.responseText);
    parseWeatherData(json);

}

function handleWeatherTimeout() {
    console.log("Weather request timed out.");
}

function handleWeatherError() {
    console.log("Weather request errored out.");
}

function getWeather(lat, lon, units) {

    // TODO: need to figure out units
    var current_channels = '&current=weather_code,temperature_2m,wind_speed_10m,wind_direction_10m,precipitation';
    var hourly_channels = '&hourly=temperature_2m,cloud_cover,precipitation,precipitation_probability,wind_speed_10m,weather_code';
    var units = "&wind_speed_unit=mph&temperature_unit=fahrenheit"
    var settings = '&timezone=auto&forecast_days=3';
    var base = 'https://api.open-meteo.com/v1/forecast?';
    var location = 'latitude=' + lat + '&longitude=' + lon;
    var url = base + location + hourly_channels + current_channels + units + settings;
    console.log("Loading weather data from: " + url);
    var xhr = new XMLHttpRequest();
    xhr.onload = handleWeatherData;
    xhr.timeout = 1000;
    xhr.ontimeout = handleWeatherTimeout;
    xhr.onerror = handleWeatherError;
    xhr.open('GET', url);
    xhr.send();

    // While that's cookin', update the sunrise/sunset times.
    var date = new Date();
    space.calculate(lat, lon, date);

}

function testWeather() {
    var testdata = {
        "latitude": 47.67519,
        "longitude": -122.1931,
        "generationtime_ms": 3006.959915161133,
        "utc_offset_seconds": -25200,
        "timezone": "America/Los_Angeles",
        "timezone_abbreviation": "GMT-7",
        "elevation": 125.0,
        "current_units": {
            "time": "iso8601",
            "interval": "seconds",
            "weather_code": "wmo code",
            "temperature_2m": "°F",
            "wind_speed_10m": "mp/h",
            "wind_direction_10m": "°",
            "precipitation": "mm"
        },
        "current": {
            "time": "2026-04-17T07:30",
            "interval": 900,
            "weather_code": 0,
            "temperature_2m": 41.1,
            "wind_speed_10m": 2.2,
            "wind_direction_10m": 37,
            "precipitation": 0.00
        },
        "hourly_units": {
            "time": "iso8601", "temperature_2m": "°F", "cloud_cover": "%", "precipitation": "mm", "precipitation_probability": "%", "wind_speed_10m": "mp/h", "weather_code": "wmo code"
        },
        "hourly": {
            "time": [
                "2026-04-17T00:00", "2026-04-17T01:00", "2026-04-17T02:00",
                "2026-04-17T03:00", "2026-04-17T04:00", "2026-04-17T05:00",
                "2026-04-17T06:00", "2026-04-17T07:00", "2026-04-17T08:00",
                "2026-04-17T09:00", "2026-04-17T10:00", "2026-04-17T11:00",
                "2026-04-17T12:00", "2026-04-17T13:00", "2026-04-17T14:00",
                "2026-04-17T15:00", "2026-04-17T16:00", "2026-04-17T17:00",
                "2026-04-17T18:00", "2026-04-17T19:00", "2026-04-17T20:00",
                "2026-04-17T21:00", "2026-04-17T22:00", "2026-04-17T23:00",
                "2026-04-18T00:00", "2026-04-18T01:00", "2026-04-18T02:00",
                "2026-04-18T03:00", "2026-04-18T04:00", "2026-04-18T05:00",
                "2026-04-18T06:00", "2026-04-18T07:00", "2026-04-18T08:00",
                "2026-04-18T09:00", "2026-04-18T10:00", "2026-04-18T11:00",
                "2026-04-18T12:00", "2026-04-18T13:00", "2026-04-18T14:00",
                "2026-04-18T15:00", "2026-04-18T16:00", "2026-04-18T17:00",
                "2026-04-18T18:00", "2026-04-18T19:00", "2026-04-18T20:00",
                "2026-04-18T21:00", "2026-04-18T22:00", "2026-04-18T23:00",
                "2026-04-19T00:00", "2026-04-19T01:00", "2026-04-19T02:00",
                "2026-04-19T03:00", "2026-04-19T04:00", "2026-04-19T05:00",
                "2026-04-19T06:00", "2026-04-19T07:00", "2026-04-19T08:00",
                "2026-04-19T09:00", "2026-04-19T10:00", "2026-04-19T11:00",
                "2026-04-19T12:00", "2026-04-19T13:00", "2026-04-19T14:00",
                "2026-04-19T15:00", "2026-04-19T16:00", "2026-04-19T17:00",
                "2026-04-19T18:00", "2026-04-19T19:00", "2026-04-19T20:00",
                "2026-04-19T21:00", "2026-04-19T22:00", "2026-04-19T23:00"],
            "temperature_2m": [43.1, 42.4, 42.6, 42.2, 42.2, 40.7, 40.3, 40.0, 42.7, 45.4, 46.7, 48.2, 49.6, 50.9, 52.0, 52.7, 52.9, 52.7, 52.0, 50.5, 47.9, 46.6, 45.7, 45.1, 45.0, 45.0, 44.4, 43.6, 42.6, 41.9, 41.2, 42.4, 46.1, 51.7, 54.7, 57.4, 59.0, 62.0, 63.7, 64.0, 64.3, 63.6, 62.3, 60.3, 57.5, 55.6, 54.4, 53.6, 52.9, 52.1, 51.5, 50.0, 49.1, 48.0, 46.8, 47.7, 50.1, 52.8, 56.7, 60.5, 63.7, 65.9, 66.8, 66.3, 63.7, 63.0, 62.7, 60.7, 56.5, 54.9, 54.2, 53.7],
            "cloud_cover":
            // synthetic no-clouds
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ],
            // "actual" data (seattle so all clouds):
            //[4, 100, 100, 100, 0, 93, 53, 10, 34, 31, 80, 32, 21, 96, 26, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 90, 100, 95, 100, 100, 100, 100, 100, 97, 100, 100, 89, 80, 91, 87, 100, 100, 100, 100, 100, 100, 100, 97, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99, 83, 65, 98, 100],
            "precipitation": [0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00],
            "precipitation_probability": [2, 5, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 2, 3, 4, 4, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5],
            "wind_speed_10m": [5.0, 4.6, 6.1, 3.2, 2.2, 2.6, 3.7, 1.1, 1.9, 2.0, 3.0, 3.4, 3.3, 3.6, 4.3, 4.7, 5.1, 5.0, 5.4, 4.1, 3.6, 2.9, 2.8, 2.8, 2.1, 2.0, 3.0, 1.1, 1.7, 3.4, 3.6, 2.5, 2.7, 1.2, 2.4, 2.9, 2.2, 2.1, 2.9, 3.8, 4.1, 4.3, 3.6, 2.7, 1.7, 0.2, 2.9, 2.4, 3.2, 3.5, 2.2, 1.6, 1.8, 2.9, 3.3, 2.6, 4.3, 4.6, 5.6, 5.6, 6.1, 6.5, 7.1, 7.0, 6.5, 6.4, 4.2, 3.7, 2.8, 3.0, 2.4, 3.1],
            "weather_code": [0, 3, 3, 3, 0, 3, 2, 0, 1, 1, 3, 1, 1, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3]
        }
    };
    parseWeatherData(testdata);
}


module.exports.getWeather = getWeather;
module.exports.testWeather = testWeather;