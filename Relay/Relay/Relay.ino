void setup() {
  // put your setup code here, to run once:
  pinMode(6, 1);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    char Data = Serial.read();
    if(Data == 'a')
    {
      digitalWrite(6, 1);
    }
    else if(Data == 'b')
    {
      digitalWrite(6, 0);
    }
  }
}
