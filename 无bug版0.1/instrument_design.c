/*
 * in.c
 *
 *  Created on: 2015-11-22
 *      Author: Administrator
 */

#include <acoreos.h>
#define LES_HEADER(file) <les/record/file>
#define LES_CONFIG_SMP
#include <les/record/lock.h>
#include "instrument_design.h"

/*
 * 自旋锁
 */
int inst_spinLockRet;
LES_SpinLockType inst_spinLock;

void inst_spinLockInit(){
	LES_SpinLockInit(&inst_spinLock);
}
void inst_spinLockLock(){
	inst_spinLockRet = LES_SpinLock(&inst_spinLock);
}
void inst_spinLockUnlock(){
	LES_SpinUnlock(&inst_spinLock, inst_spinLockRet);
}

#define SEMAPHORE_MAX 5
#define TASK_NUM_MAX 20

typedef struct{
	ACoreOs_id semaphore_id;
	SemaphoreEntry *cur;
	SemaphoreEntry *pre;
}Semaphore_pc;
Semaphore_pc semaphore_pc[SEMAPHORE_MAX];

int semaphore_pc_index = 0;

void dlk_increase_pc_index(){
	inst_spinLock();
	semaphore_pc_index ++;
	inst_spinLockUnlock();
}


typedef struct{
	ACoreOs_id semaphore_id;
	int line;
	int handle;
	int passtimes;
	ACoreOs_id task_id;
	int sametaskholdtimes;
	int difftaskholdtimes;
}SemaphoreEntry;
typedef struct{
	SemaphoreEntry entrys[SEMAPHORE_MAX];
	int currentIndex;
}TaskInstrumentEntry;
typedef struct{
	ACoreOs_id taskId;
	TaskInstrumentEntry semaphoreEntry;
}GlobalInstrumentInfo;
GlobalInstrumentInfo globalInstrumentInfo[TASK_NUM_MAX];

int globalInstrumentIndex = 0;

int dlk_init(){
    inst_spinLockLock();
    int ret = globalInstrumentIndex ++;
    inst_spinLockUnlock();
    return ret;
}

void dlk_cmpSite(int taskid, int line, ACoreOs_id semaphore_id, int handle){

	int index = globalInstrumentInfo[taskid].semaphoreEntry.currentIndex;
	int i = 0;
	SemaphoreEntry *varentry;
	while(i < index && globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].line != line){
		i ++;
	}
	if (i == index){
		memset(&globalInstrumentInfo[taskid].semaphoreEntry.entrys[i], 0, sizeof(SemaphoreEntry));
		ACoreOs_task_get_id(ACOREOS_SELF, &globalInstrumentInfo[taskid].taskId);
		globalInstrumentInfo[taskid].semaphoreEntry.currentIndex  = i + 1;
		globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].passtimes = 0;
		globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].handle = handle;
		globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].semaphore_id = semaphore_id;
		globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].line = line;
		globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].task_id = globalInstrumentInfo[taskid].taskId;
		globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].sametaskholdtimes = 0;
		globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].difftaskholdtimes = 0;
	}
	int j = 0;
	while(j < semaphore_pc_index && semaphore_pc[j].semaphore_id != semaphore_id){
		j ++;
	}
	if(j == semaphore_pc_index){
		memset(&semaphore_pc[j],0,sizeof(Semaphore_pc));
		semaphore_pc[j].semaphore_id = semaphore_id;
		semaphore_pc[j].cur = NULL;
		semaphore_pc[j].pre = NULL;
	}

}


void dlk_uptSite(int taskid, int line) {
	
	int index = globalInstrumentInfo[taskid].semaphoreEntry.currentIndex;
	int i = 0;
	int j = 0;
	SemaphoreEntry *varentry;
	ACoreOs_id semaphore_id;
	int handle；
	while(i < index && globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].line != line){
		i ++;
	}
	if (i < index){
		varentry = &(globalInstrumentInfo[taskid].semaphoreEntry.entrys[i]);
		varentry->passtimes ++;
		semaphore_id = varentry->semaphore_id;
		handle = varentry->handle;
	}
	while(j < semaphore_pc_index && semaphore_pc[j].semaphore_id != semaphore_id){
		j ++;
	}
	if(j < semaphore_pc_index){
		semaphore_pc[j].pre = semaphore_pc[j].cur;
		semaphore_pc[j].cur = varentry;
	}
	if(handle){
		if((semaphore_pc[j].pre->handle == 0) && (semaphore_pc[j].cur->handle == 1))
		{
			if(semaphore_pc[j].pre->task_id == semaphore_pc[j].cur->task_id)
				varentry->sametaskholdtimes ++;
			else
				varentry->difftaskholdtimes ++;
		}
	}
}

extern unsigned int agtLogData(char * pDataBuf, unsigned int dataSize);
void dlk_inst_log(int line, int taskid, int passtimes, ACoreOs_id semaphore_id, int handle, int sametaskholdtimes, int difftaskholdtimes){

    char buffer[100];
    unsigned int databuffersize;
    unsigned int loglen = 0;
    {
    	inst_spinLockLock();
    	memset(buffer,0,100);
        sprintf(buffer,"    %5d:%5d:%x:%2d:%2d:%5d:%5d"\r\n", line, passtimes, taskid, semaphore_id,handle,sametaskholdtimes,difftaskholdtimes);
        databuffersize = strlen(buffer);
        loglen = agtLogData(buffer, databuffersize);
        inst_spinLockUnlock();

    }

}

void print_in(){

	int taskid = 0;
	int line;
	int passtimes;
	int handle;
	int sametaskholdtimes;
	int difftaskholdtimes;
	ACoreOs_id semaphore_id;
	while(taskid < globalInstrumentIndex){
		int index = globalInstrumentInfo[taskid].semaphoreEntry.currentIndex;
		int i = 0;
		SemaphoreEntry *varentry;
		while(i < index){
			line = globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].line;
			handle = globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].handle;
			sametaskholdtimes = globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].sametaskholdtimes;
			difftaskholdtimes = globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].difftaskholdtimes;
			semaphore_id = globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].semaphore_id;
			passtimes = globalInstrumentInfo[taskid].semaphoreEntry.entrys[i].passtimes;
			dlk_inst_log(line,  globalInstrumentInfo[taskid].taskId, passtimes, semaphore_id, handle, sametaskholdtimes, difftaskholdtimes);
			i ++;
		}
		taskid ++;
	}

}


void task_monitor(){
	while(1){
		ACoreOs_task_wake_after(3000);
		int i;
		int flag;
		ACoreOs_task_state state;
		for(i = 0; i < globalInstrumentIndex; i ++){
			
			flag = ACoreOs_task_get_state(globalInstrumentInfo[i].taskId, &state);
			if(flag == ACOREOS_INVALID_ID)
				continue ;
			else if(flag == ACOREOS_INVALID_ADDRESS){
				printk_spinLock("ACOREOS_INVALID_ADDRESS!!\n");
				ACoreOs_task_wake_after(1000);
				break ;
			}else{
				if((state & ACOREOS_TASK_STATES_WAITING_FOR_MUTEX) == 0)
					break ;
			}
		}
		if(i == globalInstrumentIndex){
			print_in();
			ACoreOs_task_delete(ACOREOS_SELF);
		}	
	}
}

