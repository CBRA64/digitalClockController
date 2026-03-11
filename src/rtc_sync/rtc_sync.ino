#include <RTClib.h>

RTC_DS1307 rtc;

// Time duration of the compiling and programming process in seconds. 
int32_t buffer_time_s = 8;

void setup() {
  Serial.begin(115200);

  while (! rtc.begin()) {
    Serial.println("Couldn't find RTC.");
    Serial.flush();
    delay(5000);
    buffer_time_s += 5;
  }
  Serial.println("RTC Module found.");
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DateTime time = rtc.now();
  rtc.adjust(time + TimeSpan(buffer_time_s));
  Serial.println("Time syncronization complete.");
}

void loop() {
  DateTime now = rtc.now();
  char display_structure[] = "YYYY/MM/DD-hh:mm:ss";
  Serial.println(now.toString(display_structure));
}