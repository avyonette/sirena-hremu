#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/window.h>
#include <iniparser/dictionary.h>
#include <iniparser/iniparser.h>
#include <stdio.h>
#include <stdlib.h>
#include "gui.h"
#include "gui/event_log.h"
#include "gui/mem_view.h"
#include "gui/prog_view.h"
#include "gui/registers.h"
#include "vm_ctrl.h"
#include "log.h"

#define CONFIG_FILE_NAME "hremu_gui.ini"

u32 gui_display_width = 0;
u32 gui_display_height = 0;

dictionary* config_dict = NULL;

bool gui_init() {
	FILE* f = fopen(CONFIG_FILE_NAME, "r");

	if(f == NULL) {
		EMU_WARN("couldn't load " CONFIG_FILE_NAME);
		config_dict = dictionary_new(0);
	} else {
		config_dict = iniparser_load_file(f, CONFIG_FILE_NAME);
		fclose(f);
	}

	if(AG_InitCore("hremu", AG_CREATE_DATADIR) == -1) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	if(AG_InitGraphics("sdl2gl") == -1) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	AG_GetDisplaySize(NULL, &gui_display_width, &gui_display_height);

	// the agar console often freezes when writing a lot of messages, nothing we can really do about it
	//if(!gui_event_log_init()) return false;
	if(!gui_mem_view_init()) return false;
	if(!gui_prog_view_init()) return false;
	if(!gui_registers_init()) return false;
	if(!gui_vm_ctrl_init()) return false;

	return true;
}

void gui_run(void (*iter)()) {
	AG_DriverEvent e;
	bool running = true;

	while(running) {
		iter();

		while(AG_PendingEvents(NULL) > 0) {
			if(AG_GetNextEvent(NULL, &e)) {
				switch(e.type) {
					case AG_DRIVER_CLOSE: {
						running = false;
						break;
					}
					default:
						break;
				}

				if(AG_ProcessEvent(NULL, &e) == -1) {
					running = false;
					break;
				}
			}
		}

		AG_GetDisplaySize(NULL, &gui_display_width, &gui_display_height);

		//gui_event_log_process();
		gui_mem_view_process();
		gui_prog_view_process();
		gui_registers_process();
		gui_vm_ctrl_process();

		AG_WindowDrawQueued();
		AG_Delay(1);
	}

	gui_free();
}

void gui_free() {
	//gui_event_log_free();
	gui_mem_view_free();
	gui_prog_view_free();
	gui_registers_free();
	gui_vm_ctrl_free();

	AG_DestroyGraphics();
	AG_Destroy();

	// save config file
	FILE* f = fopen(CONFIG_FILE_NAME, "w+");

	if(f == NULL) {
		EMU_WARN("couldn't open " CONFIG_FILE_NAME " for saving");
	} else {
		iniparser_dump_ini(config_dict, f);
	}

	fclose(f);
	dictionary_del(config_dict);
}

void gui_set_window_defaults(AG_Window* window, const char* title) {
	AG_WindowSetCaption(window, "%s", title);
	AG_WindowSetCloseAction(window, AG_WINDOW_HIDE);
	AG_WindowShow(window);
}

void gui_load_window_config(
	const char* key, AG_Window* window,
	int default_x, int default_y, int default_w, int default_h
) {
	char key_x[32];
	char key_y[32];
	char key_w[32];
	char key_h[32];

	snprintf(key_x, sizeof(key_x), "%s:%s", key, "x");
	snprintf(key_y, sizeof(key_y), "%s:%s", key, "y");
	snprintf(key_w, sizeof(key_w), "%s:%s", key, "w");
	snprintf(key_h, sizeof(key_h), "%s:%s", key, "h");

	int x = iniparser_getint(config_dict, key_x, default_x);
	int y = iniparser_getint(config_dict, key_y, default_y);
	int w = iniparser_getint(config_dict, key_w, default_w);
	int h = iniparser_getint(config_dict, key_h, default_h);

	if(x != -1 && y != -1 && w != -1 && h != -1) {
		AG_WindowSetGeometry(window, x, y, w, h);
	}
}

void gui_save_window_config(const char* key, AG_Window* window) {
	iniparser_set(config_dict, key, NULL); // set section

	char x[16];
	char y[16];
	char w[16];
	char h[16];

	AG_WindowSaveGeometry(window);
	
	snprintf(x, sizeof(x), "%d", window->rSaved.x);
	snprintf(y, sizeof(y), "%d", window->rSaved.y);
	snprintf(w, sizeof(w), "%d", window->rSaved.w);
	snprintf(h, sizeof(h), "%d", window->rSaved.h);

	char key_x[32];
	char key_y[32];
	char key_w[32];
	char key_h[32];

	snprintf(key_x, sizeof(key_x), "%s:%s", key, "x");
	snprintf(key_y, sizeof(key_y), "%s:%s", key, "y");
	snprintf(key_w, sizeof(key_w), "%s:%s", key, "w");
	snprintf(key_h, sizeof(key_h), "%s:%s", key, "h");

	iniparser_set(config_dict, key_x, x);
	iniparser_set(config_dict, key_y, y);
	iniparser_set(config_dict, key_w, w);
	iniparser_set(config_dict, key_h, h);
}
