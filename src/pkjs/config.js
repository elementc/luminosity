module.exports = [
    {
        "type": "heading",
        "defaultValue": "Luminosity Configuration"
    },
    {
        "type": "text",
        "defaultValue": "Digital or analog watchface that shows a 24 hour weather forecast."
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Display"
            },
            {
                "type": "toggle",
                "messageKey": "CFG_ANALOG",
                "defaultValue": false,
                "label": "Use analog face instead of digital"
            },
            {
                "type": "toggle",
                "messageKey": "CFG_INVERT_COLORS",
                "defaultValue": false,
                "label": "Use lighter color palette"
            }
        ]
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Weather"
            },
            {
                "type": "toggle",
                "messageKey": "CFG_ENABLE_WIND_RING",
                "defaultValue": true,
                "label": "Flick your wrist to swap between the precipitation ring and a ring showing wind forecast."
            },
            {
                "type": "toggle",
                "messageKey": "CFG_CELSIUS",
                "defaultValue": false,
                "label": "Use metric units (temp in celsius, wind in m/s instead of temp in farenheit, wind in mph)."
            },
            {
              "type": "toggle",
              "messageKey": "CFG_KNOTS",
              "defaultValue": false,
              "label": "Use knots instead of the units dictated by your above units settings for wind speed."
            },
            {
                "type": "text",
                "defaultValue": "Weather data is <a href='https://darksky.net/poweredby/'>Powered by Dark Sky.</a> It costs the developer about a dollar per day to access this weather data. Please consider <a href='https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=AU8HEZGRU3P2L'>donating a dollar or two</a> if you like this face."
            }
        ]
    },
    {
        "type":"section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Location"
            },
            {
                "type": "toggle",
                "messageKey": "CFG_OVERRIDE_LOC",
                "defaultValue": false,
                "label": "Override detected Location"
            },
            {
                "type": "input",
                "messageKey": "CFG_LAT",
                "label": "Latitude"
            },
            {
                "type": "input",
                "messageKey": "CFG_LON",
                "label": "Longitude"
            }
        ]
    },
    {
        "type": "submit",
        "defaultValue": "Save Settings"
    }
];
