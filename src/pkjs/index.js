var space = require('./space');


// Listen for when an AppMessage is received
// this is a once-every-15 min ping for new weather data
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

function getWeather(lat, lon) {
    
    var apiKey = settings.CFG_DARKSKY_KEY;
    var units = settings.CFG_CELSIUS ? 'si' : 'us';

    if (apiKey !== "") {

        var url = 'https://api.darksky.net/forecast/' + apiKey + '/' + lat + ',' + lon + '?units=' + units;
        console.log(url);
        var xhr = new XMLHttpRequest();
        xhr.onload = function () {
            // responseText contains a JSON object with weather info
            var json = JSON.parse(this.responseText);

            // Temperature in Kelvin requires adjustment
            var temperature = Math.round(json.currently.temperature);
            console.log('Temperature is ' + temperature);

            // Conditions
            var conditions = json.currently.icon;      
            console.log('Conditions are ' + conditions);

            var nextDay = "";
            for (var i = 0; i < 24; i++) {
                //clear-day, clear-night, rain, snow, sleet, wind, fog, cloudy, partly-cloudy-day, or partly-cloudy-night
                // c, r, s, p, _, ?
                switch(json.hourly.data[i].icon) {
                    case 'clear-day':
                    case 'clear-night':
                    case 'wind':
                        nextDay += '_';
                        break;
                    case 'rain':
                        nextDay += 'r';
                        break;                    
                    case 'snow':
                    case 'sleet':
                        nextDay += 's';
                        break;
                    case 'fog':
                    case 'cloudy':
                        nextDay += 'c';
                        break;
                    case 'partly-cloudy-day':
                    case 'partly-cloudy-night':
                        nextDay += 'p';
                        break;
                    default:
                        nextDay += '?';
                        break;
                }
            }

            console.log("Next 24 hours: " + nextDay);

            // Assemble dictionary using our keys
            var dictionary = {
                'TEMPERATURE': temperature,
                'CONDITIONS': conditions,
                'FORECAST': nextDay
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
        } ;   
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
    var dictionary = {
        'SPACE_SUN': space.sun,
        'SPACE_SUNRISE': space.sunrise,
        'SPACE_SUNSET': space.sunset,
        'SPACE_MOON': space.moon,
        'SPACE_MERCURY': space.mercury,
        'SPACE_VENUS': space.venus,
        'SPACE_MARS': space.mars,
        'SPACE_JUPITER': space.jupiter,
        'SPACE_SATURN': space.saturn,
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

var defaultSettings = {'CFG_ANALOG':true, 'CFG_CELSIUS':true, 'CFG_DARKSKY_KEY':'' };
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