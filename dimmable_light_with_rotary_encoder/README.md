# Dimmable light with rotary encoder control

This device use a ESP8266 (Wemos D1 Mini) in pair with a rotary encoder and a MOSFET to control a light via PWM. In my case, the light is a warm white led strip, but any device controllable via PWM could work (unless it draws too much amps).

## BOM

More details coming soon :
- Wemos D1 Mini
- Mofset
- Voltage regulator
- Rotary encoder

## Schematics

Coming soon

## Software

The software uses the EspMQTTClient, HAMQTTDevice ans ESP_EEPROM libraries. You can finbd the arduino sketch in the "dimmable_light_rotary_encoder" folder.

## PCB and 3D printed case

For a more finished product, milling your own PCB and enclosing the electronics into a case would be great.

You will find the .stl files in the "3D printed case" folder anf the PCB for milling in the "PCB" folder.

PCB Coming soon.

![Case](https://github.com/plapointe6/CustomHADevicesCollection/blob/master/dimmable_light_rotary_encoder/media/printed_case.png)