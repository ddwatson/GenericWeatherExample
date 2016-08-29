# GenericWeatherExample

Welcome to my demo pebble watchface that demonstrates the pebble-clay and pebble-generic-weather libraries 
as well as time display and persistent variable storage.  There were no other demos out there combining these 
features so after I figured it out, I figured I would provide on for you.

To use:
1) Get a Cloudpebble Account
2) get an OpenWeatherMap api key openweathermap.org/appid
3)Set your app settings to a watchface
4)Set your app settings to 
  Check JavaScript
  Uses Location
  Configurable
4) Add these two message keys for facilitate JavaScript talking back to C...they are case sensitive
  OWMAPIKEY
  READY
5)Add the following dependencies
  pebble-clay
  pebble-generic-weather
6)Create the 3 sources files, save, and compile
  
