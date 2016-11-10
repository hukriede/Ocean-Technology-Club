#include "Wire.h"
#include "Adafruit_MCP9808.h"

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);

  if (!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
}

void loop() {
  char indicator = (char)Serial.read();
  if (indicator == 't') {
    int numReadings = (Serial.readStringUntil('z')).toInt();
    digitalWrite(10, LOW);
    double t = averageValues(numReadings);
    delay(15);
    digitalWrite(10, HIGH);
    Serial.print(t);
    Serial.print((char)00);
    digitalWrite(10, LOW);
  }
}

double temp;
double getTemp() {
  tempsensor.shutdown_wake(0);
  delay(100);
  temp = tempsensor.readTempC();
  delay(175);
  tempsensor.shutdown_wake(1);
  return temp;
}
int count = 0;
double averageValues(int numReadings){
  double average = 0;
  for(int i = 0; i < numReadings; i++){
    double cTemp = getTemp();
    if (cTemp == 0.00){
      if (numReadings > (count + 1)){
        count++;
      }
    }
    average = (average + cTemp);
  }
  numReadings -= count;
  average /= numReadings;
  return average;
}
