#include "lib_LoRa_mouse_net_test.h"

void setup() {
    
  Serial.begin(115200);
  S_Serial.begin(115200);
  //pinMode(PB4, INPUT);                               //кнопка
  //pinMode(PС15, INPUT);                              //переключатель 1
 // pinMode(PС14, INPUT);                              //переключатель 2
 // PB13 кнопка энкодер

  
   Serial.println("Настройка передатчика");

   send_command("AT+SRC_ADDR=1,0");                    // задаем собственный адрес
   send_command("AT+DST_ADDR=5,0");                     // задаем целевой адрес
   send_command("AT+POWER=-9,0");                      // задаем мощность передатчика  
   send_command("AT+OPTION=1,0");                       // задаем режим передачи (1 - unicast (одноадресная))
   send_command("AT+RATE=0");                          // устанавливаем режим скорость/дальность
}

void loop(){
bool connect_s = 0;
int setup_delay = 1000;                                 // настраиваем задержку между передачами при настройке
int test_delay = 300-300;                               // настраиваем задержку между передачами при тесте
int butt_count = 1;
int switch_count =0;
int enc_count = 15;
   
   Serial.println("Режим установления сети");
   int flag_router=0;
while(true){

  flag_router++;
if(flag_router == 5){
   S_Serial.print("AT+ROUTER_READ=?");           // выводим информацию о устройстве (на всякий)
  read_SSerial();
  flag_router=0;
  }

  S_Serial.println("56.45205 84.96131 450 1.5 50 2");     // отправляем пакет
  read_SSerial();
 delay(setup_delay);

 if(digitalRead(PC14)== true){                          // устанавливаем паузы между передачами
      switch_count++;
      if(switch_count==1){
        test_delay = set_pause(300);
      }
      if(switch_count==2){
        set_pause(1000);
      }
      if(switch_count==3){
        set_pause(3000);
      }
      if(switch_count==4){
        set_pause(5000);
        switch_count =0;
      }
      }
      
  if (digitalRead(PA15) == true) {              // переключаем мощность
      enc_count--;
      set_power(enc_count);                        // устанавливаем мощность

      if (enc_count == -9) {
        enc_count = 16;
      }
      read_SSerial();
      delay(500);
    }
        
       if(digitalRead(PB4)== true){                  // Переключение скорости передачи
          butt_count++;
          if(butt_count==1){
          setup_delay = 1000;
          set_rs(0);
          }
       if(butt_count == 2){
          setup_delay = 1000;
          set_rs(1);
          }
       if(butt_count ==3) {
          setup_delay = 3000;
          set_rs(2);
          butt_count = 0;
          }
 read_SSerial();
  }
    

bool sw1 = digitalRead(PC15);
if(sw1 == true){                                      // переходим в режим тестов
delay(1000);
int start__time = millis();
S_Serial.println("START TEST");
  
  read_SSerial();

Serial.println("Режим проведения теста");

  int scet = 0;
  while(true){
     int i =0;
     char buff_send[100]="not";
     String success="SUCCESS\n";
     int del_preamb=0;
     String sendd="0";
   
   flag_router++;
if(flag_router == 5){
  send_command("AT+ROUTER_READ=?");                  // выводим информацию о таблице маршрутизации
    flag_router=0;
  }
   
if (digitalRead(PB4)==true){                         // команда на перезапуск (пока что не работает)
  S_Serial.println("RESTART");
  Serial.println("RESTART");
  delay(100);
  }  

  if(millis()-start__time>=300000){                 // устанавливаем длительность теста равную 5 минутам
    while(true){
    S_Serial.println("ALL PACK END");               // спамим командой окончания пакетов
  delay(100);
  //Читаем ответ
  while(S_Serial.available()){
    byte buff123 = S_Serial.read();
    //Serial.print(buff123);  
    buff_send[i]= buff123;
    i++;
}   
    }}
  
S_Serial.println("56.45205 84.96131 450 1.5 50 2");       // отправляем пакет в режиме теста
  delay(300);
 
  while(S_Serial.available()){
    byte buff123 = S_Serial.read();
    //Serial.print(buff123);  
    buff_send[i]= buff123;
    i++;
}   

   scet++;
   String text = "Отправлено пакетов:" ;
   text.concat(String(scet));
   Serial.println(text); 
   
   sendd = String(buff_send);
   
   
   if (sendd.startsWith("SUCCESS")==true){            // отрабатываем успешную передачу пакета
    Serial.println("Пакет доставлен"); //
  
    }
    else{
    Serial.println("Пакет НЕ доставлен");
    
      }

   if(digitalRead(PC15)==false){                      // прерывание теста при переключении PC15
   Serial.println("Тест досрочно завершен");
   String text = "Отправлено пакетов:" ;
   text.concat(String(scet));
   Serial.println(text); 
   delay(100); 
   scet++;

   S_Serial.println("TEST END D");
  delay(100);
  //Читаем ответ
  while(S_Serial.available()){
    byte buff123 = S_Serial.read();
    Serial.write(buff123); 
    }
    while(true){
      delay(1000);
      }
   
    }
    delay(test_delay);
  }
delay(test_delay);
}
}}