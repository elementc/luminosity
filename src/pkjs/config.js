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
                "defaultValue": "Time"
            },
            {
                "type": "toggle",
                "messageKey": "CFG_ANALOG",
                "defaultValue": false,
                "label": "Analog Face"
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
                "messageKey": "CFG_CELSIUS",
                "defaultValue": false,
                "label": "Use Celsius"
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
                "label": "Lattitude"
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