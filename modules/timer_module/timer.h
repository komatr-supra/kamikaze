/**
 * @file timer.h
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief universal timers
 * @version 0.1
 * @date 14-02-2025
 * @copyright Copyright (c) 2025
 *
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

#define MAX_TIMERS 1000 /** max timers running at once */
#define TIMER_UNLIMITED -1 /** constant for never ending timer */
#define TIMER_ONCE 1 /** constant for 1 time timer */



void TimerTicks(float deltaTime);

/**
 * @brief create a timer, return handle
 *
 * @param duration timer duration
 * @param repeats number of repeats (-1 infinity, 0 and 1 are the same -> 1)
 * @param callback callback fnc, if callback isnt provided, then timer will NOT be created
 * @param callbackData data passed to callback function
 * @return int handle for timer
 */
int TimerSet(float duration, int repeats, void (*callback)(void*), void* callbackData);

/**
 * @brief cancel a given timer
 *
 * @param handle timer handle
 * @param triggerCallback if callback should be executed when timer is cancelled
 */
void TimerCancel(int handle, bool triggerCallback);

/**
 * @brief set pause state
 *
 * @param isPaused TRUE for pause, FALSE for run
 */
void TimerPauseSet(bool isPaused);

/**
 * @brief getter for a pause state
 *
 * @return true timers are paused
 * @return false timers running
 */
bool TimerGetPauseState(void);
#endif