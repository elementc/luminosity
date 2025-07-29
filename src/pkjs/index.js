var space = require('./space');
var weather = require('./openmeteoweatherdata');


// Listen for when an AppMessage is received
// this is a once-every-30 min ping for new weather data
Pebble.addEventListener('appmessage',
    function (e) {
        console.log('AppMessage received!');
        loadSettings();
        getLocation();
    }
);

function locationSuccess(pos) {
    var lat = pos.coords.latitude;
    var lon = pos.coords.longitude;

    var date = new Date();

    weather.getWeather(lat, lon);
    getSpace(lat, lon, date);
}

function locationError(err) {
    console.log('location error (' + err.code + '): ' + err.message);
}

function getSpace(lat, lon, date) {
    space.calculate(lat, lon, date);
    // Assemble dictionary using our keys
    console.log('Sunrise hour: ' + space.sunrise);
    console.log('Sunset hour: ' + space.sunset);
    var dictionary = {
        'SPACE_SUNRISE': space.sunrise,
        'SPACE_SUNSET': space.sunset,
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
        function (e) {
            console.log('Space info sent to Pebble successfully!');
        },
        function (e) {
            console.log('Error sending space info to Pebble!');
        }
    );
}

function getLocation() {
    if (settings.CFG_OVERRIDE_LOC) {
        var date = new Date();
        var lat = Number(settings.CFG_LAT);
        var lon = Number(settings.CFG_LON);
        weather.getWeather(lat, lon);
        getSpace(lat, lon, date);
        return;
    }
    // else
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        { timeout: 15000, maximumAge: 60000 }
    );
}

// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);

var defaultSettings = { 'CFG_OVERRIDE_LOC': false, 'CFG_ANALOG': false, 'CFG_CELSIUS': false, 'CFG_INVERT_COLORS': false };
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
    function (e) {
        console.log('PebbleKit JS ready!');

        // Get the initial weather
        getLocation();
    }
);
