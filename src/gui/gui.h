#pragma once
#include "types.h"

extern u32 gui_display_width;
extern u32 gui_display_height;

bool gui_init();
void gui_run(void (*iter)());
void gui_free();
