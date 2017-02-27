var space = require('./space');


// Listen for when an AppMessage is received
// this is a once-every-30 min ping for new weather data
Pebble.addEventListener('appmessage',
                        function(e) {
                            console.log('AppMessage received!');
                            loadSettings();
                            getLocation();
                        }
                       );

function locationSuccess(pos) {
    var lat = pos.coords.latitude;
    var lon = pos.coords.longitude;
    //delete me debug
    //lat = 28.6082;
    //lon = -80.6041;

    var date = new Date();

    //getCity(lat, lon);
    getWeather(lat, lon);
    getSpace(lat, lon, date);
}

function getCity(lat, lon) {
    var key = '1fea09dcd5fb19e812f9';
    var url = 'http://locationiq.org/v1/reverse.php?format=json&key=' + key + '&lat=' + lat + '&lon=' + lon + '&zoom=10';
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        // responseText contains a JSON object with weather info
        var json = JSON.parse(this.responseText);

        console.log(JSON.stringify(json, null, 2));

        var cityname = json.address.city;

        // Assemble dictionary using our keys
        var dictionary = {
            'CITY': cityname,
        };

        // Send to Pebble
        Pebble.sendAppMessage(dictionary,
                              function(e) {
                                  console.log('City info sent to Pebble successfully!');
                              },
                              function(e) {
                                  console.log('Error sending city info to Pebble!');
                              }
                             );
    } ;
    xhr.open('GET', url);
    xhr.send();
}

function handleWeatherData() {
    // responseText contains a JSON object with weather info
    var json = JSON.parse(this.responseText);

    // Temperature in Kelvin requires adjustment
    var temperature = Math.round(json.currently.temperature);
    var windSpeed = Math.round(json.currently.windSpeed);
    var windBearing = -1;
    if (windSpeed !== 0)
        windBearing = Math.round(json.currently.windBearing);
    console.log('Wind speed is '+windSpeed);
    console.log('Wind bearing is '+windBearing+'˚');

    console.log('Temperature is ' + temperature);

    // Conditions
    var conditions = json.currently.icon;
    console.log('Conditions are ' + conditions);

    var cloudsNextDay = "";
    var precipTypeNextDay = "";
    var precipIntensityNextDay = "";
    var windIntensityNextDay = "";
    var temps = [];
    var windSpeeds = [];
    var minTemp = 99999;
    var maxTemp = -99999;
    var minWind = 999999;
    var maxWind = -99999;
    var tempNextDay = "";
    for (var i = 0; i < 24; i++) {

        //do cloud coverage estimates
        if (json.hourly.data[i].cloudCover < 0.4)
            cloudsNextDay += '0';
        else if (json.hourly.data[i].cloudCover < 0.75)
            cloudsNextDay += '1';
        else
            cloudsNextDay += '2';

        // do precip type and intensity
        if (json.hourly.data[i].precipIntensity > 0 ){
            var precipScore = Math.round(3 * json.hourly.data[i].precipProbability) + Math.round(2 * json.hourly.data[i].precipIntensity);
            precipIntensityNextDay+= (""+Math.min(9, precipScore));
            switch(json.hourly.data[i].precipType){
                case 'snow':
                    precipTypeNextDay += 's';
                    break;
                case 'sleet':
                    precipTypeNextDay += 'l';
                    break;
                case 'rain':
                    precipTypeNextDay += 'r';
                    break;
                default:
                    precipTypeNextDay += '_';
                    break;
            }
        } else{
            precipIntensityNextDay += ""+0;
            precipTypeNextDay += '_';
        }
        //scrape temperature data
        temps.push(json.hourly.data[i].temperature);
        if (json.hourly.data[i].temperature > maxTemp)
            maxTemp = json.hourly.data[i].temperature;
        if (json.hourly.data[i].temperature < minTemp)
            minTemp = json.hourly.data[i].temperature;
        windSpeeds.push(json.hourly.data[i].windSpeed);
        if (windSpeeds[i] > maxWind)
            maxWind = windSpeeds[i];
        if (windSpeeds[i] < minWind)
            minWind = windSpeeds[i];
    }

    for (i = 0; i < temps.length; i++){
        tempNextDay += "" + Math.round( ( (temps[i] - minTemp) / (maxTemp - minTemp) ) * 9);
        windIntensityNextDay += "" + Math.round( ( (windSpeeds[i] - minWind) / (maxWind - minWind) ) * 9 );
    }

    console.log("Clouds next 24 hours: " + cloudsNextDay);
    console.log("precipitation types next 24 hours: " + precipTypeNextDay);
    console.log("precipitation intensity next 24 hours: " + precipIntensityNextDay);
    console.log("temperature scale next 24 hours: " + tempNextDay);
    console.log("wind intesnity next 24 hours: " + windIntensityNextDay);
    console.log('high wind forecast ' + maxWind);
    console.log('low wind forecast ' + minWind);
    console.log('high temp forecast '+ maxTemp);
    console.log('low temp forecast ' + minTemp);

    // Assemble dictionary using our keys
    var dictionary = {
        'TEMPERATURE': temperature,
        'CONDITIONS': conditions,
        'WIND_SPEED': windSpeed,
        'WIND_BEARING': windBearing,
        'FORECAST_CLOUDS': cloudsNextDay,
        'FORECAST_PRECIP_TYPE': precipTypeNextDay,
        'FORECAST_PRECIP_INTENSITY': precipIntensityNextDay,
        'FORECAST_WIND_INTENSITY': windIntensityNextDay,
        'FORECAST_TEMP': tempNextDay,
        'FORECAST_HIGH': maxTemp,
        'FORECAST_LOW': minTemp,
        'FORECAST_WIND_HIGH': maxWind,
        'FORECAST_WIND_LOW': minWind
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) {
                              console.log('Weather info sent to Pebble successfully!');
                          },
                          function(e) {
                              console.log('Error sending weather info to Pebble!');
                          }
                         );
}

