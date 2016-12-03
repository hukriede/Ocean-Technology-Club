#include "Wire.h"
#include "Adafruit_MCP9808.h"

Adafruit_MCP9808 temperatureSensor = Adafruit_MCP9808();

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);

  if (!temperatureSensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
}

/*
  loop()

  The BelowWater Arduino continuously waits for serial input from the AboveWater
  Arduino. If it sees the character 't' it looks for an integer between that 't'
  and the following 'z' ('t5z'). It takes the average of that number of readings
  and sends it back to the AboveWater Arduino.
*/
void loop() {
  char indicator = (char)Serial.read();
  if (indicator == 't') {
    int numberOfReadings = (Serial.readStringUntil('z')).toInt();
    digitalWrite(10, LOW);
    double temperatureReading = averageValues(numberOfReadings);
    delay(15);
    digitalWrite(10, HIGH);
    Serial.print(temperatureReading);
    Serial.print((char)00);
    digitalWrite(10, LOW);
  }
}

/*
  readTemperature()

  This function wakes up the temperature sensor, waits for it to settle down, and
  then takes and returns a measured temperature value in degrees Celcius.
*/
double readTemperature() {
  temperatureSensor.shutdown_wake(0);
  delay(100);
  double temperature = temperatureSensor.readTempC();
  delay(175);
  temperatureSensor.shutdown_wake(1);
  return temperature;
}

/*
  averageValues()

  For a predetermined number of readings( int numberOfReadings) we collect a
  reading from the MCP9809 sensor using the readTemperature function and add
  it to the current sum.

  The average value is returned as the collected sum divided by the number of
  readings that were not 0.
*/
double averageValues(int numberOfReadings){
  int count = 0;
  double sum = 0;
  for(int i = 0; i < numberOfReadings; i++){
    double reading = readTemperature();
    if (reading == 0.00){
      if (numberOfReadings > (count + 1)){
        count++;
      }
    }
    sum = (sum + reading);
  }
  numberOfReadings -= count;
  double average = sum / numberOfReadings;
  return average;
}
