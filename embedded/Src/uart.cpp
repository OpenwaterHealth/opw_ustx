#include "uart.hpp"
#include "board.hpp"
#include "cmsis_gcc.h"
#include "main.h"
#include <cstdio>
#include <cstring>

static UART* the_uart;

void UART::Init(UART_HandleTypeDef* huart, GPIO* rts, int buflen) {
  the_uart = this;
  huart_ = huart;
  rts_ = rts;
  tx_buf_.Init(buflen);
  rx_buf_.Init(buflen);
  HAL_UART_Receive_IT(huart_, &rx_byte_, 1);
  rts_->Set(false);
}

void UART::Transmit(uint8_t* bytes, size_t len) {
  while (tx_buf_.PushAvailable() < len) {
    if (huart_->gState == HAL_UART_STATE_READY) {
      __disable_irq();
      TxIRQ();
      __enable_irq();
    }
  }

  for (unsigned int i = 0 ; i < len ; ++i) {
    tx_buf_.Push(bytes[i]);
  }

  if (huart_->gState == HAL_UART_STATE_READY) {
    __disable_irq();
    TxIRQ();
    __enable_irq();
  }
}


uint8_t UART::Receive() {
  if (rx_buf_.PushAvailable() > 120) rts_->Set(false);
  return rx_buf_.Pop();
}


size_t UART::ReceiveAvailable() {
  return rx_buf_.PopAvailable();
}

void UART::RxIRQ() {
  if (rx_buf_.PushAvailable() < 80) rts_->Set(true);
  rx_buf_.Push(rx_byte_);
  HAL_UART_Receive_IT(huart_, &rx_byte_, 1);
}

void UART::TxIRQ() {
  if (tx_buf_.PopAvailable()) {
    tx_byte_ = tx_buf_.Pop();
    HAL_UART_Transmit_IT(huart_, &tx_byte_, 1);
  }
}

extern "C" void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
  led_red->Set(false);
  while (1) {}
}


extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  the_uart->RxIRQ();
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
  the_uart->TxIRQ();
}
