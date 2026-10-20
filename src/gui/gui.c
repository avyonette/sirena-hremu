#include "gui.h"
#include "gui/event_log.h"
#include "gui/prog_view.h"
#include "gui/registers.h"
#include "vm_ctrl.h"
#include "log.h"
#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/window.h>

u32 gui_display_width = 0;
u32 gui_display_height = 0;

bool gui_init() {
	if(AG_InitCore("hremu", AG_CREATE_DATADIR) == -1) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	if(AG_InitGraphics("sdl2gl") == -1) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	AG_GetDisplaySize(NULL, &gui_display_width, &gui_display_height);

	if(!gui_event_log_init()) return false;
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

		gui_event_log_process();
		gui_prog_view_process();
		gui_registers_process();
		gui_vm_ctrl_process();

		AG_WindowDrawQueued();
		AG_Delay(1);
	}

	gui_free();
}

void gui_free() {
	AG_DestroyGraphics();
	AG_Destroy();
}
