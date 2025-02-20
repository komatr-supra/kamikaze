/**
 * structure for keeping track of current character animation
 * save variables for right draw
 * each character got this structure
 */
#ifndef ANIMATOR_H
#define ANIMATOR_H
#include <stdlib.h>
#include "raylib.h"
#include "animation.h"

#define ANIMATION_SPEED_DEFAULT 1

typedef struct AnimatorBaseData
{
    int speedMultiplier;
    int currentFrame;
} AnimatorBaseData;


typedef struct Animator3D
{
    bool isRunning;
    DIRECTION direction;
    AnimatorBaseData data;
    DatabaseRecord3DAnimation* animations;
    AnimationDir* currentAnimation;
    size_t timerHandle;
} Animator3D;



void Animator3DCreate(Animator3D* animator, const DatabaseRecord3DAnimation* objectAnimations, DIRECTION direction);

void Animator3DSetAnimation(Animator3D* animator, int animationIndex);

void Animator3DDirectionSet(Animator3D* animator, DIRECTION dir);

void Animator3DDraw(Animator3D* animator, float x, float y);

#endif