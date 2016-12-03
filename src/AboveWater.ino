#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <RTC_DS3234.h>

#undef PROGMEM
#define PROGMEM __attribute__(( section(".progmem.data") ))
#undef PSTR
#define PSTR(s) (__extension__(oooooo{static prog_char __c[] PROGMEM = (s); &__c[0];}))
#define SDchipSelect (10)
#define RTCchipSelect (8)
#define codeVersion ("0.0.1,")

SoftwareSerial mySerial = SoftwareSerial(2, 3);
RTC_DS3234 RTC(8);

String configVariables;
String dataString;
String fileName;
int numReadings;
String under;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);

  SPI.begin();
  RTC.begin();

  pinMode(RTCchipSelect, OUTPUT);
  pinMode(SDchipSelect, OUTPUT);

  while (!SD.begin(SDchipSelect)) {
    Serial.println("Card failed, or not present");
  }

  if (!RTC.isrunning()) {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  File myFile = SD.open("_config.txt");
  if (myFile) {
    while (myFile.available()) {
      numReadings = (myFile.readStringUntil(',')).toInt();
      configVariables += myFile.readStringUntil('\n');
      break;
    }
    configVariables.trim();
    myFile.close();
  } else {
    Serial.println("error opening _config.txt");
  }
  myFile.close();
}


void loop() {
  char indicator = (char)Serial.read();
  if (indicator == 'd') {
    Serial.flush();
    mySerial.print('t');
    mySerial.print(numReadings);
    mySerial.print('z');
    delay(500);
    for (int i = 0; i < 3; i++) {
      for (int i = 0; i < 200; i++) {
        under = mySerial.readStringUntil((char)00);
        if (under.length() > 0) {
          break;
        }
      }
      if (under.length() > 0) {
        break;
      }
      else {
        Serial.println("Didn't receive temp, trying again");
        mySerial.print('t');
      }
    }
    if (under.length() < 1) {
      Serial.println("Error: did not receive temperature. Check sensor.");
    }

    String dt = getDateAndTime();
    String d = dt.substring(0, 11);
    String t = dt.substring(12);

    dataString = d + "," + t + "," + under + ",";

    File dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
    }
    else {
      Serial.print("error opening ");
      Serial.println(fileName);
    }

    Serial.print(configVariables);
    Serial.print(codeVersion);
    Serial.print(dataString);
    Serial.print('z');
  }
}

const int len = 32;
static char buf[len];

String getDateAndTime() {
  DateTime now = RTC.now();
  fileName = now.month();
  fileName += "_";
  fileName += now.year();
  fileName += ".txt";
  String datetime = now.toString(buf, len);
  return datetime;
}
