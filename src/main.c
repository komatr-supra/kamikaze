#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"
#include "map_manager.h"
#include "animation.h"
#include "timer.h"

void TimerTestPrintOneTime(void)
{
    TraceLog(LOG_INFO, "one time timer");
}

void TimerTestPrintRepeat(void)
{
    TraceLog(LOG_INFO, "repeated timer");

}

void TimerTestPrintTimes(void)
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
    TimerSet(5.0f, TimerTestPrintOneTime, 1);
    TimerSet(2.0f, TimerTestPrintRepeat, -1);
    TimerSet(4.0f, TimerTestPrintTimes, 10);



    while (!WindowShouldClose())
    {
        TimersTicks(GetFrameTime());

        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 2.0f;

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
