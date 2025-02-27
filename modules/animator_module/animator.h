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

#define ANIMATOR_SPEED_DEFAULT 1

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
    AnimatorCallback callbacks[ANIMATION_CALLBACK_FLAGS_COUNT];
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

/**
 * @brief set function and data for given callback type
 * @warning this should be reset at animation change
 * @param animatorBaseData basic animation data 2D and 3D
 * @param callbackType type of callback
 * @param callbackFunction callback function "void (*fnc)(void*)"
 * @param callbackData data passed to callback function "void*"
 */
void AnimatorSetCallback(AnimatorBaseData* animatorBaseData, AnimationCallbackFlags callbackType, void(*callbackFunction)(void*), void* callbackData);

/**
 * @brief get index animation for given animation name
 * 
 * @param animator animator, where to kook at
 * @param animationName name of the animation
 * @return int index of animation or -1 if FAIL(animation not found)
 */
int AnimatorGetAnimationIndex(Animator3D* animator, const char* animationName);
#endif