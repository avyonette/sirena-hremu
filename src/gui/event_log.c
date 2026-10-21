#include "event_log.h"
#include "gui/gui.h"
#include "log.h"
#include <agar/core.h>
#include <agar/gui.h>
#include <stdarg.h>
#include <stdio.h>

AG_Window* el_window = NULL;
AG_Console* el_console = NULL;

int _gui_event_log_fn(FILE* _, const char* fmt, ...) {
	// make string
	static char buf[1024];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	usize n = strlen(buf);

	// remove newline
	if(buf[n - 1] == '\n') {
		buf[n - 1] = '\0';
	}

	AG_ConsoleMsgS(el_console, buf);
	return 0;
}

bool gui_event_log_init() {
	if((el_window = AG_WindowNew(0)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}
	
	gui_load_window_config("el", el_window, gui_display_width - 500, gui_display_height - 320, 500, 320);

	if((el_console = AG_ConsoleNew(el_window, AG_CONSOLE_EXPAND)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	gui_set_window_defaults(el_window, "Event log");

	// redirect log writes to our function
	emu_log_function = _gui_event_log_fn;
	
	return true;
}

void gui_event_log_process() {}

void gui_event_log_free() {
	gui_save_window_config("el", el_window);
}
