#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <Servo.h>

MPU6050 sensor;
Servo miServoIzq;
Servo miServoDer;

int ax, ay, az;
int gx, gy, gz;

long tiempo_prev;
float dt;
float ang_x;
float ang_x_prev = 0;

long f_ax, f_ay, f_az;
int p_ax, p_ay, p_az;
long f_gx, f_gy, f_gz;
int p_gx, p_gy, p_gz;
int ax_o, ay_o, az_o;
int gx_o, gy_o, gz_o;

int angulo_servo_izq = 180; 
int angulo_servo_der = 0;
bool escalon_activado = false; 
long tiempo_inicio_loop = 0;
long tiempo_impresion = 0;

void setup() {
  Serial.begin(57600);
  Wire.begin();
  sensor.initialize();

  miServoIzq.write(angulo_servo_izq); 
  miServoIzq.attach(9);
  miServoDer.write(angulo_servo_der); 
  miServoDer.attach(6);

  Serial.println("\nEnvie cualquier caracter para empezar la calibracion");
  while (true) { if (Serial.available()) break; }
  
  Serial.println("Calibrando, no mover IMU. Esto tomara aprox. 5 segundos...");
  calibrarMPU(); 
  Serial.println("¡Calibracion finalizada!");
  delay(2000);

  tiempo_prev = millis();
  tiempo_inicio_loop = millis(); 
}

void loop() {
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  
  dt = (millis() - tiempo_prev) / 1000.0;
  tiempo_prev = millis();
  
  float accel_ang_x = atan2(ay, sqrt(pow((float)ax, 2) + pow((float)az, 2))) * (180.0 / 3.14);
  ang_x = 0.98 * (ang_x_prev + (gx / 131.0) * dt) + 0.02 * accel_ang_x;
  
  if (isnan(ang_x)) {
    ang_x = 0;
  }
  
  ang_x_prev = ang_x;

  if (!escalon_activado && (millis() - tiempo_inicio_loop > 3000)) {
    
    angulo_servo_izq = 70;
    angulo_servo_der = 110;
    
    miServoIzq.write(angulo_servo_izq);
    miServoDer.write(angulo_servo_der);
    
    escalon_activado = true; 
  }

  if (millis() - tiempo_impresion > 10) {
    tiempo_impresion = millis();
    Serial.print(angulo_servo_izq); 
    Serial.print(","); 
    Serial.print(angulo_servo_der); 
    Serial.print(","); 
    Serial.println(ang_x);
  }

  delay(10);
}

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