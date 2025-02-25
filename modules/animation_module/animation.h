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

#define MAX_ANIMATIONS_PER_ENTITY 20    //< maximum animations per entity
#define MAX_OWNER_NAME_LENGHT 32        //< max lenght of the key value
#define MAX_ANIMATION_FRAMES 16         //< max animation frames per clip

#pragma region CALLBACK FLAGS
typedef enum AnimationCallbackFlags{
    ANIM_CALL_NONE = 0,
    ANIM_CALL_WEAPON_START = 1 << 0,
    ANIM_CALL_WEAPON_ACTION = 1 << 1,
    ANIM_CALL_WEAPON_END = 1 << 2,
    ANIM_CALL_STEP = 1 << 3,
    ANIM_CALL_JUMP = 1 << 4,
    ANIM_CALL_LAND = 1 << 5
} AnimationCallbackFlags;
#pragma endregion

#pragma region STRUCTS
/// @brief animation play type (loop,single,pingpong...)
typedef enum ANIM_TYPE{
    ANIM_TYPE_LOOP,     //< play animation in loop 0->n and again from zero (forever)
    ANIM_TYPE_SINGLE,   //< play animation one time and then stop
    ANIM_TYPE_PINGPONG  //< play animation one time forward then backward and then stop
} ANIM_TYPE;

/// @brief Direction ENUM, 8 options. From right, anti-clockwise.
typedef enum DIRECTION{
    EAST, NORTH_EAST, NORTH, NORTH_WEST, WEST, SOUTH_WEST, SOUTH, SOUTH_EAST, DIRECTION_COUNT
} DIRECTION;

/// @brief structure with values used in each animation
typedef struct AnimationBaseData
{
    char name[32];                            //< name as ID
} AnimationBaseData;

/// @brief data for each frame
typedef struct FrameData{
    Sprite sprite;          //< sprite of this frame
} FrameData;

/// @brief one "direction" of frames (just an array)
typedef struct AnimationFrames
{
    FrameData frames[MAX_ANIMATION_FRAMES]; //< array of frames
    int frameCount;                         //< frames count
} AnimationFrames;

/// @brief struct of the 3D(8 dir) animation
typedef struct Animation3D
{
    AnimationBaseData data;                         //< base data for this animation
    AnimationFrames dirAnimations[DIRECTION_COUNT]; //< array of frames struct for all 8 directions
} Animation3D;


/// @brief all animations records of the object
typedef struct DatabaseRecord3DAnimation{
    char object[MAX_OWNER_NAME_LENGHT];                 //< name of the object(also used as a key)
    Animation3D animations[MAX_ANIMATIONS_PER_ENTITY];  //< 3D animations for this object
    int animationsCount;                                //< number of used animations
} DatabaseRecord3DAnimation;


/// @brief common data for animations
/// @warning shared between all animations with the same name!!!
typedef struct CommonAnimData{
    char animName[32];
    int speedMS;
    ANIM_TYPE animationType;
    u_int callbackFlags[MAX_ANIMATION_FRAMES];    //< flag bits (none, step, particle, action, ...)
} CommonAnimData;
#pragma endregion

#pragma region DECLARATIONS
/// @brief initialize animation system, ONLY ONE animation system exist at given time
/// @warning this will allocate a space on the HEAP - don't forget to call AnimationDestroy()
void AnimationInit(void);

/// @brief cleanup for animation system (destructor)
void AnimationDestroy(void);

/**
 * @brief Get the 3D Animation Database Record
 *
 * @param characterName the character which own returned record
 * @return DatabaseRecordAnimationDir* pointer to record or NULL
 */
const DatabaseRecord3DAnimation* AnimationGetCharacterData(char* characterName);

/**
 * @brief get given animation(melee,idle,...)
 *
 * @param animationCollection record of all animations of this object
 * @param animationName name of the animation
 * @return const Animation3D* animation pointer of NULL
 */
const Animation3D* Animation3DGetAnimation(DatabaseRecord3DAnimation* animationCollection, char* animationName);

/**
 * @brief get common animation data for given animations
 *
 * @param animName name of the animation (melee,idle,...)
 * @return CommonAnimData* common animation data pointer or NULL
 */
CommonAnimData* AnimationGetCommonAnimData(const char* animName);
#pragma endregion

#endif
