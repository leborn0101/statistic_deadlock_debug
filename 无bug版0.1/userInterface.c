#include <acoreos.h>
#include <stdio.h>
#include <stdlib.h>
#include <spinLockNdLib.h>
#include "instrument_design.h"

void delay(UINT32 del){
	ACoreOs_task_wake_after(del);
}
#define HANDLEP 0
#define HANDLEV 1
#define LES_TASKNUMS 4
ACoreOs_id task_id[LES_TASKNUMS];
UINT32 task_stack[LES_TASKNUMS][ACOREOS_MINIMUM_STACK_SIZE * 4];

ACoreOs_id semaphore_mutex1;
ACoreOs_id semaphore_mutex2;
ACoreOs_id semaphore_mutex3;
ACoreOs_id semaphore_mutex4;
ACoreOs_id semaphore_mutex5;
ACoreOs_id semaphore_mutex6;

UINT32 delay_time;

#define LES_EXECTIMES 500


void semaphore_init(){
	ACoreOs_semaphore_create("semaphore_mutex1", 1, ACOREOS_MUTEX_SEMAPHORE, 20, &semaphore_mutex1);
	ACoreOs_semaphore_create("semaphore_mutex2", 1, ACOREOS_MUTEX_SEMAPHORE, 20, &semaphore_mutex2);
	ACoreOs_semaphore_create("semaphore_mutex3", 1, ACOREOS_MUTEX_SEMAPHORE, 20, &semaphore_mutex3);
	ACoreOs_semaphore_create("semaphore_mutex4", 1, ACOREOS_MUTEX_SEMAPHORE, 20, &semaphore_mutex4);
	ACoreOs_semaphore_create("semaphore_mutex5", 1, ACOREOS_MUTEX_SEMAPHORE, 20, &semaphore_mutex5);
	ACoreOs_semaphore_create("semaphore_mutex6", 1, ACOREOS_MUTEX_SEMAPHORE, 20, &semaphore_mutex6);
}

void task_1(const void* a){
	DLK_INIT();
	const char* msg = (const char*)a;
	int cnt = 0;
	while(cnt<LES_EXECTIMES){
		DLK_CMPSITE(semaphore_mutex1,HANDLEP);
		ACoreOs_semaphore_obtain(semaphore_mutex1, ACOREOS_WAIT, ACOREOS_FOREVER);
		DLK_UPTSITE();
		//delay(delay_time);
		DLK_CMPSITE(semaphore_mutex2,HANDLEP);
		ACoreOs_semaphore_obtain(semaphore_mutex2, ACOREOS_WAIT, ACOREOS_FOREVER);
		DLK_UPTSITE();
		printk_spinLock("in turns of %d task_1 do something!!!\n", cnt);
		DLK_CMPSITE(semaphore_mutex2,HANDLEV);
		ACoreOs_semaphore_release(semaphore_mutex2);
		DLK_UPTSITE();
		DLK_CMPSITE(semaphore_mutex1,HANDLEV);
		ACoreOs_semaphore_release(semaphore_mutex1);
		DLK_UPTSITE();
		delay(100);
		cnt++;

	}
}


void task_2(const void* a){
	DLK_INIT();
	const char* msg = (const char*)a;
	int cnt = 0;
	while(cnt<LES_EXECTIMES){
		DLK_CMPSITE(semaphore_mutex2,HANDLEP);
		ACoreOs_semaphore_obtain(semaphore_mutex2, ACOREOS_WAIT, ACOREOS_FOREVER);
		DLK_UPTSITE();
		//delay(delay_time);
		DLK_CMPSITE(semaphore_mutex1,HANDLEP);
		ACoreOs_semaphore_obtain(semaphore_mutex1, ACOREOS_WAIT, ACOREOS_FOREVER);
		DLK_UPTSITE();
		printk_spinLock("in turns of %d task_2 do something!!!\n", cnt);
		DLK_CMPSITE(semaphore_mutex1,HANDLEV);
		ACoreOs_semaphore_release(semaphore_mutex1);
		DLK_UPTSITE();
		DLK_CMPSITE(semaphore_mutex2,HANDLEV);
		ACoreOs_semaphore_release(semaphore_mutex2);
		DLK_UPTSITE();

		delay(100);
		cnt++;

	}

}

