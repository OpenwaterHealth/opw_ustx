#pragma once


#include "main.h"
#include "stm32f4xx_hal.h"


#include "ti7332.hpp"
#include "uart.hpp"
#include "flash.hpp"
#include "fsm.hpp"

extern UART* dbg_uart;
extern TX7332* tx[4];
extern Flash* flash;
extern FSM* fsm;

extern GPIO* led_green;
extern GPIO* led_red;
extern GPIO* trigger;
extern GPIO* cw_en;
extern GPIO* shutz;
extern GPIO* tr_en;
extern GPIO* drive[2];

void BoardInit();
