# Real-time Temperature Sensor

A simple prototype for a real-time temperature sensor using an Arduino and LM35 temperature sensor.

## Components

- Arduino Uno
- LM35 Temperature Sensor
- Wires
- Breadboard 

## Wiring Diagram

Connect the LM35 sensor to the Arduino as follows:

```
LM35 Pin    Arduino Pin
--------    -----------
VCC         5V
GND         GND
OUT         A0
```

```
     +-----+
     | LM35|
     +--+--+
        |
  +-----+-----+
  |     |     |
 VCC   OUT   GND
  |     |     |
  5V    A0   GND
  |     |     |
  +-----+-----+
     Arduino
```

## Installation

1. Open the Arduino IDE
2. Open `temperature_sensor/temperature_sensor.ino`
3. Select your Arduino board from Tools > Board
4. Select the correct port from Tools > Port
5. Click Upload

## Usage

1. After uploading, open the Serial Monitor (Tools > Serial Monitor)
2. Set the baud rate to 9600
3. Temperature readings will appear every second in both Celsius and Fahrenheit

Example output:
```
Real-time Temperature Sensor
----------------------------
Reading temperature from LM35 sensor...
Temperature: 25.4 C / 77.7 F
Temperature: 25.5 C / 77.9 F
Temperature: 25.4 C / 77.7 F
```

## How It Works

The LM35 is an analog temperature sensor that outputs a voltage proportional to temperature:
- Output: 10mV per degree Celsius
- Range: -55°C to 150°C

The Arduino reads the analog voltage and converts it to temperature using:
```
Temperature (°C) = Voltage (V) × 100
```

## License
This project is open source in our group project
