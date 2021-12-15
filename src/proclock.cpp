#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "proclock.h"

static uint32_t s_pid=0;

#define asm_cpu_pause()  __asm__ ("pause");

static  uint32_t cmp_set(volatile uint32_t *l, uint32_t o, uint32_t n)
{
    unsigned char  r;
    __asm__ volatile (
            "lock;"
            "cmpxchgl  %3, %1;"
            "sete      %0;    "
            : "=a" (r)
            : "m" (*l), "a" (o), "r" (n)
            : "cc", "memory");
    /*
       if(!r)
       printf("spp_cmp_set ret %d\n",r);
     */
    return r;
}

/* *
 * ret: 0 succ !=0:failed
 * */
int ShmTryLock(ProLock * lock)
{
	if(!lock){
		//Attr_API(2064913,1);
		return 0;
	}
	if(!s_pid) {
		s_pid = getpid();
	}
	uint32_t pid;
	volatile uint32_t * p_pid = &(lock->_lock_pid);
	if(!(*p_pid) && cmp_set(p_pid, 0, s_pid)) {
		//Attr_API(2064914,1);
		return 0;
	}
	pid = *p_pid;

	if(pid) {
		if(kill(pid,0)==-1 && errno == ESRCH) {
			//Attr_API(2064915,1);
			if(cmp_set(p_pid, pid, s_pid)){
				//Attr_API(2064916,1);
				return 0;
			}
			//Attr_API(2064917,1);
		} 
	}
	return 1;
}

void ShmLock(ProLock * lock) 
{
	if(!lock){
		//Attr_API(2064918,1);
		sleep(1);//illegal sleep 1 s
		return;
	}
	if(!s_pid) {
		s_pid = getpid();
	}

	uint32_t i,n,pid;
	volatile uint32_t * p_pid = &(lock->_lock_pid);
	while(1) {
		if(*p_pid== 0 && cmp_set(p_pid, 0, s_pid)) {
			//Attr_API(2064919,1);
			return;
		}

		for (n = 1; n < 2048; n <<= 1) {//log2(spin)次
            for (i = 0; i < n; i++) { //惩罚性pause
                asm_cpu_pause();
            }
            if (*p_pid == 0 && cmp_set(p_pid, 0, s_pid)) {
				//Attr_API(2064920,1);
                return;
            }
        }
		//Attr_API(2064921,1);

		sched_yield();
        
		pid = *p_pid;

		if(pid) {
			if(kill(pid,0)==-1 && errno == ESRCH) {
                cmp_set(p_pid, pid, 0);//safely reset lock
				//Attr_API(2064922,1);
			} 
		}
	}
}

void ShmUnLock(ProLock * lock) 
{
	if(!lock) {
		//Attr_API(2064923,1);
		return;
	}

	if(lock->_lock_pid == 0)
	{
	    //已经置0了，不需要在解锁
	    //Attr_API(123,1);
	    return;
	}
	
	if(!cmp_set(&(lock->_lock_pid), s_pid, 0)) {
		//Attr_API(2064924,1);
		sched_yield();
	}
}
