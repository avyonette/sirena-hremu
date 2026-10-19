#include <stdio.h>
#include <assert.h>
#include <argtable3.h>
#include "dbg/dbg.h"
#include "emu.h"
#include "gui/gui.h"
#include "log.h"
#include "opts.h"

// struct stumpless_target* log_emu_target;
// struct stumpless_target* log_cpu_target;

char emu_log_buffer[EMU_LOG_BUFFER_SIZE] = {0};

bool opt_dump_regs_after_cycle = false;
bool opt_auto_disassemble = false;
bool opt_headless = false;

struct arg_file* cli_blip;
struct arg_lit* cli_auto_dump_regs;
struct arg_lit* cli_auto_disasm;
struct arg_lit* cli_headless;
struct arg_end* cli_end;

//#define ITERATIONS 50

int main(int argc, char* argv[]) {
	void* argt[] = {
		cli_blip = arg_filen(NULL, "blip", "<file>", 1, 1, "BLIP rom file"),
		cli_auto_dump_regs = arg_litn("r", "dump-regs", 0, 1, "automatically dump registers by the end of each cycle"),
		cli_auto_disasm = arg_litn("d", "auto-disasm", 0, 1, "automatically disassemble the RIP at the beginning of each cycle"),
		cli_headless = arg_litn("h", "headless", 0, 1, "run in headless mode (no gui)"),
		cli_end = arg_end(4)
	};

	// log_emu_target = stumpless_open_stdout_target("emu");
	// log_cpu_target = stumpless_open_stdout_target("cpu");

	if(arg_parse(argc, argv, argt) > 0) {
		printf("Usage:\n");
		arg_print_syntax(stdout, argt, "\n");
		goto exit;
	} else {
		opt_dump_regs_after_cycle = cli_auto_dump_regs->count > 0;
		opt_auto_disassemble = cli_auto_disasm->count > 0;
		opt_headless = cli_headless->count > 0;
	}

	EMU_INFO("initialising emulator");
	emu_init();

	if(!opt_headless) {
		EMU_INFO("initialising GUI");
		
		if(!gui_init()) {
			EMU_ERR("GUI initialisation failed");
			opt_headless = true;
			emu_run = true;
		} else {
			gui_run(emu_iter);
		}
	}

	if(opt_headless) {
	#ifdef ITERATIONS
		for(int i = 0; i < ITERATIONS; i++) {
	#else
		while(true) {
	#endif
			emu_iter();
		}
	}

	EMU_INFO("\nGOODBYE :)");
	EMU_INFO("Total cycles: %lu\n", emu_iter_count);
	dbg_cpu_dump_regs(stdout);

exit: ;
	arg_freetable(argt, sizeof(argt) / sizeof(argt[0]));
	stumpless_free_all();
	return 0;
}
