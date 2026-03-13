#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <Servo.h>

// --- DECLARACIÓN DE MOTORES Y SENSORES ---
MPU6050 sensor;
Servo hombro;
Servo codo; // Controlado por Lazo Cerrado (PID)
Servo mun_der;
Servo mun_izq;
Servo base;
Servo gripper;

// --- VARIABLES DE MEMORIA DE POSICIÓN (Trayectoria) ---
float pos_base = 90.0;
float pos_hombro = 90.0;
float pos_codo = 0; // Este es ahora el SETPOINT del PID
float pos_mun_der = 90.0;
float pos_mun_izq = 90.0;
int ap_gripper = 10;

// --- VARIABLES DEL MPU6050 ---
int ax, ay, az;
int gx, gy, gz;
long f_ax, f_ay, f_az; int p_ax, p_ay, p_az;
long f_gx, f_gy, f_gz; int p_gx, p_gy, p_gz;
int ax_o, ay_o, az_o; int gx_o, gy_o, gz_o;

float ang_x;
float ang_x_prev = 0;

// --- Variables de control ---
float Kp = 0.0935588; 
float Ki = 0.1192379; 
float error_integral = 0.0;
long tiempo_prev_pid = 0;

void setup() {
  Serial.begin(57600); // Velocidad requerida por el MPU
  Wire.begin();
  sensor.initialize();

  // Posiciones iniciales (Home)
  hombro.write(pos_hombro);     
  mun_der.write(pos_mun_der);  
  mun_izq.write(pos_mun_izq);  
  base.write(pos_base);
  gripper.write(ap_gripper);
  // Nota: Al codo no se le hace .write(), lo controla el PID

  // Conexión de pines escalonada
  hombro.attach(6); delay(200);
  codo.attach(7);   delay(200);
  mun_der.attach(4);delay(200);
  mun_izq.attach(5);delay(200);
  base.attach(8);   delay(200);
  gripper.attach(9);delay(200);

  Serial.println("Calibrando MPU6050... No mover la estructura");
  calibrarMPU();

  Serial.println("Iniciando rutina en 3 segundos");
  tiempo_prev_pid = millis(); // Arranca el reloj del PID
  esperarActivo(3000); // Mantiene el brazo firme durante la espera
}

