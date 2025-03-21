#include "lib_LoRa_mouse_net_test.h"

void setup() {

  Serial.begin(115200);
  S_Serial.begin(115200);

  send_command("AT+POWER=15,0");              // задаем изначальную мощность
  send_command("AT+SRC_ADDR=2,0");            // задаем собственный адрес (!!!Лучше использовать 2-3-4!!!(1 - master, 5 - target))
  send_command("AT+DST_ADDR=232,0");          // задаем целевой адрес
  send_command("AT+OPTION=1,0");              // задаем режим передачи (1 - unicast (одноадресная))
  send_command("AT+RATE=0");                  // задаем базовый режим скорость/дальность

}

void loop() {
  int butt_count = 1;
  int switch_count = 15;
  while (true) {

    if (digitalRead(PC14) == true) {              // переключаем мощность
      switch_count--;
      set_power(switch_count);                        // устанавливаем мощность

      if (switch_count == -9) {
        switch_count = 16;
      }

      delay(100);
      while (S_Serial.available()) {
        byte buff123 = S_Serial.read();
        Serial.write(buff123);
      }
      delay(500);
    }

    if (digitalRead(PB4) == true) {               // Переключение скорости передачи
      butt_count++;

      if (butt_count == 1) {
        set_rs(0);
      }
      if (butt_count == 2) {
        set_rs(1);
      }
      if (butt_count == 3) {
        set_rs(2);
        butt_count = 0;
      }
      //Читаем ответ
      while (S_Serial.available()) {
        byte buff123 = S_Serial.read();
        Serial.write(buff123);
      }
      delay(500);
    }
  }
}