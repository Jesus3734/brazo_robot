/*
 * =========================================================================================
 * Proyecto: brazo_robot
 * Archivo: rutina_cicloidal.ino
 * Autor: Team Rocket. Equipo 3. 
 * UF: MR3001B
 * Fecha: Marzo 13, 2026
 * 
 * DESCRIPCIÓN:
 * Programa de control cinemático a lazo abierto para un brazo robótico de 5 DOF + Gripper.
 * Ejecuta una rutina autónoma de "Pick and Place".
 * Todo el movimiento fluido del sistema se logra generando trayectorias punto a punto 
 * mediante una función cicloidal. Esto garantiza arranques y frenados suaves, 
 * eliminando el "jerk" (tirones) y protegiendo la integridad mecánica de los servomotores.
 * 
 * MAPA DE CONEXIONES CON ARDUINO UNO:
 * - Base         -> Pin 8
 * - Hombro       -> Pin 6
 * - Codo         -> Pin 7
 * Viendo al brazo de frente al gripper
 * - Muñeca Der   -> Pin 4
 * - Muñeca Izq   -> Pin 5 
 * - Gripper      -> Pin 9
 *
 * ADVERTENCIAS DE HARDWARE:
 * 1. VOLTAJES DE OPERACIÓN: Los servomotres de 150 y 60 kg cm no deben alimentarse con 5V
 * del Arduino. Se requiere una fuente de alimentación externa capaz de entregar hasta 12V
 * y 8V respecticvamente para cada servo, y 3A para su correcto funcionamiento.
 * 2. CIERRE DE GRIPPER: El gripper utiliza un bucle 'for' con pausas de 15ms para 
 * evitar picos de corriente durante el agarre y evitar el reinicio del microcontrolador.
 * 3. EVITAR JERK AL INICIO: Siempre dejar el brazo en una posición cercana a su Home para 
 * proteger a las articulaciones de "jerks" durante el "setup" del programa.
 * =========================================================================================
 */

#include <Servo.h>

// Resto de tu código...

#include <Servo.h>

Servo hombro;
Servo codo;
Servo mun_der;
Servo mun_izq;
Servo base;
Servo gripper;

// ---Variables de posición de Home para cada articulación del robot---
float pos_base = 55.0;
float pos_hombro = 90.0;
float pos_codo = 90.0;
float pos_mun_der = 180.0;
float pos_mun_izq = 0.0; // Por la posición espejeado de los servos del diferencial, ambos inician en valores opuestos.
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

  Serial.println("Iniciando rutina en 5 segundos"); // Tiempo de seguridad antes de inciar la rutina
  delay(5000); 
}

