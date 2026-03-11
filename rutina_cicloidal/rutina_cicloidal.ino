#include <Servo.h>

Servo hombro;
Servo codo;
Servo mun_der;
Servo mun_izq;
Servo base;
Servo gripper;

// --- VARIABLES DE MEMORIA DE POSICIÓN ---
// Necesitamos recordar dónde está cada motor para poder aplicar la ecuación
float pos_base = 90.0;
float pos_hombro = 90.0;
float pos_codo = 90.0;
float pos_mun_der = 90.0;
float pos_mun_izq = 90.0;
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
  hombro.attach(6);
  delay(200);
  codo.attach(7);
  delay(200);
  mun_der.attach(4);
  delay(200);
  mun_izq.attach(5);
  delay(200);
  base.attach(8);
  delay(200);
  gripper.attach(9);
  delay(200);

  Serial.println("Iniciando rutina en 3 segundos");
  delay(3000); 
}

void loop() {
  // La sintaxis ahora es: moverCicloidal(Base, Hombro, Codo, Mun_Der, Mun_Izq, Tiempo_en_segundos);
  // Nota: Pasamos las variables de posición actual (ej. pos_hombro) a los motores que NO queremos mover en ese paso.

  // Paso 1: Gira la base a 
  Serial.println("Moviendo base");
  moverCicloidal(120, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 2. Llevar el brazo antes de su extension maxima
  Serial.println("350 mm.");
  moverCicloidal(pos_base, 39, 30, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 3. Ajusta codo para recoger la pieza
  Serial.println("Inclinando muñeca hacia abajo...");
  moverCicloidal(pos_base, pos_hombro, 29, pos_mun_der, pos_mun_izq, 1.5);
  
  // Paso 4. Baja muneca
  moverCicloidal(pos_base, pos_hombro, pos_codo, 0, 180, 1.5);
  
  // Paso 5. Baja hombro para recoger la pieza
  moverCicloidal(pos_base, 23, pos_codo, pos_mun_der, pos_mun_izq, 1.5);
  delay(10000);

  // TODO: Agregar cierre de pinza
  ap_gripper = 50;
  gripper.write(ap_gripper);

  // Paso 7. Sube brazo tras recoger la pieza
  Serial.println("Subiendo brazo");
  moverCicloidal(pos_base, 50, pos_codo, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 8. Girar base
  Serial.println("Moviendo base");
  moverCicloidal(45, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 9. Llevar el brazo a una posicion antes de su extension mínima de 150mm girando muneca
  Serial.println("150 mm.");
  moverCicloidal(pos_base, 77, 100, 60, 120, 1.5);
  delay(1500);
  
  // Paso 10. Baja el codo para dejar la pieza
  moverCicloidal(pos_base, pos_hombro, 131, pos_mun_der, pos_mun_izq, 1.5);
  delay(500);

  //TODO: Agregar apertura de pinza
  ap_gripper = 10;
  gripper.write(ap_gripper);
  delay(1500);

  // Paso 11. Levantar hombro y dejar codo en Home
  Serial.println("Levantando brazo");
  moverCicloidal(pos_base, 77, 90, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 12. Regresar a Home
  Serial.println("Regresando a Home");
  moverCicloidal(pos_base, 90, pos_codo, pos_mun_der, pos_mun_izq, 1.5);
  
  // Paso 13. Girar base a Home
  moverCicloidal(90, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 1.5);

  // Paso 14. Centrar la muñeca en su posición de Home
  Serial.println("Centrando muñecas");
  moverCicloidal(pos_base, pos_hombro, pos_codo, 90, 90, 1.0);
  
  Serial.println("Fin de la rutina");
  delay(10000); 
}

// --- LA ECUACIÓN MATEMÁTICA EN ACCIÓN ---
void moverCicloidal(float obj_base, float obj_hombro, float obj_codo, float obj_mun_der, float obj_mun_izq, float T_segundos) {
  
  // Guardamos desde dónde arranca cada articulación
  float ini_base = pos_base;
  float ini_hombro = pos_hombro;
  float ini_codo = pos_codo;
  float ini_mun_der = pos_mun_der;
  float ini_mun_izq = pos_mun_izq;

  // Configuramos el salto de tiempo (20 milisegundos es ideal para Servos)
  float dt = 0.02; 
  int pasos = T_segundos / dt; 

  // El ciclo que grafica la curva cicloidal
  for (int i = 0; i <= pasos; i++) {
    float t = i * dt;       // Tiempo transcurrido
    float tau = t / T_segundos; // Tiempo normalizado de 0 a 1 (lo que en tu pizarrón es t/T)

    // Calculamos el multiplicador cicloidal en este instante
    float ciclo = tau - (1.0 / (2.0 * PI)) * sin(2.0 * PI * tau);

    // Aplicamos la fórmula a cada motor (Posición Inicial + Diferencia * Ciclo)
    pos_base = ini_base + (obj_base - ini_base) * ciclo;
    pos_hombro = ini_hombro + (obj_hombro - ini_hombro) * ciclo;
    pos_codo = ini_codo + (obj_codo - ini_codo) * ciclo;
    pos_mun_der = ini_mun_der + (obj_mun_der - ini_mun_der) * ciclo;
    pos_mun_izq = ini_mun_izq + (obj_mun_izq - ini_mun_izq) * ciclo;

    // Físicamente movemos los motores a este pequeño incremento
    base.write(pos_base);
    hombro.write(pos_hombro);
    codo.write(pos_codo);
    mun_der.write(pos_mun_der);
    mun_izq.write(pos_mun_izq);

    // Esperamos 20ms antes de calcular el siguiente punto de la curva
    delay(dt * 1000); 
  }

  // Aseguramos que lleguen exactamente al objetivo al terminar para evitar errores de decimales
  pos_base = obj_base; base.write(pos_base);
  pos_hombro = obj_hombro; hombro.write(pos_hombro);
  pos_codo = obj_codo; codo.write(pos_codo);
  pos_mun_der = obj_mun_der; mun_der.write(pos_mun_der);
  pos_mun_izq = obj_mun_izq; mun_izq.write(pos_mun_izq);
}