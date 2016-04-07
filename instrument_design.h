#ifndef INSTRUMENT_DESIGN_H
#define INSTRUMENT_DESIGN_H

extern int globalInstrumentIndex;


#define DLK_INIT() \
	int dlk_task_index = dlk_init(); \
	ACoreOs_task_set_note(ACOREOS_SELF,ACOREOS_NOTEPAD_0,dlk_task_index);
	
#define DLK_CMPSITE(semaphore_id) {\
	int taskid; \
	int line = __LINE__ + 1; \
	ACoreOs_task_get_note(ACOREOS_SELF,ACOREOS_NOTEPAD_0,&taskid); \
	dlk_cmpSite(taskid, line, semaphore_id); \
}

#define DLK_UPTSITE() { \
	int taskid; \
	int line = __LINE__ - 1; \
	ACoreOs_task_get_note(ACOREOS_SELF,ACOREOS_NOTEPAD_0,&taskid); \
	dlk_uptSite(taskid, line); \
}

#define CREATE_MONITOR() \
	ACoreOs_id monitor_id; \
	UNIT32 task_monitor_stack[ACOREOS_MINIMUM_STACK_SIZE*4]; \
	const char *task_monitor_name = "task_monitor"; \
	int monitor_ret = ACoreOs_task_create_smp( task_monitor_name, 20, task_monitor_stack, \
    			ACOREOS_MINIMUM_STACK_SIZE*4, \
				ACOREOS_PREEMPT | ACOREOS_TIMESLICE,NULL, monitor_id, 1); \
	ret = ACoreOs_task_start( monitor_id, (ACoreOs_task_entry)task_monitor, task_monitor_name); \
	if(monitor_ret) \
		printk_spinLock("start task_monitor failed\n");
	

#endif