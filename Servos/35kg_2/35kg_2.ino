#include <Servo.h>

Servo servo1;   // crea un objeto servo para controlar un servo
Servo servo2;   // crea un objeto servo para controlar un servo

void setup() { 
  Serial.begin(9600);
  servo1.attach(8);   // conecta el servo en el pin 6 al objeto servo
  servo1.write(90);    // gira el servo a 0 grados inmediatamente
  //servo2.attach(5);   // conecta el servo en el pin 6 al objeto servo
  //servo2.write(0);    // gira el servo a 0 grados inmediatamente
  delay(3000);
}

void loop() { 
  
  // para gripper abajo 0 derecha (servo1) y 180 izquierda (servo2)

  servo1.write(70); // DERECHO lleva el servo a su posicion inicial LIMITE INFERIOR 0 LIMITE SUPERIOR 110
  //servo2.write(0); // IZQUIERDA lleva el servo a su posicion inicial LIMITE INFERIOR 180 LIMITE SUPERIOR 70
  
}