void task_3(const void* a){
	DLK_INIT();
	const char* msg = (const char*)a;
	int cnt = 0;
	while(cnt<LES_EXECTIMES){
		DLK_CMPSITE(semaphore_mutex3，HANDLEP);
		ACoreOs_semaphore_obtain(semaphore_mutex3, ACOREOS_WAIT, ACOREOS_FOREVER);
		DLK_UPTSITE();
		//delay(delay_time);
		DLK_CMPSITE(semaphore_mutex4，HANDLEP);
		ACoreOs_semaphore_obtain(semaphore_mutex4, ACOREOS_WAIT, ACOREOS_FOREVER);
		DLK_UPTSITE();
		printk_spinLock("in turns of %d task_3 do something!!!\n", cnt);
		DLK_CMPSITE(semaphore_mutex4,HANDLEV);
		ACoreOs_semaphore_release(semaphore_mutex4);
		DLK_UPTSITE();
		DLK_CMPSITE(semaphore_mutex3,HANDLEV);
		ACoreOs_semaphore_release(semaphore_mutex3);
		DLK_UPTSITE();
		delay(100);
		cnt++;

	}

}

void task_4(const void* a){
	DLK_INIT();
	const char* msg = (const char*)a;
	int cnt = 0;
	while(cnt<LES_EXECTIMES){
		DLK_CMPSITE(semaphore_mutex3，HANDLEP);
		ACoreOs_semaphore_obtain(semaphore_mutex3, ACOREOS_WAIT, ACOREOS_FOREVER);
		DLK_UPTSITE();
		//delay(delay_time);
		DLK_CMPSITE(semaphore_mutex4，HANDLEP);
		ACoreOs_semaphore_obtain(semaphore_mutex4, ACOREOS_WAIT, ACOREOS_FOREVER);
		DLK_UPTSITE();
		printk_spinLock("in turns of %d task_4 do something!!!\n", cnt);
		DLK_CMPSITE(semaphore_mutex4,HANDLEV);
		ACoreOs_semaphore_release(semaphore_mutex4);
		DLK_UPTSITE();
		DLK_CMPSITE(semaphore_mutex3,HANDLEV);
		ACoreOs_semaphore_release(semaphore_mutex3);
		DLK_UPTSITE();
		delay(100);
		cnt++;

	}
}


const char *g_msg[LES_TASKNUMS] = {
		"task_1",
		"task_2",
		"task_3",
		"task_4",

};

void (*task_entry[])(void*) = {
		task_1,task_2,task_3,task_4,
};

const char *task_nams[LES_TASKNUMS] = {
		"task_1",
		"task_2",
		"task_3",
		"task_4",
};



void Init()
{

	int i = 0;
	int ret;
	semaphore_init();
	ACoreOs_task_wake_after(3000);
	//scanf("%d",&delay_time);
	delay_time = 10;
	printk_spinLock("delay_time = %d\n",delay_time);

    for(i = 0; i < LES_TASKNUMS; i ++){

    	ret = ACoreOs_task_create_smp( task_nams[i], 20,&task_stack[i][0],
    			ACOREOS_MINIMUM_STACK_SIZE*4,
				ACOREOS_PREEMPT | ACOREOS_TIMESLICE,NULL, &task_id[i], i%2+1);
		printk_spinLock("task id = 0x%x \n",task_id[i]);
		ret = ACoreOs_task_start( task_id[i], (ACoreOs_task_entry)task_entry[i], g_msg[i]);
		if(ret)
			printk_spinLock("start task%d failed\n",i);
    }
	CREATE_MONITOR();

}
