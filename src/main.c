#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"
#include "map_manager.h"
#include "animation.h"
#include "timer.h"
#include "animator.h"

static int testDamage = 8;

void AttackTestFNC(void* data)
{
    TraceLog(LOG_INFO, "attacking for: %d", *(int*)data);
}

void TimerTestPrintRepeat(void* data)
{
    int numberTest = -1;
    if(data != NULL)
    {
        numberTest = *(int*)data;
        *(int*)data += 1;
    }
    TraceLog(LOG_INFO, "forever timer %d", numberTest);
}

void TimerTestPrintTimes(void* data)
{
    TraceLog(LOG_INFO, "times timer");
}

int main(void)
{
    const int screenWidth = 600;
    const int screenHeight = 400;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Kamikaze");

    Vector2 playerPosition = { (float)screenWidth/2, (float)screenHeight/2 };

    Camera2D camera = { 0 };
    camera.offset = (Vector2){300, 200};
    camera.target = playerPosition;
    camera.zoom = 2.0f;

    SetTargetFPS(60);

    //timer tests
SetTraceLogLevel(LOG_DEBUG);
    TimerInit();
    AnimationInit();
    const DatabaseRecord3DAnimation* data = AnimationGetCharacterData("knight");
    Animator3D animator;
    Animator3DCreate(&animator, data, EAST);
    int animIndex = AnimatorGetAnimationIndex(&animator, "melee");
    Animator3DSetAnimation(&animator, animIndex);
    AnimatorSetCallback(&animator.data, ANIM_CALL_EVENT_ACTION, AttackTestFNC, &testDamage);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 2.0f;

        //timer tests
        int tickMs = (int)(GetFrameTime() * 1000);
        TimerTicks(tickMs);
        if(IsKeyPressed(KEY_P))
        {
            TimerPauseSet(!TimerGetPauseState());
        }
        if(IsKeyPressed(KEY_M))
        {
            DIRECTION dir = (animator.direction + 1) % 8;
            Animator3DDirectionSet(&animator, dir);
        }
        if(IsKeyPressed(KEY_N))
        {
            //if(animator.isRunning) Animator3DStop(&animator);
            //else Animator3DStart(&animator);

        }
        if(IsKeyPressed(KEY_K))
        {
            animIndex++;
            if(animIndex >= animator.animations->animationsCount) animIndex = 0;
            Animator3DSetAnimation(&animator, animIndex);
        }
        else if(IsKeyPressed(KEY_J))
        {
            animIndex--;
            if(animIndex < 0) animIndex = animator.animations->animationsCount - 1;
            Animator3DSetAnimation(&animator, animIndex);
        }
        BeginDrawing();

            ClearBackground(DARKGREEN);
            BeginMode2D(camera);
            Animator3DDraw(&animator, playerPosition.x, playerPosition.y);
            const char* animName = animator.currentAnimation->data.name;
            char* animType;
            if(animator.sharedData->animationType == ANIM_TYPE_LOOP) animType = "loop";
            else if(animator.sharedData->animationType == ANIM_TYPE_SINGLE) animType = "single";
            else if(animator.sharedData->animationType == ANIM_TYPE_PINGPONG) animType = "ping-pong";
            else animType = "NOT DEFINED";
            DrawText(animName, playerPosition.x, playerPosition.y + 10, 10, WHITE);
            DrawText(animType, playerPosition.x, playerPosition.y + 20, 10, WHITE);
            char buffer[16];
            sprintf(buffer, "%d", animator.data.currentFrame);
            DrawText(buffer, playerPosition.x, playerPosition.y + 30, 10, WHITE);

            DrawText(animator.currentAnimation->dirAnimations[EAST].frames[animator.data.currentFrame].sprite.name,
                playerPosition.x, playerPosition.y + 40, 10, WHITE);
            DrawCircle(playerPosition.x, playerPosition.y, 3, MAROON);
            EndMode2D();
        EndDrawing();
    }


    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
