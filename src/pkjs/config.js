module.exports = [
    {
        "type": "heading",
        "defaultValue": "Planets Configuration"
    },
    {
        "type": "text",
        "defaultValue": "Digital or analog watchface that shows the visible planets, and a 24 hour weather forecast."
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
                "defaultValue": true,
                "label": "Use Celsius"
            },
            {
                "type": "input",
                "messageKey": "CFG_DARKSKY_KEY",
                "defaultValue": "f7409454bf25dc25fd6e245771b1e484",
                "label": "Dark Sky API Key (please use your own, if you can)",
            }
        ]
    }, 
    {
        "type": "submit",
        "defaultValue": "Save Settings"
    }
];