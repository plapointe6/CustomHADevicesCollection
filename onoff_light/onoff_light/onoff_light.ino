#include "EspMQTTClient.h"
#include "HAMqttDevice.h"
#include <ESP_EEPROM.h>

// MQTT client setup
EspMQTTClient client(
  "mySSID",         // Wifi ssid
  "myPassword",     // Wifi password
  "192.168.1.101",  // MQTT broker ip
  "myUsername",     // MQTT username
  "myMQTTPassword", // MQTT password
  "my_light"        // MQTT Client name
);

// Home Assitant device creation for MQTT discovery
HAMqttDevice light("My Nice Lamp", HAMqttDevice::LIGHT);

// Pin assignation
const int relayPin = D1;

// Current status of the light
bool lightOn = false;

void setup() 
{
  //Serial.begin(115200);
  
  pinMode(relayPin, OUTPUT);

  // Charge saved values from eeprom
  EEPROM.begin(sizeof(lightOn));
  if(EEPROM.percentUsed() >= 0)
    EEPROM.get(0, lightOn);
  
  // Setup the parameters of the device for MQTT discovery in HA
  light.enableAttributesTopic();

  client.enableHTTPWebUpdater();
  //client.enableDebuggingMessages();
}

void onConnectionEstablished()
{
  // Subscribe to the command topic ("cmd_t") of this device.
  client.subscribe(light.getCommandTopic(), [] (const String &payload)
  {
    // Turn the light on/off depending on the command received from Home Assitant
    if (payload.equals("ON"))
      lightOn = true;
    else if (payload.equals("OFF"))
      lightOn = false;

    // Confirm to Home Assitant that we received the command and updated the state.
    // HA will then update the state of the device in HA
    client.publish(light.getStateTopic(), payload);

    // Save the new state in memory
    EEPROM.put(0, lightOn);
    EEPROM.commit();
  });
  
  // Subscribe to Home Assitant connection status events.
  client.subscribe("ha/status", [] (const String &payload)
  {
    // When the status of Home assistant get online, publish the device config
    if (payload.equals("online"))
    {
      client.publish(light.getConfigTopic(), light.getConfigPayload());
      
      // After sending the device config to HA, wait a little to allow HA to create the entity.
      client.executeDelayed(5 * 1000, []() {

        // Send the current state of the light
        client.publish(light.getStateTopic(), lightOn ? "ON" : "OFF");
        
        // Send the IP address of the device to HA
        light
          .clearAttributes()
          .addAttribute("IP", WiFi.localIP().toString());
          
        client.publish(light.getAttributesTopic(), light.getAttributesPayload());
      });
    }
  });
}

void loop() 
{
  client.loop();

  if(lightOn)
    digitalWrite(relayPin, HIGH);
  else
    digitalWrite(relayPin, LOW);
}
