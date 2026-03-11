#include <Servo.h>

Servo miServo;
int angle1 = 90; 

void setup() {
  Serial.begin(9600);
  miServo.write(angle1); 
  miServo.attach(8);
}

void loop() {
  // verifica si hay nuevos datos enviados por el usb
  if (Serial.available() > 0) {
    
    // lee los datos enviados por el usb
    String mensaje = Serial.readStringUntil('\n'); 
    
    // conversion de mensaje a numero para ser usado como angulo
    int angle1 = mensaje.toInt(); 
    
    // seguridad de movimiento para el servo
    if (angle1 >= 0 && angle1 <= 180) {
      miServo.write(angle1);
    }
  }
}