#pragma once

#include <cstdint>

#include "circular_buffer.hpp"
#include "stm32f4xx_hal.h"
#include "gpio.hpp"


class UART {
public:
  UART() {}
  ~UART() {}

  void Init(UART_HandleTypeDef* huart, GPIO* rts, int buflen);

  void Transmit(uint8_t* bytes, size_t len);

  uint8_t Receive();
  size_t ReceiveAvailable();

  friend void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
  friend void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart);
private:
  void RxIRQ();
  void TxIRQ();

  UART_HandleTypeDef* huart_;
  GPIO* rts_;
  uint8_t rx_byte_, tx_byte_;
  CircularBuffer<uint8_t> tx_buf_;
  CircularBuffer<uint8_t> rx_buf_;
};
