#include "stdafx.h"
#include "Thread_Pool.h"


//source insight

DWORD WINAPI pool_init(int max_threads) {

	tpool = new threadpool_t;
	if (InitializeCriticalSectionAndSpinCount(&tpool->t_mutex, 4000) != TRUE) {

		cout << "Initial Critical Section Failed !" << endl;
		exit(0);

	}
	tpool->task_cnt = 0;
	tpool->max_threads = max_threads;
	tpool->first = NULL;
	tpool->last = tpool->first;
	tpool->exit = false;
	tpool->t_ids = new HANDLE[max_threads];
	for (int i = 0; i < max_threads; i++)
	{
		tpool->t_ids[i] = CreateThread(NULL,1024,(LPTHREAD_START_ROUTINE)pool_thread_loop,NULL,NULL,NULL);
	}
	//cout << "task pool init done!" << endl;
}

DWORD WINAPI pool_thread_loop(LPVOID lpParam) {

	while (true)
	{
		EnterCriticalSection(&tpool->t_mutex);
		
		while (tpool->task_cnt == 0 && tpool->exit == false)
		{
			SleepConditionVariableCS(&tpool->t_cond,&tpool->t_mutex,INFINITE);
		}

		if (tpool->exit) {
			LeaveCriticalSection(&tpool->t_mutex);
			ExitThread(1);
		}

		task_t * ttask;

		ttask = tpool->first;

		tpool->first = tpool->first->next;

		tpool->task_cnt--;

		cout << "get a task from pool" << endl;

		LeaveCriticalSection(&tpool->t_mutex);

		(*(ttask->pooltask))(ttask->lpParam);	//start task

		delete ttask->lpParam;

	}

	return 0;

}

DWORD WINAPI pool_add_task(LPVOID(_stdcall*pooltask)(LPVOID lpParam), LPVOID lpParam) {

	task_t *ttask = new task_t;

	sockparam *tParam = new sockparam;

	*tParam = *(sockparam *)lpParam;

	ttask->pooltask = pooltask;
	ttask->lpParam = (LPVOID)tParam;
	ttask->next = NULL;

	EnterCriticalSection(&tpool->t_mutex);

	if (tpool->first == NULL) {
		tpool->first = ttask;
		tpool->last = ttask;
	}
	else
	{
		tpool->last->next = ttask;
		tpool->last = ttask;
	}

	tpool->task_cnt++;

	cout << "add task into pool" << endl;

	WakeConditionVariable(&tpool->t_cond);
	LeaveCriticalSection(&tpool->t_mutex);
	return 0;
}

DWORD WINAPI pool_destroy() {

	tpool->exit = true;

	WakeAllConditionVariable(&tpool->t_cond);

	for (int i = 0; i < tpool->max_threads; i++)
	{
		WaitForSingleObject(tpool->t_ids[i],INFINITE);
	}

	task_t * ftask;

	while (tpool->first)
	{
		ftask = tpool->first;
		tpool->first = tpool->first->next;
		delete ftask;
	}
	return 0;
}
