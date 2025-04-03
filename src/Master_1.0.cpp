#include "lib_LoRa_mouse_net_test.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>
#include <serialEEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
serialEEPROM myEEPROM(0x50, 32768, 64);

// On-Board LEDs
uint8_t LED_PC13 = PC13;

// LoRa LEDs
uint8_t LED_LED1 = PB8;
uint8_t LED_LED2 = PB9;

// I2C
// uint8_t SDA = PB7;
// uint8_t SCL = PB6;

// UART1
// uint8_t UART1_TX = PA9;
// uint8_t UART1_RX = PA10;

// LoRa UART
// uint8_t UART2_TX = PA2;
// uint8_t UART2_RX = PA3;

// SIM868 UART
uint8_t UART3_TX = PB10;
uint8_t UART3_RX = PB11;

// HardwareSerial MySerial1(UART1_RX, UART1_TX);
// HardwareSerial MySerial2(UART2_RX, UART2_TX);
HardwareSerial MySerial3(UART3_RX, UART3_TX);
// On-Board buttons                                                               // КНОПОЧКИ
uint8_t STM_BTN1 = PB0;
uint8_t LORA_PA0 = PB1;
uint8_t LORA_RST = PA6;

// uint8_t STM_SW1 = PC13;                                                        // ПЕРЕКЛЮЧАТЕЛИ
uint8_t STM_SW2 = PC14;
uint8_t STM_SW3 = PC15;
uint8_t STM_SW4 = PA0;
uint8_t STM_SW5 = PA1;
uint8_t STM_SW6 = PA7;

// SIM868 GPIO
uint8_t SIM_SLEEP = PA4;
uint8_t SIM_PWRK = PA5;

uint8_t switches[] = {
    // STM_SW1,
    STM_SW2,
    STM_SW3,
    STM_SW4,
    STM_SW5,
    STM_SW6,
};

const int switchesSize = sizeof(switches) / sizeof(uint8_t);
uint8_t switchesState[switchesSize] = {0};

static uint8_t UART2_TX = PA2;
static uint8_t UART2_RX = PA3;
static HardwareSerial S_Serial(UART2_RX, UART2_TX);

void send_command(String command)
{                                                                       // функиця отправки AT-команды в Е52
  S_Serial.println(command);
  delay(100);
  while (S_Serial.available())
  {
    byte buff123 = S_Serial.read();
    MySerial1.write(buff123);
  }
}

void set_power(int power)
{                                                                           // функция настройки мощности
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

void read_SSerial()
{                                                                             // функция чтения Soft UART с задержкой
  delay(100);
  while (S_Serial.available())
  {
    byte buff123 = S_Serial.read();
    MySerial1.write(buff123);
  }
}

int set_pause(int pause)
{                                                                             // функция установки паузы передачи
  MySerial1.print("Пауза, мс: ");
  MySerial1.println(pause);
  int test_delay = pause;
  return (test_delay);
}

void set_rs(int rs)
{                                                                             // функция изменения параметра скорость/дальность
  String range_speed = String(rs);
  String at = "AT+RATE=";
  at.concat(range_speed);
  S_Serial.println(at);
  MySerial1.print("Скорость/Дальность = ");
  MySerial1.println(range_speed);
}

void setup()
{
  for (int i = 0; i < switchesSize; i++)
  {
    pinMode(switches[i], INPUT_PULLUP);
  }
  // buttons init
  pinMode(STM_BTN1, INPUT);
  pinMode(LORA_PA0, INPUT);
  pinMode(LORA_RST, INPUT);

  MySerial1.begin(115200);
  S_Serial.begin(115200);
  MySerial3.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    MySerial1.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true);
  display.clearDisplay();

  display.println("Setup TX...");
  display.display();
  MySerial1.println("Настройка передатчика...");

  send_command("AT+SRC_ADDR=1,0"); // задаем собственный адрес
  display.println("src_addr_OK");
  display.display();
  send_command("AT+DST_ADDR=5,0"); // задаем целевой адрес
  display.println("dst_addr_OK");
  display.display();
  send_command("AT+POWER=-8,0"); // задаем мощность передатчика
  display.println("power_OK");
  display.display();
  send_command("AT+OPTION=1,0"); // задаем режим передачи (1 - unicast (одноадресная))
  display.println("unicast_OK");
  display.display();
  send_command("AT+RATE=0"); // устанавливаем режим скорость/дальность
  display.println("rate_OK");
  display.display();
  delay(1000);
}

