#include "raylib.h"
#include "map_manager.h"

int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 360;

    InitWindow(screenWidth, screenHeight, "Kamikaze");

    Vector2 playerPosition = { (float)screenWidth/2, (float)screenHeight/2 };

    SetTargetFPS(60);
    Map* map;
    map = MapParser(0, map);
    if(!map){
        TraceLog(LOG_ERROR, "Map Loader Error");
    }
    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 2.0f;
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextureV(map->tilesets[0].tileTexture, (Vector2){0}, WHITE);

            DrawRectangleV(playerPosition, (Vector2){25, 25}, MAROON);

        EndDrawing();
    }

    MapFree(map);

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}