#include "flash.hpp"

#include <cstdio>
#include <cstring>

static const uint32_t& SwapEndian(uint32_t& val) {
  val = (val >> 24) |
        ((val >> 8) & 0xFF00) |
        ((val << 8) & 0xFF0000) |
        ((val << 24));
  return val;
}

void Flash::Init(SPI_HandleTypeDef* hspi, GPIO* chip_select) {
  hspi_ = hspi;
  cs_ = chip_select;
  cs_->Set(true);
  addr_ = 0;
  block0_ = true;
  memset(pp_, 0xFF, 255);
  page_addr_ = 0;
}


void Flash::Erase() {
  WriteEnable();
  cs_->Set(false);
  HAL_SPI_Transmit(hspi_, (uint8_t*)"\xC7", 1, HAL_MAX_DELAY);
  cs_->Set(true);
  WaitBusy();
}


void Flash::EraseBlock2(uint32_t addr) {
  SwapEndian(addr);
  addr |= 0xD8;
  WriteEnable();
  cs_->Set(false);
  HAL_SPI_Transmit(hspi_, (uint8_t*)&addr, 4, HAL_MAX_DELAY);
  cs_->Set(true);
  WaitBusy();
}


void Flash::Write(uint32_t addr, uint8_t* data, size_t len) {
  SwapEndian(addr);
  addr |= 0x02;
  WriteEnable();
  cs_->Set(false);
  HAL_SPI_Transmit(hspi_, (uint8_t*)&addr, 4, HAL_MAX_DELAY);
  HAL_SPI_Transmit(hspi_, data, len, HAL_MAX_DELAY);
  cs_->Set(true);
  WaitBusy();
}


void Flash::Read(uint32_t addr, uint8_t *data, size_t len) {
  SwapEndian(addr);
  addr |= 0x03;
  cs_->Set(false);
  HAL_SPI_Transmit(hspi_, (uint8_t*)&addr, 4, HAL_MAX_DELAY);
  HAL_SPI_Receive(hspi_, data, len, HAL_MAX_DELAY);
  cs_->Set(true);
}


bool Flash::IntelHex(const char* s) {
  uint32_t len;
  uint32_t addr;
  uint32_t type;
  s += 1;
  if (sscanf(s, "%2lx%4lx%2lx", &len, &addr, &type) == 3) {
    s += 8;
    switch (type) {

      // Data record
      case 0: {
        addr = addr_ | addr;
        if ((addr & (uint32_t)0xFFFF00) > page_addr_) {
          Write(page_addr_, pp_, 256);
          page_addr_ = addr & 0xFFFF00;
          memset(pp_, 0xFF, 256);
        }
        uint32_t read;
        for (uint32_t i = (addr & 0xFF) ; i < ((addr & 0xFF) + len) ; ++i) {
          sscanf(s, "%2lx", &read);
          pp_[i] = read;
          s += 2;
        }
        if (block0_ && addr == 0) {
          EraseBlock2(0);
          block0_ = false;
        }

      } break;

      // EOF
      case 1: {
        addr_ = 0;
        block0_ = true;
        Write(page_addr_, pp_, 256);
        page_addr_ = 0;
        memset(pp_, 0xFF, 256);
        return true;
      } break;

      // extended address
      case 4: {
        sscanf(s, "%4lx", &addr_);
        addr_ = addr_ << 16;
        if (addr_ == 0) block0_ = false;
        EraseBlock2(addr_);
      } break;

      default: {
        printf("Unsupported record type\n");
      } break;
    }
  } else {
    printf("Parse error\n");
  }

  return false;
}


void Flash::WriteEnable() {
  cs_->Set(false);
  HAL_SPI_Transmit(hspi_, (uint8_t*)"\x06", 1, HAL_MAX_DELAY);
  cs_->Set(true);
}


void Flash::WaitBusy() {
  cs_->Set(false);
  HAL_SPI_Transmit(hspi_, (uint8_t*)"\x05", 1, HAL_MAX_DELAY);
  uint8_t busy;
  do {
    HAL_SPI_Receive(hspi_, &busy, 1, HAL_MAX_DELAY);
  } while (busy & 1);
  cs_->Set(true);
}
