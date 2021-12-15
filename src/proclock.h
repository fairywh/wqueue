#ifndef _HTTPCACHE_PROCLOCK_H_
#define _HTTPCACHE_PROCLOCK_H_
#include <stdint.h>

#pragma pack(1)
typedef struct {
	volatile uint32_t _lock_pid;
	//volatile uint32_t _lock_time;
	//volatile uint64_t _lock_ext;
}ProLock;
#pragma pack()

//void ShmLockInit(ProLock * lock);
void ShmLock(ProLock * lock);
int ShmTryLock(ProLock * lock);
void ShmUnLock(ProLock * lock);

#endif
