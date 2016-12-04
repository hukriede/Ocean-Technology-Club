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

/*
  loop()

  The BelowWater Arduino continuously waits for serial input from the AboveWater
  Arduino. If it sees the character 't' it looks for an integer between that 't'
  and the following 'z' ('t5z'). It takes the average of that number of readings
  and sends it back to the AboveWater Arduino.

  If the numberOfReadings value is less than 1 it's assigned to 1 to prevent
  Nan results.
*/
void loop() {
  if ((char)Serial.read() == 't') {
    int numberOfReadings = (Serial.readStringUntil('z')).toInt();
    double temperatureReading = averageValues(numberOfReadings <= 0 ? 1 : numberOfReadings );
    reportMeasuredTemperature(temperatureReading);
  }
}

/*
  averageValues()

  For a predetermined number of readings(int numberOfReadings) we collect a
  measurement from the MCP9809 sensor using the readTemperature function and add
  it to the current sum.

  The average value is returned as the collected sum divided by the number of
  readings.
*/
double averageValues(int numberOfReadings){
  ledOff();
  double sum = 0;
  for(int i = 0; i < numberOfReadings; i++){
    double reading = readTemperature();
    sum = (sum + reading);
    i = reading == 0.00 ? i-- : i;
  }
  double average = sum / numberOfReadings;
  delay(15);
  ledOn();
  return average;
}

double readTemperature() {
  temperatureSensor.shutdown_wake(0);
  delay(100);
  double temperature = temperatureSensor.readTempC();
  delay(175);
  temperatureSensor.shutdown_wake(1);
  return temperature;
}

void reportMeasuredTemperature(double temperature){
    Serial.print(temperature);
    Serial.print((char)00);
    ledOff();
}

void ledOn() {
    digitalWrite(10, HIGH);
}

void ledOff() {
    digitalWrite(10, LOW);
}
