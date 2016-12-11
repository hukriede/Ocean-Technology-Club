#include <SparkFunDS3234RTC.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define RTC_PIN 8
#define SD_PIN 10
SoftwareSerial BelowWaterSerial = SoftwareSerial(2, 3);

struct configs {
  int number;
  String values;
  String file;
  String date;
  String time;
};

void setup() {
  Serial.begin(115200);
  BelowWaterSerial.begin(9600);
  SPI.begin();
  rtc.begin(RTC_PIN);
  while (!SD.begin(SD_PIN)) {
    Serial.println("Card failed, or not present");
  }
}

void loop() {
  /* Asks the BelowWater Arduino for the temperature when prompted.

  The AboveWater Arduino continuously waits for serial input from the Huzzah ESP8266.
  If it sees the character 'd' it gathers configuration variables from the SD card and
  asks the BelowWater Arduino for the temperature.

  Then it gets the date and time from the DeadOn RTC, writes the current temperature value
  to the SD card, and sends the gathered data (including the new temperature) to the Huzzah.
  */

  if ((char)Serial.read() == 'd') {
    struct configs configuration = setConfigVariables();
    askForTemperature(configuration.number);
    String temperature = watchForTemperatureResults();
    String data = createDataString(temperature, &configuration);
    writeDataToDisk(configuration.file, data);
    sendDataToClient(data, configuration.values);
  }
}

struct configs setConfigVariables(){
  struct configs currentConfigs;
  File configFile = SD.open("_config.txt");
  if (configFile) {
    while (configFile.available()) {
      currentConfigs.number = (configFile.readStringUntil(',')).toInt();
      String configVariables = configFile.readStringUntil('\n');
      configVariables.trim();
      currentConfigs.values = configVariables;
      break;
    }
    configFile.close();
  } else {
    Serial.println("error opening _config.txt");
  }
  return currentConfigs;
}

void askForTemperature(int numberOfReadings){
  Serial.flush();
  BelowWaterSerial.print('t');
  BelowWaterSerial.print(numberOfReadings);
  BelowWaterSerial.print('z');
  delay(500);
}

String watchForTemperatureResults(){
  String results;
  while (results.length() <= 0){
    results = BelowWaterSerial.readStringUntil((char)00);
  }
  return results;
}

String createDataString(String data, struct configs* configuration){
  getDateAndTime(configuration);
  String dataString = configuration->date + "," + configuration->time + "," + data + ",";
  return dataString;
}

void getDateAndTime(struct configs* configuration) {
  /* Gets the current datetime from the DeadOn and formats it appropriately

  This function is passed a reference to our configuration object. We start by updating
  the RTC's registers so that they reflect the proper time. Then we create a file name
  and two representative strings for later use.

  Setting the time on the DeadOn is required prior if we want the returned string to
  represent the correct values.
  */

  rtc.update();
  String fileName = (String(rtc.month()) + "_" + String(rtc.year()) + ".txt");
  String date = (String(rtc.month()) + "/" + String(rtc.date()) + "/" + String(rtc.year()));
  String time =  (String(rtc.hour()) + ":" + String(rtc.minute()) + ":" + String(rtc.second()));
  configuration->file = fileName;
  configuration->date = date;
  configuration->time = time;
}

void sendDataToClient(String data, String configVariables){
  Serial.print(configVariables);
  Serial.print(data);
  Serial.print('z');
}

void writeDataToDisk(String fileName, String dataString){
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.print("error opening " + fileName);
  }
}
