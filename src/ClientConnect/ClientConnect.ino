
/* @author Halle Fields
   @version 1.0.0 07/11/2016
   @info Receives data from the Serial port and inserts the data into a JSON String. This string is then sent to a WiFi client. 
*/

/*
   Initialization of variables and libraries to include in the code
*/

// ESP8266 functionality library
#include <ESP8266WiFi.h>
// ESP8266 WiFi connectivity library
#include <ESP8266WiFiMulti.h>
// Library to easily generate JSON Strings
#include <ArduinoJson.h>
// SoftwareSerial library
#include <SoftwareSerial.h>

//Create a new ESP8266 WiFiMulti object
ESP8266WiFiMulti WiFiMulti;

//Create a new SoftwareSerial object to send indicator characters to the arduino
SoftwareSerial mySerial(12, 14);

//Create int variable to record the required time between readings
double delayTime = 0;

//Create a new client object
WiFiClient client;
//set the port number and the host address as universal variables
const uint16_t port = 4001;
const char * host = "clouddev.mote.org"; // ip or dns

//initialize each exterior method, other than  setup() and loop()
// method, that receives a String array as a parameter, to get the data from the arduino and save it to an accessible String array
void getData(String*);
// method, that receives no parameters, to take the data and turn it into a JSON String and return that String
String getJSON();
// method, that receives no parameters, to take the delay time from the arduino's config file and put it in milliseconds
void setDelayTime();
// method that receives the JSON String as a parameter and sends that String to the client object
void sendToClient(String);
// method, that receives no parameters, to connect to the client and return a boolean of whether the client was successfully connected
boolean connectToHost();


/* This is our set-up code
   Put code here to be run once
*/

void setup() {
  //initialize the serial port to a baud rate of 115200 bits/second
  Serial.begin(115200);
  //initialize the SoftwareSerial port to a baud rate of 115200 bits/second
  mySerial.begin(115200);
  //delay 10 milliseconds to allow the ports to initialize
  delay(10);

  // start by connecting to a WiFi network, the first parameter is the Wifi name, the second is the password
  WiFiMulti.addAP("Verizon-791L-00CC", "78d8b2d1");

  //print to Serial that you are waiting for the WiFi to connect
  Serial.print("Wait for WiFi.");
  
  // while the WiFi is connecting, print dots to indicate that you are still waiting for the connection to occur
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    //wait half a second in-between checking for a connection
    delay(500);
  }
  //Once the wifi has connected, print that it's connected
  Serial.println("WiFi connected");

  // wait half a second before moving to the loop
  delay(500);
}


/* This is our loop code
   Code in this method will be run repeatedly for a designated period of time.
   Default run-time is infinite (or until the serial port is closed or the device is disconnected).
*/

void loop() {
  //here, we attempt to connect to the host site
  //when we connect, the connectToHost() method will return true and we will enter the if-statement
  if (connectToHost()) {
    //print to the client that we are sending the JSON data
    sendToClient("Sending JSON data to host");
    //send the JSON to the client
    sendToClient(getJSON());
    //tell the client when the next cycle will occur (in seconds)
    sendToClient("seconds until next cycle...");
    sendToClient((String)(delayTime / 1000));
    //tell the client that the connection is closing
    sendToClient("Closing connection");
    //stop the connection with the client
    client.stop();
    //wait for the indicated amount of time
    delay(delayTime);
  // exit the if-statement  
  } 
}


/* This is an exterior method, getData(), which requests data from the arduino and processes it into a String[]
   The method takes a String array as a parameter and manipulates it, but does not return a value, this is because the C language does not allow for arrays to be a return value
   It is important to use methods like these to compartmentalize your code
   If we did not have this method, we would have to paste all of that code into the loop body
   While that would work, it would be sloppy and make troubleshooting very difficult
   It's good practice to break your code down into simplified parts like this
*/

void getData(String* data) {
  // declares a String variable to be used to receive the data from the Serial port
  String s;
  // prints 'd', for data, to the arduino to indicate that the ESP8266 is ready to receive and send data
  mySerial.print('d');
  // a loop to check that data is received, there are 3 opportunities for the data to be received, before a timeout occurs
  for (int i = 0; i < 3; i++) {
    // checks repeatedly for data, if it is received, the loop breaks and the code moves forward, if not, the loop continues until the timeout sequence
    for (int i = 0; i < 200; i++) {
      // read the String from the serial port and set it equal to s, declared previously
      s = Serial.readStringUntil('z');
      // check if s contains the String data; if so, break the loop
      if (s.length() > 0) {
        break;
      }
      // if not, continue sending the indicator character and checking to see if data is received
      else {
      }
    }
    // if s contains information, break the loop
    if (s.length() > 0) {
      break;
    }
    // if not, try again
    else {
      // print to the serial, indicating that data was not received the first time through
      Serial.println("Didn't receive data, trying again");
      mySerial.print('d');
    }
  }
  // once again, check if s is not null. if it is not, you know that there is an error happening. 
  if (s.length() < 1) {
    // print that there was an error to the client, so that the error will be seen by those checking the log
    sendToClient("Error: did not receive data. Check arduino.");
  }

  // initialize the length of a char array to the length of s, plus one
  char holder[s.length() + 1];
  // set s to a char array
  s.toCharArray(holder, s.length() + 1);

  // create counter and previous counter value variables
  int counter = 0;
  int prev = 0;

  // traverse through each element in the char array
  for (int i = 0; i <= s.length() + 1 ; i++) {
    // the values sent to the ESP8266 are in the form of comma-seperated values (CSV)
    // search the array for a comma, to know when to seperate the strings and add them to the String array
    if (holder[i] == ',') {
      // set the current index of the data array to the string value
      data[counter] = s.substring(prev, i);
      // increment the prev value to the value of i + 1, which is the next letter after the comma
      prev = i + 1;
      // increase the counter to know the next value in the data array
      counter++;
    }
  }
}


