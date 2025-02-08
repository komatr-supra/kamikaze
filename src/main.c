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

    Shader blurrShader = LoadShader(0, "resources/shaders/blur.fs");
    SetShaderValue(blurrShader, GetShaderLocation(blurrShader, "resolution"), &((Vector2){1200.0f, 800.0f}), SHADER_UNIFORM_VEC2);


    // animation test
    AnimationInit();
    FILE* f_animData = fopen("resources/characters/knight/knight.txt", "r");
    if(f_animData == NULL)
    {
        TraceLog(LOG_ERROR, "cant open file");
        exit(EXIT_FAILURE);
    }
    char bufferLine[128];
    while (fgets(bufferLine, sizeof(bufferLine), f_animData))
    {
        AnimationPush3DFrame("knight", bufferLine);
    }
    Animation3DSortFrames("knight");
    const AnimationDir* anim = Animation3DGetFromDatabase("knight", "melee2");
    int currentFrame = 0;
    float frameDuration = 0.1f;
    float timePassed = 0;
    DIRECTION dir = EAST;

    SetTargetFPS(60);
    //if(!MapParser(0)){
    //    TraceLog(LOG_ERROR, "Map Loader Error");
    //}
    while (!WindowShouldClose())
    {
        timePassed += GetFrameTime();
        if(timePassed > frameDuration)
        {
            timePassed = 0;
            currentFrame++;
            if(currentFrame >= anim->frames->frameCount) currentFrame = 0;
        }


        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 2.0f;


        Sprite spr = anim->frames[dir].sprites[currentFrame];
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

    AnimationDestroy();
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}