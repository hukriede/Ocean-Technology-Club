#include <SoftwareSerial.h>
#include <RTC_DS3234.h>
#include <SPI.h>
#include <SD.h>

#define SDchipSelect (10)
#define RTCchipSelect (8)

SoftwareSerial BelowWaterSerial = SoftwareSerial(2, 3);
RTC_DS3234 RTC(8);

String configVariables;
String dataString;
String fileName;
int numReadings;

void setup() {
  Serial.begin(115200);
  BelowWaterSerial.begin(9600);

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
  setConfigVariables();
}

void loop() {
  if ((char)Serial.read() == 'd') {
    askForTemperature();
    String temperature = watchForTemperatureResults();
    createDataString(temperature);
    sendDataToClient();
  }
}

void sendDataToClient(){
    Serial.print(configVariables);
    Serial.print(dataString);
    Serial.print('z');
}

void writeDataToDisk(){
    File dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
    }
    else {
      Serial.print("error opening ");
      Serial.println(fileName);
    }
}

void createDataString(String data){
    String dt = getDateAndTime();
    String d = dt.substring(0, 11);
    String t = dt.substring(12);
    dataString = d + "," + t + "," + data + ",";
}

String watchForTemperatureResults(){
  String results;
  while (results.length() <= 0){
    results = BelowWaterSerial.readStringUntil((char)00);
  }
  return results;
}

void askForTemperature(){
    Serial.flush();
    BelowWaterSerial.print('t');
    BelowWaterSerial.print(numReadings);
    BelowWaterSerial.print('z');
    delay(500);
}

void setConfigVariables(){
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
}

String getDateAndTime() {
  DateTime now = RTC.now();
  fileName = now.month();
  fileName += "_";
  fileName += now.year();
  fileName += ".txt";
  const int length = 32;
  static char buffer[length];
  String datetime = now.toString(buffer, length);
  return datetime;
}
