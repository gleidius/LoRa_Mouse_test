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
//uint8_t UART1_TX = PA9;
//uint8_t UART1_RX = PA10;

// LoRa UART
//uint8_t UART2_TX = PA2;
//uint8_t UART2_RX = PA3;

// SIM868 UART
uint8_t UART3_TX = PB10;
uint8_t UART3_RX = PB11;

//HardwareSerial MySerial1(UART1_RX, UART1_TX);
//HardwareSerial MySerial2(UART2_RX, UART2_TX);
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

  void send_command(String command){                // функиця отправки AT-команды в Е52
    S_Serial.println(command);
    delay(100); 
    while(S_Serial.available()){
      byte buff123 = S_Serial.read();
      MySerial1.write(buff123);
    }}

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
    }


void setup() {

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
    for (;;);
  }
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true);
  display.clearDisplay();

    display.println("Setup RX...");
    display.display();
    MySerial1.println("Настройка приемника...");

  int NUM_KEYS = 2;
  char buff;
  char buff2[NUM_KEYS];

  MySerial1.println("Настройка приемника");
  send_command("AT+POWER=10,0");                          // устанавливаем базовую мощность
  send_command("AT+SRC_ADDR=5,0");                        // задаем собственный адрес
  send_command("AT+DST_ADDR=1,0");                        // задаем целевой адрес
  send_command("AT+OPTION=1,0");                          // задаем режим передачи (1 - unicast (одноадресная))
  send_command("AT+RATE=0");                              // устанавливаем параметр скорость/дальность
}

void loop() {

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("    === MASTER ===");

  display.print("Power (3), dBm: ");
  int Power_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int Power_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("10");

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

  display.print("Received: ");
  int RX_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int RX_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("0");
  display.display();

  MySerial1.println("Гостов к тесту");

  int tx_pack = 0;
  int rx_pack = 0;
  int discon = 0;
  int disconnect_timeout = 15;
  int timeout_tx = 100;
  int butt_count = 1;
  int flag_router = 0;

  while (true) {
/*
    flag_router++;
    if (flag_router == 50) {
      S_Serial.print("AT+ROUTER_READ=?");           // выводим информацию о устройстве (на всякий)
      read_SSerial();
      flag_router=0;                // выводим информацию о таблице маршрутизации
     
    }*/

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
    MySerial1.println(sendd);

    if (digitalRead(STM_SW4) == true) {               // Переключение скорости передачи
      butt_count++;
      if (butt_count == 1) {
        set_rs(0);

        display.setCursor(SR_Xpos, SR_Ypos);
        display.fillRect(SR_Xpos, SR_Ypos, 128, 8, SSD1306_BLACK);
        display.print("0");
        display.display();
      }
      if (butt_count == 2) {
        set_rs(1);

        display.setCursor(SR_Xpos, SR_Ypos);
        display.fillRect(SR_Xpos, SR_Ypos, 128, 8, SSD1306_BLACK);
        display.print("1");
        display.display();
      }
      if (butt_count == 3) {
        set_rs(2);

        display.setCursor(SR_Xpos, SR_Ypos);
        display.fillRect(SR_Xpos, SR_Ypos, 128, 8, SSD1306_BLACK);
        display.print("2");
        display.display();
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
        MySerial1.println(text); //
        MySerial1.println("Mesh сеть потеряна");

      display.fillRect(Mesh_Xpos, Mesh_Ypos, 128, 8, SSD1306_BLACK);
      display.setCursor(Mesh_Xpos, Mesh_Ypos);
      display.print("Discon");
      display.display();
      }

      continue;
    }

    else if (sendd.startsWith("START TEST")) {
      MySerial1.println("Тест начинается...");
      tx_pack = 0;
      flag_router = 0;

      display.setCursor(Stat_Xpos, Stat_Ypos);
      display.fillRect(Stat_Xpos, Stat_Ypos, 128, 8, SSD1306_BLACK);
      display.print("Test");
      display.display();
    }

    else if (sendd.startsWith("TEST END D") == true) {
      String text = "Принято пакетов:" ;
      text.concat(String(tx_pack));
      MySerial1.println(text);
      MySerial1.println("Тест завершен досрочно");

      display.setCursor(Stat_Xpos, Stat_Ypos);
      display.fillRect(Stat_Xpos, Stat_Ypos, 128, 8, SSD1306_BLACK);
      display.print("Interrupt");
      display.display();
      while (true) {
      }
    }

    else {
      tx_pack++;
      discon = 0;
      String text = "Принято пакетов:" ;
      text.concat(String(tx_pack));
      MySerial1.println(text); //
      MySerial1.println("tftPrintln(Mesh сеть подключена)");

      display.fillRect(Mesh_Xpos, Mesh_Ypos, 128, 8, SSD1306_BLACK);
      display.setCursor(Mesh_Xpos, Mesh_Ypos);
      display.print("Connect");
      display.display();

      display.setCursor(RX_Xpos, RX_Ypos);
      display.fillRect(RX_Xpos, RX_Ypos, 128, 8, SSD1306_BLACK);
      display.print(tx_pack);
      display.display();

      if (sendd.startsWith("ALL PACK END") == true) {                     // отрабатываем окончание передачи
        String text = "Принято пакетов:" ;
        text.concat(String(tx_pack));
        MySerial1.println(text); //
        MySerial1.println("tftPrintln(Тест завершен)");

        display.setCursor(Stat_Xpos, Stat_Ypos);
        display.fillRect(Stat_Xpos, Stat_Ypos, 128, 8, SSD1306_BLACK);
        display.print("end");
        display.display();

        while (true) {
        }
      }
    }

    delay(timeout_tx);
  }
}