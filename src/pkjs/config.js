module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Here is some introductory text."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "My SETTINGS"
      },
      {
        "type": "input",
        "messageKey": "OWMAPIKEY",
        "defaultValue": "",
        "label": "OpenWeatherMap API Key"
      },
      {
        "type": "input",
        "messageKey": "USERLATITUDE",
        "defaultValue": "",
        "label": "User defined latitude"
      },
      {
        "type": "input",
        "messageKey": "USERLONGITUDE",
        "defaultValue": "",
        "label": "User defined longitude"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];