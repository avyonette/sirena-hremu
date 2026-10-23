#pragma once
#include "types.h"

bool gui_virt_mon_init();
void gui_virt_mon_process();
void gui_virt_mon_free();

void gui_virt_mon_uart_rx(char c);
