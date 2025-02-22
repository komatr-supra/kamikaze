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
    animator->isRunning = false;

}

void Animator3DFrameNext(void* animator)
{
    Animator3D* animatorLocal = ((Animator3D*)animator);
    switch (animatorLocal->currentAnimType)
    {
        case ANIM_TYPE_LOOP:
        ++animatorLocal->data.currentFrame;
        if(animatorLocal->data.currentFrame >= animatorLocal->currentAnimation->dirAnimations->frameCount)
        {
            animatorLocal->data.currentFrame = 0;
        }
        break;
    case ANIM_TYPE_SINGLE:
        ++animatorLocal->data.currentFrame;
        if(animatorLocal->data.currentFrame >= animatorLocal->currentAnimation->dirAnimations->frameCount)
        {
            animatorLocal->data.currentFrame = animatorLocal->currentAnimation->dirAnimations->frameCount - 1;
            Animator3DStop(animatorLocal);
        }
        break;
    case ANIM_TYPE_PINGPONG:
        if(animatorLocal->isPingpongGoingBack)
        {
            --animatorLocal->data.currentFrame;
            if(animatorLocal->data.currentFrame == 0)
            {
                Animator3DStop(animatorLocal);
                animatorLocal->isPingpongGoingBack = false;
            }
        }
        else
        {
            ++animatorLocal->data.currentFrame;
            if(animatorLocal->data.currentFrame >= animatorLocal->currentAnimation->dirAnimations->frameCount)
            {
                animatorLocal->data.currentFrame = animatorLocal->currentAnimation->dirAnimations->frameCount - 1;
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

void Animator3DSetAnimation(Animator3D* animator, int animationIndex)
{
    //TimerCancel(animator->timerHandle, false);
    if(animator->currentAnimation == &animator->animations->animations[animationIndex]) return;
    animator->currentAnimation = &animator->animations->animations[animationIndex];
    CommonAnimData* data = AnimationGetCommonAnimData(animator->currentAnimation->data.name);
    if(data == NULL)
    {
        TraceLog(LOG_ERROR, "cant find common data for animation: %s", animator->currentAnimation->data.name);
        animator->currentAnimType = ANIM_TYPE_LOOP;
    }
    else
    {
        animator->currentAnimType = data->animationType;
    }
    animator->data.currentFrame = 0;
    Animator3DStart(animator);
}

void Animator3DStart(Animator3D* animator)
{
    CommonAnimData* cad = AnimationGetCommonAnimData(animator->currentAnimation->data.name);
    if(cad == NULL)
    {
        TraceLog(LOG_ERROR, "cant find common animation data for: %s", animator->currentAnimation->data.name);
        return;
    }
    int frameTime = (int)(cad->speedMS * animator->speedMultiplier);
    TimerCancel(animator->timerHandle, false);
    animator->timerHandle = TimerSet(frameTime, -1, Animator3DFrameNext, animator);
    animator->isRunning = true;
    animator->isPingpongGoingBack = false;
}

void Animator3DStop(Animator3D* animator)
{
    animator->isRunning = false;
    TimerCancel(animator->timerHandle, false);
    animator->timerHandle = 0;
}