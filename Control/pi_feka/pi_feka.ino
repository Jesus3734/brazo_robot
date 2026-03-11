#include <Servo.h>

Servo miServoIzq;
Servo miServoDer;

// --- 1. LAS GANANCIAS DE TU TAREA ---
float Kp = 0.1; 
float Ki = 0.75; 

// --- 2. VARIABLES DEL CONTROLADOR PI ---
float setpoint = 90.0; // Arranca en 90 por seguridad, pero cambiará en vivo
float input = 0.0;     
float output = 0.0;    

float error = 0.0;
float error_integral = 0.0;
long tiempo_prev = 0;

void setup() {
  Serial.begin(57600);
  miServoIzq.attach(9);
  miServoDer.attach(6);

  miServoIzq.write(180); 
  miServoDer.write(0);
  
  Serial.println("Iniciando Control PI... Escribe un angulo (0-180) y presiona Enter:");
  delay(2000);
  
  tiempo_prev = millis();
}

void loop() {
  // --- A. LECTURA EN DIRECTO DEL SETPOINT ---
  if (Serial.available() > 0) {
    String mensaje = Serial.readStringUntil('\n'); 
    float nuevo_angulo = mensaje.toFloat(); 
    
    // Filtro para evitar que le mandes letras o números fuera de rango
    if (nuevo_angulo >= 0 && nuevo_angulo <= 180) {
      setpoint = nuevo_angulo;
    }
  }

  // --- B. Calcular tiempo transcurrido (dt) ---
  float dt = (millis() - tiempo_prev) / 1000.0;
  tiempo_prev = millis();

  // --- C. RETROALIMENTACIÓN SIMULADA ---
  input = input + (output - input) * 0.08; 

  // --- D. ECUACIONES DEL CONTROL PI ---
  error = setpoint - input;
  
  float P = Kp * error;
  
  error_integral = error_integral + (error * dt);
  float I = Ki * error_integral;
  
  // --- E. ANTI-WINDUP ---
  if (I > 180) I = 180;
  else if (I < -180) I = -180;

  // --- F. SEÑAL DE CONTROL TOTAL ---
  output = P + I;

  // --- G. SATURACIÓN ---
  if (output > 180) output = 180;
  else if (output < 0) output = 0;

  // --- H. MOVER LOS MOTORES ---
  miServoIzq.write(180 - output); 
  miServoDer.write(output);

  // --- I. IMPRIMIR DATOS ---
  // Imprimirá el Setpoint actual y la respuesta del PI separadas por coma
  Serial.print(setpoint);
  Serial.print(",");
  Serial.println(output);

  delay(20); 
}