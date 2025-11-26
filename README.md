# Real-time Temperature Sensor

A simple prototype for a real-time temperature sensor using an Arduino and R2 thermistor temperature sensor.

## Components

- Arduino Uno
- R2 thermistor - Temperature Sensor
- Wires
- Breadboard 

## Wiring Diagram

Connect the R2 thermistor sensor to the Arduino as follows:

```
R2 thermistor Pin    Arduino Pin
-----------------    -----------
VCC                  5V
GND                  A0
```

### Schematic of the circuit
![Setare](Assets/Schematic.png)


### Simple diagram explanation
```
     +--------------+
     | R2 thermistor|
     +------+-------+
            |
         +-----+
         |     | 
         VCC   GND 
         |     |  
         5V    A0   
         |     |  
         +-----+
         Arduino

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
Reading temperature from R2 thermistor sensor...
Temperature: 25.4 C / 77.7 F
Temperature: 25.5 C / 77.9 F
Temperature: 25.4 C / 77.7 F
```

## How It Works

The R2 thermistor is an analog temperature sensor that outputs a voltage proportional to temperature:
- Output: 10mV per degree Celsius
- Range: -55°C to 120°C

The Arduino reads the analog voltage and converts it to temperature using:
```
Temperature (°C) = Voltage (V) × 100
```

## License
This project is open source created in our group project