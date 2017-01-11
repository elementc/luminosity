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
                "type": "input",
                "messageKey": "CFG_DARKSKY_KEY",
                "defaultValue": "09a66d34ccc4feb9a2b56e3c6cfee527",
                "label": "Dark Sky API Key (please use your own, if you can)",
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