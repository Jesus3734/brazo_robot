#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <Servo.h>

MPU6050 sensor;
Servo miServo1;
Servo miServo2;

// --- VARIABLES DEL MPU6050 ---
int ax, ay, az;
int gx, gy, gz;
long f_ax, f_ay, f_az; int p_ax, p_ay, p_az;
long f_gx, f_gy, f_gz; int p_gx, p_gy, p_gz;
int ax_o, ay_o, az_o; int gx_o, gy_o, gz_o;

float ang_x;
float ang_x_prev = 0;
long tiempo_prev = 0;

// --- VARIABLES DE LOS SERVOS ---
int angle1 = 90; 
int angle2 = 90; 

void setup() {
  // Velocidad de 57600 para que el MPU no se sature
  Serial.begin(57600);
  Wire.begin();
  sensor.initialize();

  // Configuración del Motor 1
  miServo1.write(angle1); 
  miServo1.attach(6);
  
  // Configuración del Motor 2
  miServo2.write(angle2);
  miServo2.attach(7); 

  // Proceso de calibración inicial
  Serial.println("Calibrando MPU6050...");
  calibrarMPU();
  Serial.println("Listo. Esperando comando de Python...");

  // Inicializar reloj del filtro
  tiempo_prev = millis();
}

void loop() {
  // 1. LEER EL COMANDO DESDE PYTHON CON PROTOCOLO A/B
  if (Serial.available() > 0) {
    String mensaje = Serial.readStringUntil('\n'); 
    
    // ¿El mensaje es para el Motor 1?
    if (mensaje.charAt(0) == 'A') {
      int nuevo_angulo1 = mensaje.substring(1).toInt(); 
      if (nuevo_angulo1 >= 0 && nuevo_angulo1 <= 180) {
        angle1 = nuevo_angulo1; // Actualizamos la memoria
        miServo1.write(angle1); // Movemos el motor
      }
    }
    
    // ¿El mensaje es para el Motor 2?
    else if (mensaje.charAt(0) == 'B') {
      int nuevo_angulo2 = mensaje.substring(1).toInt();
      if (nuevo_angulo2 >= 0 && nuevo_angulo2 <= 180) {
        angle2 = nuevo_angulo2; // Actualizamos la memoria
        miServo2.write(angle2); // Movemos el motor
      }
    }
  }

  // 2. CÁLCULO DE TIEMPO (dt) PARA EL MPU
  float dt = (millis() - tiempo_prev) / 1000.0;
  tiempo_prev = millis();

  // 3. ADQUISICIÓN Y FILTRADO DE DATOS (Filtro Complementario)
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  
  float accel_ang_x = atan2(ay, sqrt(pow((float)ax, 2) + pow((float)az, 2))) * (180.0 / 3.14);
  ang_x = 0.98 * (ang_x_prev + (gx / 131.0) * dt) + 0.02 * accel_ang_x;
  
  if (isnan(ang_x)) { ang_x = 0; }
  ang_x_prev = ang_x;

  // 4. IMPRESIÓN DE DATOS POR SERIAL PARA PYTHON
  // Formato CSV: "Angulo1,Angulo2,AnguloMPU"
  Serial.print(angle1); 
  Serial.print(",");
  Serial.print(angle2); 
  Serial.print(",");
  Serial.println(ang_x); 

  // Pausa de 20ms recomendada para la frecuencia del Servo y del I2C
  delay(20); 
}

// --- FUNCIÓN DE CALIBRACIÓN DEL MPU ---
void calibrarMPU() {
  ax_o = sensor.getXAccelOffset(); ay_o = sensor.getYAccelOffset(); az_o = sensor.getZAccelOffset();
  gx_o = sensor.getXGyroOffset(); gy_o = sensor.getYGyroOffset(); gz_o = sensor.getZGyroOffset();

  for(int i = 0; i < 1500; i++) {
    sensor.getAcceleration(&ax, &ay, &az);
    sensor.getRotation(&gx, &gy, &gz);

    f_ax = f_ax - (f_ax >> 5) + ax; p_ax = f_ax >> 5;
    f_ay = f_ay - (f_ay >> 5) + ay; p_ay = f_ay >> 5;
    f_az = f_az - (f_az >> 5) + az; p_az = f_az >> 5;
    f_gx = f_gx - (f_gx >> 3) + gx; p_gx = f_gx >> 3;
    f_gy = f_gy - (f_gy >> 3) + gy; p_gy = f_gy >> 3;
    f_gz = f_gz - (f_gz >> 3) + gz; p_gz = f_gz >> 3;

    if (i % 100 == 0 && i > 0) {
      if (p_ax > 0) ax_o--; else ax_o++;
      if (p_ay > 0) ay_o--; else ay_o++;
      if (p_az - 16384 > 0) az_o--; else az_o++;
      sensor.setXAccelOffset(ax_o); sensor.setYAccelOffset(ay_o); sensor.setZAccelOffset(az_o);
      if (p_gx > 0) gx_o--; else gx_o++;
      if (p_gy > 0) gy_o--; else gy_o++;
      if (p_gz > 0) gz_o--; else gz_o++;
      sensor.setXGyroOffset(gx_o); sensor.setYGyroOffset(gy_o); sensor.setZGyroOffset(gz_o);
    }
    delay(2);
  }
}