/**
 * @file animator.c
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief functions for animator module, each animated object should have its own animator (control animations, speed, callbacks, etc...)
 * @version 0.1
 * @date 21-02-2025
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "animator.h"
#include "timer.h"

#pragma region DECLARATIONS
/**
 * @brief move to the next frame(not nessesary mean frame++)
 * @details used as callback in timers! => not callded directly
 * @param animator animator, which should change frame
 */
const void Animator3DFrameNext(void* animator)
{
    Animator3D* animatorLocal = ((Animator3D*)animator);
    u_int flags = animatorLocal->sharedData->callbackFlags[animatorLocal->data.currentFrame];
    if(flags)
    {
        switch (flags)
        {
        case ANIM_CALL_ATK:
            TraceLog(LOG_DEBUG, "atk in animation: %s at frame %d, object ID: %u",
                animatorLocal->currentAnimation->data.name, animatorLocal->data.currentFrame, animatorLocal->data.ownerID);
            if(animatorLocal->data.callbacks[ANIMATOR_CALL_ACTION].callback != NULL) animatorLocal->data.callbacks[ANIMATOR_CALL_ACTION].callback(animatorLocal->data.callbacks[ANIMATOR_CALL_ACTION].callbackData);
            break;
        case ANIM_CALL_SPECIAL:
            TraceLog(LOG_DEBUG, "special in animation: %s at frame %d, object ID: %u",
                animatorLocal->currentAnimation->data.name, animatorLocal->data.currentFrame, animatorLocal->data.ownerID);
            if(animatorLocal->data.callbacks[ANIMATOR_CALL_SPECIAL].callback != NULL) animatorLocal->data.callbacks[ANIMATOR_CALL_SPECIAL].callback(animatorLocal->data.callbacks[ANIMATOR_CALL_SPECIAL].callbackData);
            break;
        case ANIM_CALL_SND:
            TraceLog(LOG_DEBUG, "sound in animation: %s at frame %d, object ID: %u",
                animatorLocal->currentAnimation->data.name, animatorLocal->data.currentFrame, animatorLocal->data.ownerID);
            if(animatorLocal->data.callbacks[ANIMATOR_CALL_SOUND].callback != NULL) animatorLocal->data.callbacks[ANIMATOR_CALL_SOUND].callback(animatorLocal->data.callbacks[ANIMATOR_CALL_SOUND].callbackData);
            break;
        case ANIM_CALL_PART:
            TraceLog(LOG_DEBUG, "particle in animation: %s at frame %d, object ID: %u",
                animatorLocal->currentAnimation->data.name, animatorLocal->data.currentFrame, animatorLocal->data.ownerID);
            if(animatorLocal->data.callbacks[ANIMATOR_CALL_PARTICLES].callback != NULL) animatorLocal->data.callbacks[ANIMATOR_CALL_PARTICLES].callback(animatorLocal->data.callbacks[ANIMATOR_CALL_PARTICLES].callbackData);
            break;
        case ANIM_CALL_START:
            TraceLog(LOG_DEBUG, "start event in animation: %s at frame %d, object ID: %u",
                animatorLocal->currentAnimation->data.name, animatorLocal->data.currentFrame, animatorLocal->data.ownerID);
            if(animatorLocal->data.callbacks[ANIMATOR_CALL_START].callback != NULL) animatorLocal->data.callbacks[ANIMATOR_CALL_START].callback(animatorLocal->data.callbacks[ANIMATOR_CALL_START].callbackData);
            break;
        case ANIM_CALL_END:
            TraceLog(LOG_DEBUG, "end event in animation: %s at frame %d, object ID: %u",
                animatorLocal->currentAnimation->data.name, animatorLocal->data.currentFrame, animatorLocal->data.ownerID);
            if(animatorLocal->data.callbacks[ANIMATOR_CALL_END].callback != NULL) animatorLocal->data.callbacks[ANIMATOR_CALL_END].callback(animatorLocal->data.callbacks[ANIMATOR_CALL_END].callbackData);
            break;
        default:
            TraceLog(LOG_DEBUG, "non specific callback in animation");
            break;
        }
    }
    switch (animatorLocal->sharedData->animationType)
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
        if(animatorLocal->data.isPingpongGoingBack)
        {
            --animatorLocal->data.currentFrame;
            if(animatorLocal->data.currentFrame == 0)
            {
                Animator3DStop(animatorLocal);
                animatorLocal->data.isPingpongGoingBack = false;
            }
        }
        else
        {
            ++animatorLocal->data.currentFrame;
            if(animatorLocal->data.currentFrame >= animatorLocal->currentAnimation->dirAnimations->frameCount)
            {
                animatorLocal->data.currentFrame = animatorLocal->currentAnimation->dirAnimations->frameCount - 1;
                animatorLocal->data.isPingpongGoingBack = true;
            }
        }
        break;
    }
}
#pragma endregion

#pragma region API
void Animator3DCreate(Animator3D* animator, const DatabaseRecord3DAnimation* objectAnimations, DIRECTION direction)
{
    animator->data.ownerID = 1;  //todo fix ID
    animator->direction = direction;
    animator->animations = objectAnimations;
    animator->data.currentFrame = 0;
    animator->data.speedMultiplier = ANIMATION_SPEED_DEFAULT;
    animator->currentAnimation = NULL;
    animator->data.speedMultiplier = ANIMATION_SPEED_DEFAULT;
    for (int i = 0; i < ANIMATOR_CALL_TOTAL_COUNT; i++)
    {
        animator->data.callbacks[i] = (AnimatorCallback){0};
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

    DrawTextureRec(texture, rect, pos, WHITE);
}

void Animator3DSetAnimation(Animator3D* animator, int animationIndex)
{
    if(animator->currentAnimation == &animator->animations->animations[animationIndex]) return;
    animator->currentAnimation = &animator->animations->animations[animationIndex];
    CommonAnimData* data = AnimationGetCommonAnimData(animator->currentAnimation->data.name);
    if(data == NULL)
    {
        TraceLog(LOG_ERROR, "cant find common data for animation: %s", animator->currentAnimation->data.name);
    }
    else
    {
        animator->sharedData = data;
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
    int frameTime = (int)(cad->speedMS * animator->data.speedMultiplier);
    TimerCancel(animator->data.timerHandle, false);
    animator->data.timerHandle = TimerSet(frameTime, -1, Animator3DFrameNext, animator);
    animator->data.isPingpongGoingBack = false;
}

void Animator3DStop(Animator3D* animator)
{
    if(TimerCancel(animator->data.timerHandle, false)) animator->data.timerHandle = 0;
}

void AnimatorSetCallback(AnimatorBaseData* animatorBaseData, AnimatorCallbackEnum callbackType, void(*callbackFunction)(void*), void* callbackData)
{
    animatorBaseData->callbacks[callbackType].callback = callbackFunction;
    animatorBaseData->callbacks[callbackType].callbackData = callbackData;
}
#pragma endregion