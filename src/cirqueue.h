#ifndef __EX_QUEUE__H__
#define __EX_QUEUE__H__


/*
    
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>


extern "C"
{
    #include "oi_shm.h"
}
#include "pthread.h"


#pragma pack(1)
struct shm_queue_header
{
    uint64_t m_max_nr;
    
    volatile uint64_t m_curr_nr;
    
    volatile uint64_t m_ReadPos; //申请的位置
    volatile uint64_t m_ReadPos_r;
    
    volatile uint64_t m_WritePos; //申请的位置
    volatile uint64_t m_WritePos_r;


    unsigned int node_size;
};
#pragma pack()

template <typename T, unsigned BATCH>
class cir_queue
{
public:

    cir_queue() : m_info(NULL), m_pData(NULL)
    {
        
    }

    ~cir_queue()
    {
        
    }
    
    int Init(int shm_key, unsigned int max_count = 0)
    {
        if (max_count > BATCH) {
            return -1;
        }
        void *pData;
        
        uint32_t shm_size = BATCH * sizeof(T) + sizeof(shm_queue_header);
        
        if (GetShm2(&pData, shm_key, shm_size, IPC_CREAT | 0666) < 0) {
            return -21;
        }

        m_info = (shm_queue_header *)pData; 
        m_pData = (T *)((char *)pData + sizeof(shm_queue_header));

        // 判断最大消息个数有没有调整过
        
        if (((max_count > 0) && (max_count != m_info->m_max_nr))
			|| (m_info->node_size != sizeof(T))) {

			memset(pData, 0, shm_size);

            if (max_count > 0) {
                m_info->m_max_nr = max_count;
            }

            m_info->node_size = sizeof(T);

            //printf("change size, max_count=%d, node size = %d\r\n", m_info->m_max_nr, m_info->node_size);
        }
        
        
        return 0;
    }
    /*
        ret:
        -1  full
        0   ok
        
    */
    int push(const T& item)
    {
        if ((!m_info) || (!m_pData)) {// not initialize
            return -2;
        }

		uint64_t Pos, NextPos;
        
        do {
            Pos = m_info->m_WritePos;
            NextPos = (Pos + 1) % m_info->m_max_nr;
            
            if (NextPos == m_info->m_ReadPos_r) {
                return -1;
            }
        } while (!__sync_bool_compare_and_swap(&m_info->m_WritePos, Pos, NextPos));

        m_pData[Pos] = item;

        while(!__sync_bool_compare_and_swap(&m_info->m_WritePos_r, Pos, NextPos)) {
            sched_yield();
        }
        __sync_add_and_fetch(&m_info->m_curr_nr, 1);    // 消息个数
        
        return 0;
    }

    /*
        ret:
        -1 no data
        0   ok
    */
    int pop(T& out)
    {   
        if((!m_info) || (!m_pData)) {// not initialize
            return -2;
        }
        
        uint64_t Pos, NextPos;
        do {
            Pos = m_info->m_ReadPos;
            if (Pos == m_info->m_WritePos_r) {
                return -1;
            }
            NextPos = (Pos + 1) % m_info->m_max_nr;
        } while(!__sync_bool_compare_and_swap(&m_info->m_ReadPos, Pos, NextPos));
        
        out = m_pData[Pos];
        
        while (!__sync_bool_compare_and_swap(&m_info->m_ReadPos_r, Pos, NextPos)) {
            sched_yield();
        }
        __sync_sub_and_fetch(&m_info->m_curr_nr, 1);

        return 0;
    }

	unsigned get_curr_nr() const
    {
        if(!m_info || !m_pData)
        {// not initialize
            return 0;
        }
        return m_info->m_curr_nr;
    }

private:
    cir_queue(const cir_queue&);
    const cir_queue& operator=(const cir_queue&);

	shm_queue_header *m_info;

    T* m_pData;
        
};


#endif




