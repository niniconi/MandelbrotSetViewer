/*  
    MandelbrotSet.c
    曼德博集合 实时观测程序
    version 1.3
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MandelbrotSet.h"

#define ACCURACY long double
int width = 90;//指定渲染的像素宽度
int heigth = 90;//指定渲染的像素高度

const unsigned int fps = 60;//画面的帧率

ACCURACY real_width = 3;//计算时的默认宽度
ACCURACY real_heigth = 3;//计算时的默认高度

ACCURACY postionR = 0;//默认的中心点
ACCURACY postionI = 0;//默认的中心点


int iterations_number = 50;//迭代的次数

unsigned int (*buffer)[500][500] = NULL;//存储画面的像素最高[500 x 500]的指针
unsigned int buffer0[500][500];//存储计算的结果
unsigned int buffer1[500][500];//存储计算的结果
char current_buffer  = 0;//用于标记buffer1和buffer0那个的数据是已完成的


struct number{
	ACCURACY c_real;
	ACCURACY c_imag;
	int iteration_count;
	ACCURACY z_real;
	ACCURACY z_imag;
};

unsigned int iteration(struct number *);
void process(int *);
void listen_input(void);
void info(void);
void print_set(void);
void print_info(void);
void check_progress();//检测完成度
void init();

int main(const int count,char **args){
    debug("MandelbrotSet","start");
	args++;
	if(count == 5){
		iterations_number = atoi(*args++);
		if (atoi(*args) > 500 || atoi(*args) <= 0){
			info();return 1;
		}
		width = atoi(*args++);
		if (atoi(*args) > 500 || atoi(*args) <= 0){
			info();return 1;
		}
		heigth = atoi(*args++);
		if (atoi(*args) <= 0 || atoi(*args) > width){
			info();return 1;
		}
		thread_count = atoi(*args++);
		init();
	}else if(count == 1){	
		init();
		return 0;
	}else{
		info();
		return 0;
	}
	return 1;
}

void init(){
	init_other_thread(listen_input,check_progress);//初始化监听数据和计算进度的线程
    clean();
    hidecur();
	while(1){
		debug("sign","--------------------------------DEBUG---------------------------------------------\n");

        restpos();
		if(buffer != NULL){
#ifndef DEBUG
            print_set();
            print_info();
#endif
		}
		pSleep(1000/fps);
	}
}

unsigned int iteration(struct number *num){//每一个复数值的迭代计算
	num->iteration_count++; 
	//(a+bi)^2 + C = a^2 - b^2 + 2abi + C; C = real + imag i
 	ACCURACY temp = num->z_real;
	num->z_real = num->z_real*num->z_real - num->z_imag*num->z_imag + num->c_real;
	num->z_imag = 2 * temp * num->z_imag + num->c_imag;
	//a+bi=======> (a^2 + b^2) < 9
	if((num->z_real*num->z_real + num->z_imag*num->z_imag) < 9){
		if(num->iteration_count == iterations_number){
			return num->iteration_count;
		}else{
			return iteration(num);
		}
	}
	return num->iteration_count;
}


void process(int *args){//将每个点从像素坐标转换为真实对应的复数
	int centerX = width/2;
	int centerY = heigth/2;
	for(int x = (*args)*(width/thread_count);x <= ((*args)+1)*(width/thread_count) - 1;x++){
		for(int y = 0;y < heigth; y++){
			ACCURACY real = (ACCURACY)(x - centerX)*real_width/width+postionR;
			ACCURACY imag = (ACCURACY)(y - centerY)*real_heigth/heigth+postionI;
			struct number num = {real,imag,0,0,0};
			if(current_buffer){
				buffer1[x][y] = iteration(&num);
			}else{
				buffer0[x][y] = iteration(&num);
			}
		}
	}
	// lock
	mutex_thread_lock(LOCK);
	calculate_progress++;//全局变量完成计算的线程的数量自加一

    debug("exit","Thread OVER");

	// unlock
	mutex_thread_lock(UNLOCK);
	exit_thread();
}


#define R (real_width+real_heigth)/60

#define F(x) (x)/60

void listen_input(void){//监听键盘输入
	init_listener();
target:
	while(1){
		ACCURACY temp = real_width+real_heigth;
		char key = listener_get_char();
        debug("input","input key %c",key);
		switch(key){
			case 'w':
				postionI += R;
				break;
			case 'a':
				postionR -= R;
				break;
			case 's':
				postionI -= R;
				break;
			case 'd':
				postionR += R;
				break;
			case 'k':
				real_width += F(temp);
				real_heigth += F(temp);
				break;
			case 'j':
				real_heigth -= F(temp);
				real_width -= F(temp);
				break;
			case 'u':
				iterations_number++;
				break;
			case 'i':
				if(iterations_number > 1){
					iterations_number--;
				}
				break;
			case 'q':
				close_listener();
                clean();
				pSleep(10);
                showcur();
				exit(EXIT_SUCCESS);
				break;
			default:
				goto target;
				break;
		}
	}
}

void info(void){
	printf("version v1.2\n");
	printf("用法：viewer [iteration] [width] [height] [threads]\n");
	printf("\titeration 指定迭代的次数                          \n");
	printf("\twidth height 指定渲染的分辨率                范围 width（0,500] height (0,500]\n");
	printf("\tthreads 指定计算使用的线程数量               范围 (0,width] \n");
}

void print_info(void){
	printf("=====================================================================================\n");
	printf("\e[K calculate_progress =            %d/%d\n",calculate_progress,thread_count);
	printf("\e[K iteration          =            %d\n",iterations_number);
	printf("\e[K center_postion     =            (%Lf,%Lfi)\n",postionR,postionI);
	printf("\e[K real_size          =            [width = %Lf,height = %Lf]\n",real_width,real_heigth);
	printf("=====================================================================================\n");
}

char print_set_lock = 1;//标记当前是否在打印集合的数据
void print_set(void){//渲染集合
	print_set_lock = 0;

    for(int i = heigth/2 - 1;i>=0;i--){
        for(int j = 0;j<width;j++){
            int block_bk = ((*buffer)[j][i*2]);
            int block_bg = ((*buffer)[j][i*2 + 1]);
            int color_bk = (block_bk*0x1000000)/iterations_number;
            int color_bg = (block_bg*0x1000000)/iterations_number;

            printf("\e[48;5;%um",color_bk);
            printf("\e[38;5;%um▀",color_bg);

        }
        putchar('\n');
    }
    printf("\033[0m");
	print_set_lock = 1;
}

void check_progress(){
	init_thread(process);
	while(1){
		if(calculate_progress == thread_count && print_set_lock){
			if(current_buffer){
				buffer = &buffer1;
				current_buffer = 0;
			}else{
				buffer = &buffer0;
				current_buffer = 1;
			}
			calculate_progress = 0;
			init_thread(process);
		}
		pSleep(1000/fps);
	}

}
