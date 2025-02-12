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

#define MAX_ANIMATIONS_PER_ENTITY 20 /**< maximum animations per entity */
#define MAX_OWNER_NAME_LENGHT 32    /**< max lenght of the key value */
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

/**
 * @brief 3D animation record struct, all animation of given object
 *
 */
typedef struct DatabaseRecordAnimationDir{
    char object[MAX_OWNER_NAME_LENGHT]; /**< same as a key for this record */
    AnimationDir animations[MAX_ANIMATIONS_PER_ENTITY]; /**< animations of this object - directions only */
    int animationsCount;    /**< total count of animations of this object */
} DatabaseRecordAnimationDir;

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
 * @brief Get the Entity Database Record object
 *
 * @param entity the entity which own returned record
 * @return DatabaseRecordAnimationDir* pointer to record
 */
DatabaseRecordAnimationDir* AnimationGetEntityData(char* entity);

void AnimationLoadCharacter(char* characterName);

#pragma endregion

#endif