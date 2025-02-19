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

#define MAX_TIMERS 1001 /** max timers running at once */
#define TIMER_UNLIMITED -1 /** constant for never ending timer */
#define TIMER_ONCE 1 /** constant for 1 time timer */

/**
 * @brief initialization of timer system
 * 
 */
void TimerInit(void);

/**
 * @brief update all active timers
 * 
 * @param deltaTime time between frames in ms
 */
void TimerTicks(int deltaTime);

/**
 * @brief set a new timer
 * 
 * @param duration duration in ms
 * @param repeats number of repeats -1 infinite 0=1
 * @param callback callback function
 * @param callbackData data passed to callback function
 * @return size_t unique ID used as handler
 */
size_t TimerSet(unsigned int duration, int repeats, void (*callback)(void*), void* callbackData);

/**
 * @brief cancel a timer
 * 
 * @param handle unique handle of the timer
 * @param triggerCallback if callback should be triggered at cancel
 */
void TimerCancel(size_t handle, bool triggerCallback);

/**
 * @brief set pause for timers
 * 
 * @param isPaused true for pause, false for run
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