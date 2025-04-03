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

    display.println("Setup Noode...");
    display.display();
    MySerial1.println("Настройка узла...");
  

  send_command("AT+POWER=0,0");              // задаем изначальную мощность
  send_command("AT+SRC_ADDR=2,0");            // задаем собственный адрес (!!!Лучше использовать 2-3-4!!!(1 - master, 5 - target))
  send_command("AT+DST_ADDR=232,0");          // задаем целевой адрес
  send_command("AT+OPTION=1,0");              // задаем режим передачи (1 - unicast (одноадресная))
  send_command("AT+RATE=0");                  // задаем базовый режим скорость/дальность

}

void loop() {

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("    === NOODE ===");

  display.print("Power (3), dBm: ");
  int Power_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int Power_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("0");

  display.print("S/R (4): ");
  int SR_Xpos = display.getCursorX(); // позиция Х курсора при написании мощности
  int SR_Ypos = display.getCursorY(); // позиция Y курсора при написании мощности
  display.println("0");
  display.display();

  int butt_count = 1;
  int switch_count = 15;
  while (true) {

    if (digitalRead(STM_SW3) == true) {              // переключаем мощность
      switch_count--;
      set_power(switch_count);                        // устанавливаем мощность
      display.setCursor(Power_Xpos, Power_Ypos);
      display.fillRect(Power_Xpos, Power_Ypos, 128, 8, SSD1306_BLACK);
      display.print(switch_count);
      display.display();
      if (switch_count == -9) {
        switch_count = 16;
      }

      read_SSerial();
      delay(500);
    }

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
  }
}