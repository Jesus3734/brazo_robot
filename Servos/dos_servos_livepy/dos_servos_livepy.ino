#include <Servo.h>

Servo miServo1;
Servo miServo2;

int angle1 = 90; 
int angle2 = 90; // Ángulo inicial para el segundo motor

void setup() {
  Serial.begin(9600);
  
  // Configuración del Motor 1
  miServo1.write(angle1); 
  miServo1.attach(6);
  
  // Configuración del Motor 2
  miServo2.write(angle2);
  miServo2.attach(7); // Conectado al pin 9
}

void loop() {
  // verifica si hay nuevos datos enviados por el usb
  if (Serial.available() > 0) {
    
    // lee los datos enviados por el usb
    String mensaje = Serial.readStringUntil('\n'); 
    
    // --- DECODIFICADOR DEL PROTOCOLO ---
    
    // ¿El mensaje es para el Motor 1 (Empieza con A)?
    if (mensaje.charAt(0) == 'A') {
      // Recorta la letra, convierte el resto a número y mueve el motor
      int nuevo_angulo1 = mensaje.substring(1).toInt(); 
      if (nuevo_angulo1 >= 0 && nuevo_angulo1 <= 180) {
        miServo1.write(nuevo_angulo1);
      }
    }
    
    // ¿El mensaje es para el Motor 2 (Empieza con B)?
    else if (mensaje.charAt(0) == 'B') {
      // Recorta la letra, convierte el resto a número y mueve el motor
      int nuevo_angulo2 = mensaje.substring(1).toInt();
      if (nuevo_angulo2 >= 0 && nuevo_angulo2 <= 180) {
        miServo2.write(nuevo_angulo2);
      }
    }
    
  }
}