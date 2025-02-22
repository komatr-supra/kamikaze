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



typedef struct Callback{
    void (*fnc)(void*, void*);  //< callback function "void function(animator, param->below)"
    void* param;    //< parameter passed to function
    struct Callback* next;  //< for list of callbacks
} Callback;

typedef enum ANIM_TYPE{
    ANIM_TYPE_LOOP, ANIM_TYPE_SINGLE, ANIM_TYPE_PINGPONG
} ANIM_TYPE;

/**
 * @brief Direction ENUM, 8 options. From right, anti-clockwise.
 *
 */
typedef enum DIRECTION{
    EAST, NORTH_EAST, NORTH, NORTH_WEST, WEST, SOUTH_WEST, SOUTH, SOUTH_EAST, DIRECTION_COUNT
} DIRECTION;

// ------------------------------region STRUCTURES

/**
 * @brief structure with base values used in each animation
 */
typedef struct AnimationBaseData
{
    char name[32]; /**< name as ID */
} AnimationBaseData;

typedef struct FrameData{
    Sprite sprite;
    Callback* callback;
} FrameData;

/**
 * @brief collection of frames and lenght of this collection
 *
 */
typedef struct AnimationFrames
{
    FrameData frames[MAX_ANIMATION_FRAMES]; /**< simple array of sprites */
    int frameCount; /**< lenght of the animation(number of used sprites) */
} AnimationFrames;

/**
 * @brief data used for single "3D" animation
 *
 */
typedef struct AnimationDir
{
    AnimationBaseData data; /**< base data about animation */
    AnimationFrames dirAnimations[DIRECTION_COUNT]; /**< directions for this animation */
} AnimationDir;

/**
 * @brief 3D animation record struct, all animation of given object
 *
 */
typedef struct DatabaseRecord3DAnimation{
    char object[MAX_OWNER_NAME_LENGHT]; /**< same as a key for this record */
    AnimationDir animations[MAX_ANIMATIONS_PER_ENTITY]; /**< animations of this object - directions only */
    int animationsCount;    /**< total count of animations of this object */
} DatabaseRecord3DAnimation;

typedef struct CommonAnimData{
    char animName[32];
    int speedMS;
    ANIM_TYPE animationType;
} CommonAnimData;
// ------------------------------ endregion

// ------------------------------ region DECLARATIONS
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
 * @brief Get the Animation Database Record object
 *
 * @param characterName the character which own returned record
 * @return DatabaseRecordAnimationDir* pointer to record or NULL
 */
const DatabaseRecord3DAnimation* AnimationGetCharacterData(char* characterName);

/// @brief get specific animation
/// @param animationCollection animation record of given character
/// @param animationName name of the animation
/// @return animation data or NULL
const AnimationDir* Animation3DGetAnimation(DatabaseRecord3DAnimation* animationCollection, char* animationName);

void AnimationGetAnimationsNames(char* buffer, DatabaseRecord3DAnimation* animationCollection);
// ------------------------------
CommonAnimData* AnimationGetCommonAnimData(char* animName, bool* isNew);
#endif