#pragma once

#include <cstdint>

#include "circular_buffer.hpp"
#include "stm32f4xx_hal.h"
#include "gpio.hpp"

class Flash {
public:
  Flash() {}
  ~Flash() {}

  void Init(SPI_HandleTypeDef* hspi, GPIO* chip_select);
  void EraseBlock2(uint32_t addr);
  void Erase();
  void Write(uint32_t addr, uint8_t* data, size_t len);
  void Read(uint32_t addr, uint8_t* data, size_t len);
  bool IntelHex(const char* s);

private:
  void WriteEnable();
  void WaitBusy();

  SPI_HandleTypeDef* hspi_;
  GPIO* cs_;
  uint32_t addr_;
  bool block0_;
  uint32_t page_addr_;
  uint8_t pp_[256];
};
