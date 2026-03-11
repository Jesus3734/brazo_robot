#include <Servo.h>

Servo servo;   // crea un objeto servo para controlar un servo

void setup() { 
  servo.attach(6);   // conecta el servo en el pin 6 al objeto servo
  servo.write(0);    // gira el servo a 0 grados inmediatamente
}

void loop() { 
  // gira lentamente de 0 grados a 180 grados en pasos de 1 grado
  for (int angle = 0; angle <= 90; angle += 1) {   
    servo.write(angle);   // controla que el servo vaya a la posición en la variable 'angle' 
    delay(500);            // espera 10ms para que el servo alcance la posición   
  }
} 

