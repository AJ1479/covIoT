# covIoT: Integrated Patient Monitoring And Sanitization System

An automatic hand sanitizer dispenser, integrated with an oximeter, a heart rate monitor, a non-contact body temperature sensor, and voice assistant feedback.

## Technologies

The project is developed and deployed on an Arduino UNO with the following sensors and devices

- Arduino UNO
- 9V Pump
- 9V Battery
- MAX30102 (Heart Rate Monitor & Pulse Oximeter)
> Note: We include the MAX30105 header due to compatibility issues.
- Arduino MicroSD Card Module
- 8GB MicroSD Card
- PAM8403 Type-D Amplifier
- Speakers (around 3W / 4 ohm)
- 128x32 OLED
- MLX90614 (Infrared Non-Contact Thermometer)
- 5V Relay
- Ultrasonic Distance Sensor
- Breadboard and Jumper Wires



## Deployment

- Flash the code to the Arduino UNO via an appropriate COM port.
- Format the MicroSD card to FAT32 using an appropriate card reader. Then, extract and copy [the files](https://drive.google.com/file/d/1hFaS7tp6d4xF5ni8bmdZ2aJVOVaNTmBx/view) to it.

> Note: Due to memory constraints, the code was split between two Arduino UNOs. An Arduino MEGA, having higher memory capacity, might be a more suitable option.

## Installation

The Arduino IDE needs the following packages installed either manually or through the GUI package manager

- [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [MAX3010x](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library)
- [SD](https://www.arduino.cc/reference/en/libraries/sd/)
- [TMRpcm](https://github.com/TMRh20/TMRpcm)
- [Adafruit_MLX90614](https://github.com/adafruit/Adafruit-MLX90614-Library)
- [SSD1306Ascii](https://github.com/greiman/SSD1306Ascii)

## Circuit Diagram

<br/>

![Circuit Diagram-1](https://user-images.githubusercontent.com/67030839/216771739-80824350-5279-431d-8755-64f6d53568ba.JPG)

<br/>

![Circuit Diagram-2](https://user-images.githubusercontent.com/67030839/216771765-3963b5f9-5871-4327-9e46-ab411eee5de6.png)



## Authors

<table>
  <tr>
    <td align="center"><a href="https://github.com/AJ1479"><img src="https://avatars2.githubusercontent.com/u/67030839?v=4" width="100px;" alt=""/><br /><sub><b>AJ1479</b></sub></a><br /><a href="https://github.com/csivitu/CodeGolf-Backend/commits?author=AJ1479" title="Code">💻</a> <a href="https://github.com/csivitu/CodeGolf-Backend/commits?author=AJ1479" title="Documentation">📖</a></td>
    <td align="center"><a href="https://github.com/theProgrammerDavid"><img src="https://avatars.githubusercontent.com/u/35698009?v=4" width="100px;" alt=""/><br /><sub><b>David</b></sub></a><br /><a href="https://github.com/csivitu/CodeGolf-Backend/commits?author=ashikka" title="Code">💻</a> <a href="https://github.com/csivitu/CodeGolf-Backend/commits?author=ashikka" title="Documentation">📖</a></td>
  </tr>
</table>
