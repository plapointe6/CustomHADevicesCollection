#include "EspMQTTClient.h"
#include "HAMqttDevice.h"
#include <ESP_EEPROM.h>
#include <Encoder.h>

EspMQTTClient client(
  "mySSID",         // Wifi ssid
  "myPassword",     // Wifi password
  "192.168.1.101",  // MQTT broker ip
  "myUsername",     // MQTT username
  "myMQTTPassword", // MQTT password
  "my_dimmable_light" // MQTT Client name
);

HAMqttDevice dimmableLight("My led strip", HAMqttDevice::LIGHT);

// Update the new brightness/state only after some delay after a manual change with the rotary encoder is finished. 
const unsigned int eeprom_min_millis_after_change = 5 * 1000;
const unsigned int brightness_min_millis_after_change = 1 * 1000;
const unsigned int send_states_delay = 5 * 1000;

unsigned long valueChangedMillis = 0;
unsigned long lastBrightnessChangeMillis = 0;

// Pins assignments
const int ledPin = D1;
const int encoderPinA = D6;
const int encoderPinB = D7;
const int buttonPin = D5;

struct LightValues {
  bool lightOn;
  int brightness;
};
LightValues lightValues = { .lightOn = false, .brightness = 100 };

unsigned int pushButtonCoolDownDue = 0;
long oldEncoderPosition  = -999;

Encoder encoder(encoderPinA, encoderPinB);

void setup() 
{
  //Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  analogWriteRange(100);  
  
  // Charge saved values from eeprom
  EEPROM.begin(sizeof(lightValues));
  if(EEPROM.percentUsed() >= 0)
    EEPROM.get(0, lightValues);

  // Configure extra config vars for Home assistant
  dimmableLight
    .enableAttributesTopic()
    .addConfigVar("bri_stat_t", "~/br/state")
    .addConfigVar("bri_cmd_t", "~/br/cmd")
    .addConfigVar("bri_scl", "100");

  client.enableHTTPWebUpdater();
  //client.enableDebuggingMessages();
}

void onConnectionEstablished()
{
  // ON/OFF command received
  client.subscribe(dimmableLight.getCommandTopic(), [] (const String &payload)
  {
    if (payload.equals("ON"))
      lightValues.lightOn = true;
    else if (payload.equals("OFF"))
      lightValues.lightOn = false;

    client.publish(dimmableLight.getStateTopic(), payload);
    valueChangedMillis = millis();
  });
  
  // Brightness command received
  client.subscribe(dimmableLight.getTopic() + "/br/cmd", [] (const String &payload)
  {
    lightValues.brightness = payload.toInt();
    client.publish(dimmableLight.getTopic() + "/br/state", payload);
    valueChangedMillis = millis();
  });

  // Home Assitant connection status events.
  client.subscribe("ha/status", [] (const String &payload)
  {
    if (payload.equals("online"))
    {
      client.publish(dimmableLight.getConfigTopic(), dimmableLight.getConfigPayload());

      client.executeDelayed(send_states_delay, []() {
        client.publish(dimmableLight.getStateTopic(), lightValues.lightOn ? "ON" : "OFF");
        client.publish(dimmableLight.getTopic() + "/br/state", String(lightValues.brightness));

        dimmableLight
          .clearAttributes()
          .addAttribute("IP", WiFi.localIP().toString());
        client.publish(dimmableLight.getAttributesTopic(), dimmableLight.getAttributesPayload());
      });
    }
  });
}

void loop() 
{
  client.loop();

  int currentMillis = millis();
  
  // Push button as been pressed to toggle the light on/off
  if (currentMillis > pushButtonCoolDownDue && digitalRead(buttonPin) == LOW) 
  {
    lightValues.lightOn = !lightValues.lightOn;
    valueChangedMillis = currentMillis;

    if(client.isConnected())
      client.publish(dimmableLight.getStateTopic(), lightValues.lightOn ? "ON" : "OFF");
      
    pushButtonCoolDownDue = currentMillis + 500;
  }

  if (lightValues.lightOn) 
  {
    // Check if the rotary encoder is moving and adjust brightness accordingly
    long newPosition = encoder.read();

    if(newPosition != oldEncoderPosition)
    {
      if (newPosition > oldEncoderPosition) 
      {
        lightValues.brightness += 1;
        if (lightValues.brightness > 100)
          lightValues.brightness = 100;
      } 
      else if (newPosition < oldEncoderPosition) 
      {
        lightValues.brightness -= 1;
        if (lightValues.brightness < 0)
          lightValues.brightness = 0;
      }

      lastBrightnessChangeMillis = currentMillis;
      valueChangedMillis = currentMillis;
    }

    oldEncoderPosition = newPosition;
    analogWrite(ledPin, lightValues.brightness);
  }
  else
    analogWrite(ledPin, 0);

  // Send the new brightness to HA when the rotary encoder stop to move
  if(client.isConnected() && lastBrightnessChangeMillis != 0 &&
      currentMillis - lastBrightnessChangeMillis >= brightness_min_millis_after_change)
  {
    client.publish(dimmableLight.getTopic() + "/br/state", String(lightValues.brightness));
    lastBrightnessChangeMillis = 0;
  }

  // Save the new brightness when the rotary encoder stop to move
  if(valueChangedMillis != 0 &&
      currentMillis - valueChangedMillis > eeprom_min_millis_after_change)
  {
    EEPROM.put(0, lightValues);
    EEPROM.commit();
    valueChangedMillis = 0;
  }
}
