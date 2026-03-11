#include <Servo.h>

Servo hombro;
Servo codo;
Servo mun_der;
Servo mun_izq;
Servo base;

void setup() {
  Serial.begin(9600);

  // Posición de Home al inicio del programa
  hombro.write(90);     // Hombro vertical (ajusta a tu vertical real)
  codo.write(90);       // Codo ligeramente flexionado (límite 70)
  mun_der.write(90);    // Muñeca derecha recta
  mun_izq.write(90);    // Muñeca izquierda recta
  base.write(90);

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

  // Tiempo de seguridad-
  Serial.println("Iniciando rutina en 3 segundos");
  delay(3000); 
}

void loop() {
  // Girar base
  Serial.println("Moviendo base");
  base.write(120);
  delay(1500);

  // Paso 1. Llevar el brazo antes de su extension maxima
  Serial.println("350 mm.");
  hombro.write(39);
  codo.write(30);
  delay(1500);

  // Paso 2: Bajar brazo para recoger pieza
  Serial.println("Inclinando muñeca hacia abajo...");
  codo.write(29);
  delay(1500);
  mun_der.write(0);
  mun_izq.write(180);
  delay(1500);
  hombro.write(23);
  delay(1500);

  //Subir brazo
  Serial.println("Subiendo brazo");
  hombro.write(50);
  delay(1500);

  //Girar base
  Serial.println("Moviendo base");
  base.write(45);
  delay(1500);

  // Paso 3: Llevar el brazo a su extension mínima de 150mm
  Serial.println("150 mm.");
  hombro.write(77);
  codo.write(100);
  delay(1500);
  mun_der.write(60);
  mun_izq.write(120);
  delay(1500);
  codo.write(131);
  delay(1500);

  // Paso 5: Levantar brazo
  Serial.println("Levantando brazo");
  hombro.write(77);
  codo.write(90);
  delay(1500);

  // Paso 6: Levantar brazo
  Serial.println("Regresando a Home");
  hombro.write(90);
  codo.write(90);
  delay(1500);
  base.write(90);
  delay(1500);

  // Paso 4: Centrar la muñeca en su posición de Home
  Serial.println("Centrando muñecas");
  mun_der.write(90);
  mun_izq.write(90);
  delay(1000);
  
  // Tiempo largo de seguridad antes de volver a inciar la rutina
  Serial.println("Fin de la rutina");
  delay(10000); 
}