# MQTT-SINGLE-LIGHT

This is a sketch that control a single light, via a relay connected to pin D2 on NodeMCU and show the current temperature and humidty via a DHT22 connected to pin D3, with all messages and commands being sent via especifics MQTT topics created for each functions, that paired with https://github.com/hobbyquaker/homekit2mqtt allows the light to be controlled via Apple HomeKit as well as the current temperature and humidity be shown in HomeKit devices