/* This is an exterior method, getJSON(), which calls the getData() method to manipulate a String array, and then uses that array to form a JSON string of the data
   The method does not take any parameters and returns a String, which is the JSON String
*/

String getJSON () {
  // initialize a String array variable, with a length of however many objects are being sent from the arduino. Must change this value if more variables are being added!
  String data[10];
  // call the getData method to add the string values to the data array
  getData(data);
  // reserve memory space for the JSON
  StaticJsonBuffer<375> jsonBuffer;

  // build object tree in memory
  // there are 4 individual JSONs being used
  // root is the "root" of the JSON tree, it contains the 3 other JSONs
  JsonObject& root = jsonBuffer.createObject();
  // systems contains all the system data from the arduino, and systems is nested inside of root
  JsonObject& systems = jsonBuffer.createObject();
  // sensors contains all the sensor data from the arduino, and sensors is nested inside of root
  JsonObject& sensors = jsonBuffer.createObject();
  // gps contains the lat and long coordinates from the arduino, and gps is nested inside of systems, which is inside root
  JsonObject& gps = jsonBuffer.createObject();

  // adds information to each sub-section of the JSON file
  // add model number to systems
  systems["ModelNumber"] = data[0].toInt();
  // add serial number to systems
  systems["SerialNumber"] = data[1].toInt();
  // add location to systems
  systems["Location"] = data[2];
  // add latitude to gps
  gps["Lat"] = data[3].toFloat();
  // add longitude to gps
  gps["Lon"] = data[4].toFloat();
  // add code version to systems
  systems["CodeVersion"] = data[6];
  // add date of sample to systems
  systems["Date"] = data[7];
  // add time of sample to systems 
  systems["Time"] = data[8];

  // add temperature of sample to sensors
  sensors["Water_Temperature"] = data[9].toFloat();

  // set the delay time to the number of minutes indicated in the config file 
  delayTime = data[5].toFloat();
  setDelayTime();
  
  // nest systems inside root
  JsonArray& System = root.createNestedArray("System");
  System.add(systems);
  // nest gps inside systems
  JsonArray& GPS = systems.createNestedArray("GPS");
  GPS.add(gps);
  // nest sensors inside root
  JsonArray& Sensor = root.createNestedArray("Sensors");
  Sensor.add(sensors);

  //Generate the JSON string
  // declare a string variable to hold the json
  String json;
  // create a char array to print the json to
  char buff[350];
  // print root to the char array
  root.printTo(buff, sizeof(buff));
  // set the char array equal to String json
  json = buff;

  // add a name to the beginning of the JSON string
  json = "JSON_OTClub: " + json;
  json.replace("[","");
  json.replace("]","");

  // return the string
  return json;
}


/* This is an exterior method, setDelayTime(), which takes the existing delayTime, in minutes, and converts it into milliseconds (the unit of time used by delay statements)
   The method takes no parameters and returns no values, it simply manipulates an existing variable
*/

void setDelayTime() {
  // set the delay time equal to however many minutes indicated, but converted into milliseconds
  delayTime = delayTime * 60 * 1000;
}


/* This is an exterior method, sendToClient(), which sends String data to the client
   The method takes a String as a parameter and sends that value, but does not return anything
*/

void sendToClient(String data) {
  // print the string to the serial port for troubleshooting
  Serial.println(data);
  // print the string to the client
  client.print(data);  
  //read back one line from server
  String line = client.readStringUntil('\r');
  // print that line to the client, to handle the transfer
  client.println(line);
  // delay 10 milliseconds before the loop closes the connection
  delay(10);
}


/* This is an exterior method, connectToHost(), which connects our ESP8266 to the host to send JSON data to
   The method takes takes no parameters and returns a boolean value for whether or not the client successfully connected to the host
*/

boolean connectToHost() {
  // Check if a client has connected
  while (!client.connect(host, port)) {
    //if not, wait 5 seconds and then check again
    delay(5000);
    Serial.print(".");
  }
  // once you exit the loop, return true
  return true;
}