// ---Rutina principal de movimiento para el brazo robot---
void loop() {
  // Paso 1: Gira la base para su posición de PICK
  moverCicloidal(160, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 2);
  delay(500);

  // Paso 2. Llevar el brazo antes de su extension maxima
  moverCicloidal(pos_base, 39, 25, pos_mun_der, pos_mun_izq, 1.5);
  delay(500);
  
  // Paso 3. Ajusta la muñeca para recoger la pieza
  moverCicloidal(pos_base, pos_hombro, pos_codo, 178, 178, 5);
  delay(500);
  
  // Paso 5. Mueve el brazo a su extensión máximo para recoger la pieza
  moverCicloidal(pos_base, 21, pos_codo, pos_mun_der, pos_mun_izq, 1.5);
  delay(2000);

  // Paso 6. Cierre del gripper
  // Este ciclo for es esencial para evitar un pico de corriente al cerrar el gripper
  // Al mover el servo gradualmente nos aseguramos de evitar este pico y tener problemas eléctricos con el brazo
  Serial.println("Cerrando gripper");
      // El grado del servomotor aumenta gradualmente cada 15 ms
      for (int g = 10; g <= 50; g++) {
        gripper.write(g);
        delay(15); 
      }
      ap_gripper = 50; 
      delay(200); 

  // Paso 7. Sube brazo tras recoger la pieza
  moverCicloidal(pos_base, 50, pos_codo, pos_mun_der, pos_mun_izq, 1.5);
  delay(500);

  // Paso 8. Retraer el brazo de manera lenta
  moverCicloidal(pos_base, 77, 115, pos_mun_der, pos_mun_izq, 5);
  delay(1000);

  // Paso 9. Girar base a su posición de PLACE
  moverCicloidal(20, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 2.5);
  delay(500);
  
  // Paso 10. Acomoda la muñeca para dejar la pieza
  moverCicloidal(pos_base, pos_hombro, pos_codo, 150, 30, 2);
  delay(500);

  // Paso 11. Baja el brazo para dejar la pieza
  moverCicloidal(pos_base, pos_hombro, 127, pos_mun_der, pos_mun_izq, 2.5);
  delay(1000);

  // Paso 12. Apertura del gripper
  Serial.println("Abriendo gripper");
      for (int g = 50; g >= 10; g--) {   // OJO: g-- (Restando) y la condición es g >= 10
        gripper.write(g);
        delay(15); 
      }
      ap_gripper = 10; // Variable para guardar la posicion final del gripper
      delay(500);

  // Paso 13. Levanta el brazo tras dejar la pieza
  moverCicloidal(pos_base, 77, 90, pos_mun_der, pos_mun_izq, 2);
  delay(1000);

  // Paso 12. Regresar a Home del hombro
  moverCicloidal(pos_base, 90, pos_codo, pos_mun_der, pos_mun_izq, 1);
  delay(500);
  
  // Paso 13. Girar base a Home
  moverCicloidal(55, pos_hombro, pos_codo, pos_mun_der, pos_mun_izq, 2.5);
  delay(500);

  // Paso 14. Centrar la muñeca en su posición de Home
  moverCicloidal(pos_base, pos_hombro, pos_codo, 180, 0, 2.5);
  delay(1000);
  
  Serial.println("Fin de la rutina");
  delay(5000); 
}

// ---Función cicloidal para el control de velocidad ---
void moverCicloidal(float obj_base, float obj_hombro, float obj_codo, float obj_mun_der, float obj_mun_izq, float T_segundos) {
  
  // Guardamos desde dónde arranca cada articulación
  float ini_base = pos_base;
  float ini_hombro = pos_hombro;
  float ini_codo = pos_codo;
  float ini_mun_der = pos_mun_der;
  float ini_mun_izq = pos_mun_izq;

  // Salto de tiempo de 20 ms para sincronizar con los servos
  float dt = 0.02; 
  int pasos = T_segundos / dt; 

  // Función cicloidal
  for (int i = 0; i <= pasos; i++) {
    float t = i * dt;       // Tiempo transcurrido
    float tau = t / T_segundos; // Tiempo normalizado de 0 a 1 

    float ciclo = tau - (1.0 / (2.0 * PI)) * sin(2.0 * PI * tau);

    pos_base = ini_base + (obj_base - ini_base) * ciclo;
    pos_hombro = ini_hombro + (obj_hombro - ini_hombro) * ciclo;
    pos_codo = ini_codo + (obj_codo - ini_codo) * ciclo;
    pos_mun_der = ini_mun_der + (obj_mun_der - ini_mun_der) * ciclo;
    pos_mun_izq = ini_mun_izq + (obj_mun_izq - ini_mun_izq) * ciclo;

    // Los servos se mueven a las posiciones calculadas por la función
    base.write(pos_base);
    hombro.write(pos_hombro);
    codo.write(pos_codo);
    mun_der.write(pos_mun_der);
    mun_izq.write(pos_mun_izq);

    // 20 ms de delay para calcular la nueva posición
    delay(dt * 1000); 
  }

  // Verficación de que los servos estén en su posición final correcta
  pos_base = obj_base; base.write(pos_base);
  pos_hombro = obj_hombro; hombro.write(pos_hombro);
  pos_codo = obj_codo; codo.write(pos_codo);
  pos_mun_der = obj_mun_der; mun_der.write(pos_mun_der);
  pos_mun_izq = obj_mun_izq; mun_izq.write(pos_mun_izq);
}