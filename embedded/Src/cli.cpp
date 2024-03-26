#include "cli.hpp"

#include "board.hpp"

#include <cstdio>
#include <cstring>


static const char usage[] =
    "--------------USTx Commands:-----------------\r\n"
    "f <n>:\r\n"
    "  set to foucs <n>.  will stay on this focus indefinitely\r\n"
    "i <n>\r\n"
    "  set to focus <n>.  will auto increment foci, wrapping around\r\n"
    "  after the last focus\r\n"
    "c <n>\r\n"
    "  set the cycle count to <n>\r\n"
    "t\r\n"
    "  return (OKAY|SHUTDOWN) if the device is in thermal shutdown\r\n"
    "reset\r\n"
    "  reset the device\r\n"
    "r <chip> <address>\r\n"
    "  read from TI <chip> (0-3) at <address>\r\n"
    "w <chip> <address> <data>\r\n"
    "  write to TI <chip> (0-3) with 32-bit <data> at <address>\r\n"
    "rf <address> <n>\r\n"
    "  read <n> bytes from flash <address>\r\n"
    "wf <address> <data>\r\n"
    "  write <data> byte to <address>\r\n"
    "ef <address>\r\n"
    "  erase 64KB sector containing <address>\r\n"
    "help\r\n"
    "  print this message\r\n"
    "you may also send an intel hex file (copy/paste into terminal)\r\n\r\n";



void Usage() {
  for (unsigned int i = 0 ; i < sizeof(usage) ; ++i) dbg_uart->Transmit((uint8_t*)&usage[i], 1);
}


void ParseCMD(const char* s) {
  if (strncmp(s, ":", 1) == 0) {
    if (flash->IntelHex(s)) {
      fsm->LoadFlash();
    }

  } else if (strncmp(s, "reset", 5) == 0) {
    NVIC_SystemReset();

  } else if (strncmp(s, "t", 1) == 0) {
    if (shutz->Get()) {
      printf("OKAY\r\n");
    } else {
      printf("SHUTDOWN\r\n");
    }

  } else if (strncmp(s, "cw ", 3) == 0) {
    if (strcmp(s+3, "on") == 0) {
      fsm->SetCW(true);
      printf("CW Enable\r\n");
    } else {
      fsm->SetCW(false);
      printf("CW Disable\r\n");
    }

  } else if (strncmp(s, "f ", 2) == 0) {
    int focus;
    if (sscanf(s+2, "%d", &focus)) {
      fsm->SetFocus(focus);
      printf("Set focus to %d\r\n", focus);
    } else {
      printf("Usage: f <focus number>\r\n");
    }

  } else if (strncmp(s, "f", 1) == 0) {
    printf("Focus: %d\r\n", fsm->GetFocus());

  } else if (strncmp(s, "i ", 2) == 0) {
    int focus;
    if (sscanf(s+2, "%d", &focus)) {
      fsm->SetFocus(focus, true);
      printf("Set initial focus to %d\r\n", focus);
    } else {
      printf("Usage: i <start focus number>\r\n");
    }

  } else if (strncmp(s, "c ", 2) == 0) {
    int count;
    if (sscanf(s+2, "%d", &count)) {
      for (int chip = 0 ; chip < 4 ; ++chip) tx[chip]->SetRepeat(count);
      printf("Set repeat count to %d\r\n", count);
    } else {
      printf("Usage: c <repeat count>\r\n");
    }

  } else if (strncmp(s, "e ", 2) == 0) {
    printf("Echo: %s\r\n", s+2);

  } else if (strncmp(s, "rf ", 3) == 0) {
    uint32_t addr;
    size_t len = 1;
    uint8_t buf[256];
    if (sscanf(s + 3, "%lx %u", &addr, &len) > 1) {
      flash->Read(addr, buf, len);
      printf("Reading address %lx: ", addr);
      for (unsigned int i = 0 ; i < len ; ++i) printf("%02X ", buf[i]);
      printf("\r\n");
    } else {
      printf("Usage: rf <address> <length>\r\n");
    }

  } else if (strncmp(s, "wf ", 3) == 0) {
    uint32_t addr;
    uint32_t data;
    if (sscanf(s + 3, "%lx %lx", &addr, &data) == 2) {
      printf("Writing address %lx: %02lx\r\n", addr, data);
      flash->Write(addr, (uint8_t*)&data, 1);
    } else {
      printf("Usage: wf <address> <data>\r\n");
    }

  } else if (strncmp(s, "ef ", 3) == 0) {
    uint32_t addr;
    uint32_t data;
    if (sscanf(s + 3, "%lx %lx", &addr, &data) == 1) {
      flash->EraseBlock2(addr);
      printf("Erasing address %lx: %02lx\r\n", addr, data);
    } else {
      printf("Usage: ef <address>\r\n");
    }

  } else if (strncmp(s, "r ", 2) == 0) {
    uint32_t chip, addr;
    if (sscanf(s + 2, "%lu %lx", &chip, &addr) == 2) {
      uint32_t data = tx[chip]->ReadReg(addr);
      printf("Read Chip %ld %lx: %lx\r\n", chip, addr, data);
    } else {
      printf("Usage: r <chip> <address>\r\n");
    }

  } else if (strncmp(s, "w ", 2) == 0) {
    uint32_t chip, addr, data;
    if (sscanf(s + 2, "%lu %lx %lx", &chip, &addr, &data) == 3) {
      tx[chip]->WriteReg(addr, data);
      printf("Write Chip %ld %lx: %lx\r\n", chip, addr, data);
    } else {
      printf("Usage: w <chip> <address> <data>\r\n");
    }
  } else if (s[0] == 'h') {
      Usage();

  } else {
    printf("\r\n");
  }
}


static const char NEWLINE_CHAR = '\n';
static constexpr char CSI[] = {0x1B, 0x5B};
extern "C" int _read(int file, char* data, int len);
extern "C" int _write(int file, const char* data, int len);


static inline void WriteCSI(const char* str) {
  _write(0, CSI, 2);
  for ( ; *str ; ++str) {
    _write(0, str, 1);
  }
}


static inline void WriteCSI(int n, const char *str) {
  char num[3];
  sprintf(num, "%d", n);
  _write(0, CSI, 2);
  for (char* p = &num[0] ; *p ; ++p) {
    _write(0, p, 1);
  }
  for ( ; *str ; ++str) {
    _write(0, str, 1);
  }
}


void ParseCLI() {
  static char buf[80];
  static char* buf_ptr = buf;
  while (dbg_uart->ReceiveAvailable()) {
    // echo to prompt
    uint8_t c = dbg_uart->Receive();

    if (c == 0x7F) {
      --buf_ptr;
      if (buf_ptr < buf) buf_ptr = buf;
      WriteCSI("D");
      WriteCSI("K");
      _write(0, " ", 1);
      WriteCSI("D");
      WriteCSI("K");
      continue;
    }

    if (c == '\r') {
      c = '\n';
    }
    dbg_uart->Transmit(&c, 1);

    // if newline, parse the command
    if (c == '\n') {
      *buf_ptr = 0;
      buf_ptr = buf;
      ParseCMD(buf);
    } else {
      *(buf_ptr++) = c;
    }
  }
}

