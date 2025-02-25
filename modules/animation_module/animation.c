/**
 * @file animation.c
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief complete system for animation
 * @version 0.1
 * @date 08-02-2025
 * @note only one animation system will be in entire game, need an animator to work
 * @copyright Copyright (c) 2025
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "animation.h"
#include "resource_manager.h"

#define MAX_ANIMATIONS_IN_DATABASE 500  //< maximum animations loaded at the same time
#define ANIMATION_SPEED_DEFAULT 10      //< default speed of animation (fps)
#define MAX_DIFF_ANIMS 30               //< maximum number of common animation data
#define CHARACTERS_PATH "resources/characters/"

#pragma region VARIABLES
static DatabaseRecord3DAnimation* databaseRecords3D;    //< 3D animations record collection
static int animationsInDatabaseCount;                   //< total animation records
static ResourceManagerTexture* resourceManager;         //< local resource manager
static CommonAnimData* commonAnimationsDatas;           //< shared common data
static int commonAnimationsDatasCount;                  //< count of shared common data
#pragma endregion

#pragma region DECLARATIONS
/**
 * @brief Get the Anim Type enum = parser
 *
 * @param text text representation of the animation type (loop,single,pingpong)
 * @return ANIM_TYPE enum of the animation type
 */
static ANIM_TYPE GetAnimType(char* text);

/**
 * @brief load character of the given name
 * @note file "characterName.kmtd" must be in: "resources/characters/characterName/"
 * @param characterName name of the character for load
 * @return DatabaseRecord3DAnimation* created record of the character or NULL
 */
static DatabaseRecord3DAnimation* AnimationLoadCharacter(char* characterName);

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
 * @return Animation3D* single animation of given name(key)
 */
static Animation3D* GetAnimationFromRecord(DatabaseRecord3DAnimation* animationRecord3D, char* animationName);

/**
 * @brief Set the Frame inside animation
 *
 * @param spriteData data about sprite in specific format (animationName_angle_spriteNameID,textureName,xPosOnTexture,yPosOnTexture,width,height,xOrigin,yOrigin)
 * @param objectFrames single animation data (no direction)
 */
static void AddFrame(char* entity, char* spriteData, AnimationFrames* objectFrames, char* characterName);

/**
 * @brief Create an animation Record
 * @bug created record is not set to zero - may cause bugs
 * @param entity name of the entity for a record
 * @return DatabaseRecord3DAnimation*
 */
static DatabaseRecord3DAnimation* CreateRecord(char* entity);

/**
 * @brief insert a frame at given object
 * @todo sort frames after each insert? maybe before use? just after all loads?, also change string object to some other type?
 * @param ad animation databese, where to put a frame
 * @param frameData data from textrurepacker
 * @note frameData format: "animationName_angle_spriteNameID,textureName,xPosOnTexture,yPosOnTexture,width,height,xOrigin,yOrigin" x and y origin are top left corner offset
 */
static void AnimationPush3DFrame(DatabaseRecord3DAnimation* ad, char* frameData, char* characterPath);

static void LoadDefaultCallbacks(char* path);
#pragma endregion

#pragma region API
CommonAnimData* AnimationGetCommonAnimData(const char* animName)
{
    for (int i = 0; i < commonAnimationsDatasCount; i++)
    {
        if(strcmp(animName, commonAnimationsDatas[i].animName) == 0)
        {
            return &commonAnimationsDatas[i];
        }
    }
    return NULL;
}

void AnimationInit(void)
{
    databaseRecords3D = MemAlloc(sizeof(DatabaseRecord3DAnimation) * MAX_ANIMATIONS_IN_DATABASE);
    if(databaseRecords3D == NULL)
    {
        TraceLog(LOG_ERROR, "memory allocation for animations records failed!");
        return;
    }
    resourceManager = MemAlloc(sizeof(ResourceManagerTexture));
    if(resourceManager == NULL)
    {
        TraceLog(LOG_ERROR, "memory allocation for animation resource manager failed!");
        free(databaseRecords3D);
        return;
    }
    commonAnimationsDatas = MemAlloc(sizeof(CommonAnimData) * MAX_DIFF_ANIMS);
    if(commonAnimationsDatas == NULL)
    {
        TraceLog(LOG_ERROR, "memory allocation for common animation data failed!");
        free(databaseRecords3D);
        free(resourceManager);
        return;
    }
    // common shared animation data(name,speed,animType)
    char* sharedDataPath = "resources/characters/characters.aed";
    FILE* f_animData = fopen(sharedDataPath, "r");
    if(f_animData == NULL)
    {
        TraceLog(LOG_ERROR, "character animation data file cant be opened: %s", sharedDataPath);
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
                comAnimData = &commonAnimationsDatas[commonAnimationsDatasCount];
                commonAnimationsDatasCount++;
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
        // load default callbacks
        char* defaultCallbacksPath = "resources/characters/characters.acd";
        LoadDefaultCallbacks(defaultCallbacksPath);
    }
}

