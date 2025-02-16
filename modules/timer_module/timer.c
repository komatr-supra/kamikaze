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
#include "timer.h"
#include "raylib.h"
#include <stdlib.h>

typedef struct Timer{
    int handler;
    float duration;
    float elapsed;
    void (*callback)(void*);
    void* callbackData;
    int repeats;
} Timer;

static bool m_isPaused = false;
static Timer m_timers[MAX_TIMERS];

static int m_timerIndexes[MAX_TIMERS];
static int m_timersCount = 0;

static int m_timerFreeIndexes[MAX_TIMERS];
static int n_timerFreeIndexSize = 0;

static int m_handlers[MAX_TIMERS];
static int m_handleIndex = 1; //** zero is reserved for unused */

static int m_handleFreeIndexes[MAX_TIMERS];
static int m_handleFreeIndexSize = 0;

static int GetHandleIndex(void)
{
    if(m_handleFreeIndexSize > 0) return m_handleFreeIndexes[--m_handleFreeIndexSize];
    else m_handleIndex++;
}

int TimerSet(float duration, int repeats, void (*callback)(void*), void* callbackData)
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
    Timer* newTimer = &m_timers[m_timersCount];
    newTimer->duration = duration;
    newTimer->elapsed = 0.0f;
    newTimer->callback = callback;
    newTimer->callbackData = callbackData;
    newTimer->repeats = repeats > 0 ? repeats - 1 : -1;
    newTimer->handler = m_handleIndex;
    m_handlers[m_handleIndex] = m_timersCount;
    m_timersCount++;
    return m_handleIndex++;
}

void TimerTicks(float deltaTime)
{
    if(m_isPaused) return;
    for (int i = 0; i < m_timersCount; i++)
    {
        m_timers[i].elapsed += deltaTime;
        if(m_timers[i].elapsed < m_timers[i].duration) continue;

        if(m_timers[i].callback != NULL) m_timers[i].callback(m_timers[i].callbackData);
        m_timers[i].elapsed = 0;
        if(m_timers[i].repeats > 0)
        {
            m_timers[i].repeats--;
        }
        else if(m_timers[i].repeats != -1)
        {
            m_timersCount--;
            if(m_timersCount > 0)
            {
                m_handleFreeIndexes[m_handleFreeIndexSize++] = m_timers[i].handler;
                m_timers[i] = m_timers[m_timersCount];
                m_handlers[m_timers[i].handler] = i;
                i--;
            }
        }
    }
}

void TimerCancel(int handle, bool triggerCallback)
{
    if(handle == 0) return;
    int timerIndex = m_handlers[handle];
    if(timerIndex == -1) return;
    m_handlers[handle] = -1;
    if(triggerCallback && m_timers[timerIndex].callback != NULL) m_timers[timerIndex].callback(m_timers[timerIndex].callbackData);

    m_handleFreeIndexes[m_handleFreeIndexSize++] = m_timers[timerIndex].handler;
    m_timersCount--;
    if(timerIndex >= 0 && timerIndex < m_timersCount)
    {
        m_timers[timerIndex] = m_timers[m_timersCount];
        m_handlers[m_timers[timerIndex].handler] = timerIndex;
    }

    TraceLog(LOG_DEBUG, "cancelling handler id: %d (timer index: %d)", handle, timerIndex);
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
