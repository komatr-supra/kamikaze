/**
 * @file timer.c
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief
 * @version 0.1
 * @date 15-02-2025
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include "timer.h"
#include "raylib.h"
#include "timer_common.h"
#include "timer_pool.h"

static bool m_isPaused = false;
static Timer** m_hashTable;
static size_t m_timerID = 0;

static Timer** m_activeTimers;
static int m_activeTimersCount = 0;

static void* m_allocatedMemory;

static size_t GetID(void)
{
    return m_timerID++ % SIZE_MAX;
}

static size_t GetHashIndex(size_t ID)
{
    return ID % MAX_TIMERS;
}

void TimerInit(void)
{
    m_allocatedMemory = MemAlloc(sizeof(Timer*) * MAX_TIMERS * 2);
    if(m_allocatedMemory == NULL)
    {
        TraceLog(LOG_ERROR, "can't allocate space for timers");
        return;
    }
    m_hashTable = m_allocatedMemory;
    m_activeTimers = m_hashTable + MAX_TIMERS;
    TimerPoolInit();
}

size_t TimerSet(unsigned int duration, int repeats, void (*callback)(void*), void* callbackData)
{
    if(callback == NULL)
    {
        TraceLog(LOG_WARNING, "no callback provided for timer -> no real handler returned");
        return 0;
    }
    if(repeats == 0)
    {
        TraceLog(LOG_DEBUG, "timer set with 0 repeats! -> executing once");
        repeats = 1;
    }
    Timer* newTimer = TimerPoolGetTimer();
    newTimer->isUsed = true;
    newTimer->id = GetID();
    newTimer->duration = duration;
    newTimer->elapsed = 0;
    newTimer->callback = callback;
    newTimer->callbackData = callbackData;
    newTimer->repeats = repeats > 0 ? repeats - 1 : -1;
    newTimer->usedIndex = m_activeTimersCount;
    newTimer->next = NULL;

    Timer* timerInHash = m_hashTable[GetHashIndex(newTimer->id)];
    if(timerInHash == NULL) m_hashTable[GetHashIndex(newTimer->id)] = newTimer;
    while(timerInHash != NULL && timerInHash->next != NULL)
    {
        timerInHash = timerInHash->next;
    }
    timerInHash = newTimer;
    m_activeTimers[m_activeTimersCount++] = newTimer;
    return newTimer->id;
}

void TimerTicks(int deltaTimeMs)
{
    if(m_isPaused) return;
    for (int i = 0; i < m_activeTimersCount; i++)
    {
        m_activeTimers[i]->elapsed += deltaTimeMs;
        if(m_activeTimers[i]->elapsed < m_activeTimers[i]->duration) continue;
        //reset timer
        m_activeTimers[i]->elapsed = 0;
        // trigger callback
        if(m_activeTimers[i]->callback != NULL) m_activeTimers[i]->callback(m_activeTimers[i]->callbackData);
        // handle repeats
        if(m_activeTimers[i]->repeats == -1) continue;
        int remainingRepeats = m_activeTimers[i]->repeats--;
        // remove timer
        if(remainingRepeats == 0)
        {
            // from hash
            int hashIndex = GetHashIndex(m_activeTimers[i]->id);
            Timer* checkedTimer = m_hashTable[hashIndex];
            Timer* previous = NULL;
            while (checkedTimer->id != m_activeTimers[i]->id && checkedTimer->next != NULL)
            {
                previous = checkedTimer;
                checkedTimer = checkedTimer->next;
            }
            if(previous != NULL) previous->next = checkedTimer->next;
            else m_hashTable[hashIndex] = checkedTimer->next;
            // return to pool
            if(checkedTimer != NULL) TimerPoolReturnTimer(m_activeTimers[i]);
            // from active
            m_activeTimers[i] = m_activeTimers[m_activeTimersCount - 1];
            m_activeTimers[i]->usedIndex = i;
            m_activeTimersCount--;
            i--;
        }

    }
}

void TimerCancel(size_t handle, bool triggerCallback)
{
    // find
    Timer* checkedTimer = m_hashTable[GetHashIndex(handle)];
    if(checkedTimer == NULL) return; // no entry at given index
    Timer* previous = NULL;
    while (checkedTimer->id != handle && checkedTimer->next != NULL)
    {
        previous = checkedTimer;
        checkedTimer = checkedTimer->next;
    }
    // return to pool
    TimerPoolReturnTimer(checkedTimer);
    // from active
    int indexInUsedArray = checkedTimer->usedIndex;
    m_activeTimers[indexInUsedArray] = m_activeTimers[m_activeTimersCount - 1];
    m_activeTimers[indexInUsedArray]->usedIndex = indexInUsedArray;
    m_activeTimersCount--;
    // from hash
    if(previous != NULL) previous->next = checkedTimer->next;
    else checkedTimer == NULL;
}

void TimerPauseSet(bool isPaused)
{
    m_isPaused = isPaused;
    TraceLog(LOG_DEBUG, m_isPaused ? "timers are paused" : "timers running");
}

bool TimerGetPauseState(void)
{
    return m_isPaused;
}
