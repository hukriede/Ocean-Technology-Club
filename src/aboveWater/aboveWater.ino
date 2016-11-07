
/*  @author Halle Fields
    @version 1.0.0 7/11/2016
    @info Program to record sensor and system data in the form of a CSV. This data is then sent to a seperate microcontroller for handling, and to an SD card for datalogging.
    @info NOTE: data is read from a config file on an SD card, for information on how to format config information and other code information, please see the README file
    @info NOTE: before uploading this program to an arduino, set your computer's time zone to UTC (GMT) for correct clock time
*/

/*
   Initialization of variables and libraries to include in the code
*/

//Software Serial library
#include <SoftwareSerial.h>
//SPI library
#include <SPI.h>
//SD Card Library
#include <SD.h>
//Real-Time Clock Libraries
#include <RTClib.h>
#include <RTC_DS3234.h>

// Avoid spurious warnings according to RTC library (DO NOT REMOVE)
#undef PROGMEM
#define PROGMEM __attribute__(( section(".progmem.data") ))
#undef PSTR
#define PSTR(s) (__extension__(oooooo{static prog_char __c[] PROGMEM = (s); &__c[0];}))

// Create a new SoftwareSerial object for the under-water sensor
SoftwareSerial mySerial = SoftwareSerial(2, 3);

// Create an RTC instance, using the chip select pin it's connected to
RTC_DS3234 RTC(8);

//Set the SD card and RTC chip-select pins to variables
#define SDchipSelect (10)
#define RTCchipSelect (8)

// define the current code version as a constant variable
// change this when you change the code version according to standard software versioning protocol
#define codeVersion ("0.0.1,")
// initialize variables for the config file, data, and name of file to save the data to for datalogging
String configVariables;
String dataString;
String fileName;
int numReadings;
// initializes a String to hold the temperature data
String under;


/* This is our set-up code
   Put code here to be run once
*/

void setup() {
  //initialize the serial port to a baud rate of 115200 bits/second
  Serial.begin(115200);

  //set the data rate for the Software Serial ports to 9600 bits/second
  mySerial.begin(9600);

  //begin the SPI and RTC libraries
  SPI.begin();
  RTC.begin();

  // set the chip select pins of both the RTC and the SD card to be outputs
  pinMode(RTCchipSelect, OUTPUT);
  pinMode(SDchipSelect, OUTPUT);

  // see if the card is present and can be initialized:
  while (!SD.begin(SDchipSelect)) {
    // if the card is not present or can't be initialized, print an error message
    Serial.println("Card failed, or not present");
    // continue checking for the card until it is found
  }

  //check to see if the RTC is running. If not, initialize it and set the correct date/time. Don't forget that it must be in UTC/GMT time!
  if (!RTC.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  // open the config file on the SD card and initialize it to a File variable
  File myFile = SD.open("_config.txt");
  // if the file is available:
  if (myFile) {
    // while the file is available:
    while (myFile.available()) {
      // read the number of readings taken from the top of the config file
      numReadings = (myFile.readStringUntil(',')).toInt();
      // read from the file until a new line has been reached, so only the config variables are read (and not the information about the config file)
      configVariables += myFile.readStringUntil('\n');
      // once the file has been read, break the loop
      break;
    }
    // get rid of any extra whitespace around the variables
    configVariables.trim();
    // close the file
    myFile.close();
    // if the file didn't open, print an error message
  } else {
    // if the file didn't open, print an error
    Serial.println("error opening _config.txt");
  }
  // close the file
  myFile.close();
}


/* This is our loop code
   Code in this method usually runs repeatedly for a designated period of time.
   In this case, the device is constantly looking for the Serial port to receive the indicator char
   This char tells the device to gather all the required information, then send it back to the Serial port
   This way, the ESP8266 can process the data when it needs it
*/

void loop() {
  // receives the indicator char from the serial port
  // if no char has been received, then the indicator is a null value and will not be used
  char indicator = (char)Serial.read();
  // if the indicator char received is the letter 'd' (for data), the code will run to gather the data and send the CSV to the ESP and the SD file
  // the letter 'd' is sent on purpose by the ESP8266, when the device wants the data
  if (indicator == 'd') {
    // clear the Serial port
    Serial.flush();
    // prints the 't' indicator char that tells the SoftwareSerial for the underwater sensor to turn on and listen for information
    mySerial.print('t');
    // prints the number of readings we want from the temperature sensors
    mySerial.print(numReadings);
    // prints a stop character to the below-water arduino
    mySerial.print('z');
    //delay to wait for the arduino below to wake up
    delay(500);
    // a loop to check that data is received, there are 3 opportunities for the data to be received, before a timeout occurs
    for (int i = 0; i < 3; i++) {
      //  checks repeatedly for data, if it is received, the loop breaks and the code moves forward, if not, the loop continues until the timeout sequence
      for (int i = 0; i < 200; i++) {
        // read the String from the serial port and set it equal to under, declared previously
        under = mySerial.readStringUntil((char)00);
        // check if under contains the String data; if so, break the loop
        if (under.length() > 0) {
          break;
        }
      }
      // if under contains information, break the loop
      if (under.length() > 0) {
        break;
      }
      // if not, try again
      else {
        Serial.println("Didn't receive temp, trying again");
        mySerial.print('t');
      }
    }
    // once again, check if s is not null. if it is not, you know that there is an error happening.
    if (under.length() < 1) {
      Serial.println("Error: did not receive temperature. Check sensor.");
    }

    //retreive all the data from the getDateAndTime() method below
    String dt = getDateAndTime();
    //Uses a substring method to split the data into date and time, as opposed to together as one unit
    String d = dt.substring(0, 11);
    String t = dt.substring(12);

    // saves data, time, and temperature data into a CSV-format string
    dataString = d + "," + t + "," + under + ",";

    // print data to SD card
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open(fileName, FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      // print the data to the card
      dataFile.println(dataString);
      // close the file
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.print("error opening ");
      Serial.println(fileName);
    }

    // print each set of information to the Serial port, for the ESP8266 to receive
    Serial.print(configVariables);
    Serial.print(codeVersion);
    Serial.print(dataString);
    // send a specific stop char, so the ESP8266 knows when the information is all been sent
    Serial.print('z');
  }
}

// here are variables being initialized to be used within the getDateAndTime() method
const int len = 32;
static char buf[len];

/* This is an exterior method, getDateAndTime(), which allows for the calculation of the current date and time using the RTC library
   The method takes no parameters and returns a String value, the date and time read by the DeadOn RTC
*/
String getDateAndTime() {
  //gets the date and time from the RTC library and sets it as a DateTime variable of the object class
  DateTime now = RTC.now();
  // set the file name equal to the date, in the form of month_year.txt, where month and year are both integers
  fileName = now.month();
  fileName += "_";
  fileName += now.year();
  fileName += ".txt";
  // makes the DateTime object into a String
  String datetime = now.toString(buf, len);
  //returns the date and time as a string
  return datetime;
}

