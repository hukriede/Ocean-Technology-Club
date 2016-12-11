#include "Wire.h"
#include "Adafruit_MCP9808.h"

Adafruit_MCP9808 temperatureSensor = Adafruit_MCP9808();

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(10, OUTPUT);
  ledOff();

  while (!temperatureSensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
  }
}

void loop() {
  /* Probes for temperature measurement when prompted

  The BelowWater Arduino continuously waits for serial input from the AboveWater
  Arduino. If it sees the character 't' it looks for an integer between the found 't'
  and the following 'z' ('t5z'). It takes the average of that number of readings
  and sends it back to the AboveWater Arduino.

  If the numberOfReadings value is less than 1 it's assigned to 1 to prevent
  Nan results in the case of config failure.
  */

  if ((char)Serial.read() == 't') {
    int numberOfReadings = (Serial.readStringUntil('z')).toInt();
    double temperatureReading = averageValues(numberOfReadings <= 0 ? 1 : numberOfReadings );
    reportMeasuredTemperature(temperatureReading);
  }
}

double averageValues(int numberOfReadings){
  /* Gathers an averaged temperature reading

  For a predetermined number of readings(numberOfReadings) we collect a
  measurement from the MCP9809 sensor using the readTemperature function and add
  it to the current sum.

  If the temperature value comes back as 0.00 we assume the reading was faulty,
  so we decrement the counter to make sure we collect the appropriate number of
  readings.

  The average value is returned as the collected sum divided by the number of
  readings.
  */

  double sum = 0;
  for(int readingIndex = 0; readingIndex < numberOfReadings; readingIndex++){
    ledOn();
    double reading = readTemperature();
    sum = (sum + reading);
    readingIndex = reading == 0.00 ? readingIndex-- : readingIndex;
    ledOff();
  }
  double average = sum / numberOfReadings;
  delay(15);
  return average;
}

double readTemperature() {
  temperatureSensor.shutdown_wake(0);
  double temperature = temperatureSensor.readTempC();
  temperatureSensor.shutdown_wake(1);
  Serial.print(c * 9.0 / 5.0 + 32);
  Serial.print("*F\n");
  return temperature;
}

void reportMeasuredTemperature(double temperature){
  Serial.print(temperature);
  Serial.print((char)00);
}

void ledOn() {
  digitalWrite(10, HIGH);
}

void ledOff() {
  digitalWrite(10, LOW);
}
