#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>

#define SCREEN_ADDRESS 0x3C   // Common I2C address
#define OLED_RESET -1
#define LED_BUILTIN 15
#define IN1 19
#define IN2 20
#define STBY 18
#define SOIL0 0
#define SOIL1 1

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET); 
// Width & height here are placeholders
Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

// Adafruit_BME280 bme1; // use I2C interface
// Adafruit_Sensor *bme_temp1 = bme1.getTemperatureSensor();
// Adafruit_Sensor *bme_pressure1 = bme1.getPressureSensor();
// Adafruit_Sensor *bme_humidity1 = bme1.getHumiditySensor();

void setup() {
  Serial.begin(115200);
  Wire.begin(6, 7);
  Wire.setClock(50000);
  // Wire1.begin(8, 9);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  if (!bme.begin(0x76, &Wire)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1) delay(10);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED not found");
    while (1);
  }

  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();

  // bme_temp1->printSensorDetails();
  // bme_pressure1->printSensorDetails();
  // bme_humidity1->printSensorDetails();

  display.clearDisplay();

  Serial.print("OLED Width: ");
  Serial.println(display.width());

  Serial.print("OLED Height: ");
  Serial.println(display.height());
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true);
  display.print("H: ");
  display.setCursor(32, 0); 
  display.println(display.height());
  display.setCursor(0, 32);
  display.print("W: ");
  display.setCursor(32, 32);
  display.println(display.width());
  display.display();
  display.clearDisplay();
}

void loop() {
  int value0 = analogRead(SOIL0);
  int value1 = analogRead(SOIL1);

  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);

  // sensors_event_t temp_event1, pressure_event1, humidity_event1;
  // bme_temp1->getEvent(&temp_event1);
  // bme_pressure1->getEvent(&pressure_event1);
  // bme_humidity1->getEvent(&humidity_event1);

  display.clearDisplay();
  display.setCursor(0, 0);
  digitalWrite(STBY, LOW);
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW); 
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  // Serial.print(value0);
  // Serial.print(", ");
  // Serial.println(value1);

  display.print("S1: ");
  display.setCursor(20, 0); 
  display.println(value0);
  display.setCursor(0, 15);

  display.print("S2: ");
  display.setCursor(20, 15);
  display.println(value1);

  display.setCursor(0, 30);
  display.printf("Tmp0: %.2f C", temp_event.temperature);

  // display.setCursor(0, 60);
  // display.printf("Tmp1: %.2f °C", temp_event1.temperature);

  display.setCursor(0, 45);
  display.printf("Hum0: %.2f %", humidity_event.relative_humidity);

  // display.setCursor(0, 100);
  // display.printf("Hum1: %.2f %", humidity_event1.relative_humidity);

  display.display();
  delay(1000);
}
