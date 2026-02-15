
#define LEDC_FREQ 8000
#define LEDC_RES 8

uint8_t motorInputOne = 0x13;
uint8_t motorInputTwo = 0x14;
uint8_t motorStandby = 0x12;

void setup() {
  ledcAttach(motorInputOne, LEDC_FREQ, LEDC_RES);
  ledcAttach(motorInputTwo, LEDC_FREQ, LEDC_RES);
  pinMode(motorStandby, OUTPUT);
}

void forward(){
  digitalWrite(motorStandby, HIGH);
  ledcWrite(motorInputOne, 255);
  ledcWrite(motorInputTwo, 0);

  // digitalWrite(motorInputOne, HIGH);
  // digitalWrite(motorInputTwo, LOW);
}

void reverse(){
  digitalWrite(motorStandby, HIGH);
  ledcWrite(motorInputTwo, 255);
  ledcWrite(motorInputOne, 0);

  // digitalWrite(motorInputTwo, HIGH);
  // digitalWrite(motorInputOne, LOW);
}

void loop() {
  forward();
  vTaskDelay(pdMS_TO_TICKS(1000));
  reverse();
  vTaskDelay(pdMS_TO_TICKS(1000));
}
