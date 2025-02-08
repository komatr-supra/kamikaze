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
#define MAX_ANIMATIONS_PER_ENTITY 20 /**< maximum animations per entity */
#define MAX_OWNER_NAME_LENGHT 32    /**< max lenght of the key value */

/**
 * @brief 3D animation record struct, all animation of given object
 *
 */
typedef struct DatabaseRecordAnimationDir{
    char object[MAX_OWNER_NAME_LENGHT]; /**< same as a key for this record */
    AnimationDir animations[MAX_ANIMATIONS_PER_ENTITY]; /**< animations of this object - directions only */
    int animationsCount;    /**< total count of animations of this object */
} DatabaseRecordAnimationDir;

#pragma region VARIABLES
static DatabaseRecordAnimationDir* databaseRecords3D;   /**< 3D animations record collection */
static int animationsInDatabaseCount;   /**< total animation records */
static ResourceManagerTexture* resourceManager;    /**< local resource manager */
#pragma endregion

#pragma region DECLARATIONS

/**
 * @brief Get the Entity Database Record object
 *
 * @param entity the entity which own returned record
 * @return DatabaseRecordAnimationDir* pointer to record
 */
static DatabaseRecordAnimationDir* GetEntityDatabaseRecord(char* entity);

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

static DatabaseRecordAnimationDir* CreateRecord(char* entity);
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

void AnimationPush3DFrame(char* entity, char* frameDataConst)
{
    DatabaseRecordAnimationDir* ad = GetEntityDatabaseRecord(entity);
    if(ad == NULL) ad = CreateRecord(entity);
    char frameData[64];
    strcpy(frameData, frameDataConst);
    // animationDir
    char* token = strtok(frameData, "_");
    AnimationDir* anim = GetAnimationFromRecord(ad, token);

    // real direction from frameData
    token = strtok(NULL, "_");
    DIRECTION dir = GetDirectionEnum(atoi(token));

    //check if there is a frame in this direction
    token = strtok(NULL, ",");

    AddFrame(entity, token, &anim->frames[dir]);
}

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
// just for sorting sprites inside animation
static int CompareFrameNames(const void* a, const void* b)
{
    return strcmp(((Sprite*)a)->name, ((Sprite*)b)->name);
}

void Animation3DSortFrames(char* object)
{
    DatabaseRecordAnimationDir* ad = GetEntityDatabaseRecord(object);
    if(ad == NULL) return;
    for (size_t i = 0; i < ad->animationsCount; i++)
    {
        for (size_t j = 0; j < DIRECTION_COUNT; j++)
        {
            qsort(ad->animations[i].frames[j].sprites, ad->animations[i].frames[j].frameCount, sizeof(Sprite), CompareFrameNames);
        }
    }

}
#pragma endregion

#pragma region PRIVATE FNC
static DatabaseRecordAnimationDir* GetEntityDatabaseRecord(char* entity)
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
    frames->sprites[frames->frameCount++] = sprite;
}

static DatabaseRecordAnimationDir* CreateRecord(char* entity)
{
    strcpy(databaseRecords3D[animationsInDatabaseCount].object, entity);
    databaseRecords3D[animationsInDatabaseCount].animationsCount = 0;
    return &databaseRecords3D[animationsInDatabaseCount++];
}
#pragma endregion