/**
 * @file timer_pool.h
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief pool for timers
 * @version 0.1
 * @date 20-02-2025
 *
 * @copyright Copyright (c) 2025
 *
 */
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

/// @brief constructor of the timer pool
static void TimerPoolInit(void)
{
    pool = MemAlloc(sizeof(Timer) * POOL_SIZE);
    if(pool == NULL)
    {
        TraceLog(LOG_ERROR, "can't allocate space for timer pool");
        return;
    }
    freeIndexes = MemAlloc(sizeof(int) * POOL_SIZE);
    if(freeIndexes == NULL)
    {
        TraceLog(LOG_ERROR, "can't allocate space for pool free indexes");
        free(pool);
        return;
    }
}

/// @brief destructor of the timer pool
static void TimerPoolDestroy(void)
{
    free(pool);
    free(freeIndexes);
}

/**
 * @brief find free timer in pool and return it
 * @bug can't reallocate space, when all timers are used
 * @return Timer* free timer from pool
 */
static Timer* TimerPoolGetTimer(void)
{
    if(freeIndexesSize > 0)
    {
        int poolIndex = freeIndexes[freeIndexesSize--];
        return (pool + poolIndex);
    }
    return (pool + nextIndex++);
}

/**
 * @brief mark timer as unused, and can be returned with fnc "Timer* TimerPoolGetTimer(void)"
 * @bug make a guard, like double free, real timer pointer, null, etc...
 * @param timerToReturn timer, which should be returned
 */
static void TimerPoolReturnTimer(Timer* timerToReturn)
{
    int index = timerToReturn - pool;
    freeIndexes[freeIndexesSize++] = index;
}

#endif