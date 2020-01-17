int motion = 2;
int cup = 0;
void setup() {
  pinMode(motion, INPUT);
  Serial.begin(9600);
}

void loop() {
  // 적외선 인체감지 센서에서 값을 읽는다
  int sensor = digitalRead(motion);
  // 센서값을 시리얼 모니터에 출력
  Serial.println(sensor);

  if (sensor == HIGH) {
    cup = 1;
    delay(500);
    cup = 0;
    delay(500);
  }
}
