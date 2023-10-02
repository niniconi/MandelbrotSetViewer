#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"

#include "../MandelbrotSet.h"


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

        int policy = SCHED_FIFO;
        struct sched_param sp;
        bzero((void*)&sp, sizeof(sp));

        sp.sched_priority = sched_get_priority_max(policy);

        if (0 == pthread_setschedparam(thread, policy, &sp)) {
            printf("IO Thread #%d using high-priority scheduler!", (int)thread);
        }

        debug("create","index = %d，status = %d，thread_tid = %ld",index,status,thread);
	}
	return 0;
}

void init_other_thread(void (*listen)(void),void (*check_progress)(void)){
	pthread_t thread0;
	int status0 = pthread_create(&thread0,NULL,(void *)listen,NULL);//创建监听数据的线程
	pthread_t thread1;
	int status1 = pthread_create(&thread1,NULL,(void *)check_progress,NULL);//检测计算进度的线程
    debug("listen","status = %d ,thread0_tid = %ld",status0,thread0);
    debug("check_progress","status = %d ,thread1_tid = %ld",status1,thread1);
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
