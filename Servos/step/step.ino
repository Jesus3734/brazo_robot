#include <Servo.h>

Servo servo;   // crea un objeto servo para controlar un servo

void setup() { 
  Serial.begin(9600);
  servo.attach(6);   // conecta el servo en el pin 6 al objeto servo
  servo.write(0);    // gira el servo a 0 grados inmediatamente
}

void loop() { 
  for (int angle = 0; angle <= 90; angle += 1) {   
    servo.write(angle);   // controla que el servo vaya a la posición en la variable 'angle' 
    delay(700);            // espera 10ms para que el servo alcance la posición   
  } 
  
  
}