void AnimationDestroy(void)
{
    free(resourceManager);
    resourceManager = NULL;
    free(databaseRecords3D);
    databaseRecords3D = NULL;
    free(commonAnimationsDatas);
    commonAnimationsDatas = NULL;
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

const Animation3D* Animation3DGetAnimation(DatabaseRecord3DAnimation* animationCollection, char* animationName)
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
#pragma endregion

#pragma region PRIVATE FNC
AnimationCallbackFlags GetCallbackFlag(char* text)
{
    if(strcmp(text, "weapon_start") == 0) return ANIM_CALL_WEAPON_START;
    if(strcmp(text, "weapon_action") == 0) return ANIM_CALL_WEAPON_ACTION;
    if(strcmp(text, "weapon_end") == 0) return ANIM_CALL_WEAPON_END;
    if(strcmp(text, "step") == 0) return ANIM_CALL_STEP;
    if(strcmp(text, "jump") == 0) return ANIM_CALL_JUMP;
    if(strcmp(text, "land") == 0) return ANIM_CALL_LAND;
    TraceLog(LOG_WARNING, "callback flag: \"%s\" not found!", text);
}

static void LoadDefaultCallbacks(char* path)
{
    FILE* f_defaultCallbacks = fopen(path, "r");
    if(f_defaultCallbacks == NULL)
    {
        TraceLog(LOG_ERROR, "can't load default animation callback file: %s", path);
        return;
    }
    char buffer[32] = {0};
    while (fgets(buffer, sizeof(buffer), f_defaultCallbacks))
    {
        // animation name
        char* token = strtok(buffer, ",");
        CommonAnimData* animData = AnimationGetCommonAnimData(token);
        // frame
        token = strtok(NULL, ",");
        int frame = atoi(token);
        // callback type
        token = strtok(NULL, ",");
        AnimationCallbackFlags flag = GetCallbackFlag(token);
        animData->callbackFlags[frame] |= flag;
    }
    fclose(f_defaultCallbacks);
}

static ANIM_TYPE GetAnimType(char* text)
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

static DatabaseRecord3DAnimation* AnimationLoadCharacter(char* characterName)
{
    char buffer[128] = CHARACTERS_PATH;
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
    if(ad == NULL)
    {
        fclose(f_character);
        return NULL;
    }
    char* line = NULL;
    size_t size = 0;
    while (getline(&line, &size, f_character) != -1)
    {
        AnimationPush3DFrame(ad, line, characterName);
    }
    free(line);
    fclose(f_character);
    return ad;
}

static void AnimationPush3DFrame(DatabaseRecord3DAnimation* ad, char* frameData, char* characterName)
{
    char frameDataBuffer[64];
    strcpy(frameDataBuffer, frameData);
    // animationDir
    char* token = strtok(frameDataBuffer, "_");
    Animation3D* anim = GetAnimationFromRecord(ad, token);

    // real direction from frameData
    token = strtok(NULL, "_");
    DIRECTION dir = GetDirectionEnum(atoi(token));

    //check if there is a frame in this direction
    token = strtok(NULL, ",");

    AddFrame(ad->object, token, &anim->dirAnimations[dir], characterName);
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

static Animation3D* GetAnimationFromRecord(DatabaseRecord3DAnimation* ad, char* animationName)
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

static void AddFrame(char* entity, char* token, AnimationFrames* frames, char* characterName)
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
    // bug: working just for characters
    char buffer[128] = CHARACTERS_PATH;
    strcat(buffer, characterName);
    strcat(buffer, "/");
    strcat(buffer, token);
    Texture2D texture = ResourceManagerGetTexture(resourceManager, buffer);
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
    if(animationsInDatabaseCount + 1 > MAX_ANIMATIONS_IN_DATABASE)
    {
        TraceLog(LOG_ERROR, "cant create another record, there is no room for more animations");
        return NULL;
    }
    strcpy(databaseRecords3D[animationsInDatabaseCount].object, entity);
    databaseRecords3D[animationsInDatabaseCount].animationsCount = 0;
    return &databaseRecords3D[animationsInDatabaseCount++];
}
#pragma endregion

