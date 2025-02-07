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

static AnimationDataDir* animationDatabaseDir;
static int animationsInDatabaseCount;


static AnimationDataDir* GetEntityDatabaseRecord(char* entity);
static DIRECTION GetDirectionEnum(int angleInDegree);
static AnimationDir* GetAnimationFromRecord(AnimationDataDir* animationData, char* animationName);
static void SetFrame(char* frameDataFrameNumber, AnimationFrames* frames);


void AnimationInit(void)
{
    animationDatabaseDir = MemAlloc(sizeof(AnimationDataDir) * MAX_ANIMATIONS_IN_DATABASE);
}
void AnimationDestroy(void)
{
    free(animationDatabaseDir);
}
void AnimationPushFrame(char* entity, char* frameData)
{
    AnimationDataDir* ad = GetEntityDatabaseRecord(entity);

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

}
static AnimationDataDir* GetEntityDatabaseRecord(char* entity)
{
    for (size_t i = 0; i < animationsInDatabaseCount; i++)
    {
        if(strcmp(animationDatabaseDir[i].ownerName, entity) == 0)
        {
            return &animationDatabaseDir[i];
        }
    }
    strcpy(animationDatabaseDir[animationsInDatabaseCount].ownerName, entity);
    return &animationDatabaseDir[animationsInDatabaseCount++];
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

    Sprite sprite;
    strcpy(sprite.name, token);
    token = strtok(NULL, ",");
    sprite.origin = (Vector2){};
    //frames->sprites[frames->frameCount]
}