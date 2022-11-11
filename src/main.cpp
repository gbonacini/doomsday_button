#include <doomsday.h>

doomsday::Doomsday* dd = nullptr;

void setup() {
   dd = new doomsday::Doomsday();
}

void loop() {
  delay(50);     
  dd->checkStatus();
}