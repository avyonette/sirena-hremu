#pragma once
#include <agar/core.h>
#include <agar/gui/window.h>
#include "types.h"

extern u32 gui_display_width;
extern u32 gui_display_height;

bool gui_init();
void gui_run(void (*iter)());
void gui_free();

void gui_set_window_defaults(AG_Window* window, const char* title);
void gui_load_window_config(
	const char* key, AG_Window* window,
	int default_x, int default_y, int default_w, int default_h
);
void gui_save_window_config(const char* key, AG_Window* window);
