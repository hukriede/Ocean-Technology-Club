#include <SPI.h>
#include <SD.h>

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
  writeConfiguration();
}

void loop() {
  readFile();
  delay(5000);
}

void writeConfiguration(){

  int NumberOfReadings = 5;
  int ModelNumber = 1;
  int SerialNumber = 1;
  String Location = "Phillippi Creek";
  float Latitude = 27.272;
  float Longitude = -82.534;
  int DelayTime = 10;

  String Configuration = (String(NumberOfReadings) + "," + String(ModelNumber)  + ","
                         + String(SerialNumber)  + "," + String(Location)  + "," + String(Latitude)
                         + "," + String(Longitude)  + "," + String(DelayTime) + ",\n");

    if (SD.exists("_config.txt")){
    SD.remove("_config.txt");
    }

  File configFile = SD.open("_config.txt", FILE_WRITE);

  if (configFile) {
    configFile.println(Configuration);
    configFile.close();
  }

  else {
    Serial.println("Error opening file...");
  }
}

void readFile(){
  File configFile = SD.open("_config.txt");
  if (configFile.available()) {
      String configVariables = configFile.readStringUntil('\n');
      configVariables.trim();
      Serial.println(configVariables);
      configFile.close();
  } else {
    Serial.println("error opening _config.txt");
  }
}
