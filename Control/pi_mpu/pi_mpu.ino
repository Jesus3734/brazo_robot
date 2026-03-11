#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <Servo.h>

MPU6050 sensor;
Servo miServo;

// inicialización de variables
int ax, ay, az;
int gx, gy, gz;
long f_ax, f_ay, f_az; int p_ax, p_ay, p_az;
long f_gx, f_gy, f_gz; int p_gx, p_gy, p_gz;
int ax_o, ay_o, az_o; int gx_o, gy_o, gz_o;

float ang_x;
float ang_x_prev = 0;

// ganancias del controlador
float Kp = 0.10255; 
float Ki = 0.75; 

// variables para el cálculo de la salida del controlador
float setpoint = 0.0; 
float input = 0.0;     
float output = 0.0;    

float error = 0.0;
float error_integral = 0.0;
long tiempo_prev = 0;

// configuración inicial del MPU y servo
void setup() {
  Serial.begin(57600);
  Wire.begin();
  sensor.initialize();

  miServo.attach(6);
  miServo.write(0); 
  
  Serial.println("Calibrando MPU6050... No mover");
  calibrarMPU();
  
  Serial.println("Ingresa un ángulo entre 0 y 180:");
  delay(2000);
  
  tiempo_prev = millis();
}

void loop() {
  // lectura de setpoint
  if (Serial.available() > 0) {
    String mensaje = Serial.readStringUntil('\n'); 
    float nuevo_angulo = mensaje.toFloat(); 
    
    if (nuevo_angulo >= 0 && nuevo_angulo <= 180) {
      setpoint = nuevo_angulo;
    }
  }

  // cálculo de tiempo
  float dt = (millis() - tiempo_prev) / 1000.0;
  tiempo_prev = millis();

  // adquisición de datos del MPU
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  
  float accel_ang_x = atan2(ay, sqrt(pow((float)ax, 2) + pow((float)az, 2))) * (180.0 / 3.14);
  ang_x = 0.98 * (ang_x_prev + (gx / 131.0) * dt) + 0.02 * accel_ang_x;
  
  if (isnan(ang_x)) { ang_x = 0; }
  ang_x_prev = ang_x;

  input = ang_x; 

  // ecuación del controlador PI
  error = setpoint - input;
  
  float P = Kp * error;
  
  error_integral = error_integral + (error * dt);
  float I = Ki * error_integral;
  
  // protección matemática anti-windup para limitar el error
  if (I > 180) I = 180;
  else if (I < -180) I = -180;

  // salida del controlador
  output = P + I;

  // protección de saturación para limitar el giro del servo de 0 a 180 grados
  if (output > 180) output = 180;
  else if (output < 0) output = 0;

  // manda el ángulo al servo
  miServo.write(output); 

  // registro de datos en términal
  Serial.print(setpoint); // imprime la entrada o el setpoint al que se desea llegar
  Serial.print(",");
  Serial.print(input); // imprime la lectura del MPU, el valor real en grados
  Serial.print(",");
  Serial.println(output); // imprime lo que envia el controlador

  delay(20); 
}

// calibración del MPU
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
