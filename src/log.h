#pragma once
#include <stumpless.h>

#define EMU_DBG(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_DEBUG, __VA_ARGS__)
#define EMU_INFO(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_INFO, __VA_ARGS__)
#define EMU_WARN(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_WARNING, __VA_ARGS__)
#define EMU_ERR(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_ERR, __VA_ARGS__)
#define EMU_ALERT(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_ALERT, __VA_ARGS__)
#define EMU_EMER(...) stumpless_add_log(log_emu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_EMER, __VA_ARGS__)

#define CPU_DBG(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_DEBUG, __VA_ARGS__)
#define CPU_INFO(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_INFO, __VA_ARGS__)
#define CPU_WARN(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_WARNING, __VA_ARGS__)
#define CPU_ERR(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_ERR, __VA_ARGS__)
#define CPU_ALERT(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_ALERT, __VA_ARGS__)
#define CPU_EMER(...) stumpless_add_log(log_cpu_target, STUMPLESS_FACILITY_USER | STUMPLESS_SEVERITY_EMER, __VA_ARGS__)

extern struct stumpless_target* log_emu_target;
extern struct stumpless_target* log_cpu_target;
