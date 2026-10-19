#include "event_log.h"
#include "gui/gui.h"
#include "log.h"
#include <agar/core.h>
#include <agar/gui.h>
#include <stdio.h>

AG_Window* el_window = NULL;
AG_Console* el_console = NULL;

long _gui_event_log_stdout_write(void* c, const char* s, size_t n) {
	// remove newline from s
	char* ns = s;
	if(ns[n - 1] == '\n') {
		ns[n - 1] = '\0';
	}

	AG_ConsoleMsgS(el_console, ns);
	return 0;
}

long _gui_event_log_stderr_write(void* c, const char* s, size_t n) {
	// remove newline from s
	char* ns = s;
	if(ns[n - 1] == '\n') {
		ns[n - 1] = '\0';
	}

	AG_ConsoleLine* line = AG_ConsoleMsgS(el_console, ns);

	AG_Color red = {255, 0, 0, 255};
	AG_ConsoleMsgColor(line, &red);

	return 0;
}

bool gui_event_log_init() {
	if((el_window = AG_WindowNew(0)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	AG_WindowSetGeometry(el_window, gui_display_width - 500, gui_display_height - 320, 500, 320);

	if((el_console = AG_ConsoleNew(el_window, AG_CONSOLE_EXPAND)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	AG_WindowSetCaption(el_window, "Event log");
	AG_WindowSetCloseAction(el_window, AG_WINDOW_IGNORE);
	AG_WindowShow(el_window);

	// redirect stdout/err writes to our function
	stdout = fopencookie(NULL, "w", (cookie_io_functions_t) { NULL, _gui_event_log_stdout_write, NULL, NULL });
	stderr = fopencookie(NULL, "w", (cookie_io_functions_t) { NULL, _gui_event_log_stderr_write, NULL, NULL });

	return true;
}

void gui_event_log_process() {}
