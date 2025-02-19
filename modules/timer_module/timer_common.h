#ifndef TIMER_COMMON_H
#define TIMER_COMMON_H

#include <stdbool.h>

typedef struct Timer{
    bool isUsed;
    size_t id;
    int usedIndex;
    unsigned int duration;
    unsigned int elapsed;
    void (*callback)(void*);
    void* callbackData;
    int repeats;
    struct Timer* next;
} Timer;

#endif