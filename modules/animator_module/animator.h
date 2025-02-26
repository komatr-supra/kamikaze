/**
 * @file animator.h
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief API for animator module
 * @version 0.1
 * @date 21-02-2025
 *
 * @copyright Copyright (c) 2025
 * @todo move all data related to animator into AnimatorBaseData, create 2D animator
 */
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <stdlib.h>
#include "raylib.h"
#include "animation.h"

#define ANIMATION_SPEED_DEFAULT 1

typedef enum AnimatorCallbackEnum{
    ANIMATOR_CALL_START,
    ANIMATOR_CALL_END,
    ANIMATOR_CALL_ACTION,
    ANIMATOR_CALL_SOUND,
    ANIMATOR_CALL_PARTICLES,
    ANIMATOR_CALL_SPECIAL,
    ANIMATOR_CALL_TOTAL_COUNT
} AnimatorCallbackEnum;

typedef struct AnimatorCallback{
    void (*callback)(void*);
    void* callbackData;
} AnimatorCallback;

/// @brief base data for 2D and 3D animations
typedef struct AnimatorBaseData
{
    size_t ownerID;
    float speedMultiplier;
    int currentFrame;
    bool isPingpongGoingBack;
    size_t timerHandle;
    AnimatorCallback callbacks[ANIMATOR_CALL_TOTAL_COUNT];
} AnimatorBaseData;

/// @brief all data for 3D animation
typedef struct Animator3D
{    
    DIRECTION direction;
    AnimatorBaseData data;
    const DatabaseRecord3DAnimation* animations;
    const Animation3D* currentAnimation;
    CommonAnimData* sharedData;    
} Animator3D;

/**
 * @brief initialize animator - at this stage is not running(call: Animator3DSetAnimation())
 *
 * @param animator animator for initializing
 * @param objectAnimations animation record for this animator
 * @param direction direction of the animation
 */
void Animator3DCreate(Animator3D* animator, const DatabaseRecord3DAnimation* objectAnimations, DIRECTION direction);

/**
 * @brief set animation of the animator, change speed, play it
 * @todo in animation probably missing fnc "GetAnimationIndex()"
 * @param animator animator, where animation will be changed
 * @param animationIndex index of the animation
 */
void Animator3DSetAnimation(Animator3D* animator, int animationIndex);

/**
 * @brief start the animation
 *
 * @param animator animator, which should start play
 */
void Animator3DStart(Animator3D* animator);

/**
 * @brief stop the animation
 *
 * @param animator animator, which should stop
 */
void Animator3DStop(Animator3D* animator);

/**
 * @brief change direction of the animation, keep frame, speed, etc...
 *
 * @param animator animator, which should change animation the direction
 * @param dir new direction of the animation
 */
void Animator3DDirectionSet(Animator3D* animator, DIRECTION dir);

/**
 * @brief draw animation at the given position
 *
 * @param animator animator, which will be drawn
 * @param x x coordination
 * @param y y coordination
 */
void Animator3DDraw(Animator3D* animator, float x, float y);


void AnimatorSetCallback(AnimatorBaseData* animatorBaseData, AnimatorCallbackEnum callbackType, void(*callbackFunction)(void*), void* callbackData);
#endif