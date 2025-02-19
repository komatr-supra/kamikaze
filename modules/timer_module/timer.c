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

static bool m_isPaused = false; //< if timers are paused(true) or timers running
static Timer** m_hashTable; //< array of pointers, for quick access
static size_t m_timerID = 0;    //< timers unique ID number(increase each timer)

static Timer** m_activeTimers;  //< array of pointers for quick loop of active timers
static int m_activeTimersCount = 0; //< total count of active timers(array length)

/**
 * @brief create an unique number as ID
 * 
 * @return size_t returned ID
 */
static size_t GetID(void)
{
    return m_timerID++ % SIZE_MAX;
}

/**
 * @brief Get the Hash Index from ID
 * 
 * @param ID unique id of the timer it is also a handle
 * @return int index in hash table 
 */
static int GetHashIndex(size_t ID)
{
    return ID % MAX_TIMERS;
}


void TimerInit(void)
{    
    m_hashTable = MemAlloc(sizeof(Timer*) * MAX_TIMERS);;
    m_activeTimers = MemAlloc(sizeof(Timer*) * MAX_TIMERS);
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
    else
    {
        while(timerInHash != NULL && timerInHash->next != NULL)
        {
            timerInHash = timerInHash->next;
        }
        timerInHash->next = newTimer;
    }
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
    int hashIndex = GetHashIndex(handle);
    Timer* checkedTimer = m_hashTable[hashIndex];
    if(checkedTimer == NULL) return; // no entry at given index
    if(triggerCallback) checkedTimer->callback(checkedTimer->callbackData);
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
    else m_hashTable[hashIndex] = checkedTimer->next;
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
