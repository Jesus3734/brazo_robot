#include <Servo.h> 
#define Servo_PWM 6 
Servo MG995_Servo;  
  
void setup() {
  Serial.begin(9600); 
  MG995_Servo.writeMicroseconds(1500); // Forzamos el punto muerto inicial
  MG995_Servo.attach(Servo_PWM); 
  Serial.println("--- INICIANDO PRUEBA DE DIAGNOSTICO ---");
}

void loop() {
  Serial.println("\nEnviando: write(0)"); 
  MG995_Servo.write(0); 
  delay(4000);
  
  Serial.println("Enviando: write(90)"); 
  MG995_Servo.write(90); 
  delay(4000);
  
  Serial.println("Enviando: write(130)"); 
  MG995_Servo.write(130);
  delay(4000);
}