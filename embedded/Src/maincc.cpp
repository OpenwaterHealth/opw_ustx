#include "maincc.h"

#include "board.hpp"
#include "cli.hpp"
#include "main.h"
#include "ti7332.hpp"
#include "uart.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>


extern "C" void MainCC() {
  BoardInit();
  Usage();

  while (1) {
    fsm->Loop();
    ParseCLI();
  }
}
