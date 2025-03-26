#pragma once
#include<SoftSerial.h>
#include <HardwareSerial.h>


static uint8_t UART2_TX = PA2;
static uint8_t UART2_RX = PA3;
static HardwareSerial S_Serial(UART2_RX, UART2_TX);


void send_command(String command);
void set_power(int power);
void read_SSerial();
int set_pause(int pause);
void set_rs(int rs);