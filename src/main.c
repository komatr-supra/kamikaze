#include "raylib.h"
#include "map_manager.h"

#include "animation.h"

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

    Shader blurrShader = LoadShader(0, "../resources/shaders/blur.fs");
    SetShaderValue(blurrShader, GetShaderLocation(blurrShader, "resolution"), &((Vector2){1200.0f, 800.0f}), SHADER_UNIFORM_VEC2);

    AnimationInit();
    AnimationPushFrame("knight", "melee2_0_019,knight-0,1,786,62,98,5,60");
    AnimationDir* anim = AnimationGetFromDatabase("knight", "melee2");

    SetTargetFPS(60);
    if(!MapParser(0)){
        TraceLog(LOG_ERROR, "Map Loader Error");
    }
    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 2.0f;

        Sprite spr = anim->frames[0].sprites[0];
        SetTextureFilter(spr.texture, TEXTURE_FILTER_BILINEAR);
        BeginDrawing();

            ClearBackground(DARKGREEN);
            BeginShaderMode(blurrShader);
            BeginMode2D(camera);
            
            DrawTexturePro(spr.texture, spr.sourceRect, (Rectangle){300,300, spr.sourceRect.width, spr.sourceRect.height}, spr.origin, 0, WHITE);
            
            EndMode2D();
            EndShaderMode();
            
            DrawRectangleV(playerPosition, (Vector2){25, 25}, MAROON);
        EndDrawing();
    }

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}