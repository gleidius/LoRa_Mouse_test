#pragma once
#include <HardwareSerial.h>
#include <Arduino.h>
#include<SoftSerial.h>


static uint8_t UART1_TX = PA9;
static uint8_t UART1_RX = PA10;
//static uint8_t UART2_TX = PA2;
//static uint8_t UART2_RX = PA3;
//static HardwareSerial S_Serial(UART2_RX, UART2_TX);
//static SoftSerial S_Serial(UART2_RX, UART2_TX,3);
static HardwareSerial MySerial1(UART1_RX, UART1_TX);

void send_command(String command);

/*
void set_power(int power);
void read_SSerial();
int set_pause(int pause);
void set_rs(int rs);
*/