#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

#define MAX_TIMERS 1000

typedef struct Timer{
    float duration;
    float elapsed;
    void (*callback)(void);
    int repeats;
} Timer;

void TimersTicks(float deltaTime);

void TimerSet(float duration, void (*callback)(void), int repeats);

#endif