# OpenTSS
+ **OpenTSS** is a tiny but useful thread-like system for Arduino.  
+ **If delay() is used, the callback may not come around.** 

**Sample:**  
```
#include <Arduino.h>
#include <OpenTSS.hpp>

OpenTSS tss; // time sharing system

void Fnc1(uint32_t _delta){
  Serial.printf("Trig1 calls each %dms\n", _delta);
  delay(100); // delay() can be used only for a time shorter than the task interval
}

void setup() {
  Serial.begin(115200); // initialize serial
  tss.Setup(2); // setup tss with 2 triggers
  tss.AddTrig(Fnc1, 1000); // add trigger1
  tss.AddTrig([](uint32_t _delta){Serial.printf("Trig2 calls each %dms\n", _delta);}, 2000); // add trigger2
  Serial.println("Start");
}

void loop() {
  tss.Update(); // call this in loop()
  delay(1); // delay() can be used only for a time shorter than the task interval
}
```

**Result:**  
```
           :
Trig1 calls each 1000ms
Trig1 calls each 1000ms
Trig2 calls each 2000ms
Trig1 calls each 1000ms
Trig1 calls each 1000ms
Trig2 calls each 2000ms
```


