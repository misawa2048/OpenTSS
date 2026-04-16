#include <Arduino.h>
#include <OpenTSS.hpp>

OpenTSS tss(2); // time sharing system with 2 triggers (default=8)
//uint32_t m_currentMillis;

void Fnc1(uint32_t _delta){
  Serial.printf("Trig1 calls each %dms\n", _delta);
  delay(10); // delay() can only be used for a time shorter than any trigger interval
}

void setup() {
  Serial.begin(115200); // initialize serial
  tss.AddTrig(Fnc1, 1000); // add trigger1
  tss.AddTrig([](uint32_t _delta){Serial.printf("Trig2 calls each %dms\n", _delta);}, 2500); // add trigger2
  
  //m_currentMillis=millis();
  Serial.print(TSS_VERSION);
  Serial.println("Start");
}

void loop() {
#if true
  tss.Update(); // call this in loop()
#else
  // If you need to update TSS with a different time interval, 
  // call the overloaded Update() with the desired time interval in milliseconds
  uint32_t nowMillis = millis();
  uint32_t deltaMillis = nowMillis - m_currentMillis;
  m_currentMillis = nowMillis;
  tss.Update(deltaMillis); // call this in loop()
#endif
  delay(1); // delay() can only be used for a time shorter than any trigger interval
}
