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
    animator->speedMultiplier = 1.0f;
}

void Animator3DFrameNext(void* animator)
{
    Animator3D* animatorLocal = ((Animator3D*)animator);
    int frameNumber = 0;
    switch (animatorLocal->currentAnimType)
    {
        case ANIM_TYPE_LOOP:
        frameNumber = ++animatorLocal->data.currentFrame;
        if(frameNumber >= animatorLocal->currentAnimation->dirAnimations->frameCount)
        {
            animatorLocal->data.currentFrame = 0;
        }
        break;
    case ANIM_TYPE_SINGLE:
        frameNumber = ++animatorLocal->data.currentFrame;
        if(frameNumber >= animatorLocal->currentAnimation->dirAnimations->frameCount)
        {
            animatorLocal->data.currentFrame = animatorLocal->currentAnimation->dirAnimations->frameCount - 1;
            Animator3DStop(animatorLocal);
        }
        break;
    case ANIM_TYPE_PINGPONG:
        if(animatorLocal->isPingpongGoingBack)
        {
            frameNumber = --animatorLocal->data.currentFrame;
            if(frameNumber == 0)
            {
                Animator3DStop(animatorLocal);
                animatorLocal->isPingpongGoingBack = false;
            }
        }
        else
        {
            frameNumber = ++animatorLocal->data.currentFrame;
            if(frameNumber >= animatorLocal->currentAnimation->dirAnimations->frameCount)
            {
                animatorLocal->isPingpongGoingBack = true;
            }
        }

        break;
    }

}

void Animator3DDirectionSet(Animator3D* animator, DIRECTION dir)
{
    animator->direction = dir;
}

void Animator3DDraw(Animator3D* animator, float x, float y)
{
    Texture2D texture = animator->currentAnimation->dirAnimations[animator->direction].frames[animator->data.currentFrame].sprite.texture;
    Rectangle rect = animator->currentAnimation->dirAnimations[animator->direction].frames[animator->data.currentFrame].sprite.sourceRect;
    Vector2 orig = animator->currentAnimation->dirAnimations[animator->direction].frames[animator->data.currentFrame].sprite.origin;
    Vector2 pos = {x - orig.x, y - orig.y};
    //Vector2 pos = {x, y};

    DrawTextureRec(texture, rect, pos, WHITE);
}

static size_t Run(Animator3D* animator)
{
    int frameTime = (int)(AnimationGetCommonAnimData(animator->animations->object, NULL)->speedMS * animator->speedMultiplier);
    animator->timerHandle = TimerSet(frameTime, -1, Animator3DFrameNext, animator);
    animator->data.currentFrame = 0;
}

void Animator3DSetAnimation(Animator3D* animator, int animationIndex)
{
    animator->currentAnimation = &animator->animations->animations[animationIndex];
    animator->currentAnimType = AnimationGetCommonAnimData(animator->currentAnimation->data.name, NULL)->animationType;
    Animator3DStart(animator);
}

void Animator3DStart(Animator3D* animator)
{
    if(animator->isRunning) return;
    Run(animator);
}

void Animator3DStop(Animator3D* animator)
{
    animator->isRunning = false;
    TimerCancel(animator->timerHandle, false);
    animator->timerHandle = 0;
}