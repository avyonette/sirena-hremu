#include <agar/core.h>
#include <agar/gui.h>
#include "vm_ctrl.h"
#include "agar/gui/box.h"
#include "agar/gui/window.h"
#include "cpu/cu.h"
#include "emu.h"
#include "gui/gui.h"
#include "log.h"

#define STATE_LABEL_FMT "(%s) RIP: %08X\nIteration %lu"

AG_Window* vc_window = NULL;

AG_Box* vc_box = NULL;
AG_Box* vc_box_btn = NULL;

AG_Label* vc_lbl_state = NULL;
AG_Button* vc_btn_toggle = NULL;
AG_Button* vc_btn_step = NULL;

void _gui_vm_ctrl_btn_toggle(AG_Event* e) {
	if(emu_run) {
		emu_run = false;
		EMU_INFO("VM paused");
	} else {
		emu_run = true;
		EMU_INFO("VM resumed");
	}

	AG_ButtonText(vc_btn_toggle, "%s", emu_run ? "Pause" : "Resume");
}

void _gui_vm_ctrl_btn_step(AG_Event* e) {
	if(emu_run) return;

	word rip_prev = cu_read_reg(REG_RIP);
	emu_run = true;
	emu_iter();
	emu_run = false;

	EMU_INFO("VM step (RIP: %08X->%08X)", rip_prev, cu_read_reg(REG_RIP));
}

bool gui_vm_ctrl_init() {
	if((vc_window = AG_WindowNew(0)) == NULL) {
		EMU_ERR("%s", AG_GetError());
		return false;
	}

	AG_WindowSetMinSize(vc_window, 180, 120);
	gui_load_window_config("vc", vc_window, -1, -1, -1, -1);

	vc_box = AG_BoxNewVert(vc_window, AG_BOX_EXPAND);
	vc_lbl_state = AG_LabelNew(vc_box, 0, STATE_LABEL_FMT, "Paused", 0, 0ul);

	vc_box_btn = AG_BoxNewHoriz(vc_box, AG_BOX_EXPAND);
	vc_btn_toggle = AG_ButtonNewFn(vc_box_btn, 0, "Resume", _gui_vm_ctrl_btn_toggle, NULL);
	vc_btn_step = AG_ButtonNewFn(vc_box_btn, 0, "Step", _gui_vm_ctrl_btn_step, NULL);

	gui_set_window_defaults(vc_window, "VM control");
	return true;
}

void gui_vm_ctrl_process() {
	AG_LabelText(vc_lbl_state, STATE_LABEL_FMT,
		emu_run ? "Running" : "Paused",
		cu_read_reg(REG_RIP),
		emu_iter_count
	);
}

void gui_vm_ctrl_free() {
	gui_save_window_config("vc", vc_window);
}
