#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"
#include "map_manager.h"
#include "animation.h"
#include "timer.h"

void TimerTestPrintOneTime(void* data)
{
    TraceLog(LOG_INFO, "one time timer");
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
    const int screenWidth = 1200;
    const int screenHeight = 800;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Kamikaze");

    Vector2 playerPosition = { (float)screenWidth/2, (float)screenHeight/2 };

    Camera2D camera = { 0 };
    camera.offset = (Vector2){600, 400};
    camera.target = (Vector2){300, 300};
    camera.zoom = 2.0f;

    SetTargetFPS(60);

    //timer tests
SetTraceLogLevel(LOG_DEBUG);

    int foreverTimerCount = 0;

    int handle = TimerSet(2.0f, -1, TimerTestPrintRepeat, (void*)&foreverTimerCount);
    TimerSet(1.0f, 5, TimerTestPrintTimes, NULL);
    int handle2 = TimerSet(2.0f, -1, TimerTestPrintRepeat, NULL);
    int handle3 = TimerSet(5.0f, 10, TimerTestPrintTimes, NULL);

    //TimerSet(1.0f, 4, TimerTestPrintTimes, NULL);



    while (!WindowShouldClose())
    {


        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 2.0f;

        //timer tests
        TimerTicks(GetFrameTime());
        if(IsKeyPressed(KEY_SPACE))
        {
            TimerCancel(handle, true);
        }
        if(IsKeyPressed(KEY_M)) TimerCancel(handle2, false);
        if(IsKeyPressed(KEY_N)) TimerCancel(handle3, true);
        if(IsKeyPressed(KEY_P))
        {
            TimerPauseSet(!TimerGetPauseState());
        }

        BeginDrawing();

            ClearBackground(DARKGREEN);
            BeginMode2D(camera);

            DrawRectangleV(playerPosition, (Vector2){25, 25}, MAROON);
        EndDrawing();
    }


    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
