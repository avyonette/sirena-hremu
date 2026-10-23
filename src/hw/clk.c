#include <bits/types/sigevent_t.h>
#include <pthread.h>
#include <signal.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include "clk.h"
#include "log.h"

typedef struct ClockTickerArgs {
	timer_t id;
	pthread_t th;
	bool paused;
	u64 interval_ns;
	ClockCallbackFn callback;
} ClockInfo;

#define MAX_CLOCKS 2
//pthread_t clk_threads[MAX_CLOCKS] = {0};
ClockInfo clk_clocks[MAX_CLOCKS] = {0};

// TODO this intervaling shit sucks and never works because of inaccuracies or some shit (pit.c as well)
// it can't even be called "good enough" because it won't work for shit that needs a steady clock like UART
// void* clk_ticker(void* ptr) {
// 	usize id = (usize) ptr;
// 	ClockInfo* args = &clk_clocks[id];

// 	struct timespec t_last = {0};

// 	while(args->state) {
// 		if(args->paused) continue;

// 		struct timespec t_now = {0};
// 		clock_gettime(CLOCK_MONOTONIC, &t_now);

// 		if(t_now.tv_nsec - t_last.tv_nsec >= args->interval_ns) {
// 			args->callback();
// 		}

// 		clock_gettime(CLOCK_MONOTONIC, &t_last);
// 	}

// 	return NULL;
// }

bool clk_signal_defined = false;

void clk_timeout_handler(int sig) {
	ClockInfo* info = &clk_clocks[sig == SIGUSR1 ? 0 : 1];
	if(info->paused) return;
	info->callback();
	//EMU_INFO("click clack");
}

void* clk_ticker(void* ptr) {
	usize id = (usize) ptr;
	// ClockInfo* args = &clk_clocks[id];
	
	signal(id == 0 ? SIGUSR1 : SIGUSR2, clk_timeout_handler);

	while(true) {
		//__asm__ volatile("hlt");
	}

	return NULL;
}

void clk_create(usize id, usize frequency_hz, ClockCallbackFn callback, bool initial_state) {
	assert(id < MAX_CLOCKS);

	// if(!clk_signal_defined) {
	// 	signal(SIGUSR1, clk_timeout_handler);
	// 	signal(SIGUSR2, clk_timeout_handler);
	// 	clk_signal_defined = true;
	// }
	
	if(clk_clocks[id].id != 0) {
		clk_destroy(id);
	}

	pthread_t clock_thread;
	pthread_create(&clock_thread, NULL, clk_ticker, (void*) id);

	struct sigevent sev = {
		.sigev_notify = SIGEV_THREAD_ID,
		.sigev_signo = id == 0 ? SIGUSR1 : SIGUSR2,
		._sigev_un._tid = pthread_gettid_np(clock_thread)
	};

	timer_t timer_id;
	timer_create(CLOCK_MONOTONIC, &sev, &timer_id);

	ClockInfo info = {
		.id = timer_id,
		.th = 0,
		.paused = initial_state,
		.interval_ns = 1e9 / frequency_hz,
		.callback = callback
	};

	struct itimerspec timer_value = {
		.it_value = { .tv_sec = 1, .tv_nsec = 0 },
		.it_interval = { .tv_sec = 0, .tv_nsec = info.interval_ns }
	};
	timer_settime(info.id, 0, &timer_value, NULL);

	clk_clocks[id] = info;
}

void clk_destroy(usize id) {
	timer_delete(clk_clocks[id].id);
	clk_clocks[id].id = 0;
}

void clk_pause(usize id) {
	clk_clocks[id].paused = true;
}

void clk_resume(usize id) {
	clk_clocks[id].paused = false;
}
