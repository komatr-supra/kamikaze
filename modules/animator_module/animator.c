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
#include <string.h>
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
    Animator3D* animatorLocal = (Animator3D*)animator;
    // callback flags
    u_int flags = animatorLocal->sharedData->callbackFlags[animatorLocal->data.currentFrame];
    if (flags)
    {
        for (int i = 2; i < ANIMATION_CALLBACK_FLAGS_COUNT; i++)
        {
            if (flags & (1 << i))
            {
                TraceLog(LOG_DEBUG, "%s in animation: %s at frame %d, object ID: %u",
                    AnimationFlagCallbackNames[i], animatorLocal->currentAnimation->data.name,
                    animatorLocal->data.currentFrame, animatorLocal->data.ownerID);

                if (animatorLocal->data.callbacks[i].callback != NULL)
                {
                    animatorLocal->data.callbacks[i].callback(
                        animatorLocal->data.callbacks[i].callbackData
                    );
                }
            }
        }
    }
    // frame
    size_t frameCount = animatorLocal->currentAnimation->dirAnimations->frameCount;
    switch (animatorLocal->sharedData->animationType)
    {
        case ANIM_TYPE_LOOP:
            ++animatorLocal->data.currentFrame;
            if (animatorLocal->data.currentFrame >= frameCount)
            {
                animatorLocal->data.currentFrame = 0;
            }
            break;

        case ANIM_TYPE_SINGLE:
            ++animatorLocal->data.currentFrame;
            if (animatorLocal->data.currentFrame >= frameCount)
            {
                animatorLocal->data.currentFrame = frameCount - 1;
                Animator3DStop(animatorLocal);
            }
            break;

        case ANIM_TYPE_PINGPONG:
            if (animatorLocal->data.isPingpongGoingBack)
            {
                --animatorLocal->data.currentFrame;
                if (animatorLocal->data.currentFrame == 0)
                {
                    Animator3DStop(animatorLocal);
                    animatorLocal->data.isPingpongGoingBack = false;
                }
            }
            else
            {
                ++animatorLocal->data.currentFrame;
                if (animatorLocal->data.currentFrame >= frameCount)
                {
                    animatorLocal->data.currentFrame = frameCount - 1;
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
    *animator = (Animator3D){
        .data = {
            .callbacks = {0},
            .currentFrame = 0,
            .isPingpongGoingBack = false,
            .ownerID = 0,
            .speedMultiplier = ANIMATOR_SPEED_DEFAULT,
            .timerHandle = 0
        },
        .animations = objectAnimations,
        .currentAnimation = NULL,
        .direction = direction,
        .sharedData = NULL
    };
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
    if(animationIndex < 0 || animationIndex >= animator->animations->animationsCount)
    {
        TraceLog(LOG_ERROR, "want set animation in animator to index: %d, for character id: %u, maximum index is: %d", animationIndex, animator->data.ownerID, animator->animations->animationsCount - 1);
        return;
    }
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
    if(animator->data.callbacks[0].callback != NULL) animator->data.callbacks[0].callback(animator->data.callbacks[0].callbackData);
}

void Animator3DStop(Animator3D* animator)
{
    if(TimerCancel(animator->data.timerHandle, false))
    {
        if(animator->data.callbacks[1].callback != NULL) animator->data.callbacks[1].callback(animator->data.callbacks[1].callbackData);
        animator->data.timerHandle = 0;
    }
}

void AnimatorSetCallback(AnimatorBaseData* animatorBaseData, AnimationCallbackFlags callbackType, void(*callbackFunction)(void*), void* callbackData)
{
    int index = 0;
    while ((callbackType >>= 1) > 0) index++;

    animatorBaseData->callbacks[index].callback = callbackFunction;
    animatorBaseData->callbacks[index].callbackData = callbackData;
}

int AnimatorGetAnimationIndex(Animator3D* animator, const char* animationName)
{
    for(int i = 0; i < animator->animations->animationsCount; i++)
    {
        if(strcmp(animator->animations->animations[i].data.name, animationName) == 0) return i;
    }
    return -1;
}
#pragma endregion