void loop() {
  // Paso 1: Gira la base 
  Serial.println("Moviendo base");
  moverCicloidal(120, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 2. Llevar el brazo antes de su extension maxima
  Serial.println("350 mm.");
  moverCicloidal(pos_base, 55, -20.5, pos_mun_der, pos_mun_izq, 1.5);
  delay(2000);
  Serial.println("aaaaaaaaaaaa");
  moverCicloidal(pos_base, 39, 10, pos_mun_der, pos_mun_izq, 1.5);
  delay(2000);

  // Paso 3. Ajusta codo para recoger la pieza
  Serial.println("Inclinando muñeca hacia abajo...");
  moverCicloidal(pos_base, pos_hombro, 0, pos_mun_der, pos_mun_izq, 1.5);
  delay(1000);
  
  // Paso 4. Baja muneca
  moverCicloidal(pos_base, pos_hombro, pos_codo, 0, 180, 1.5);
  
  // Paso 5. Baja hombro para recoger la pieza
  Serial.println("bbbbbbb");
  moverCicloidal(pos_base, pos_hombro, 12, pos_mun_der, pos_mun_izq, 1.5);
  delay(1000);
  Serial.println("ccccccc");
  moverCicloidal(pos_base, 23, 20, pos_mun_der, pos_mun_izq, 1.5);
  esperarActivo(1500); // Damos tiempo a estabilizarse

  // Paso 6. Cierre de pinza
  Serial.println("Cerrando pinza");
  ap_gripper = 50;
  gripper.write(ap_gripper);
  esperarActivo(1500);

  // Paso 7. Sube brazo tras recoger la pieza
  Serial.println("Subiendo brazo");
  moverCicloidal(pos_base, pos_hombro, 0, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 8. Girar base
  Serial.println("Moviendo base");
  moverCicloidal(45, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 9. Llevar el brazo a posicion de 150mm girando muneca
  Serial.println("150 mm.");
  moverCicloidal(pos_base, 77, -57, 60, 120, 1.5);
  esperarActivo(1500);
  
  // Paso 10. Baja el codo para dejar la pieza
  moverCicloidal(pos_base, pos_hombro, -70, pos_mun_der, pos_mun_izq, 1.5);
  esperarActivo(1000);

  // Paso 11. Apertura de pinza
  Serial.println("Abriendo pinza");
  ap_gripper = 10;
  gripper.write(ap_gripper);
  esperarActivo(1500);

  // Paso 12. Levantar hombro y dejar codo en Home
  Serial.println("Levantando brazo");
  moverCicloidal(pos_base, 77, 0, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 13. Regresar a Home
  Serial.println("Regresando a Home");
  moverCicloidal(pos_base, 90, 0, pos_mun_der, pos_mun_izq, 1.5);
  
  // Paso 14. Girar base a Home
  moverCicloidal(90, pos_hombro, 0, pos_mun_der, pos_mun_izq, 1.5);
  delay(1000);

  // Paso 15. Centrar la muñeca
  Serial.println("Centrando muñecas");
  moverCicloidal(pos_base, pos_hombro, pos_codo, 90, 90, 1.0);
  
  Serial.println("Fin de la rutina");
  esperarActivo(10000); // Mantiene activo el PID durante los 10s de espera
}

// --- FUNCIONES MAESTRAS ---

// 1. ESPERA ACTIVA: Pausa la rutina pero mantiene vivo el PID
void esperarActivo(int milisegundos) {
  unsigned long inicio = millis();
  while (millis() - inicio < (unsigned long)milisegundos) {
    actualizarPID_Codo(pos_codo); 
    delay(10); 
  }
}

// Control PI
void actualizarPID_Codo(float setpoint_mpu) {
  float dt_pid = (millis() - tiempo_prev_pid) / 1000.0;
  tiempo_prev_pid = millis();

  // 1. Lectura del sensor
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  
  float accel_ang_x = atan2(ay, sqrt(pow((float)ax, 2) + pow((float)az, 2))) * (180.0 / 3.14);
  ang_x = 0.98 * (ang_x_prev + (gx / 131.0) * dt_pid) + 0.02 * accel_ang_x;
  
  if (isnan(ang_x)) { ang_x = 0; }
  ang_x_prev = ang_x;

  // 2. Cálculo del error 
  float error = setpoint_mpu - ang_x;
  
  float P = Kp * error;
  
  error_integral = error_integral + (error * dt_pid);
  float I = Ki * error_integral;
  
  // Anti-windup 
  if (I > 30) I = 30;
  else if (I < -30) I = -30;

  // 3. Feedforward
  float feedforward_servo = 90.0 - setpoint_mpu;

  // 4. Calculo de la salida
  float output = feedforward_servo - (P + I);

  // Protección de hardware del servo
  if (output > 180) output = 180;
  else if (output < 0) output = 0;

  codo.write(output); 
}

// 3. GENERADOR DE TRAYECTORIA CICLOIDAL
void moverCicloidal(float obj_base, float obj_hombro, float obj_codo, float obj_mun_der, float obj_mun_izq, float T_segundos) {
  float ini_base = pos_base;
  float ini_hombro = pos_hombro;
  float ini_codo = pos_codo;
  float ini_mun_der = pos_mun_der;
  float ini_mun_izq = pos_mun_izq;

  float dt_traj = 0.02; 
  int pasos = T_segundos / dt_traj; 

  for (int i = 0; i <= pasos; i++) {
    unsigned long t_inicio_paso = millis();

    float t = i * dt_traj;      
    float tau = t / T_segundos; 
    float ciclo = tau - (1.0 / (2.0 * PI)) * sin(2.0 * PI * tau);

    pos_base = ini_base + (obj_base - ini_base) * ciclo;
    pos_hombro = ini_hombro + (obj_hombro - ini_hombro) * ciclo;
    // El codo actualiza su coordenada teórica, pero NO se escribe directo al motor
    pos_codo = ini_codo + (obj_codo - ini_codo) * ciclo; 
    pos_mun_der = ini_mun_der + (obj_mun_der - ini_mun_der) * ciclo;
    pos_mun_izq = ini_mun_izq + (obj_mun_izq - ini_mun_izq) * ciclo;

    base.write(pos_base);
    hombro.write(pos_hombro);
    mun_der.write(pos_mun_der);
    mun_izq.write(pos_mun_izq);

    // Inyectamos la nueva coordenada teórica al Lazo Cerrado
    actualizarPID_Codo(pos_codo);

    // Espera exacta de 20ms
    while(millis() - t_inicio_paso < (dt_traj * 1000)); 
  }

  // Cierre perfecto de la posición final
  pos_base = obj_base; base.write(pos_base);
  pos_hombro = obj_hombro; hombro.write(pos_hombro);
  pos_codo = obj_codo; actualizarPID_Codo(pos_codo);
  pos_mun_der = obj_mun_der; mun_der.write(pos_mun_der);
  pos_mun_izq = obj_mun_izq; mun_izq.write(pos_mun_izq);
}

// 4. CALIBRACIÓN INICIAL DEL MPU
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