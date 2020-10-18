# Dimmable light with rotary encoder control

This device use a ESP8266 (Wemos D1 Mini) in pair with a rotary encoder and a MOSFET to control a light via PWM. In my case, the light is a warm white led strip, but any device controllable via PWM could work (unless it draws too much amps).

## BOM

- Wemos D1 Mini
- Mosfet - IRLB8721
- Voltage regulator - [Mini DC-DC Converter Adjustable Power Supply Step Down Module](https://www.banggood.com/5Pcs-Mini-DC-DC-Converter-Adjustable-Power-Supply-Step-Down-Module-p-951165.html?rmmds=myorder&cur_warehouse=CN)
- Rotary encoder - 5pins, 11mm x 15mm - [like this one](https://www.banggood.com/10Pcs-15mm-Rotary-Encoder-Switch-with-Key-Switch-with-2-Bit-Gray-Scale-Micro-Switch-p-1593839.html?rmmds=search&cur_warehouse=CN)
- Resistor - 100ohm

## Schematics

![Schematics](https://github.com/plapointe6/CustomHADevicesCollection/blob/master/dimmable_light_with_rotary_encoder/media/schematics.png)

## Software

The software uses the EspMQTTClient, HAMQTTDevice and ESP_EEPROM libraries. You can find the arduino sketch in the "dimmable_light_rotary_encoder" folder.

## PCB and 3D printed case

For a more finished product, milling your own PCB and enclosing the electronics into a case would be great.

You will find the .stl files in the "3D printed case" folder and the PCB for milling in the "PCB" folder.

DISCLAIMER: I was pretty new to electronics when a created this device, use at your own risk.

![Case](https://github.com/plapointe6/CustomHADevicesCollection/blob/master/dimmable_light_with_rotary_encoder/media/printed_case.png)