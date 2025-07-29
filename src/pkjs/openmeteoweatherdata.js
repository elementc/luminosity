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

function build_precip_intensity_forecast_string(json, start_offset){
    var precip_intensity_str = "";

    for (var i = start_offset; i < start_offset + 24; i++) {
        var probability = json.hourly.precipitation_probability[i];
        if (probability != 0){
            var intensity = json.hourly.precipitation[i];
            var score = Math.min(Math.round(3 * probability / 100) + Math.round(2 * intensity), 9);
            precip_intensity_str += ("" + score);
        } else{
            precip_intensity_str += "0";
        }
    }

    console.log("calculated precip_intensity_str: " + precip_intensity_str);

    return precip_intensity_str;

}

function handleWeatherData() {
    // responseText contains a JSON object with weather info
    var json = JSON.parse(this.responseText);

    var start_offset = new Date().getHours(); // Start counting from the current hour (god i hope this is base 24)

    console.log("Assembling data for " + json.hourly.time[start_offset] + " through " + json.hourly.time[start_offset + 24] + "...");
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
        'FORECAST_LOW': get_low_temperature(json, start_offset)
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


module.exports.getWeather = getWeather;