/**
 * animation.h
 * komatr 6.2.2025
 * this will be complete animation solution
 * it will contain collection of animations
 */
#ifndef ANIMATION_H
#define ANIMATION_H

#include "sprite.h"

#define MAX_ANIMATION_FRAMES 15

/**
 * enum direction, will be probably used in more places
 * move it to HELPER COMMON???
 */
typedef enum DIRECTION{
    EAST, NORTH_EAST, NORTH, NORTH_WEST, WEST, SOUTH_WEST, SOUTH, SOUTH_EAST, DIRECTION_COUNT
} DIRECTION;

/**
 * structure for single animation
 */
typedef struct AnimationBaseData
{
    char name[32];
} AnimationBaseData;

typedef struct AnimationFrames
{
    Sprite sprites[MAX_ANIMATION_FRAMES];
    int frameCount;
} AnimationFrames;

typedef struct AnimationDir
{
    AnimationBaseData data;
    AnimationFrames frames[DIRECTION_COUNT];
} AnimationDir;

// init - this is a complete system with database, so init and destroy must be called
void AnimationInit(void);
void AnimationDestroy(void);
// insert frame inside animation format entity:"knight" frameData:"animationName_angle_frameNumber(sprite name)"
void AnimationPushFrame(char* entity, char* frameData, Sprite* sprite);
// get animation pointer TODO: do something about char* selection... its bad - maybe hash, enum, ...?
AnimationDir* AnimationGetFromDatabase(char* entity, char* animationName);

/**
 * structure for keeping track of current character animation
 * save variables for right draw
 * each character got this structure
 */
typedef struct AnimatorBaseData
{
    int speedMultiplier;
    int currentFrame;
} AnimatorBaseData;

typedef struct AnimatorDir
{
    AnimatorBaseData data;
    AnimationDir* animation;
} AnimatorDir;

#endif