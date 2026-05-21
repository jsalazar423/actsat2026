#include <HardwareSerial.h>

HardwareSerial loraSerial(2);

#define LoraRX 16
#define LoraTX 17
void setup() {
  Serial.begin(115200);
  loraSerial.begin(9600, SERIAL_8N1, LoraRX, LoraTX);

  Serial.println("Ground Station Ready. No Probelm");
}

void loop() {

  if (loraSerial.available()) {
    
    String received = loraSerial.readStringUntil('\n');

    Serial.print("Received: ");
    Serial.println(received);
  }
}
