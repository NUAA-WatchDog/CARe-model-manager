#pragma once
#include"CPlusPlus.h"
#include"WDSocket.h"
#define MAX_THREAD_SIZE 100

typedef struct task {

	LPVOID (_stdcall*pooltask) (LPVOID lpParam);
	LPVOID lpParam;
	struct task * next;

}task_t;

typedef struct threadpool {

	CRITICAL_SECTION t_mutex;
	CONDITION_VARIABLE t_cond;
	HANDLE *t_ids;
	task_t *first;
	task_t *last;
	int task_cnt;
	int max_threads;
	bool exit;

}threadpool_t;

static	threadpool_t *tpool;

DWORD WINAPI pool_init(int max_threads = MAX_THREAD_SIZE);
DWORD WINAPI pool_thread_loop(LPVOID lpParam);
DWORD WINAPI pool_add_task(LPVOID(_stdcall*pooltask)(LPVOID lpParam), LPVOID lpParam);
DWORD WINAPI pool_destroy();
