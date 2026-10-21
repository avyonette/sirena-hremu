#include <agar/core.h>
#include <agar/gui.h>
#include <ctype.h>
#include "mem_view.h"
#include "arch_types.h"
#include "cpu/mmu.h"
#include "gui/gui.h"
#include "log.h"

#define MV_RANGE_MIN 0x0
#define MV_RANGE_MAX 0xFFFFFFFF
#define MV_RANGE_MAX_ROWS 1000

AG_Window* mv_window = NULL;
//AG_Menu* mv_menu_view = NULL;
//AG_MenuItem* mv_menu_view_refresh = NULL;
AG_Table* mv_mem_table = NULL;
AG_Numerical* mv_range_input_base = NULL;
AG_Numerical* mv_range_input_limit = NULL;

u32 mv_range_base = PMM0_BLIP_BASE;
u32 mv_range_limit = PMM0_BLIP_BASE + 0x1000;

bool gui_mem_view_init() {
	if((mv_window = AG_WindowNew(0)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	gui_load_window_config("mv", mv_window, 360 + 20, 0, 460 + 20, 520);

	mv_mem_table = AG_TableNew(mv_window, AG_TABLE_EXPAND | AG_TABLE_NOAUTOSORT);
	AG_TableAddCol(mv_mem_table, "address", "100px", NULL);
	AG_TableAddCol(mv_mem_table, "data", "200px", NULL);
	AG_TableAddCol(mv_mem_table, "ascii", "160px", NULL);
	AG_TableSetSeparator(mv_mem_table, ",");

	mv_range_input_base = AG_NumericalNewUintR(mv_window, 0, "", "Base", &mv_range_base, MV_RANGE_MIN, MV_RANGE_MAX);
	mv_range_input_limit = AG_NumericalNewUintR(mv_window, 0, "", "Limit", &mv_range_limit, MV_RANGE_MIN, MV_RANGE_MAX);

	gui_set_window_defaults(mv_window, "Memory inspector");
	return true;
}

void gui_mem_view_process() {
	if(mv_range_limit <= mv_range_base) return;
	if((mv_range_limit - mv_range_base) / 8 > MV_RANGE_MAX_ROWS) {
		return;
	}
	
	// update table
	AG_TableBegin(mv_mem_table);
	{
		for(int i = mv_range_base; i < mv_range_limit; i += 8) {
			char address[9];
			sprintf(address, "%08X", i);

			byte mem_read[8];

			for(int j = 0; j < sizeof(mem_read); j++) {
				byte* mem_ptr = g2h_phy_addr(i + j);
				
				if(mem_ptr == NULL) {
					mem_read[j] = 0;
				} else {
					mem_read[j] = *mem_ptr;
				}
			}

			char data[128];
			sprintf(
				data, "%02X %02X %02X %02X %02X %02X %02X %02X",
				mem_read[0], mem_read[1], mem_read[2], mem_read[3],
				mem_read[4], mem_read[5], mem_read[6], mem_read[7]
			);

			char ascii[128];
			sprintf(
				ascii, "%c%c%c%c%c%c%c%c",
				isprint(mem_read[0]) > 0 ? mem_read[0] : '.',
				isprint(mem_read[1]) > 0 ? mem_read[1] : '.',
				isprint(mem_read[2]) > 0 ? mem_read[2] : '.',
				isprint(mem_read[3]) > 0 ? mem_read[3] : '.',
				isprint(mem_read[4]) > 0 ? mem_read[4] : '.',
				isprint(mem_read[5]) > 0 ? mem_read[5] : '.',
				isprint(mem_read[6]) > 0 ? mem_read[6] : '.',
				isprint(mem_read[7]) > 0 ? mem_read[7] : '.'
			);

			AG_TableAddRow(mv_mem_table, "%s,%s,%s", address, data, ascii);
		}
	}
	AG_TableEnd(mv_mem_table);
}

void gui_mem_view_free() {
	gui_save_window_config("mv", mv_window);
}
