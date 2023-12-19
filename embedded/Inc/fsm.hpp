#pragma once

#include <cstdint>

class FSM {
public:
  FSM() {}
  ~FSM() {}

  void Init();
  bool LoadFlash();

  void SetFocus(int focus, bool auto_increment = false);

  void SetCW(bool mode);

  void Trigger();

  void Loop();

  int GetFocus();

private:
  void RegMap(uint32_t start_addr, uint32_t len);
  uint32_t init_start_;
  uint32_t init_len_;
  uint32_t foci_start_;
  uint32_t foci_len_;
  uint32_t foci_count_;

  uint32_t focus_;
  bool auto_increment_;
  bool cw_mode_;
  bool cw_en_;

  volatile bool triggered_;
};
