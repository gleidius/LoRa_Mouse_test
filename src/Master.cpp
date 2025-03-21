// to do: для нормальной работы сравнения сделать мвссивы равными длинной предполагаемой длинне сообщения
// это нужно для нормального сравнения полученой стороки с "SUCCESS"

#include<SoftSerial.h>
SoftSerial S_Serial(PB0,PA0,3);

void send_command(String command){                // функиця отправки AT-команды в Е52
  S_Serial.println(command);
  delay(100); 
  while(S_Serial.available()){
    byte buff123 = S_Serial.read();
    Serial.write(buff123);
  }}

void set_power(int power){                        // функция настройки мощности
    String pw = String(power);
    String at = "AT+POWER=";
    String zero = ",0";
    at.concat(pw);
    at.concat(zero);
    S_Serial.println(at);
    Serial.print("Мощность: ");
    String dbm = " дБм";
    pw.concat(dbm);
    Serial.println(pw);
}

void read_SSerial(){                              // функция чтения Soft UART с задержкой
  delay(100);
  while(S_Serial.available()){
    byte buff123 = S_Serial.read();
    Serial.write(buff123);  
}}

int set_pause(int pause){                         // функция установки паузы передачи
  Serial.print("Пауза, мс: ");
  Serial.println(pause);
  int test_delay=pause-300;
  return(test_delay);
  }

void set_rs(int rs){                              // функция изменения параметра скорость/дальность
    String range_speed = String(rs);
    String at = "AT+RATE=";
    at.concat(range_speed);
    S_Serial.println(at);
    Serial.print("Скорость/Дальность = ");
    Serial.println(range_speed);
  }

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
int colvo_pack = 10000;
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

  S_Serial.println("56.45205 84.96131 450 1.5 50 2");
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
  send_command("AT+ROUTER_READ=?");
    flag_router=0;
  }
   
if (digitalRead(PB4)==true){
  S_Serial.println("RESTART");
  Serial.println("RESTART");
  delay(100);
  }  

  if(millis()-start__time>=300000){
    while(true){
    S_Serial.println("ALL PACK END");
  delay(100);
  //Читаем ответ
  while(S_Serial.available()){
    byte buff123 = S_Serial.read();
    //Serial.print(buff123);  
    buff_send[i]= buff123;
    i++;
}   
    }}
  
S_Serial.println("56.45205 84.96131 450 1.5 50 2");
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
   
   
   if (sendd.startsWith("SUCCESS")==true){
    Serial.println("Пакет доставлен"); //
  
    }
    else{
    Serial.println("Пакет НЕ доставлен");
    
      }

   if(digitalRead(PC15)==false){
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