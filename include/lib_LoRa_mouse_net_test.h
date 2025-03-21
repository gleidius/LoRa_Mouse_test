#pragma once
#include<SoftSerial.h>

SoftSerial S_Serial(PB0,PA0,3);

void send_command(String command);
void set_power(int power);
void read_SSerial();
int set_pause(int pause);
void set_rs(int rs);