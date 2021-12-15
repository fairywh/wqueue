#ifndef _OI_SHM_H
#define _OI_SHM_H

#include "ENVIRONMENT"

#include <sys/ipc.h>
#include <sys/shm.h>
#include "oi_macro.h"



char* GetReadOnlyShm(int iKey, size_t iSize);
char* GetShm(int iKey, size_t iSize, int iFlag);
int GetShm2(void **pstShm, int iShmID, size_t iSize, int iFlag);
int GetShm3(void **pstShm, int iShmID, size_t iSize, int iFlag);
char* GetShm4(int iKey, size_t iSize, int iFlag, int *piShmID);
int DelShm(int iShmID);


#endif
