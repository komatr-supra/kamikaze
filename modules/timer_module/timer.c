#include "timer.h"
#include "raylib.h"

static Timer timers[MAX_TIMERS];
static int activeTimersCount;

static void TimerProceed(Timer* timer, float deltaTime);

void TimerSet(float duration, void (*callback)(void), int repeats)
{
    if(MAX_TIMERS < activeTimersCount + 1 )
    {
        TraceLog(LOG_ERROR, "no more space for a new TIMER");
        return;
    }

    Timer* newTimer = &timers[activeTimersCount++];
    newTimer->duration = duration;
    newTimer->elapsed = 0.0f;
    newTimer->callback = callback;
    newTimer->repeats = repeats - 1;
}

void TimersTicks(float deltaTime)
{
    for (int i = 0; i < activeTimersCount; i++)
    {
        TimerProceed(&timers[i], deltaTime);
    }

}

static void TimerProceed(Timer* timer, float deltaTime)
{
    timer->elapsed += deltaTime;
    if(timer->duration > timer->elapsed) return;
    timer->callback();
    switch (timer->repeats)
    {
    case 0:
        activeTimersCount--;
        *timer = timers[activeTimersCount];
        TimerProceed(timer, deltaTime);
        break;
    default:
        timer->repeats--;
    case -1:
        timer->elapsed = 0.0f;
        break;
    }

}

