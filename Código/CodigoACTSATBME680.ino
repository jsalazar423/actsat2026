#include <Wire.h>
#include <Adafruit_BME680.h>
#include <HardwareSerial.h>


#define SDA_PIN 7
#define SCL_PIN 8



#define SEALEVELPRESSURE_HPA 1013.25
#define BME_ADDRESS 0x76
#define BME_ADDRES2 0x77

// -------- OBJECTS --------
Adafruit_BME680 bme;

HardwareSerial loraSerial(2);

//DEW POINT
float calculateDewPoint(float tempC, float humidity) {
  float a = 17.27;
  float b = 237.7;
  float alpha = ((a * tempC) / (b + tempC)) + log(humidity / 100.0);
  return (b * alpha) / (a - alpha);
}

// Absolute humidity (g/m³)
float calculateAbsoluteHumidity(float tempC, float humidity) {
  float tempK = tempC + 273.15;
  float es = 6.112 * exp((17.67 * tempC) / (tempC + 243.5));
  float ah = (es * humidity * 2.1674) / tempK;
  return ah;
}

// Heat index (approximation in °C)
float calculateHeatIndex(float tempC, float humidity) {
  float T = tempC;
  float R = humidity;
  float HI = -8.784695 +
             1.61139411 * T +
             2.338549 * R +
             -0.14611605 * T * R +
             -0.012308094 * T * T +
             -0.016424828 * R * R +
             0.002211732 * T * T * R +
             0.00072546 * T * R * R +
             -0.000003582 * T * T * R * R;
  return HI;
}


unsigned long packetCounter = 0;


void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Starting CANSAT Lite...");

  if (!bme.begin(BME_ADDRESS)) {
    Serial.println("❌ BME680 not found!");
    while (1);
  }

  Serial.println("✅ BME680 connected");

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);



  Serial.println("✅ Sensors ready, moving to loop...");
}

void loop() {

  if (!bme.performReading()) {
    Serial.println("❌ BME680 reading error");
    delay(1000);
    return;
  }

  float temp_bme = bme.temperature;
  float hum_bme  = bme.humidity;
  float pres     = bme.pressure / 100.0;
  float gas      = bme.gas_resistance / 1000.0;

  float altitude = 44330.0 * (1.0 - pow(pres / SEALEVELPRESSURE_HPA, 0.1903));
  float dewPoint = calculateDewPoint(temp_bme, hum_bme);
  float absHumidity = calculateAbsoluteHumidity(temp_bme, hum_bme);
  float heatIndex = calculateHeatIndex(temp_bme, hum_bme);



  packetCounter++;  // ---- BME680 ----

  String packet = "PKT:" + String(packetCounter) + "," +
                  "Milis:" + String(millis()) + "," +
                  "T:" + String(temp_bme,2) + "," +
                  "H:" + String(hum_bme,2) + "," +
                  "P:" + String(pres,2) + "," +
                  "AP:" + String(altitude,2) + "," +
                  "G:" + String(gas,2) + "," +
                  "DP:" + String(dewPoint,2) + ","
                  "AH:" + String(absHumidity,2) + ","
                  "HI:" + String(heatIndex, 2);
    
  Serial.println(packet);
  Serial.println("-----------------------------------------------------------------------------------------\n");

  delay(1000);
}
