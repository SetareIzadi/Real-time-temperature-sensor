/*
 * Real-time Temperature Sensor Prototype
 * 
 * This Arduino sketch reads temperature from an LM35 temperature sensor
 * and outputs the reading to the Serial Monitor in real-time.
 * 
 * Hardware:
 *   - Arduino (Uno, Nano, or compatible)
 *   - LM35 Temperature Sensor
 * 
 * Wiring:
 *   LM35 VCC  -> Arduino 5V
 *   LM35 GND  -> Arduino GND
 *   LM35 OUT  -> Arduino A0 (Analog Pin 0)
 */

// Pin connected to LM35 output
const int TEMP_SENSOR_PIN = A0;

// Interval between readings (milliseconds)
const unsigned long READ_INTERVAL = 1000;

// Arduino reference voltage (5.0V for Uno/Nano, 3.3V for some boards)
const float REFERENCE_VOLTAGE = 5.0;

// ADC resolution (10-bit = 1024 steps)
const float ADC_RESOLUTION = 1024.0;

// Variable to track last read time
unsigned long lastReadTime = 0;

void setup() {
  // Initialize serial communication at 9600 baud
  Serial.begin(9600);
  
  // Print header
  Serial.println("Real-time Temperature Sensor");
  Serial.println("----------------------------");
  Serial.println("Reading temperature from LM35 sensor...");
  Serial.println();
}

void loop() {
  // Check if it's time to take a new reading
  unsigned long currentTime = millis();
  
  if (currentTime - lastReadTime >= READ_INTERVAL) {
    lastReadTime = currentTime;
    
    // Read temperature and display
    float temperatureC = readTemperature();
    float temperatureF = celsiusToFahrenheit(temperatureC);
    
    displayTemperature(temperatureC, temperatureF);
  }
}

/*
 * Reads temperature from LM35 sensor
 * Returns temperature in Celsius
 */
float readTemperature() {
  // Read analog value (0-1023)
  int sensorValue = analogRead(TEMP_SENSOR_PIN);
  
  // Convert to voltage (using reference voltage and ADC resolution)
  float voltage = sensorValue * (REFERENCE_VOLTAGE / ADC_RESOLUTION);
  
  // LM35 outputs 10mV per degree Celsius
  // Temperature (C) = Voltage (V) * 100
  float temperatureC = voltage * 100.0;
  
  return temperatureC;
}

/*
 * Converts Celsius to Fahrenheit
 */
float celsiusToFahrenheit(float celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}

/*
 * Displays temperature readings to Serial Monitor
 */
void displayTemperature(float celsius, float fahrenheit) {
  Serial.print("Temperature: ");
  Serial.print(celsius, 1);
  Serial.print(" C / ");
  Serial.print(fahrenheit, 1);
  Serial.println(" F");
}
