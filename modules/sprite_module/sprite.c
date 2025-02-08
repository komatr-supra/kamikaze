/**
 * @file sprite.c
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief sprite related functions and manipulations
 * @version 0.1
 * @date 08-02-2025
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "sprite.h"

#include "stdio.h"
#include "string.h"


Sprite SpriteCreate(char* name, Texture2D texture, Rectangle sourceRectangle, Vector2 origin)
{
    Sprite sprite;
    strcpy(sprite.name, name);
    sprite.texture = texture;
    sprite.sourceRect = sourceRectangle;
    sprite.origin = origin;
}

void SpriteCreatePtr(Sprite* sprite, char* name, Texture2D texture, Rectangle sourceRectangle, Vector2 origin)
{
    strcpy(sprite->name, name);
    sprite->texture = texture;
    sprite->sourceRect = sourceRectangle;
    sprite->origin = origin;
}