void loop()
{
  int dlit_test = 20000;
  bool connect_s = 0;
  int setup_delay = 1000;     // настраиваем задержку между передачами при настройке
  int test_delay = 600; // настраиваем задержку между передачами при тесте
  int butt_count = 1;
  int switch_count = 0;
  int enc_count = 15;

                                    // =========================== РЕЖИМ НАСТРОЙКИ ===========================
  MySerial1.println("Режим установления сети");
  int flag_router = 0;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("  === MASTER A ===");

  display.print("Power (3), dBm: ");
  int Power_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int Power_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("-8");

  display.print("Pause (2), ms: ");
  int Pause_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int Pause_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("600");

  display.print("S/R (4): ");
  int SR_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int SR_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("0");

  display.print("Mesh: ");
  int Mesh_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int Mesh_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("Discon");

  display.print("Status (5): ");
  int Stat_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int Stat_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("Sett");

  display.print("Send pack: ");
  int Send_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int Send_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("wait start");
  display.display();

  while (true)
  {

    String sendd_settings = "0";
    char buff_send_settings[100] = "not";
    int j = 0;

    S_Serial.println("56.45205 84.96131 450 1.5 50 2"); // отправляем пакет
    delay(300);

    while (S_Serial.available())
    {
      byte buff123 = S_Serial.read();
      MySerial1.write(buff123);
      buff_send_settings[j] = buff123;
      j++;
    }

    delay(setup_delay);

    sendd_settings = String(buff_send_settings);

    if (sendd_settings.startsWith("SUCCESS") == true)
    {                                                                 // выводим на экран информацию о сети
      display.fillRect(Mesh_Xpos, Mesh_Ypos, 128, 8, SSD1306_BLACK);
      display.setCursor(Mesh_Xpos, Mesh_Ypos);
      display.print("Connect");
      display.display();
    }
    else
    {
      display.fillRect(Mesh_Xpos, Mesh_Ypos, 128, 8, SSD1306_BLACK);
      display.setCursor(Mesh_Xpos, Mesh_Ypos);
      display.print("Discon");
      display.display();
    }
     
    /*
    if (digitalRead(STM_SW2) == true)
    {                                                                           // устанавливаем паузы между передачами
      switch_count++;
      if (switch_count == 1)
      {
        test_delay = set_pause(600);

        display.setCursor(Pause_Xpos, Pause_Ypos);
        display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
        display.print("600");
        display.display();
      }
      if (switch_count == 2)
      {
        test_delay = set_pause(1200);

        display.setCursor(Pause_Xpos, Pause_Ypos);
        display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
        display.print("1200");
        display.display();
      }
      if (switch_count == 3)
      {
        test_delay = set_pause(2000);

        display.setCursor(Pause_Xpos, Pause_Ypos);
        display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
        display.print("2000");
        display.display();
      }
      if (switch_count == 4)
      {
        test_delay = set_pause(3000);

        display.setCursor(Pause_Xpos, Pause_Ypos);
        display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
        display.print("3000");
        display.display();
      }
      if (switch_count == 5)
      {
        test_delay = set_pause(5000);
        switch_count = 0;

        display.setCursor(Pause_Xpos, Pause_Ypos);
        display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
        display.print("5000");
        display.display();
      }
    }*/

    if (digitalRead(STM_SW3) == true)
    { // переключаем мощность
      enc_count--;
      set_power(enc_count); // устанавливаем мощность

      display.setCursor(Power_Xpos, Power_Ypos);
      display.fillRect(Power_Xpos, Power_Ypos, 128, 8, SSD1306_BLACK);
      display.print(enc_count);
      display.display();

      if (enc_count == -9)
      {
        enc_count = 16;
      }
      read_SSerial();
      // delay(500);
    }

    if (digitalRead(STM_SW4) == true)
    { // Переключение скорости передачи
      butt_count++;
      if (butt_count == 1)
      {
        setup_delay = 1000;
        set_rs(0);
        MySerial1.println("S/R=0");

        display.setCursor(SR_Xpos, SR_Ypos);
        display.fillRect(SR_Xpos, SR_Ypos, 128, 8, SSD1306_BLACK);
        display.print("0");
        display.display();
      }
      if (butt_count == 2)
      {
        setup_delay = 1000;
        set_rs(1);
        MySerial1.println("S/R=1");

        display.setCursor(SR_Xpos, SR_Ypos);
        display.fillRect(SR_Xpos, SR_Ypos, 128, 8, SSD1306_BLACK);
        display.print("1");
        display.display();
      }
      if (butt_count == 3)
      {
        setup_delay = 3000;
        set_rs(2);
        butt_count = 0;
        MySerial1.println("S/R=2");

        display.setCursor(SR_Xpos, SR_Ypos);
        display.fillRect(SR_Xpos, SR_Ypos, 128, 8, SSD1306_BLACK);
        display.print("2");
        display.display();
      }
      // S_Serial.print("AT+INFO=?");
      // read_SSerial();
    }

                              //          =========================== РЕЖИМ ТЕСТА ============================
    bool sw1 = digitalRead(STM_SW5);
    if (sw1 == true)
    {
      
      int start__time = millis();
      S_Serial.println("START TEST");

      read_SSerial();

      MySerial1.println("Режим проведения теста");

      display.setCursor(Stat_Xpos, Stat_Ypos);
      display.fillRect(Stat_Xpos, Stat_Ypos, 128, 8, SSD1306_BLACK);
      display.print("Testing");
      display.display();
      
      int time_counter=0; //счетчик для определения пауз
      int scet = 0;
      while (true)
      {
        
        int i = 0;
        char buff_send[100] = "not";
        String success = "SUCCESS\n";
        int del_preamb = 0;
        String sendd = "0";
        
        switch (time_counter)
        {
        case 0:
          test_delay=600;
          display.setCursor(Pause_Xpos, Pause_Ypos);
          display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
          display.print("600");
          display.display();
          break;
        case 1:
          test_delay=1200;
          display.setCursor(Pause_Xpos, Pause_Ypos);
          display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
          display.print("1200");
          display.display();
          break;
        case 2:
          test_delay=2000;
          display.setCursor(Pause_Xpos, Pause_Ypos);
          display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
          display.print("2000");
          display.display();
          break;
        case 3:
          test_delay=3000;
          display.setCursor(Pause_Xpos, Pause_Ypos);
          display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
          display.print("3000");
          display.display();
          break;
        case 4:
          test_delay=5000;
          display.setCursor(Pause_Xpos, Pause_Ypos);
          display.fillRect(Pause_Xpos, Pause_Ypos, 128, 8, SSD1306_BLACK);
          display.print("5000");
          display.display();
          break;
        default:
          display.setCursor(Stat_Xpos, Stat_Ypos);
          display.fillRect(Stat_Xpos, Stat_Ypos, 128, 8, SSD1306_BLACK);
          display.print("end");
          display.display();
          while(true){
            delay(1000);
          }
        break;
        }

        if (millis() - start__time >= dlit_test)
        { // устанавливаем длительность теста 
          int end_counter = 0;
          scet=0;
          time_counter++;
          
          while (end_counter<10)
          {
            end_counter++;

            S_Serial.println("ALL PACK END"); // спамим командой окончания пакетов
            delay(500);

          }
          delay(20000);
          start__time = millis();
        }

        S_Serial.println("56.45205 84.96131 450 1.5 50 2"); // отправляем пакет в режиме теста
        //delay(300);
        /*
        while (S_Serial.available())
        {
          byte buff123 = S_Serial.read();
          // MySerial1.print(buff123);
          buff_send[i] = buff123;
          i++;
        }*/

        scet++;
        String text = "Отправлено пакетов:";
        text.concat(String(scet));
        MySerial1.println(text);

        display.setCursor(Send_Xpos, Send_Ypos);
        display.fillRect(Send_Xpos, Send_Ypos, 128, 8, SSD1306_BLACK);
        display.print(scet);
        display.display();

        sendd = String(buff_send);

        if (sendd.startsWith("SUCCESS") == true)
        {                                       // отрабатываем успешную передачу пакета
          MySerial1.println("Пакет доставлен"); //
        }
        else
        {
          MySerial1.println("Пакет НЕ доставлен");
        }

        if (digitalRead(STM_SW5) == false)
        { // прерывание теста при переключении STM_SW5
          MySerial1.println("Тест досрочно завершен");

          display.setCursor(Stat_Xpos, Stat_Ypos);
          display.fillRect(Stat_Xpos, Stat_Ypos, 128, 8, SSD1306_BLACK);
          display.print("interrupt");
          display.display();

          String text = "Отправлено пакетов:";
          text.concat(String(scet));
          MySerial1.println(text);
          delay(100);
          scet++;

          S_Serial.println("TEST END D");
          delay(100);
          // Читаем ответ
          while (S_Serial.available())
          {
            byte buff123 = S_Serial.read();
            MySerial1.write(buff123);
          }
          while (true)
          {
            delay(1000);
          }
        }
        delay(test_delay);
      }
      delay(test_delay);
    }
  }
}