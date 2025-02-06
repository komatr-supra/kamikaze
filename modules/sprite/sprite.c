#include "sprite.h"

#include "stdio.h"
#include "string.h"


Sprite SpriteCreate(char* name, Texture2D texture, Rectangle sourceRectangle, Vector2 origin)
{
    Sprite sprite;
    strcpy(sprite.name, name);  // must make a name copy
    sprite.texture = texture;
    sprite.sourceRect = sourceRectangle;
    sprite.origin = origin;
}

