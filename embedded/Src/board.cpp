#include "board.hpp"

#include <cstdio>

UART* dbg_uart;
Flash* flash;
GPIO* rts;
GPIO* flash_cs;
TX7332* tx[4];
FSM* fsm;

GPIO* led_green;
GPIO* led_red;
GPIO* trigger;
GPIO* cw_en;
GPIO* shutz;
GPIO* tr_en;
GPIO* drive[2];

void BoardInit() {
  dbg_uart = (UART*)malloc(sizeof(UART));
  rts = (GPIO*)malloc(sizeof(GPIO));
  flash_cs = (GPIO*)malloc(sizeof(GPIO));
  led_green = (GPIO*)malloc(sizeof(GPIO));
  led_red = (GPIO*)malloc(sizeof(GPIO));
  trigger = (GPIO*)malloc(sizeof(GPIO));
  cw_en = (GPIO*)malloc(sizeof(GPIO));
  shutz = (GPIO*)malloc(sizeof(GPIO));
  tr_en = (GPIO*)malloc(sizeof(GPIO));
  drive[0] = (GPIO*)malloc(sizeof(GPIO));
  drive[1] = (GPIO*)malloc(sizeof(GPIO));
  tx[0] = (TX7332*)malloc(sizeof(TX7332));
  tx[1] = (TX7332*)malloc(sizeof(TX7332));
  tx[2] = (TX7332*)malloc(sizeof(TX7332));
  tx[3] = (TX7332*)malloc(sizeof(TX7332));
  flash = (Flash*)malloc(sizeof(Flash));
  fsm = (FSM*)malloc(sizeof(FSM));

  led_red->Init(LEDR_GPIO_Port, LEDR_Pin);
  led_green->Init(LEDG_GPIO_Port, LEDG_Pin);
  trigger->Init(TRIG_GPIO_Port, TRIG_Pin);
  cw_en->Init(CW_EN_GPIO_Port, CW_EN_Pin);
  shutz->Init(SHUTZ_GPIO_Port, SHUTZ_Pin);
  tr_en->Init(TR_EN_GPIO_Port, TR_EN_Pin);
  drive[0]->Init(DRIVE0_GPIO_Port, DRIVE0_Pin);
  drive[1]->Init(DRIVE1_GPIO_Port, DRIVE1_Pin);
  rts->Init(UART_RTS_GPIO_Port, UART_RTS_Pin);
  dbg_uart->Init(&huart1, rts, 512);

  flash_cs->Init(FLASH_CS_GPIO_Port, FLASH_CS_Pin);
  flash->Init(&hspi2, flash_cs);

  TX7332::Reset();
  tx[0]->Init(GPIOC, GPIO_PIN_4);
  tx[1]->Init(GPIOC, GPIO_PIN_5);
  tx[2]->Init(GPIOB, GPIO_PIN_0);
  tx[3]->Init(GPIOB, GPIO_PIN_1);


  fsm->Init();
  led_green->Set(false);
}



extern "C" int _write (int fd, const void *buf, size_t count) {
  dbg_uart->Transmit((uint8_t*)buf, count);
  return count;
}
