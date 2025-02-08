/**
 * @file animation.h
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief API for animation module
 * @version 0.1
 * @date 08-02-2025
 *
 * @copyright Copyright (c) 2025
 * @todo separate 2D and 3D part = expand
 */
#ifndef ANIMATION_H
#define ANIMATION_H

#include "sprite.h"
#include "resource_manager.h"

#define MAX_ANIMATION_FRAMES 16 /**< max animation frames per clip */

/**
 * @brief Direction ENUM, 8 options. From right, anti-clockwise.
 *
 */
typedef enum DIRECTION{
    EAST, NORTH_EAST, NORTH, NORTH_WEST, WEST, SOUTH_WEST, SOUTH, SOUTH_EAST, DIRECTION_COUNT
} DIRECTION;

#pragma region STRUCTURES

/**
 * @brief structure with base values used in each animation
 */
typedef struct AnimationBaseData
{
    char name[32]; /**< name as ID */
} AnimationBaseData;

/**
 * @brief collection of frames and lenght of this collection
 *
 */
typedef struct AnimationFrames
{
    Sprite sprites[MAX_ANIMATION_FRAMES]; /**< simple array of sprites */
    int frameCount; /**< lenght of the animation(number of used sprites) */
} AnimationFrames;

/**
 * @brief data used for "3D" animations
 *
 */
typedef struct AnimationDir
{
    AnimationBaseData data; /**< base data about animation */
    AnimationFrames frames[DIRECTION_COUNT]; /**< directions for this animation */
} AnimationDir;

#pragma endregion

#pragma region DECLARATIONS
/**
 * @brief initialize animation system, ONLY ONE animation system exist at given time
 * @warning this will allocate a space on the HEAP - don't forget to call AnimationDestroy()
 */
void AnimationInit(void);

/**
 * @brief clear all allocated resources, open files, ...  and prepare app for EXIT, or a NEW USE
 *
 */
void AnimationDestroy(void);

/**
 * @brief insert a frame at given object
 * @todo sort frames after each insert? maybe before use? just after all loads?, also change string object to some other type?
 * @param object name of the object (this objects will share the animation)
 * @param frameData data from textrurepacker
 * @note frameData format: "animationName_angle_spriteNameID,textureName,xPosOnTexture,yPosOnTexture,width,height,xOrigin,yOrigin" x and y origin are top left corner offset
 */
void AnimationPush3DFrame(char* object, char* frameData);

// get animation pointer TODO: do something about char* selection... its bad - maybe hash, enum, ...?
/**
 * @brief get an animation pointer for "3D" animations
 *
 * @param object name of the animated object
 * @param animationName animation, what you want use
 * @return AnimationDir*
 */
const AnimationDir* Animation3DGetFromDatabase(char* object, char* animationName);

void Animation3DSortFrames(char* object);
#pragma endregion

#endif