#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

ESP8266WiFiMulti WiFiMulti;
SoftwareSerial mySerial(12, 14);
double delayTime = 0;

WiFiClient client;
const uint16_t port = 4001;
const char * host = "clouddev.mote.org";

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  delay(10);

  WiFiMulti.addAP("Verizon-791L-00CC", "78d8b2d1");
  Serial.print("Connecting.");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi Connected");
  delay(500);
}

void loop() {
  if (connectToHost()) {
    sendToClient("Sending JSON data to host");
    sendToClient(buildJSON());
    sendToClient("seconds until next cycle...");
    sendToClient((String)(delayTime / 1000));
    sendToClient("Closing connection");
    client.stop();
    delay(delayTime);
  }
}


/*
  askForData()

  Asks the AboveWater Arduino for an entry of data and parses it into an array
  constucts an array consumable by the JSON builder
*/
void askForData(String* data) {
  String s;
  mySerial.print('d');
  for (int i = 0; i < 3; i++) {
    for (int i = 0; i < 200; i++) {
      s = Serial.readStringUntil('z');
      if (s.length() > 0) {
        break;
      }
      else {
      }
    }
    if (s.length() > 0) {
      break;
    }
    else {
      Serial.println("Didn't receive data, trying again");
      mySerial.print('d');
    }
  }
  if (s.length() < 1) {
    sendToClient("Error: did not receive data. Check arduino.");
  }

  char holder[s.length() + 1];
  s.toCharArray(holder, s.length() + 1);

  int counter = 0;
  int prev = 0;

  for (int i = 0; i <= s.length() + 1 ; i++) {
    if (holder[i] == ',') {
      data[counter] = s.substring(prev, i);
      prev = i + 1;
      counter++;
    }
  }
}

/*
 buildJSON()

 Collects data from the AboveWater Arduino, constucts a JSON object with the
 gathered data, and sends the data, using the predefined wireless connection, to
 client.
*/
String buildJSON () {
  String data[10];
  askForData(data);
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
  systems["CodeVersion"] = data[6];
  systems["Date"] = data[7];
  systems["Time"] = data[8];
  sensors["Water_Temperature"] = data[9].toFloat();

  delayTime = data[5].toFloat();
  setDelayTime();

  JsonArray& System = root.createNestedArray("System");
  System.add(systems);
  JsonArray& GPS = systems.createNestedArray("GPS");
  GPS.add(gps);
  JsonArray& Sensor = root.createNestedArray("Sensors");
  Sensor.add(sensors);

  String json;
  char buff[350];
  root.printTo(buff, sizeof(buff));
  json = buff;

  json = "JSON_OTClub: " + json;
  json.replace("[","");
  json.replace("]","");

  return json;
}

/*
setDelayTime()

Modifies the amount of time that is waited between each message sent
*/
void setDelayTime() {
  delayTime = delayTime * 60 * 1000;
}

/*
sendToClient()

Prints the message to the screen, sends it to the client, and waits for a response.
*/
void sendToClient(String data) {
  Serial.println(data);
  client.print(data);
  String line = client.readStringUntil('\r');
  client.println(line);
  delay(10);
}

/*
 connectToHost()

 Until the chip has connected to the defined host loop repeatedly waiting
 five seconds between each connection attempt
*/
boolean connectToHost() {
  while (!client.connect(host, port)) {
    delay(5000);
    Serial.print(".");
  }
  return true;
}
