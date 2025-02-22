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
#include <stdio.h>

#define MAX_ANIMATIONS_IN_DATABASE 500 /**< maximum animations loaded at the same time */
#define ANIMATION_SPEED_DEFAULT 20

#define MAX_DIFF_ANIMS 30



// ------------------------------ region VARIABLES
static DatabaseRecord3DAnimation* databaseRecords3D;   /**< 3D animations record collection */
static int animationsInDatabaseCount;   /**< total animation records */
static ResourceManagerTexture* resourceManager;    /**< local resource manager */
static CommonAnimData animationsDatas[MAX_DIFF_ANIMS];
static int animationsDatasCount;
// ------------------------------ endregion

// ------------------------------ region DECLARATIONS
ANIM_TYPE GetAnimType(char* text);

DatabaseRecord3DAnimation* AnimationLoadCharacter(char* characterName);

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
static AnimationDir* GetAnimationFromRecord(DatabaseRecord3DAnimation* animationRecord3D, char* animationName);

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
static DatabaseRecord3DAnimation* CreateRecord(char* entity);

/**
 * @brief insert a frame at given object
 * @todo sort frames after each insert? maybe before use? just after all loads?, also change string object to some other type?
 * @param ad animation databese, where to put a frame
 * @param frameData data from textrurepacker
 * @note frameData format: "animationName_angle_spriteNameID,textureName,xPosOnTexture,yPosOnTexture,width,height,xOrigin,yOrigin" x and y origin are top left corner offset
 */
static void AnimationPush3DFrame(DatabaseRecord3DAnimation* ad, char* frameData);
// ------------------------------ endregion

// ------------------------------ region API

CommonAnimData* AnimationGetCommonAnimData(const char* animName)
{
    for (int i = 0; i < animationsDatasCount; i++)
    {
        if(strcmp(animName, animationsDatas[i].animName) == 0)
        {
            return &animationsDatas[i];
        }
    }
    return NULL;
}

void AnimationInit()
{
    databaseRecords3D = MemAlloc(sizeof(DatabaseRecord3DAnimation) * MAX_ANIMATIONS_IN_DATABASE);
    resourceManager = MemAlloc(sizeof(ResourceManagerTexture));
    // other animation data(name,speed,animType)
    char* buffer = "resources/characters/characters.aed";
    FILE* f_animData = fopen(buffer, "r");
    if(f_animData == NULL)
    {
        TraceLog(LOG_ERROR, "character animation data file cant be opened: %s", buffer);
    }
    else
    {
        char* line = NULL;
        size_t size = 0;
        while (getline(&line, &size, f_animData) != -1)
        {
            char animDataBuffer[32];
            strcpy(animDataBuffer, line);
            //name
            char* token = strtok(animDataBuffer, ",");
            CommonAnimData* comAnimData = AnimationGetCommonAnimData(token);
            if(comAnimData == NULL)
            {
                comAnimData = &animationsDatas[animationsDatasCount];
                animationsDatasCount++;
                strcpy(comAnimData->animName, token);
                // speed
                token = strtok(NULL, ",");
                int animSpeed = atoi(token);
                if(animSpeed == -1) animSpeed = ANIMATION_SPEED_DEFAULT;
                comAnimData->speedMS = 1000 / animSpeed;
                // type
                token = strtok(NULL, "\n");
                comAnimData->animationType = GetAnimType(token);
            }
            else TraceLog(LOG_WARNING, "duplication common animation data: %s", comAnimData->animName);
        }
        free(line);
        fclose(f_animData);
    }
}

void AnimationDestroy()
{
    free(resourceManager);
    resourceManager = NULL;
    free(databaseRecords3D);
    databaseRecords3D = NULL;
}

const DatabaseRecord3DAnimation* AnimationGetCharacterData(char* characterName)
{

    for (int i = 0; i < animationsInDatabaseCount; i++)
    {
        if(strcmp(databaseRecords3D[i].object, characterName) == 0)
        {
            return &databaseRecords3D[i];
        }
    }
    return AnimationLoadCharacter(characterName);
}

const AnimationDir* Animation3DGetAnimation(DatabaseRecord3DAnimation* animationCollection, char* animationName)
{
    for (size_t i = 0; i < animationCollection->animationsCount; i++)
    {
        if(strcmp(animationCollection->animations[i].data.name, animationName) == 0)
        {
            return &animationCollection->animations[i];
        }
    }
    return NULL;
}

// ------------------------------ endregion

// ------------------------------ region PRIVATE FNC
ANIM_TYPE GetAnimType(char* text)
{
    if(strcmp(text, "loop") == 0) return ANIM_TYPE_LOOP;
    else if(strcmp(text, "single") == 0) return ANIM_TYPE_SINGLE;
    else if(strcmp(text, "pingpong") == 0) return ANIM_TYPE_PINGPONG;
    else
    {
        TraceLog(LOG_DEBUG, "anim type cant be found, set to loop");
        return ANIM_TYPE_LOOP;
    }
}


DatabaseRecord3DAnimation* AnimationLoadCharacter(char* characterName)
{
    char buffer[128] = "resources/characters/";
    strcat(buffer, characterName);
    strcat(buffer, "/");
    strcat(buffer, characterName);
    strcat(buffer, ".txt");
    FILE* f_character = fopen(buffer, "r");
    if(f_character == NULL)
    {
        TraceLog(LOG_ERROR, "character file cant be opened: %s", buffer);
        return NULL;
    }
    DatabaseRecord3DAnimation* ad = CreateRecord(characterName);
    char* line = NULL;
    size_t size = 0;
    while (getline(&line, &size, f_character) != -1)
    {
        AnimationPush3DFrame(ad, line);
    }
    free(line);
    fclose(f_character);
    return ad;
}

static void AnimationPush3DFrame(DatabaseRecord3DAnimation* ad, char* frameData)
{
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

    AddFrame(ad->object, token, &anim->dirAnimations[dir]);
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
static AnimationDir* GetAnimationFromRecord(DatabaseRecord3DAnimation* ad, char* animationName)
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
        ad->animations[ad->animationsCount].dirAnimations[i].frameCount = 0;
    }


    return &ad->animations[ad->animationsCount++];
}
static void AddFrame(char* entity, char* token, AnimationFrames* frames)
{
    // if it exist, just skip
    for (size_t i = 0; i < frames->frameCount; i++)
    {
        if(strcmp(frames->frames[i].sprite.name, token) == 0)
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
    //TraceLog(LOG_INFO, "%s : %d", entity, frames->frameCount);
    int comparedSpriteIndex = frames->frameCount - 1;
    while (comparedSpriteIndex >= 0 && strcmp(frames->frames[comparedSpriteIndex].sprite.name, sprite.name) > 0)
    {
        frames->frames[comparedSpriteIndex + 1].sprite = frames->frames[comparedSpriteIndex].sprite;
        comparedSpriteIndex--;
    }
    frames->frames[comparedSpriteIndex + 1].sprite = sprite;
    frames->frameCount++;
}

static DatabaseRecord3DAnimation* CreateRecord(char* entity)
{
    strcpy(databaseRecords3D[animationsInDatabaseCount].object, entity);
    databaseRecords3D[animationsInDatabaseCount].animationsCount = 0;

    return &databaseRecords3D[animationsInDatabaseCount++];
}

void AnimationGetAnimationsNames(char* buffer, DatabaseRecord3DAnimation* animationCollection)
{
    for (int i = 0; i < animationCollection->animationsCount; i++)
    {
        strcpy(buffer, animationCollection->animations[i].data.name);
    }
}
// ------------------------------ endregion

