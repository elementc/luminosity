// if (Pebble){
//     console.log("Pebble defined, using.");
// } else {
//     console.log("Pebble not defined, loading shim.");
//     var Pebble = require('./pebble_shim');
//     var XMLHttpRequest = require('xhr2');
// }

function get_current_temperature(json) {
    return Math.round(json.current.temperature_2m);
}

function get_current_wind_speed(json){
    return Math.round(json.current.wind_speed_10m);
}

function get_current_wind_bearing(json){
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

        intensity = (intensity > 10 ? 10 : intensity);
        intensity = (intensity < 0 ? 0 : intensity);

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

function handleWeatherData() {
    // responseText contains a JSON object with weather info
    var json = JSON.parse(this.responseText);

    var date = new Date();

    var start_offset = date.getHours(); // Start counting from the current hour (god i hope this is base 24)

    // Assemble dictionary using our keys
    var dictionary = {
        'TEMPERATURE': get_current_temperature(json),
        'CONDITIONS': "clear-day", // TODO
        'WIND_SPEED': get_current_wind_speed(json),
        'WIND_BEARING': get_current_wind_bearing(json),
        'FORECAST_CLOUDS': build_cloud_cover_forecast_str(json, start_offset),
        'FORECAST_PRECIP_TYPE': "_________________________", // TODO
        'FORECAST_PRECIP_INTENSITY': "0000000000000000000000000", // TODO
        'FORECAST_WIND_INTENSITY': "0000000000000000000000000", // TODO
        'FORECAST_TEMP': build_temperature_forecast_str(json, start_offset),
        'FORECAST_HIGH': get_high_temperature(json, start_offset),
        'FORECAST_LOW': get_low_temperature(json, start_offset)
        // 'FORECAST_WIND_HIGH': 20, // these dont have a render yet, can strike
        // 'FORECAST_WIND_LOW': 0
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
    var hourly_channels = '&hourly=temperature_2m,cloud_cover,precipitation,wind_direction_10m,wind_speed_10m,weather_code';
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

}


module.exports.getWeather = getWeather;