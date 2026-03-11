
#include <Servo.h> 
#define Servo_PWM 6
Servo MG995_Servo;  
  

void setup() {
  Serial.begin(9600); 
   MG995_Servo.write(0)
  MG995_Servo.attach(Servo_PWM);  
}

void loop() {
  Serial.println("0"); //Imprime un cero cada vez que se mueve el servo
  MG995_Servo.write(0); //Mueve el servo CW a maxima velocidad
  delay(250); //Se mueve solo 250ms para girar el servo 90° compensando la inercia
  MG995_Servo.detach(); //Detiene el motor 
  delay(1000);
  MG995_Servo.attach(Servo_PWM); 
  Serial.println("0"); //Mueve el servo CCW a maxima velocidad
  MG995_Servo.write(180);
  delay(250);
  MG995_Servo.detach(); 
  delay(1000);
  MG995_Servo.attach(Servo_PWM);

      
}