#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

ESP8266WiFiMulti WiFiMulti;
SoftwareSerial AboveWaterSerial(12, 14);
WiFiClient Client;
double delayTime = 1000;
String data[10];

void setup() {
  Serial.begin(115200);
  AboveWaterSerial.begin(115200);
  WiFiMulti.addAP("Verizon-791L-00CC", "78d8b2d1");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi Connected");
}

void loop() {
  if (connectToHost()) {
    askForData();
    String json = buildJSON(data);
    sendToClient(json);
    Client.stop();
    delay(delayTime);
  }
}

/*
   Asks the AboveWater Arduino for an entry of data and parses it into an array
   constucts an array consumable by the JSON builder
 */
void askForData() {
  String dataCSV;
  AboveWaterSerial.print('d');
  while(dataCSV.length() <= 0) {
    dataCSV = Serial.readStringUntil('z');
  }
  splitCSV(dataCSV);
}

void splitCSV(String dataCSV){
  char holder[dataCSV.length() + 1];
  dataCSV.toCharArray(holder, dataCSV.length() + 1);

  int counter = 0;
  int prev = 0;
  for (int i = 0; i <= dataCSV.length() + 1; i++) {
    if (holder[i] == ',') {
      data[counter] = dataCSV.substring(prev, i);
      prev = i + 1;
      counter++;
    }
  }
}

/*
   Collects data from the AboveWater Arduino, constucts a JSON object with the
   gathered data, and sends the data, using the predefined wireless connection, to
   client.
 */
String buildJSON (String* data) {
  StaticJsonBuffer<375> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  JsonObject& systems = jsonBuffer.createObject();
  JsonObject& sensors = jsonBuffer.createObject();
  JsonObject& gps = jsonBuffer.createObject();

  systems["ModelNumber"] = data[0].toInt();
  systems["SerialNumber"] = data[1].toInt();
  systems["Location"] = data[2];
  gps["Lat"] = data[3].toFloat();
  gps["Lon"] = data[4].toFloat();
  systems["Date"] = data[6];
  systems["Time"] = data[7];
  sensors["Water_Temperature"] = data[8].toFloat();

  JsonArray& System = root.createNestedArray("System");
  JsonArray& GPS = systems.createNestedArray("GPS");
  JsonArray& Sensor = root.createNestedArray("Sensors");

  System.add(systems);
  GPS.add(gps);
  Sensor.add(sensors);

  char json[350];
  root.prettyPrintTo(json, sizeof(json));
  String jsonString = json;
  jsonString.replace("[","");
  jsonString.replace("]","");

  setDelayTime(data[5].toFloat());
  return jsonString;
}

boolean connectToHost() {
  const uint16_t port = 4001;
  const char * host = "clouddev.mote.org";
  while (!Client.connect(host, port)) {
    delay(5000);
    Serial.print(".");
  }
  return true;
}

void setDelayTime(int delay) {
  delayTime = delay * 60 * 1000;
}

void sendToClient(String data) {
  Serial.println(data);
  Client.print(data);
}
