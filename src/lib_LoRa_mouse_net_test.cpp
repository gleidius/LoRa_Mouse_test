#include"lib_LoRa_mouse_net_test.h"

/*
void send_command(String command){                // функиця отправки AT-команды в Е52
    S_Serial.println(command);
    delay(100); 
    while(S_Serial.available()){
      byte buff123 = S_Serial.read();
      MySerial1.write(buff123);
    }}*/
/*
  void set_power(int power){                        // функция настройки мощности
      String pw = String(power);
      String at = "AT+POWER=";
      String zero = ",0";
      at.concat(pw);
      at.concat(zero);
      S_Serial.println(at);
      MySerial1.print("Мощность: ");
      String dbm = " дБм";
      pw.concat(dbm);
      MySerial1.println(pw);
  }
  
  void read_SSerial(){                              // функция чтения Soft UART с задержкой
    delay(100);
    while(S_Serial.available()){
      byte buff123 = S_Serial.read();
      MySerial1.write(buff123);  
  }}
  
  int set_pause(int pause){                         // функция установки паузы передачи
    MySerial1.print("Пауза, мс: ");
    MySerial1.println(pause);
    int test_delay=pause-300;
    return(test_delay);
    }
  
  void set_rs(int rs){                              // функция изменения параметра скорость/дальность
      String range_speed = String(rs);
      String at = "AT+RATE=";
      at.concat(range_speed);
      S_Serial.println(at);
      MySerial1.print("Скорость/Дальность = ");
      MySerial1.println(range_speed);
    }*/