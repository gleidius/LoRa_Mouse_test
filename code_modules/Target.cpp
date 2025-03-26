#include "lib_LoRa_mouse_net_test.h"

void setup() {
  Serial.begin(115200);
  S_Serial.begin(115200);
  int NUM_KEYS = 2;
  char buff;
  char buff2[NUM_KEYS];

  Serial.println("Настройка приемника");
  send_command("AT+POWER=-9,0");                          // устанавливаем базовую мощность
  send_command("AT+SRC_ADDR=5,0");                        // задаем собственный адрес
  send_command("AT+DST_ADDR=1,0");                        // задаем целевой адрес
  send_command("AT+OPTION=1,0");                          // задаем режим передачи (1 - unicast (одноадресная))
  send_command("AT+RATE=0");                              // устанавливаем параметр скорость/дальность
}

void loop() {

  Serial.println("Гостов к тесту");

  int tx_pack = 0;
  int rx_pack = 0;
  int discon = 0;
  int disconnect_timeout = 15;
  int timeout_tx = 100;
  int butt_count = 1;
  int flag_router = 0;

  while (true) {

    flag_router++;
    if (flag_router == 50) {
      send_command("AT+ROUTER_READ=?");                   // выводим информацию о таблице маршрутизации
      flag_router = 0;
    }

    int del_preamb = 0;
    char buff_send[100] = "not";
    String sendd = "0";
    int i = 0;
    String test = "nothing";

    while (S_Serial.available()) {                        // записываем пакет исключая преамбулу (9 символов)
      byte buff123 = S_Serial.read();
      del_preamb++;
      if (del_preamb >= 9) {
        buff_send[i] = buff123;
        i++;
      }
    }

    sendd = String(buff_send);
    Serial.println(sendd);

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
      read_SSerial();
      delay(500);
    }

    if (sendd == "not") {
      delay(timeout_tx);
      discon++;
      if (discon == disconnect_timeout) {
        discon = 0;
        String text = "Принято пакетов:" ;
        text.concat(String(tx_pack));
        Serial.println(text); //
        Serial.println("Mesh сеть потеряна");
      }

      continue;
    }

    else if (sendd.startsWith("START TEST")) {
      Serial.println("Тест начинается...");
      tx_pack = 0;
      flag_router = 0;
    }

    else if (sendd.startsWith("TEST END D") == true) {
      String text = "Принято пакетов:" ;
      text.concat(String(tx_pack));
      Serial.println(text);
      Serial.println("Тест завершен досрочно");
      while (true) {
      }
    }

    else {
      tx_pack++;
      discon = 0;
      String text = "Принято пакетов:" ;
      text.concat(String(tx_pack));
      Serial.println(text); //
      Serial.println("tftPrintln(Mesh сеть подключена)");

      if (sendd.startsWith("ALL PACK END") == true) {                     // отрабатываем окончание передачи
        String text = "Принято пакетов:" ;
        text.concat(String(tx_pack));
        Serial.println(text); //
        Serial.println("tftPrintln(Тест завершен)");
      }
    }

    delay(timeout_tx);
  }
}