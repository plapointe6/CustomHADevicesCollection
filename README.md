# Custom Home Assistant devices collection

This repo contain custom IOT devices developped to work with ESP8266/ESP32, MQTT and Home Assistant.


# Required arduino libraries

Most of the devices needs these arduino libraries to work : 
 - https://github.com/plapointe6/EspMQTTClient
 - https://github.com/plapointe6/HAMqttDevice
 - https://github.com/jwrw/ESP_EEPROM
 
 
# Required Home Assistant parameters

Home Assistant must be communicating with an MQTT broker.
Here is the suggested configuration in `configuration.yaml` : 

```yaml
mqtt:
  broker: broker ip here
  username: username here
  password: password here
  discovery: true
  discovery_prefix: ha
  birth_message:
    topic: 'ha/status'
    payload: 'online'
    retain: true
  will_message:
    topic: 'ha/status'
    payload: 'offline'
    retain: true
```


# Architecture

These devices take advantages of MQTT discovery in Home Assistant. No manual steps are needed to add these devices in Home Assistant. Also, with the provided code, if something goes wrong (Wifi goes down, MQTT goes down, etc.) everything will reconnect automatically whitout your intervention.

Nothing is retained with MQTT (except for the HA status) and no device use the optimistic mode.

## Message types

There is three messages types when communicating with Home Assitant via MQTT.
- Configuration: these messages register your device into HA with MQTT discovery. all details/config of the device are sent with this message. It is the first thing you have to send to HA.
- State: this contain the current state of the device. For a light, the state will be either "ON" or "OFF".
- Command: this contains a resquest to change the state of the device. Wen receiving that, we do the necessary to update the device (ex: close/open the light) and send back the confirmation to Home Assistant when it's done by sending the new state.
- Attributes: Custom device attributes that can be sent to Home Assitant. These attributes will be displayed in the device card.
- Home assistant status: this is a retained message and contains the state of HA, either "online" or "offline".

## Devices events chronology

1. setup(): initilisations, load the previous state from EEPROM, configure the MQTT device.
2. onConnectionEstablished(): this callback is called each time everything get up and running (MQTT connection and WiFi connection). Here we subscribe to commands topics and Home assistant status.
3. We receive a Home Assistant status message indicating that HA is online. Then, we register the device to HA by sending the configuration message. After that, we needs to wait a little to let HA register and activate the device (wait five seconds)
4. When the waiting time as ended, we send the current device state to HA. This will update the device state in HA to match what was saved into EEPROM. Also, the attributes of the device are initialised with the IP address of the device and are sent to Home assistant.

I recommend taking a look to the onoff_light code to see these concepts in action.

## Web updater

Each device host a web updater that can be accessed with your browser by typing the ip address or the name of the device. The default username/password will be the same as your MQTT username/password.


# Add yours !

If you successfully develop a new MQTT device based on these exemples (ESPMQTTClient/HAMqttDevice), create a pull request, everyones will benefit :)


