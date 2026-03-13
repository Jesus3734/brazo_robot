#include <Servo.h>

Servo hombro;
Servo codo;
Servo mun_der;
Servo mun_izq;
Servo base;
Servo gripper;

// --- VARIABLES DE MEMORIA DE POSICIÓN ---
float pos_base = 55.0;
float pos_hombro = 90.0;
float pos_codo = 90.0;
float pos_mun_der = 180.0;
float pos_mun_izq = 0.0;
int ap_gripper = 10;

void setup() {
  Serial.begin(9600);

  // Posición de Home al inicio del programa
  hombro.write(pos_hombro);     
  codo.write(pos_codo);       
  mun_der.write(pos_mun_der);    
  mun_izq.write(pos_mun_izq);    
  base.write(pos_base);
  gripper.write(ap_gripper);

  // Conexión con los motores y los pines
  hombro.attach(6); delay(200);
  codo.attach(7); delay(200);
  mun_der.attach(4); delay(200);
  mun_izq.attach(5); delay(200);
  base.attach(8); delay(200);
  gripper.attach(9); delay(200);

  Serial.println("Motores listos. Esperando cámara de MATLAB...");
  delay(1000); 
}

void loop() {
  // 1. ESPERAR INSTRUCCIÓN DE LA CÁMARA
  if (Serial.available() > 0) {
    char figura_detectada = Serial.read();

    // =========================================================
    // CASO 1: ES UN CÍRCULO (Inicia la rutina de trabajo)
    // =========================================================
    if (figura_detectada == 'C') {
      
      // La sintaxis ahora es: moverCicloidal(Base, Hombro, Codo, Mun_Der, Mun_Izq, Tiempo_en_segundos);
  // Nota: Pasamos las variables de posición actual (ej. pos_hombro) a los motores que NO queremos mover en ese paso.

      // Paso 1: Gira la base a 
      Serial.println("Moviendo base");
      moverCicloidal(160, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 2);
      delay(500);

      // Paso 2. Llevar el brazo antes de su extension maxima
      Serial.println("350 mm.");
      moverCicloidal(pos_base, 39, 25, pos_mun_der, pos_mun_izq, 1.5);
      delay(500);
  
  // Paso 4. Baja muneca
      moverCicloidal(pos_base, pos_hombro, pos_codo, 178, 178, 5);
      delay(500);
  
  // Paso 5. Baja hombro para recoger la pieza
      moverCicloidal(pos_base, 21, pos_codo, pos_mun_der, pos_mun_izq, 1.5);
      delay(20000);

  // TODO: Agregar cierre de pinza
      Serial.println("Cierre de pinza suave...");
      // Hacemos que la pinza avance grado por grado
        for (int g = 10; g <= 50; g++) {
          gripper.write(g);
          delay(15); // 15 milisegundos entre cada grado le quita el pico de corriente
        }
        ap_gripper = 50; // Actualizamos la memoria
        delay(200); // Pequeña pausa de estabilización

  // Paso 7. Sube brazo tras recoger la pieza
      Serial.println("Subiendo brazo");
      moverCicloidal(pos_base, 50, pos_codo, pos_mun_der, pos_mun_izq, 1.5);
      delay(500);


  // Paso 9. Llevar el brazo a una posicion antes de su extension mínima de 150mm girando muneca
      Serial.println("150 mm.");
      moverCicloidal(pos_base, 77, 115, pos_mun_der, pos_mun_izq, 5);
      delay(1000);

  // Paso 8. Girar base
      Serial.println("Moviendo base");
      moverCicloidal(20, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 2.5);
      delay(500);
  
      moverCicloidal(pos_base, pos_hombro, pos_codo, 150, 30, 2);
      delay(500);

  // Paso 10. Baja el codo para dejar la pieza
      moverCicloidal(pos_base, pos_hombro, 127, pos_mun_der, pos_mun_izq, 2.5);
      delay(1000);

  //TODO: Agregar apertura de pinza
      Serial.println("Apertura de pinza suave...");
        for (int g = 50; g >= 10; g--) {   // OJO: g-- (Restando) y la condición es g >= 10
          gripper.write(g);
          delay(15); 
        }
        ap_gripper = 10; // Guardamos la posición final real
        delay(500);

  // Paso 11. Levantar hombro y dejar codo en Home
      Serial.println("Levantando brazo");
      moverCicloidal(pos_base, 77, 90, pos_mun_der, pos_mun_izq, 2);
      delay(1000);

  // Paso 12. Regresar a Home
      Serial.println("Regresando a Home");
      moverCicloidal(pos_base, 90, pos_codo, pos_mun_der, pos_mun_izq, 1);
      delay(500);
  
  // Paso 13. Girar base a Home
      moverCicloidal(55, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 2.5);
      delay(500);

  // Paso 14. Centrar la muñeca en su posición de Home
      Serial.println("Centrando muñecas");
      moverCicloidal(pos_base, pos_hombro, pos_codo, 180, 0, 2.5);
      delay(1000);

      // Limpieza del búfer de la cámara (Crítico para que no repita la rutina)
      while (Serial.available() > 0) {
        Serial.read();
      }
      delay(500);
    }
    
    // =========================================================
    // CASO 2: ES UN CUADRADO (Ignora y no se mueve)
    // =========================================================
    else if (figura_detectada == 'S') {
      Serial.println("CUADRADO detectado. Pieza ignorada. Manteniendo Home...");
      
      // Limpieza del búfer para evitar que el puerto se sature de letras 'S'
      while (Serial.available() > 0) {
        Serial.read();
      }
      // Pequeña pausa para no saturar la terminal con el mensaje
      delay(1000); 
    }
  }
}

// --- LA ECUACIÓN MATEMÁTICA EN ACCIÓN ---
void moverCicloidal(float obj_base, float obj_hombro, float obj_codo, float obj_mun_der, float obj_mun_izq, float T_segundos) {
  
  float ini_base = pos_base;
  float ini_hombro = pos_hombro;
  float ini_codo = pos_codo;
  float ini_mun_der = pos_mun_der;
  float ini_mun_izq = pos_mun_izq;

  float dt = 0.02; 
  int pasos = T_segundos / dt; 

  for (int i = 0; i <= pasos; i++) {
    float t = i * dt;       
    float tau = t / T_segundos; 
    float ciclo = tau - (1.0 / (2.0 * PI)) * sin(2.0 * PI * tau);

    pos_base = ini_base + (obj_base - ini_base) * ciclo;
    pos_hombro = ini_hombro + (obj_hombro - ini_hombro) * ciclo;
    pos_codo = ini_codo + (obj_codo - ini_codo) * ciclo;
    pos_mun_der = ini_mun_der + (obj_mun_der - ini_mun_der) * ciclo;
    pos_mun_izq = ini_mun_izq + (obj_mun_izq - ini_mun_izq) * ciclo;

    base.write(pos_base);
    hombro.write(pos_hombro);
    codo.write(pos_codo);
    mun_der.write(pos_mun_der);
    mun_izq.write(pos_mun_izq);

    delay(dt * 1000); 
  }

  pos_base = obj_base; base.write(pos_base);
  pos_hombro = obj_hombro; hombro.write(pos_hombro);
  pos_codo = obj_codo; codo.write(pos_codo);
  pos_mun_der = obj_mun_der; mun_der.write(pos_mun_der);
  pos_mun_izq = obj_mun_izq; mun_izq.write(pos_mun_izq);
}