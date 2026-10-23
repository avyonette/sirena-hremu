#include <agar/core.h>
#include <agar/gui.h>
#include "agar/gui/console.h"
#include "hw/clk.h"
#include "emu.h"
#include "hw/gpio.h"
#include "hw/uart.h"
#include "virt_mon.h"
#include "agar/gui/window.h"
#include "gui/gui.h"
#include "log.h"

AG_Window* vm_window = NULL;

AG_Notebook* vm_notebook = NULL;
AG_NotebookTab* vm_tab_uart = NULL;
AG_Console* vm_uart_mon = NULL;

char vm_uart_buffer[4096] = {0};
u32 vm_uart_buffer_i = 0;

bool gui_virt_mon_init() {
	if((vm_window = AG_WindowNew(0)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	gui_load_window_config("vm", vm_window, -1, -1, -1, -1);

	vm_notebook = AG_NotebookNew(vm_window, AG_NOTEBOOK_EXPAND);
	vm_tab_uart = AG_NotebookAddTabHoriz(vm_notebook, "Serial monitor");
	vm_uart_mon = AG_ConsoleNew(vm_tab_uart, AG_CONSOLE_EXPAND);

	gui_set_window_defaults(vm_window, "Virtual monitor");

	return true;
}

void gui_virt_mon_process() {
}

void gui_virt_mon_free() {
	gui_save_window_config("vm", vm_window);
}

void gui_virt_mon_uart_rx(char c) {
	printf("%c", c);

	if(vm_uart_buffer_i >= sizeof(vm_uart_buffer)) {
		memset(vm_uart_buffer, 0, sizeof(vm_uart_buffer));
		vm_uart_buffer_i = 0;
	}

	vm_uart_buffer[vm_uart_buffer_i] = c;
	vm_uart_buffer[++vm_uart_buffer_i] = '\0';

	AG_ConsoleClear(vm_uart_mon);
	AG_ConsoleMsgS(vm_uart_mon, vm_uart_buffer);
}
