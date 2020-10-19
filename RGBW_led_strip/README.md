# RGBW led strip controller

This device use a ESP8266 (Wemos D1 Mini) in pair with 4 MOSFET to control an RGBW led strip via PWM.

## BOM

- 1x Wemos D1 Mini
- 4x Mosfet - IRLB8721
- 1x Voltage regulator - [Mini DC-DC Converter Adjustable Power Supply Step Down Module](https://www.banggood.com/5Pcs-Mini-DC-DC-Converter-Adjustable-Power-Supply-Step-Down-Module-p-951165.html?rmmds=myorder&cur_warehouse=CN)
- 4x Resistor - 100ohm
- 1x Dc power jack

## Schematics

![Schematics](https://github.com/plapointe6/CustomHADevicesCollection/blob/master/RGBW_led_strip/media/schematics.png)

## Software

The software uses the EspMQTTClient, HAMQTTDevice and ESP_EEPROM libraries. You can find the arduino sketch in the "RGBW_led_strip" folder.

## PCB and 3D printed case

For a more finished product, milling your own PCB and enclosing the electronics into a case would be great.

You will find the .stl files in the "3D printed case" folder and the PCB for milling in the "PCB" folder.

DISCLAIMER: I was pretty new to electronics when a created this device, use at your own risk.

![Case](https://github.com/plapointe6/CustomHADevicesCollection/blob/master/RGBW_led_strip/media/printed_case.png)