/**
 * @file sprite.h
 * @author komatr
 * @date 6.2.2025
 * @brief data related to identify and use image inside a texturepack
 */
#ifndef SPRITE_H
#define SPRITE_H

#define MAX_SPRITE_NAME_LENGHT 32 //**< maximum name lenght for sprite identification */

#include "raylib.h"     //there are a raylib's structs

/**
 * @brief structure with information, witch part of the texture should be used
 * @struct contain data for drawing: name, texture, source rectangle and origin
 */
typedef struct Sprite
{
    char name[MAX_SPRITE_NAME_LENGHT]; //**< name oft the sprite */
    Texture2D texture;  //**< texture, where this sprite is */
    Rectangle sourceRect;   //**< location on the texture */
    Vector2 origin; //**<  point of the sprite's origin*/
} Sprite;

/**
 * @brief create and initialize a new sprite
 *
 * This function CREATE a sprite with given name, texture, source rectangle and origin.
 *
 * @param[in] name Name as ID.
 * @param[in] texture Texture, where this sprite belong.
 * @param[in] sourceRectangle Rectangle where this sprite is located in the texture.
 * @param[in] origin Point, where this sprite will be drawn.
 *
 * @return New created Sprite struct.
 */
Sprite SpriteCreate(char* name, Texture2D texture, Rectangle sourceRectangle, Vector2 origin);

/**
 * @brief populate already created sprite (or update)
 *
 * This function initialize or change sprite, Sprite struct is NOT created.
 *
 * @param[out] sprite Pointer to a Sprite struct, where data will be changed.
 * @param[in] name Name as ID.
 * @param[in] texture Texture, where this sprite belong.
 * @param[in] sourceRectangle Rectangle where this sprite is located in the texture.
 * @param[in] origin Point, where this sprite will be drawn.
 */
void SpriteCreatePtr(Sprite* sprite, char* name, Texture2D texture, Rectangle sourceRectangle, Vector2 origin);

#endif