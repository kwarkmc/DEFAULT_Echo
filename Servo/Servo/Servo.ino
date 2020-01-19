#include<Servo.h>
Servo servo;
int ref_angle_value = 90;

void setup() {
  servo.attach(7);
  Serial.begin(9600);
}
char in_data;

int time = 0;

int target_angle = 0;
int delta_angle = 0;

void loop() {
  
  if(Serial.available()){

    in_data = Serial.read();
    if (in_data == '1')
    {
      target_angle = 0;

      delta_angle = abs(target_angle - ref_angle_value);
      
      servo.write(ref_angle_value - delta_angle);

      delay(delta_angle * 11);

      servo.write(ref_angle_value);
    }
    else if (in_data == '2')
    {
      target_angle = 180;

      delta_angle = abs(target_angle - ref_angle_value);
      
      servo.write(ref_angle_value + delta_angle);

      delay(delta_angle * 11);

      servo.write(ref_angle_value);
    }
  }
}
