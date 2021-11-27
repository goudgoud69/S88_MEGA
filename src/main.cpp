#include <Arduino.h>
#include <MEGA_S88.h> // librairie S88 pour le MEGA
void setup() {
  // put your setup code here, to run once:
  S88_Setup(1);      // METTRE 2 OU 3 OU 4 SELON LE NOMBRE DE MODULES UTILISES.
}

void loop() {
  // put your main code here, to run repeatedly:
  S88_Loop();
}