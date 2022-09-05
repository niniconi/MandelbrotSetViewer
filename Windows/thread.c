#pragma once
#include <process.h>
#include <windows.h>

#define LOCK 0
#define UNLOCK 1

int thread_count = 12;//指定计算线程数

int calculate_progress = 0;//计算进度

int args[500];

HANDLE mutex;

void pSleep(DWORD time){
	Sleep(time);
}

int init_thread(void (*process)(int*)){
	mutex = CreateMutex(NULL, 0, "mutex");
	for(int index = 0;index < thread_count;index++)	{
		args[index] = index;
		uintptr_t thread = _beginthread(process,0,(void *)&args[index]);
#ifdef DEBUG
		printf("[DEBUG][%s]:index = %d，thread_tid = %ld\n",__FUNCTION__,index,thread);
#endif
	}
	return 0;

}
void init_other_thread(void (*listen)(void),void(*check_progress)(void)){
	uintptr_t thread0 = _beginthread(listen,0,NULL);
	uintptr_t thread1 = _beginthread(check_progress,0,NULL);
#ifdef DEBUG
		printf("[DEBUG][%s][listen]:thread0_tid = %ld\n",__FUNCTION__,thread0);
		printf("[DEBUG][%s][check_progress]:thread1_tid = %ld\n",__FUNCTION__,thread1);
#endif

}

void mutex_thread_lock(int option){
	switch(option){
		case LOCK:
			WaitForSingleObject(mutex, INFINITE);
			break;
		case UNLOCK:
			ReleaseMutex(mutex);
			break;
		default:
			perror("unkown_lock_option");
			exit(EXIT_FAILURE);
			break;
	}
}

void exit_thread(){
	
}
