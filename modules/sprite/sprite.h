#ifndef SPRITE_H
#define SPRITE_H

#define MAX_SPRITE_NAME_LENGHT 64

#include "raylib.h"     //there are a raylib structs

typedef struct Sprite
{
    char name[MAX_SPRITE_NAME_LENGHT];
    Texture2D texture;
    Rectangle sourceRect;
    Vector2 origin;
} Sprite;

Sprite SpriteCreate(char* name, Texture2D texture, Rectangle sourceRectangle, Vector2 origin);

#endif //SPRITE_H