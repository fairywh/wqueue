#include <sys/mman.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "oi_shm.h"

char* GetReadOnlyShm(int iKey, size_t  iSize)
{
    int iShmID;
    char* sShm;
    char sErrMsg[50];

    if ((iShmID = shmget(iKey, iSize, 0)) < 0) {
        sprintf(sErrMsg, "shmget key %#x, size %u", (unsigned int)iKey, iSize);
        perror(sErrMsg);
        return NULL;
    }
    if ((sShm = shmat(iShmID, NULL, SHM_RDONLY)) == (char *) -1) {
        perror("shmat");
        return NULL;
    }
    return sShm;
}

char* GetShm(int iKey, size_t  iSize, int iFlag)
{
    int iShmID;
    char* sShm;
    char sErrMsg[50];
    struct shmid_ds stShmIdBuf;

    if ((iShmID = shmget(iKey, iSize, iFlag)) < 0) {
        sprintf(sErrMsg, "shmget key %#x, size %u", (unsigned int)iKey, iSize);
        perror(sErrMsg);
        return NULL;
    }
    if ((sShm = shmat(iShmID, NULL ,0)) == (char *) -1) {
        perror("shmat");
        return NULL;
    }
    if (shmctl(iShmID, IPC_STAT, &stShmIdBuf) == 0) {
        if (stShmIdBuf.shm_segsz != iSize) {
            sprintf(sErrMsg,"shmctl error! stShmIdBuf.shm_segsz:%u != %d\n", (C2_SIZE)stShmIdBuf.shm_segsz, iSize);
            perror(sErrMsg);
            //Attr_API(167278, 1);
        }
    } else {
        sprintf(sErrMsg, "shmctl Error");
        perror(sErrMsg);
        //Attr_API(167279, 1);
    }
    return sShm;
}

int GetShm2(void **pstShm, int iShmID, size_t  iSize, int iFlag)
{
    char* sShm;
    
    if (!(sShm = GetShm(iShmID, iSize, iFlag & (~IPC_CREAT)))) {
        if (!(iFlag & IPC_CREAT)) return -1;
        if (!(sShm = GetShm(iShmID, iSize, iFlag))) return -1;
        
        memset(sShm, 0, iSize);
    }
    *pstShm = sShm;
    return 0;
}

int GetShm3(void **pstShm, int iShmID, size_t  iSize, int iFlag)
{
    char* sShm;

    if (!(sShm = GetShm(iShmID, iSize, iFlag & (~IPC_CREAT)))) {
        if (!(iFlag & IPC_CREAT)) return -1;
        if (!(sShm = GetShm(iShmID, iSize, iFlag))) return -1;
        
        *pstShm = sShm;
        return 1;
    }
    *pstShm = sShm;
    return 0;
}

char* GetShm4(int iKey, size_t  iSize, int iFlag, int *piShmID)
{
    int iShmID;
    char* sShm;
    char sErrMsg[50];

    if ((iShmID = shmget(iKey, iSize, iFlag)) < 0) {
        sprintf(sErrMsg, "shmget key %#x, size %u", (unsigned int)iKey, iSize);
        perror(sErrMsg);
        return NULL;
    }
    if ((sShm = shmat(iShmID, NULL ,0)) == (char *) -1) {
        perror("shmat");
        return NULL;
    }
    *piShmID = iShmID;
    return sShm;
}

int DelShm(int iKey)
{
    int iShmID = 0;
    char sErrMsg[50];
    if ((iShmID = shmget(iKey, 0, NULL)) < 0) {
        snprintf(sErrMsg, sizeof(sErrMsg), "shmget key failed %#x", (unsigned int)iKey);
        perror(sErrMsg);
        return -1;
    }
    if (shmctl(iShmID, IPC_RMID, NULL) == 0)
    {
        return 0;
    }
    else
    {
        
        snprintf(sErrMsg, sizeof(sErrMsg), "delete shm error, key=%u.", iShmID);
        perror(sErrMsg);
        return -2;
    }
    
}

