#pragma once
#include <stumpless.h>
#include <stdio.h>

#define EMU_LOG_BUFFER_SIZE 1024
extern char emu_log_buffer[EMU_LOG_BUFFER_SIZE];

#define EMU_LOG(f_out, level, ...) \
	{\
		sprintf(emu_log_buffer, __VA_ARGS__);\
		fprintf(f_out, "%s %s\n", level, emu_log_buffer);\
		fflush(f_out);\
	}

#define EMU_DBG(...) EMU_LOG(stdout, "(emu/dbg)", __VA_ARGS__)
#define EMU_INFO(...) EMU_LOG(stdout, "(emu/info)", __VA_ARGS__)
#define EMU_WARN(...) EMU_LOG(stdout, "(emu/warn)", __VA_ARGS__)
#define EMU_ALERT(...) EMU_LOG(stdout, "(emu/alert)", __VA_ARGS__)
#define EMU_ERR(...) EMU_LOG(stderr, "(emu/err)", __VA_ARGS__)
#define EMU_EMER(...) EMU_LOG(stderr, "(emu/emer)", __VA_ARGS__)

#define CPU_DBG(...) EMU_LOG(stdout, "(cpu/dbg)", __VA_ARGS__)
#define CPU_INFO(...) EMU_LOG(stdout, "(cpu/info)", __VA_ARGS__)
#define CPU_WARN(...) EMU_LOG(stdout, "(cpu/warn)", __VA_ARGS__)
#define CPU_ALERT(...) EMU_LOG(stdout, "(cpu/alert)", __VA_ARGS__)
#define CPU_ERR(...) EMU_LOG(stderr, "(cpu/err)", __VA_ARGS__)
#define CPU_EMER(...) EMU_LOG(stderr, "(cpu/emer)", __VA_ARGS__)

// #define EMU_DBG(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_DEBUG, __VA_ARGS__)
// #define EMU_INFO(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_INFO, __VA_ARGS__)
// #define EMU_WARN(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_WARNING, __VA_ARGS__)
// #define EMU_ERR(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_ERR, __VA_ARGS__)
// #define EMU_ALERT(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_ALERT, __VA_ARGS__)
// #define EMU_EMER(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_EMER, __VA_ARGS__)

// #define CPU_DBG(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_DEBUG, __VA_ARGS__)
// #define CPU_INFO(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_INFO, __VA_ARGS__)
// #define CPU_WARN(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_WARNING, __VA_ARGS__)
// #define CPU_ERR(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_ERR, __VA_ARGS__)
// #define CPU_ALERT(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_ALERT, __VA_ARGS__)
// #define CPU_EMER(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_EMER, __VA_ARGS__)
