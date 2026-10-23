#include <agar/core.h>
#include <agar/gui.h>
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
AG_Box* vm_uart = NULL;
AG_Console* vm_uart_mon = NULL;

void gui_virt_mon_uart_clock_timeout() {
	if(!emu_run) return;

	bool v = gpio_digital_read(UART_RX);
	printf("%d", v);
}

bool gui_virt_mon_init() {
	if((vm_window = AG_WindowNew(0)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	gui_load_window_config("vm", vm_window, -1, -1, -1, -1);

	vm_notebook = AG_NotebookNew(vm_window, AG_NOTEBOOK_EXPAND);
	vm_uart = AG_BoxNew(vm_notebook, AG_BOX_VERT, AG_BOX_EXPAND);
	vm_uart_mon = AG_ConsoleNew(vm_uart, AG_CONSOLE_EXPAND);

	AG_NotebookAddTabHoriz(vm_notebook, "Serial monitor");

	gui_set_window_defaults(vm_window, "Virtual monitor");

	//clk_create(1, 9600, gui_virt_mon_uart_clock_timeout, true);
	return true;
}

void gui_virt_mon_process() {
	//--(nvm?) abusing uart_io_read here because i can't be bothered to implement another clock and everything for the emulator side to read UART
	if(!emu_run) return;

	// for(int i = 0; i < 8; i++) {
	// 	bool v = gpio_digital_read(UART_RX);
	// 	printf("%d", v);
	// }
	// printf("\n");
}

void gui_virt_mon_free() {
	gui_save_window_config("vm", vm_window);
}
