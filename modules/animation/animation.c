/**
 * animation.c
 */
#include "animation.h"
#include "string.h"
#include "stdlib.h"
#include "resource_manager.h"

#define MAX_ANIMATIONS_IN_DATABASE 500
#define MAX_ANIMATIONS_PER_ENTITY 20
#define MAX_OWNER_NAME_LENGHT 32

typedef struct AnimationDataDir{
    char ownerName[MAX_OWNER_NAME_LENGHT];
    AnimationDir animations[MAX_ANIMATIONS_PER_ENTITY];
    int animationsCount;
} AnimationDataDir;

static AnimationDataDir* animationDatabaseDirs;
static int animationsInDatabaseCount;
static ResourceManager* resourceManager;

static AnimationDataDir* GetEntityDatabaseRecord(char* entity);
static DIRECTION GetDirectionEnum(int angleInDegree);
static AnimationDir* GetAnimationFromRecord(AnimationDataDir* animationData, char* animationName);
static void SetFrame(char* frameDataFrameNumber, AnimationFrames* frames);


void AnimationInit()
{
    animationDatabaseDirs = MemAlloc(sizeof(AnimationDataDir) * MAX_ANIMATIONS_IN_DATABASE);
    resourceManager = MemAlloc(sizeof(ResourceManager));
}
void AnimationDestroy()
{
    free(resourceManager);
    free(animationDatabaseDirs);
}
void AnimationPushFrame(char* entity, char* frameDataConst)
{
    AnimationDataDir* ad = GetEntityDatabaseRecord(entity);
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

    SetFrame(token, &anim->frames[dir]);
}
AnimationDir* AnimationGetFromDatabase(char* entity, char* animationName)
{
    for (size_t i = 0; i < animationsInDatabaseCount; i++)
    {
        if(strcmp(animationDatabaseDirs[i].ownerName, entity) == 0)
        {
            for (size_t j = 0; j < animationDatabaseDirs[i].animationsCount; j++)
            {
                if(strcmp(animationDatabaseDirs[i].animations[j].data.name, animationName) == 0)
                {
                    return &animationDatabaseDirs[i].animations[j];
                }
            }
            
        }
    }
        
}
static AnimationDataDir* GetEntityDatabaseRecord(char* entity)
{
    for (size_t i = 0; i < animationsInDatabaseCount; i++)
    {
        if(strcmp(animationDatabaseDirs[i].ownerName, entity) == 0)
        {
            return &animationDatabaseDirs[i];
        }
    }
    strcpy(animationDatabaseDirs[animationsInDatabaseCount].ownerName, entity);
    return &animationDatabaseDirs[animationsInDatabaseCount++];
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
static AnimationDir* GetAnimationFromRecord(AnimationDataDir* ad, char* animationName)
{
    for (size_t i = 0; i < ad->animationsCount; i++)
    {
        if(strcmp(ad->animations[i].data.name, animationName) == 0)
        {
            return &ad->animations[i];
        }
    }
    strcpy(ad->animations[ad->animationsCount].data.name, animationName);
    return &ad->animations[ad->animationsCount++];
}
static void SetFrame(char* token, AnimationFrames* frames)
{
    for (size_t i = 0; i < frames->frameCount; i++)
    {
        if(strcmp(frames->sprites[i].name, token) == 0)
        {
            return;
        }
    }
    // frame was not found
    Sprite sprite;
    strcpy(sprite.name, token);
    // texture
    token = strtok(NULL, ",");
    Texture2D texture = ResourceManagerGetTexture(resourceManager, token);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    sprite.texture = texture;
    Rectangle rect = {.x = atoi(strtok(NULL, ",")), .y = atoi(strtok(NULL, ",")), .width = atoi(strtok(NULL, ",")), .height = atoi(strtok(NULL, ","))};
    sprite.sourceRect = rect;
    sprite.origin.x = atoi(strtok(NULL, ","));
    sprite.origin.y = atoi(strtok(NULL, ","));
    frames->sprites[frames->frameCount] = sprite;
}