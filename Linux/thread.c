#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "thread.h"

#include "../MandelbrotSet.h"


int thread_count = 12;//指定计算线程数

int calculate_progress = 0;//计算进度

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t listen_thread;
pthread_t check_progress_thread;
pthread_t render_thread;

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

void init_other_thread(void (*listen)(void),void (*check_progress)(void),void (*render)(void)){
    int status0 = pthread_create(&listen_thread,NULL,(void *)listen,NULL);//创建监听数据的线程
    int status1 = pthread_create(&check_progress_thread,NULL,(void *)check_progress,NULL);//检测计算进度的线程
    int status2 = pthread_create(&render_thread,NULL,(void *)render, NULL);//负责画面渲染
    debug("listen","status = %d ,thread0_tid = %ld",status0,listen_thread);
    debug("check_progress","status = %d ,thread1_tid = %ld",status1,check_progress_thread);
    debug("render","status = %d ,thread2_tid = %ld",status1,render_thread);
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

void destroy_threads(){
    pthread_cancel(check_progress_thread);
    pthread_cancel(listen_thread);
    pthread_cancel(render_thread);
}

void join_threads(){
    pthread_join(check_progress_thread,NULL);
    pthread_join(listen_thread, NULL);
    pthread_join(render_thread, NULL);
}
