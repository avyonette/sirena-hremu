#include <agar/core.h>
#include <agar/gui.h>
#include "registers.h"
#include "agar/gui/window.h"
#include "cpu/cu.h"
#include "dbg/disasm.h"
#include "gui/gui.h"
#include "log.h"

#define FMT "%08X"

AG_Window* rg_window = NULL;
AG_Table* rg_reg_table = NULL;

void _gui_registers_reg_table_populate(AG_Event* e) {
	AG_TableBegin(rg_reg_table);

	for(int i = 0; i < MAX_REGISTERS / 2; i++) {
		if(i == REG_RSRV0 || i == REG_RSRV1) continue;

		const char* reg_name0 = ""; 
		char reg_val0[9] = {0};

		if(i != REG_RSRV0 && i != REG_RSRV1) {
			reg_name0 = dbg_registers[i].name;
			sprintf(reg_val0, FMT, cu_read_reg(i));
		}

		int reg_i1 = i + MAX_REGISTERS / 2;
		const char* reg_name1 = "";
		char reg_val1[9] = {0};

		if(reg_i1 != REG_RSRV0 && reg_i1 != REG_RSRV1) {
			reg_name1 = dbg_registers[reg_i1].name;
			sprintf(reg_val1, FMT, cu_read_reg(reg_i1));
		}

		AG_TableAddRow(rg_reg_table, "%s,%s,%s,%s", reg_name0, reg_val0, reg_name1, reg_val1);
	}

	AG_TableEnd(rg_reg_table);
}

bool gui_registers_init() {
	if((rg_window = AG_WindowNew(0)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	AG_WindowSetGeometry(rg_window, 0, 520, 300 + 20, 460);

	rg_reg_table = AG_TableNewPolled(rg_window, AG_TABLE_EXPAND | AG_TABLE_NOAUTOSORT, _gui_registers_reg_table_populate, NULL);
	AG_TableAddCol(rg_reg_table, "name", "50px", NULL);
	AG_TableAddCol(rg_reg_table, "value", "100px", NULL);
	AG_TableAddCol(rg_reg_table, "name", "50px", NULL);
	AG_TableAddCol(rg_reg_table, "value", "100px", NULL);
	AG_TableSetSeparator(rg_reg_table, ",");
	AG_TableSetPollInterval(rg_reg_table, 100);

	AG_WindowSetCaption(rg_window, "CPU registers");
	AG_WindowSetCloseAction(rg_window, AG_WINDOW_IGNORE);
	AG_WindowShow(rg_window);
	return true;
}

void gui_registers_process() {
	_gui_registers_reg_table_populate(NULL);
}
