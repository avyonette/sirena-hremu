#include "gui.h"
#include "log.h"
#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/window.h>

AG_Window* main_window = NULL;

u32 r_cur = 0;
u32 r_nom = 0;

bool gui_init() {
	if(AG_InitCore("hremu", AG_CREATE_DATADIR) == -1) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	if(AG_InitGraphics(NULL) == -1) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	if((main_window = AG_WindowNew(AG_WINDOW_MAIN)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	AG_WindowSetCloseAction(main_window, AG_WINDOW_IGNORE);
	AG_WindowShow(main_window);
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

		AG_WindowDrawQueued();
		AG_Delay(1); 
	}

	gui_free();
}

void gui_free() {
	AG_DestroyGraphics();
	AG_Destroy();
}
