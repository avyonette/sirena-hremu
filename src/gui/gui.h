#pragma once
#include "types.h"

bool gui_init();
void gui_run(void (*iter)());
void gui_free();
