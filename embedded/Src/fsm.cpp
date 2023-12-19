#include "fsm.hpp"
#include "board.hpp"

#include <cstdio>
#include <cstring>

void FSM::Init() {
  focus_ = 0;
  auto_increment_ = false;
  triggered_ = false;
  cw_mode_ = false;
  cw_en_ = false;
  tr_en->Set(true);
  drive[0]->Set(false);
  drive[1]->Set(false);
  //LoadFlash();
}

void FSM::SetCW(bool mode) {
  cw_mode_ = mode;
  cw_en_ = mode;
  cw_en->Set(cw_en_);
}


bool FSM::LoadFlash() {
  uint32_t v;
  flash->Read(0, (uint8_t*)&v, 4);
#ifdef PRINT_FSM
  printf("Magic: %x\n", v);
#endif

  if (v != 0x12345678) return false;

  flash->Read(4, (uint8_t*)&init_start_, 4);
  flash->Read(8, (uint8_t*)&init_len_, 4);
  flash->Read(12, (uint8_t*)&foci_start_, 4);
  flash->Read(16, (uint8_t*)&foci_len_, 4);
  flash->Read(20, (uint8_t*)&foci_count_, 4);
  uint8_t cw_mode;
  flash->Read(24, &cw_mode, 1);
  SetCW(cw_mode == 0x01);

#ifdef PRINT_FSM
  printf("init_start_: %d\n", init_start_);
  printf("init_len_: %d\n", init_len_);
  printf("foci_start: %d\n", foci_start_);
  printf("foci_len: %d\n", foci_len_);
  printf("foci_count_: %d\n", foci_count_);
#endif

  RegMap(init_start_, init_len_);

  SetFocus(0);
  return true;
}


void FSM::Loop() {
  if (auto_increment_ && triggered_) {
    ++focus_;
    if (focus_ >= foci_count_) focus_ = 0;
    SetFocus(focus_, true);
    triggered_ = false;
  }
  led_red->Set(shutz->Get());
  led_green->Set((HAL_GetTick() % 2000) > 1000);
}


void FSM::Trigger() {
  if (cw_mode_) {
    cw_en_ = !cw_en_;
    cw_en->Set(cw_en_);
    if (cw_en_) triggered_ = true;
  } else {
    triggered_ = true;
  }
}


void FSM::SetFocus(int focus, bool auto_increment) {
  if (focus >= foci_count_) {
    printf("Focus %d is greater than %ld max foci\n", focus, foci_count_);
    return;
  }
  auto_increment_ = auto_increment;
  focus_ = focus;

  RegMap(foci_start_ + foci_len_ * focus, foci_len_);

  for (int chip = 0 ; chip < 4 ; ++chip) {
    tx[chip]->LoadProfile();
  }
}


int FSM::GetFocus() {
  return focus_;
}

void FSM::RegMap(uint32_t start_addr, uint32_t len) {
  uint16_t la[2];
  uint32_t end_addr = start_addr + len;
  uint32_t data[64];
  while (start_addr < end_addr) {
    flash->Read(start_addr, (uint8_t*)&la, 4);
    flash->Read(start_addr + 4, (uint8_t*)data, la[0] * 16);

    for (int chip =  0 ; chip < 4 ; ++chip) {
#ifdef PRINT_FSM
      printf("Write chip %d address %03x: ", chip, la[1]);
      for (int i = 0 ; i < la[0] ; ++i) {
        printf("%08x ",  data[chip * la[0] + i]);
      }
      printf("\n");
#endif
      tx[chip]->WriteBulk(la[1], &data[chip * la[0]], la[0]);
    }
    start_addr += 4 + 16 * la[0];
  }
}


extern "C" void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin) {
  HAL_TIM_Base_Start_IT(&htim1);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  fsm->Trigger();
}
