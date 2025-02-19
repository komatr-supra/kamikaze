/**
 * @file timer_common.h
 * @author komatr (you@domain.com)
 * @brief shared data and info across timers
 * @version 0.1
 * @date 19-02-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef TIMER_COMMON_H
#define TIMER_COMMON_H

#include <stdbool.h>

/// @brief timer structure with all data
typedef struct Timer{
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