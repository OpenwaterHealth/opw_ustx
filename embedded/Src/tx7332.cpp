#include "ti7332.hpp"

#include "main.h"

#include <cstdio>


//#define PRINT_TX7332

static const int READ_DIE1 = (1 << 1);
static const int READ_DIE2 = (1 << 2);
static const int LOAD_PROF = (1 << 3);
static const int BURST_WR_EN = (1 << 8);

static const uint32_t& SwapEndian(uint32_t& val) {
  val = (val >> 24) |
        ((val >> 8) & 0xFF00) |
        ((val << 8) & 0xFF0000) |
        ((val << 24));
  return val;
}

static uint32_t SE(uint32_t val) {
  val = (val >> 24) |
        ((val >> 8) & 0xFF00) |
        ((val << 8) & 0xFF0000) |
        ((val << 24));
  return val;
}

static SPI_HandleTypeDef* spi_ = &hspi1;
static GPIO_TypeDef* spi_port_ = GPIOA;
static const uint16_t mosi_pin_ = GPIO_PIN_7;
static const uint16_t miso_pin_ = GPIO_PIN_6;
static const uint16_t sclk_pin_ = GPIO_PIN_5;
static GPIO_TypeDef* reset_port_ = GPIOC;
static const uint16_t reset_pin_ = GPIO_PIN_1;
static GPIO_TypeDef* standby_port_ = GPIOC;
static const uint16_t standby_pin_ = GPIO_PIN_3;

void TX7332::Init(GPIO_TypeDef* cs_port,
                  uint16_t cs_pin) {
  cs_port_ = cs_port;
  cs_pin_ = cs_pin;
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);
}

void TX7332::Reset() {
  HAL_GPIO_WritePin(standby_port_, standby_pin_, GPIO_PIN_RESET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(reset_port_, reset_pin_, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(reset_port_, reset_pin_, GPIO_PIN_RESET);
  HAL_Delay(1);
}


void TX7332::WriteReg(uint16_t addr, uint32_t val) {
#ifdef PRINT_TX7332
  if (cs_pin_ == CS0_L_Pin) {
    printf("Write %03x: %08x\n", addr, val);
  }
#endif
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET);

  WriteAddr(addr);

  SwapEndian(val);
  HAL_SPI_Transmit(spi_, (uint8_t*)&val, 4, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);
}

uint32_t TX7332::ReadReg(uint16_t addr) {
  uint32_t read[2];

  // read chip 0
  WriteReg(0, READ_DIE1);
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET);
  WriteAddr(addr);
  HAL_SPI_Receive(spi_, (uint8_t*)&read[0], 4, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);

  // read chip 1
  WriteReg(0, READ_DIE2);
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET);
  WriteAddr(addr);
  HAL_SPI_Receive(spi_, (uint8_t*)&read[1], 4, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);

  WriteReg(0, 0);

  read[0] = read[0] | read[1];
  return SwapEndian(read[0]);
}

bool TX7332::WriteVerify(uint16_t addr, uint32_t val) {
  WriteReg(addr, val);
  return ReadReg(addr) == val;
}


void TX7332::WriteBulk(uint16_t addr, uint32_t* be_bytes, int len) {
  if (len > 1) WriteReg(0, BURST_WR_EN);
#ifdef PRINT_TX7332
  if (cs_pin_ == CS0_L_Pin) {
    for (int i = 0 ; i < len ; ++i) {
      printf("Write %03x: %08x\n", addr + i, SE(be_bytes[i]));
    }
  }
#endif
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET);
  WriteAddr(addr);
  HAL_SPI_Transmit(spi_, (uint8_t*)be_bytes, 4 * len, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);
}

bool TX7332::WriteVerify(uint16_t addr, uint32_t* be_bytes, int len) {
  WriteBulk(addr, be_bytes, len);
  for (int i = 0 ; i < len ; ++i) {
    uint32_t rd = ReadReg(addr + i);
    if (addr + i == 0x18) rd &= 0xFFFFFFF;
    if (rd != SE(be_bytes[i])) {
      printf("Write %X: expected %lX returned %lX\n", addr + i, SE(be_bytes[i]), rd);
      return false;
    }
  }

  return true;
}


void TX7332::LoadProfile() {
  WriteReg(0, LOAD_PROF);
}


void TX7332::SetRepeat(int count) {
  if (count > 33) {
    printf("Repeat count %d is greater than max 32\n", count);
    return;
  }

  uint32_t reg = ReadReg(0x19);
  reg &= (~((1 << 5) - 1) << 1);
  reg |= count << 1;
  WriteReg(0x19, reg);
}


void TX7332::WriteAddr(uint16_t addr) {
  GPIO_InitTypeDef init = {0};
  init.Pin = sclk_pin_ | mosi_pin_;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(spi_port_, &init);

  for (int i = 0; i < 10 ; ++i) {
    HAL_GPIO_WritePin(spi_port_, sclk_pin_, GPIO_PIN_RESET);
    if (addr & 0x200) {
      HAL_GPIO_WritePin(spi_port_, mosi_pin_, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(spi_port_, mosi_pin_, GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(spi_port_, sclk_pin_, GPIO_PIN_SET);
    addr = addr << 1;
  }
  HAL_GPIO_WritePin(spi_port_, sclk_pin_, GPIO_PIN_RESET);

  init.Pin = mosi_pin_ | sclk_pin_;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(spi_port_, &init);
}
