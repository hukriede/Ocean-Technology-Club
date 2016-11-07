
/* @author Halle Fields
   @version 1.0.0 07/18/2016
   @info Code to be assigned to the Arduino Pro Mini which is contained within the under-water housing. This code allows for the under-water temperature to be retrieved and sent to the Serial port.
*/

/*
   Initialization of variables and libraries to include in the code
*/
//I2C library
#include "Wire.h"
//Temp Sensor (MCP9808) library
#include "Adafruit_MCP9808.h"

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

/* This is our set-up code
   Put code here to be run once
*/
void setup() {
  //initialize the serial port to a baud rate of 9600 bits/second
  Serial.begin(9600);
  //setting pin 13, the on-board LED, to be an output
  pinMode(13, OUTPUT);
  //setting pin 10, the indicator LED on the exterior of the sensor package, as an output
  pinMode(10, OUTPUT);
  //initially setting the indicator light to HIGH, which turns it on
  digitalWrite(10, LOW);

  // this loop lets you know if the temperature sensor isn't working
  // the screen will read "Couldn't fine MCP9808!" until the temp sensor begins to work
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
}


/* This is our loop code
   Code in this method usually runs repeatedly for a designated period of time.
   In this case, the device is constantly looking for the Serial port to receive the indicator char
   This char tells the device to read the temperature, then send it back to the Serial port
   This way, the Arduino above water can receive this data and process it
*/
void loop() {
  // receives the indicator char from the serial port
  // if no char has been received, then the indicator is a null value and will not be used
  char indicator = (char)Serial.read();
  // if the indicator char received is the letter 't' (for temperature), the code will run to receive the current water temperature
  // the letter 't' is sent on purpose by the arduino above water, when the device wants the temperature
  if (indicator == 't') {
    // receive the number of readings to take and average
    int numReadings = (Serial.readStringUntil('z')).toInt();
    // setting the indicator light to LOW, which turns it off
    digitalWrite(10, LOW);
    // declares a decimal variable, t, to be equal to the return value of the getTemp() method below
    double t = averageValues(numReadings);
    //delays the code by 15 milliseconds to allow for some processing time
    delay(15);
    // setting the indicator light to HIGH, which turns it on
    // the code turns the light on and off to indicate that the code is running. While inside the housing, you can't tell that your application is running until you see faulty data...
    // or you can put in a little indicator lamp, like we did, and you can tell when code is running!
    digitalWrite(10, HIGH);
    // print temperature to Serial
    Serial.print(t);
    // prints a NUL character to the Serial port, which indicates the end of text. This allows the device we're attached to to be able to parse the data each time it's sent.
    Serial.print((char)00);
    // setting the indicator light to LOW, which turns it off
    digitalWrite(10, LOW);
  }
}

// here we just initialize a temperature variable to be used within the getTemp() method
double temp;

/* This is a method, getTemp(), which allows for the calculation of temperature using the MCP9808 Library
   The method takes no parameters and returns a double value, the temperature read by the MCP9808 sensor
   It is important to use methods like these to compartmentalize your code
   If we did not have this method, we would have to paste all of that code into the loop body
   While that would work, it would be sloppy and make troubleshooting very difficult
   It's good practice to break your code down into simplified parts like this
*/
double getTemp() {
  // This line wakes-up the sensor
  // Don't remove this line! required before reading temp
  tempsensor.shutdown_wake(0);
  // Read and print out the temperature
  delay(100);
  temp = tempsensor.readTempC();
  delay(175);
  // This line shuts down the sensor, which reserves power
  // Don't remove this line!
  tempsensor.shutdown_wake(1);
  // Returns the temperature, so when this method is called in the loop, it can be assigned as a double variable and the temperature value will be produced
  return temp;
}


/* This is a method, averageValues(), which allows for the calculation of the averages of temperatures according to a number of readings specified
   The method takes an integer parameter and returns a double value, the average temperature read
*/
int count = 0;
double averageValues(int numReadings){
  // declare a variable to hold our average value
  double average = 0;
  //a loop to take each reading and sum them
  for(int i = 0; i < numReadings; i++){
    // take the temperature reading and set it to a variable
    double cTemp = getTemp();
    // if our reading is equal to zero
    if (cTemp == 0.00){
      // and the number of readings we're taking is greater than the counter + 1
      // this is because we never want the counter to be greater than 1 less than numReadings
      if (numReadings > (count + 1)){
        // increment the counter to hold the number of times 
        count++;
      }
    }
    // sum the temperatures
    average = (average + cTemp);
  }
  // subtract counter from numReadings
  numReadings -= count;
  // divide the sum by the number of readings to find the mean
  average /= numReadings;
  // return that value
  return average;
}

