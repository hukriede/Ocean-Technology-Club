#include <SPI.h>
#include <SparkFunDS3234RTC.h>

#define DS13074_CS_PIN 8

void setup(){
  /* Set to Saturday Dec 9th 1:37:42 PM

  Modify the variables to set the date and time appropriately
  Monday == 1 -- Sunday == 7
  */
  Serial.begin(115200);
  rtc.begin(DS13074_CS_PIN);

  int hour = 16;
  int minute = 37;
  int second = 12;
  int day = 7;
  int date = 9;
  int month = 12;
  int year = 16;

  rtc.setTime(second, minute, hour, day, date, month, year);
}

void loop(){
  /* Reports the date that the RTC is set to

  To see the current date send the character 'd' over 115200
  */
  if ((char)Serial.read() == 'd') {
    rtc.update();
    String date = (String(rtc.month()) + "/" + String(rtc.date()) + "/" + String(rtc.year()));
    String time =  (String(rtc.hour()) + ":" + String(rtc.minute()) + ":" + String(rtc.second()));
    Serial.println(date + " " + time);
  }
}
