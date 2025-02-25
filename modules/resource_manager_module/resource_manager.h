/**
 * @file resource_manager.h
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief API for resource manager, also an ResourceManagerTexture(struct must be created *on heap* = multiple independent resource managers)
 * @version 0.1
 * @date 06-02-2025
 * @note multiple resource managers are not sync, so you can load the same resource at different spaces
 * @copyright Copyright (c) 2025
 *
 */
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "raylib.h"

#define MAX_PATH_LENGTH 128                                 //< path of the loaded texture (also a key)
#define MAX_TEXTURES 32                                     //< max textures per manager

#pragma region STRUCTS
/// @brief individual texture resource record of path and texture
typedef struct TextureResource{
    char path[MAX_PATH_LENGTH];
    Texture2D texture;
} TextureResource;

/// @brief collection of texture resources(array)
typedef struct {
    TextureResource textures[MAX_TEXTURES];
    int textureCount;
} ResourceManagerTexture;
#pragma endregion

#pragma region API
/**
 * @brief Initialize resource manager
 *
 * @param manager manager to initialize
 */
void ResourceManagerTextureInit(ResourceManagerTexture* manager);

/**
 * @brief unload all resources from memmory for the given resource manager
 *
 * @param manager resource manager which should unload the resources
 */
void ResourceManagerTextureUnloadAll(ResourceManagerTexture* manager);

/**
 * @brief get a specific texture from resource manager
 * @note if texture isnt exist, then it will be created and returned
 * @param manager manager, where the texture is
 * @param path path of the texture relative to "resources/" folder
 * @return Texture2D returned texture struct
 */
Texture2D ResourceManagerGetTexture(ResourceManagerTexture* manager, char* texturePath);
#pragma endregion

#endif
