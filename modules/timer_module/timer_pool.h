#ifndef TIMER_POOL_H
#define TIMER_POOL_H

#include <stdlib.h>
#include "timer_common.h"
#include "raylib.h"

#define POOL_SIZE 2000

static Timer* pool;
static int nextIndex = 0;
static int* freeIndexes;
static int freeIndexesSize = 0;

static void TimerPoolInit()
{
    void* allocatedMemory = MemAlloc(sizeof(Timer) * POOL_SIZE + sizeof(int) * POOL_SIZE);
    if(allocatedMemory == NULL)
    {
        TraceLog(LOG_ERROR, "can't allocate space for timer pool");
        return;
    }
    pool = allocatedMemory;
    freeIndexes = (int*)((Timer*)allocatedMemory) + POOL_SIZE;
}

static Timer* TimerPoolGetTimer()
{
    if(freeIndexesSize > 0)
    {
        int poolIndex = freeIndexes[freeIndexesSize--];
        return (pool + poolIndex);
    }
    return (pool + nextIndex++);
}

static void TimerPoolReturnTimer(Timer* timerToReturn)
{
    int index = timerToReturn - pool;
    freeIndexes[freeIndexesSize++] = index;
}

#endif