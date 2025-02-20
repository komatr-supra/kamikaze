#include "animator.h"
#include "timer.h"

void Animator3DFrameNext(void* animator);

void Animator3DCreate(Animator3D* animator, const DatabaseRecord3DAnimation* objectAnimations, DIRECTION direction)
{
    animator->direction = direction;
    animator->animations = objectAnimations;
    animator->data.currentFrame = 0;
    animator->data.speedMultiplier = ANIMATION_SPEED_DEFAULT; 
    animator->currentAnimation = NULL;
    TimerSet(40, -1, Animator3DFrameNext, animator);
}

void Animator3DFrameNext(void* animator)
{
    int frameNumber = ++((Animator3D*)animator)->data.currentFrame;
    if(frameNumber >= ((Animator3D*)animator)->currentAnimation->frames->frameCount)
    {
        ((Animator3D*)animator)->data.currentFrame = 0;
    }
}

void Animator3DDirectionSet(Animator3D* animator, DIRECTION dir)
{
    animator->direction = dir;
}

void Animator3DDraw(Animator3D* animator, float x, float y)
{
    Texture2D texture = animator->currentAnimation->frames[animator->direction].sprites[animator->data.currentFrame].texture;
    Rectangle rect = animator->currentAnimation->frames[animator->direction].sprites[animator->data.currentFrame].sourceRect;
    Vector2 orig = animator->currentAnimation->frames[animator->direction].sprites[animator->data.currentFrame].origin;
    Vector2 pos = {x - orig.x, y - orig.y};
    //Vector2 pos = {x, y};

    DrawTextureRec(texture, rect, pos, WHITE);
}

void Animator3DSetAnimation(Animator3D* animator, int animationIndex)
{
    animator->currentAnimation = &animator->animations->animations[animationIndex];
}