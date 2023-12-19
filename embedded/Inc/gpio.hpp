#pragma once

#include "stm32f4xx_hal.h"

class GPIO {
public:
  GPIO() {}
  ~GPIO() {}

  void Init(GPIO_TypeDef* port, uint16_t pin) {
    port_ = port;
    pin_ = pin;
  }

  void Set(bool val) {
    HAL_GPIO_WritePin(port_, pin_, (GPIO_PinState)val);
  }

  bool Get() {
    return (GPIO_PinState)HAL_GPIO_ReadPin(port_, pin_);
  }

private:
  GPIO_TypeDef* port_;
  uint16_t pin_;
};
