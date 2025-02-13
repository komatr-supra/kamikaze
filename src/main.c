#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"

// Uncoment these lines to be able to test code separeted (Remove this speed coding when it is time)
// #define MAICON_TESTING 1 
// #define KOMATR_TESTING 1

#ifdef MAICON_TESTING
#include "maicon_main.h"
#endif
#ifdef KOMATR_TESTING
#include "komatr_main.h"
#endif

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

#ifdef MAICON_TESTING
    MaiconInit();
#endif
#ifdef KOMATR_TESTING
    KomatrInit();
#endif
    Shader blurrShader = LoadShader(0, "resources/shaders/blur.fs");
    SetShaderValue(blurrShader, GetShaderLocation(blurrShader, "resolution"), &((Vector2){1200.0f, 800.0f}), SHADER_UNIFORM_VEC2);

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

#ifdef MAICON_TESTING
        MaiconUpdate(GetFrameTime());
#endif
#ifdef KOMATR_TESTING
        KomatrUpdate(GetFrameTime());
#endif

        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 2.0f;

        BeginDrawing();

            ClearBackground(DARKGREEN);
            BeginShaderMode(blurrShader);
            BeginMode2D(camera);

#ifdef MAICON_TESTING
            MaiconDraw();
#endif
#ifdef KOMATR_TESTING
            KomatrDraw();
#endif
            DrawRectangleV(playerPosition, (Vector2){25, 25}, MAROON);
            EndMode2D();
        EndDrawing();
    }

#ifdef MAICON_TESTING
    MaiconUnload();
#endif
#ifdef KOMATR_TESTING
    KomatrUnload();
#endif

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
