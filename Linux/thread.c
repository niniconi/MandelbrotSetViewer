#pragma once
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define LOCK 0
#define UNLOCK 1

int thread_count = 12;//指定计算线程数

int calculate_progress = 0;//计算进度

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int args[500];

void pSleep(unsigned int time){
	usleep(time*1000);
}

int init_thread(void (*process)(int*)){//创建计算线程
	for(int index = 0;index < thread_count;index++)	{
		args[index] = index;
		pthread_t thread;
		int status = pthread_create(&thread,NULL,(void *)process,(void *)&args[index]);
#ifdef DEBUG
		printf("[DEBUG][%s]:index = %d，status = %d，thread_tid = %ld\n",__FUNCTION__,index,status,thread);
#endif
	}
	return 0;
}

void init_other_thread(void (*listen)(void),void (*check_progress)(void)){
	pthread_t thread0;
	int status0 = pthread_create(&thread0,NULL,(void *)listen,NULL);//创建监听数据的线程
	pthread_t thread1;
	int status1 = pthread_create(&thread1,NULL,(void *)check_progress,NULL);//检测计算进度的线程
	#ifdef DEBUG
		printf("[DEBUG][%s][listen]:status = %d ,thread0_tid = %ld\n",__FUNCTION__,status0,thread0);
		printf("[DEBUG][%s][check_progress]:status = %d ,thread1_tid = %ld\n",__FUNCTION__,status,thread1);
	#endif
}

void mutex_thread_lock(int option){
	switch(option){
		case LOCK:
			if(pthread_mutex_lock(&mutex) != 0){
				perror("pthread_mutex_lock");
				exit(EXIT_FAILURE);
			}
			break;
		case UNLOCK:
			if(pthread_mutex_unlock(&mutex) != 0){
				perror("pthread_mutex_unlock");
				exit(EXIT_FAILURE);
			}
			break;
		default:
			perror("unkown_lock_option");
			exit(EXIT_FAILURE);
			break;
	}
}

void exit_thread(){
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}
