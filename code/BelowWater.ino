#include "Adafruit_MCP9808.h"

Adafruit_MCP9808 temperatureSensor = Adafruit_MCP9808();

void setup() {
  Serial.begin(9600);
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
  while (!Serial.available()) {
    delay(1000);
  }

  if ((char)Serial.read() == 't') {
    int numberOfReadings = (Serial.readStringUntil('z')).toInt();
    bool measured = false;
    double temperatureReading;
    while (!measured) {
      temperatureReading = averageValues(numberOfReadings <= 0 ? 1 : numberOfReadings );
      measured = (temperatureReading != 0.00) ? true : false;
    }
    reportMeasuredTemperature(temperatureReading);
  }
}

double averageValues(int numberOfReadings) {
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
  temperatureSensor.shutdown_wake(0);
  delay(250);
  double sum = 0;

  for (int readingIndex = 0; readingIndex < numberOfReadings; readingIndex++) {
    double reading = temperatureSensor.readTempC();
    sum = (sum + reading);
    readingIndex = reading == 0.00 ? readingIndex-- : readingIndex;
  }

  double average = sum / numberOfReadings;
  temperatureSensor.shutdown_wake(1);
  return average;
}

void reportMeasuredTemperature(double temperature) {
  // Sends the averaged temperature to the AboveWater Arduino

  Serial.print(temperature);
  Serial.print((char)00);
}
