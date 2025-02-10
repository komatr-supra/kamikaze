/**
 * @file animation.c
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief complete system for animation
 * @version 0.1
 * @date 08-02-2025
 * @note only one animation system will be in entire game, need an animator to work
 * @copyright Copyright (c) 2025
 * @todo better collections than array, with quicker access
 */
#include "animation.h"
#include "string.h"
#include "stdlib.h"
#include "resource_manager.h"

#define MAX_ANIMATIONS_IN_DATABASE 500 /**< maximum animations loaded at the same time */



#pragma region VARIABLES
static DatabaseRecordAnimationDir* databaseRecords3D;   /**< 3D animations record collection */
static int animationsInDatabaseCount;   /**< total animation records */
static ResourceManagerTexture* resourceManager;    /**< local resource manager */
#pragma endregion

#pragma region DECLARATIONS

/**
 * @brief Transform degrees to one of 8 degrees defined by iso view.
 *
 * @param angleInDegree angle of the sprite in degrees - NOT a radiant!
 * @return DIRECTION enum representation of direction
 */
static DIRECTION GetDirectionEnum(int angleInDegree);

/**
 * @brief Get the 3D Animation From Record
 *
 * If there is NOT any record, then it will be created.
 *
 * @param animationRecord3D record of 3D animation
 * @param animationName name of required animation
 * @todo create collection with better find access time
 * @return AnimationDir* single animation of given name(key)
 */
static AnimationDir* GetAnimationFromRecord(DatabaseRecordAnimationDir* animationRecord3D, char* animationName);

/**
 * @brief Set the Frame inside animation
 *
 * @param spriteData data about sprite in specific format (animationName_angle_spriteNameID,textureName,xPosOnTexture,yPosOnTexture,width,height,xOrigin,yOrigin)
 * @param objectFrames single animation data (no direction)
 */
static void AddFrame(char* entity, char* spriteData, AnimationFrames* objectFrames);

/// @brief 
/// @param entity 
/// @return 
static DatabaseRecordAnimationDir* CreateRecord(char* entity);

/**
 * @brief insert a frame at given object
 * @todo sort frames after each insert? maybe before use? just after all loads?, also change string object to some other type?
 * @param object name of the object (this objects will share the animation)
 * @param frameData data from textrurepacker
 * @note frameData format: "animationName_angle_spriteNameID,textureName,xPosOnTexture,yPosOnTexture,width,height,xOrigin,yOrigin" x and y origin are top left corner offset
 */
static void AnimationPush3DFrame(char* object, char* frameData);
#pragma endregion

#pragma region API

void AnimationInit()
{
    databaseRecords3D = MemAlloc(sizeof(DatabaseRecordAnimationDir) * MAX_ANIMATIONS_IN_DATABASE);
    resourceManager = MemAlloc(sizeof(ResourceManagerTexture));
}

void AnimationDestroy()
{
    free(resourceManager);
    resourceManager = NULL;
    free(databaseRecords3D);
    databaseRecords3D = NULL;
}

DatabaseRecordAnimationDir* AnimationGetEntityData(char* entity)
{
    for (size_t i = 0; i < animationsInDatabaseCount; i++)
    {
        if(strcmp(databaseRecords3D[i].object, entity) == 0)
        {
            return &databaseRecords3D[i];
        }
    }
    return NULL;
}

void AnimationLoadCharacter(char* characterName)
{

}
#pragma endregion

#pragma region PRIVATE FNC

const AnimationDir* Animation3DGetFromDatabase(char* entity, char* animationName)
{
    for (size_t i = 0; i < animationsInDatabaseCount; i++)
    {
        if(strcmp(databaseRecords3D[i].object, entity) == 0)
        {
            for (size_t j = 0; j < databaseRecords3D[i].animationsCount; j++)
            {
                if(strcmp(databaseRecords3D[i].animations[j].data.name, animationName) == 0)
                {
                    return &databaseRecords3D[i].animations[j];
                }
            }

        }
    }
    return NULL;
}

static void AnimationPush3DFrame(char* object, char* frameData)
{
    DatabaseRecordAnimationDir* ad = AnimationGetEntityData(object);
    if(ad == NULL) ad = CreateRecord(object);
    char frameDataBuffer[64];
    strcpy(frameDataBuffer, frameData);
    // animationDir
    char* token = strtok(frameDataBuffer, "_");
    AnimationDir* anim = GetAnimationFromRecord(ad, token);

    // real direction from frameData
    token = strtok(NULL, "_");
    DIRECTION dir = GetDirectionEnum(atoi(token));

    //check if there is a frame in this direction
    token = strtok(NULL, ",");

    AddFrame(object, token, &anim->frames[dir]);
}

static DIRECTION GetDirectionEnum(int angleInDegree)
{
    switch (angleInDegree)
    {
    case 0: return EAST;
    case 45: return NORTH_EAST;
    case 90: return NORTH;
    case 135: return NORTH_WEST;
    case 180: return WEST;
    case 225: return SOUTH_WEST;
    case 270: return SOUTH;
    default: return SOUTH_EAST;
    }
}
static AnimationDir* GetAnimationFromRecord(DatabaseRecordAnimationDir* ad, char* animationName)
{
    for (size_t i = 0; i < ad->animationsCount; i++)
    {
        if(strcmp(ad->animations[i].data.name, animationName) == 0)
        {
            return &ad->animations[i];
        }
    }
    strcpy(ad->animations[ad->animationsCount].data.name, animationName);
    for (size_t i = 0; i < 8; i++)
    {
        ad->animations[ad->animationsCount].frames[i].frameCount = 0;
    }


    return &ad->animations[ad->animationsCount++];
}
static void AddFrame(char* entity, char* token, AnimationFrames* frames)
{
    // if it exist, just skip
    for (size_t i = 0; i < frames->frameCount; i++)
    {
        if(strcmp(frames->sprites[i].name, token) == 0)
        {
            return;
        }
    }
    // is there a space for a new sprite?
    if((frames->frameCount + 1) > MAX_ANIMATION_FRAMES)
    {
        TraceLog(LOG_ERROR, "no space for sprite in animation");
        return;
    }
    // create a new one
    Sprite sprite;
    strcpy(sprite.name, token);
    // texture
    token = strtok(NULL, ",");
    Texture2D texture = ResourceManagerGetTexture(resourceManager, token);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    sprite.texture = texture;
    // rectangle in texture for this sprite
    Rectangle rect = {.x = atoi(strtok(NULL, ",")), .y = atoi(strtok(NULL, ",")), .width = atoi(strtok(NULL, ",")), .height = atoi(strtok(NULL, ","))};
    sprite.sourceRect = rect;
    // origin of the sprite
    sprite.origin.x = atoi(strtok(NULL, ","));
    sprite.origin.y = atoi(strtok(NULL, ","));
    TraceLog(LOG_INFO, "%s : %d", entity, frames->frameCount);
    // sort it
    int comparedSpriteIndex = frames->frameCount - 1;
    while (comparedSpriteIndex >= 0 && strcmp(sprite.name, frames->sprites[comparedSpriteIndex].name) > 0)
    {
        frames->sprites[comparedSpriteIndex + 1] = frames->sprites[comparedSpriteIndex--];
    }
    // everything is shifted
    frames->sprites[comparedSpriteIndex + 1] = sprite;
}

static DatabaseRecordAnimationDir* CreateRecord(char* entity)
{
    strcpy(databaseRecords3D[animationsInDatabaseCount].object, entity);
    databaseRecords3D[animationsInDatabaseCount].animationsCount = 0;
    return &databaseRecords3D[animationsInDatabaseCount++];
}
#pragma endregion