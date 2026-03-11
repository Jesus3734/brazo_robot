#include <Servo.h>

Servo hombro;   // crea un objeto servo para controlar un servo
Servo codo;   // crea un objeto servo para controlar un servo

void setup() { 
  Serial.begin(9600);
  hombro.attach(4);   // conecta el servo en el pin 6 al objeto servo
  hombro.write(0);    // gira el servo a 0 grados inmediatamente
  codo.attach(5);   // conecta el servo en el pin 6 al objeto servo
  codo.write(0);    // gira el servo a 0 grados inmediatamente
}

void loop() { 
  
  // para gripper abajo 0 derecha (servo1) y 180 izquierda (servo2)

  hombro.write(180); // DERECHO lleva el servo a su posicion inicial LIMITE INFERIOR 0 LIMITE SUPERIOR 110
  codo.write(180); // IZQUIERDA lleva el servo a su posicion inicial LIMITE INFERIOR 180 LIMITE SUPERIOR 70
  
}