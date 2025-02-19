/**
 * structure for keeping track of current character animation
 * save variables for right draw
 * each character got this structure
 */
#include <stdlib.h>
#include "raylib.h"
#include "animation.h"

#define ANIMATION_SPEED_DEFAULT 1

typedef struct AnimatorBaseData
{
    int speedMultiplier;
    int currentFrame;
} AnimatorBaseData;

typedef struct Animator3D
{
    DIRECTION direction;
    AnimatorBaseData data;
    DatabaseRecord3DAnimation* animations;
    AnimationDir* currentAnimation;
} Animator3D;

void Animator3DCreate(Animator3D* animator, DatabaseRecord3DAnimation* objectAnimations, DIRECTION direction)
{
    animator->direction = direction;
    animator->animations = objectAnimations;
    animator->data.currentFrame = 0;
    animator->data.speedMultiplier = ANIMATION_SPEED_DEFAULT; 
    animator->currentAnimation = NULL;
}

void Animator3DFrameNext(Animator3D* animator)
{
    int frameNumber = animator->data.currentFrame;
    frameNumber++;
    if(frameNumber >= animator->currentAnimation->frames->frameCount) frameNumber = 0;
}

void Animator3DDirectionSet(Animator3D* animator, DIRECTION dir)
{
    animator->direction = dir;
}

void Animator3DDraw(Animator3D* animator, float x, float y)
{
    Texture2D texture = animator->currentAnimation->frames->sprites[animator->data.currentFrame].texture;
    Rectangle rect = animator->currentAnimation->frames->sprites[animator->data.currentFrame].sourceRect;
    Vector2 orig = animator->currentAnimation->frames->sprites[animator->data.currentFrame].origin;
    Vector2 pos = {x + orig.x, y + orig.y};
    DrawTextureRec(texture, rect, pos, WHITE);
}