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

### Schematic of the Circuit
![Schematic](Assets/Schematic.png)

### Simple Diagram Explanation
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
```

## Installation

1. ```pip install pyserial```
2. Open the Arduino IDE.
3. Open `temperature_sensor/temperature_sensor.ino`.
4. Select your Arduino board from **Tools > Board**.
5. Select the correct port from **Tools > Port**.
6. Click **Upload**.

## Usage

1. After uploading, open the Serial Monitor (**Tools > Serial Monitor**).
2. Set the baud rate to `9600`.
3. Temperature readings will appear every second in both Celsius and Fahrenheit.

### Usage After Python Implementation:
- Arduino sends measurements (e.g., temperature, vibration, etc.).
- Python reads the data via the serial port.
- Python prints the data nicely.
- Python stores the data in CSV or JSON format.
- (Optionally) Python inserts the data into a real database later.

### Example Output:
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
- **Output**: 10mV per degree Celsius
- **Range**: -55°C to 120°C

The Arduino reads the analog voltage and converts it to temperature using the formula:
```
Temperature (°C) = Voltage (V) × 100
```

## Personal notes

### Data capacity
- If this protoype is to be used in a real life implementation for the Novonesis company, we'd need to ensure that Novonesis has the capacity enough on the servers or similar for the temperature data to be stored. Because there would be large amounts of datasets overtime during the freezer transportation which would need to be stored efficiently to not overload their data capacity in the company. 
   - Either cloud solution or their own servers? We'd need to ask Novonesis about their current data storage solution. If this issue persists, we could also further in our implementation ensure that the temperature sensor only measure every 10 minutes instead of each second to reduce dataset.


## License

This project is open source and was created as part of our group project.