function getWeather(lat, lon) {

    var apiKey = "yall lost that privelege when you started scooping the weather data i paid for"; //i know this is open source and in the clear, but please get your own key if you fork this. I'm paying for this out of my own pocket.
    var units = settings.CFG_CELSIUS ? 'si' : 'us';

    if (apiKey !== "") {

        var pebble_id = Pebble.getWatchToken();
        var url = 'http://voidrunner.m45.space/pebble-weather/' + pebble_id + '/' + lat + ',' + lon + '?units=' + units;
        console.log(url);
        var xhr = new XMLHttpRequest();
        xhr.onload = handleWeatherData;
        xhr.open('GET', url);
        xhr.send();
    }
}

function locationError(err) {
    console.log('Error requesting location!');
}

function getSpace(lat, lon, date) {
    space.calculate(lat, lon, date);
    // Assemble dictionary using our keys
    console.log('Sunrise hour: '+ space.sunrise);
    console.log('Sunset hour: ' + space.sunset);
    var dictionary = {
        'SPACE_SUNRISE': space.sunrise,
        'SPACE_SUNSET': space.sunset,
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) {
                              console.log('Space info sent to Pebble successfully!');
                          },
                          function(e) {
                              console.log('Error sending space info to Pebble!');
                          }
                         );
}

function getLocation() {
    if (settings.CFG_OVERRIDE_LOC){
        var date = new Date();
        var lat = Number(settings.CFG_LAT);
        var lon = Number(settings.CFG_LON);
        //getCity(lat, lon);
        getWeather(lat, lon);
        getSpace(lat, lon, date);
        return;
    }
    // else
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 15000, maximumAge: 60000}
    );
}

// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);

var defaultSettings = {'CFG_OVERRIDE_LOC':false, 'CFG_ANALOG':false, 'CFG_CELSIUS':false, 'CFG_INVERT_COLORS': false };
var settings = {};

function loadSettings() {
    try {
        settings = JSON.parse(localStorage.getItem('clay-settings')) || defaultSettings;
        console.log("Settings: " + settings);
    } catch (e) {
        console.log("Failed to get settings: " + e);
    }
}

loadSettings();

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
                        function(e) {
                            console.log('PebbleKit JS ready!');

                            // Get the initial weather
                            getLocation();
                        }
                       );
