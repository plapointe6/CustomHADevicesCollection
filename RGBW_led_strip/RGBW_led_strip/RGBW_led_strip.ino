#include "EspMQTTClient.h"
#include "HAMqttDevice.h"
#include <ESP_EEPROM.h>

EspMQTTClient client(
  "mySSID",         // Wifi ssid
  "myPassword",     // Wifi password
  "192.168.1.101",  // MQTT broker ip
  "myUsername",     // MQTT username
  "myMQTTPassword", // MQTT password
  "my_rgbw_strip"   // MQTT Client name
);

HAMqttDevice rgbwStrip("My RGBW Strip", HAMqttDevice::LIGHT);

// Minimum 5 seconds between each eeprom updates
const int eeprom_min_millis_after_change = 5 * 1000;

const int send_states_delay = 5 * 1000;

long valueChangedMillis = 0;

// Pins assignments
const int Rpin = D4;
const int Gpin = D3;
const int Bpin = D2;
const int Wpin = D1;

struct RgbwValues {
  bool lightOn;
  byte Rval;
  byte Gval;
  byte Bval;
  byte Wval;
  byte brightness;
};
RgbwValues rgbwValues = { .lightOn = false, .Rval = 0, .Gval = 0, .Bval = 0, .Wval = 0, .brightness = 100 };


void setup() 
{
  pinMode(Rpin, OUTPUT);
  pinMode(Gpin, OUTPUT);
  pinMode(Bpin, OUTPUT);
  pinMode(Wpin, OUTPUT);
  
  analogWriteRange(255);

  // Charge saved values from eeprom
  EEPROM.begin(sizeof(rgbwValues));
  if(EEPROM.percentUsed() >= 0)
    EEPROM.get(0, rgbwValues);

  // Configure extra config vars for Home assistant
  rgbwStrip
    .enableAttributesTopic()
    .addConfigVar("bri_stat_t", "~/br/state")
    .addConfigVar("bri_cmd_t", "~/br/cmd")
    .addConfigVar("rgb_stat_t", "~/rgb/state")
    .addConfigVar("rgb_cmd_t", "~/rgb/cmd")
    .addConfigVar("whit_val_stat_t", "~/w/state")
    .addConfigVar("whit_val_cmd_t", "~/w/cmd")
    .addConfigVar("bri_scl", "100");

  client.enableHTTPWebUpdater();
  client.setMaxPacketSize(512);
}

void onConnectionEstablished()
{
  // ON/OFF command received
  client.subscribe(rgbwStrip.getCommandTopic(), [] (const String &payload)
  {
    if (payload.equals("ON"))
      rgbwValues.lightOn = true;
    else if (payload.equals("OFF"))
      rgbwValues.lightOn = false;

    client.publish(rgbwStrip.getStateTopic(), payload);
    valueChangedMillis = millis();
  });

  // RGB brightness command received
  client.subscribe(rgbwStrip.getTopic() + "/rgb/cmd", [] (const String &payload)
  {
    int firstDelimiterPos = payload.indexOf(',');
    int secondDelimiterPos = payload.indexOf(',', firstDelimiterPos+1);

    if(firstDelimiterPos != -1 && secondDelimiterPos != -1)
    {
      rgbwValues.Rval = payload.substring(0, firstDelimiterPos).toInt();
      rgbwValues.Gval = payload.substring(firstDelimiterPos+1, secondDelimiterPos).toInt();
      rgbwValues.Bval = payload.substring(secondDelimiterPos+1, payload.length()).toInt();
      client.publish(rgbwStrip.getTopic() + "/rgb/state", payload);
      valueChangedMillis = millis();
    }
  });

  // White brightness command received
  client.subscribe(rgbwStrip.getTopic() + "/w/cmd", [] (const String &payload)
  {
    rgbwValues.Wval = payload.toInt();
    client.publish(rgbwStrip.getTopic() + "/w/state", payload);
    valueChangedMillis = millis();
  });
  
  // Brightness command received
  client.subscribe(rgbwStrip.getTopic() + "/br/cmd", [] (const String &payload)
  {
    rgbwValues.brightness = payload.toInt();
    client.publish(rgbwStrip.getTopic() + "/br/state", payload);
    valueChangedMillis = millis();
  });
  
  // Home Assitant connection status events.
  client.subscribe("ha/status", [] (const String &payload)
  {
    if (payload.equals("online"))
  	{
  		client.publish(rgbwStrip.getConfigTopic(), rgbwStrip.getConfigPayload());

      client.executeDelayed(send_states_delay, []() {
        client.publish(rgbwStrip.getStateTopic(), rgbwValues.lightOn ? "ON" : "OFF");
        client.publish(rgbwStrip.getTopic() + "/rgb/state", String(rgbwValues.Rval) + "," + String(rgbwValues.Gval) + "," + String(rgbwValues.Bval));
        client.publish(rgbwStrip.getTopic() + "/w/state", String(rgbwValues.Wval));
        client.publish(rgbwStrip.getTopic() + "/br/state", String(rgbwValues.brightness));

        rgbwStrip
          .clearAttributes()
          .addAttribute("IP", WiFi.localIP().toString());
          
        client.publish(rgbwStrip.getAttributesTopic(), rgbwStrip.getAttributesPayload());
      });
  	}
  });
}

void loop() 
{
  client.loop();

  if(rgbwValues.lightOn)
  {
    if(rgbwValues.Rval == 255 && rgbwValues.Gval == 255 && rgbwValues.Bval == 255 && rgbwValues.Wval > 0)
    {
      analogWrite(Rpin, 0);
      analogWrite(Gpin, 0);
      analogWrite(Bpin, 0);
      analogWrite(Wpin, rgbwValues.Wval);
    }
    else
    {
      analogWrite(Rpin, (int)((float)rgbwValues.Rval * ((float)rgbwValues.brightness / 100.0)));
      analogWrite(Gpin, (int)((float)rgbwValues.Gval * ((float)rgbwValues.brightness / 100.0)));
      analogWrite(Bpin, (int)((float)rgbwValues.Bval * ((float)rgbwValues.brightness / 100.0)));
      analogWrite(Wpin, (int)((float)rgbwValues.Wval * ((float)rgbwValues.brightness / 100.0)));
    }
  }
  else
  {
    analogWrite(Rpin, 0);
    analogWrite(Gpin, 0);
    analogWrite(Bpin, 0);
    analogWrite(Wpin, 0);
  }

  long currentMillis = millis();

  if(valueChangedMillis != 0 && 
      currentMillis - valueChangedMillis > eeprom_min_millis_after_change)
  {
    EEPROM.put(0, rgbwValues);
    EEPROM.commit();
    valueChangedMillis = 0;
  }
}
