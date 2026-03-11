#include <Servo.h>

Servo miServo;
int angle1 = 90;  
void setup() {
  Serial.begin(9600);
  miServo.write(angle1); 
  miServo.attach(4);
}

void loop() {
  if (Serial.available() > 0) {
    
    angle1 = Serial.parseInt(); 
    
    if (angle1 >= 0 && angle1 <= 180) {
      miServo.write(angle1);
    }
  }
}