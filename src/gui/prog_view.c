#include <agar/core.h>
#include <agar/gui.h>
#include "prog_view.h"
#include "arch_types.h"
#include "cpu/cu.h"
#include "dbg/dbg.h"
#include "dbg/disasm.h"
#include "emu.h"
#include "isa/op.h"
#include "log.h"

AG_Window* av_window = NULL;
AG_Table* av_prog_table = NULL;

bool gui_prog_view_init() {
	if((av_window = AG_WindowNew(0)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	AG_WindowSetGeometry(av_window, 0, 0, 360 + 20, 520);

	av_prog_table = AG_TableNew(av_window, AG_TABLE_EXPAND | AG_TABLE_NOAUTOSORT);
	AG_TableAddCol(av_prog_table, "address", "100px", NULL);
	AG_TableAddCol(av_prog_table, "data", "100px", NULL);
	AG_TableAddCol(av_prog_table, "program", "160px", NULL);
	AG_TableSetSeparator(av_prog_table, ",");

	AG_WindowSetCaption(av_window, "Program view");
	AG_WindowSetCloseAction(av_window, AG_WINDOW_IGNORE);
	AG_WindowShow(av_window);
	return true;
}

void gui_prog_view_process() {
	// update table
	AG_TableBegin(av_prog_table);
	{
		// from rip - 3 * word to rip + 16 * word (20 addresses)
		word ip = cu_read_reg(REG_RIP);
		word ip_start = ip - 3 * INSTRUCTION_SIZE;
		word ip_end = ip + 16 * INSTRUCTION_SIZE;

		for(word i = ip_start; i < ip_end; i += INSTRUCTION_SIZE) {
			word inst_data = 0;
			dbg_cpu_read32_v(i, &inst_data);

			DebugDasmInstruction inst;
			dbg_disasm_init(&inst);
			dbg_disasm_inst(&inst, inst_data);

			char addr[9];
			sprintf(addr, "%08X", i);

			char data[9];
			sprintf(data, "%08X", inst_data);

			char asm[64];
			sprintf(asm, "%s%s %s%s %s%s %s",
				i == ip ? "==> " : "",
				inst.name,
				inst.param0,
				inst.param1[0] == '\0' ? "" : ",", inst.param1,
				inst.param2[0] == '\0' ? "" : ",", inst.param2
			);

			AG_TableAddRow(av_prog_table, "%s,%s,%s", addr, data, asm);
		}
	}
	AG_TableEnd(av_prog_table);
}
