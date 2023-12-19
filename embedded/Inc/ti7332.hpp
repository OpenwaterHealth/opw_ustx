#pragma once

#include "stm32f4xx_hal.h"


class TX7332 {
public:
 TX7332() {}
 ~TX7332() {}

 void Init(GPIO_TypeDef* cs_port,
           uint16_t cs_pin);

 void WriteReg(uint16_t addr, uint32_t val);

 bool WriteVerify(uint16_t addr, uint32_t val);

 uint32_t ReadReg(uint16_t addr);

 void WriteBulk(uint16_t addr, uint32_t* be_bytes, int len);

 bool WriteVerify(uint16_t addr, uint32_t* be_bytes, int len);

 static void Reset();

 void LoadProfile();

 void SetRepeat(int count);

private:
 GPIO_TypeDef* cs_port_;
 uint16_t cs_pin_;

 void WriteAddr(uint16_t addr);
};
