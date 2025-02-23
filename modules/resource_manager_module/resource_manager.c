/**
 * @file resource_manager.c
 * @author komatr (NONE_DONKEY@domain.com)
 * @brief complete system for resource management to avoid duplication and centralized load/unload management
 * @version 0.1
 * @date 06-02-2025
 *
 * @copyright Copyright (c) 2025
 * @todo add other resources as well, like sounds, etc...
 */
#include "resource_manager.h"
#include <string.h>

#pragma region DECLARATIONS
/**
 * @brief load a texture and save it inside given manager
 *
 * @param manager manager where to load the texture
 * @param path texture path
 * @return Texture2D created texture data
 * @todo complete it
 */
static Texture2D ResourceManagerTextureLoad(ResourceManagerTexture* manager, char* textureLocation);
#pragma endregion

#pragma region API
void ResourceManagerTextureInit(ResourceManagerTexture* manager) {
    manager->textureCount = 0;
}

Texture2D ResourceManagerGetTexture(ResourceManagerTexture* manager, char* textureLocation) {
    for (int i = 0; i < manager->textureCount; i++) {
        if (strcmp(manager->textures[i].path, textureLocation) == 0) {
            return manager->textures[i].texture;
        }
    }
    return ResourceManagerTextureLoad(manager, textureLocation);
}

void ResourceManagerTextureUnloadAll(ResourceManagerTexture* manager) {
    for (int i = 0; i < manager->textureCount; i++) {
        UnloadTexture(manager->textures[i].texture);
    }
    manager->textureCount = 0;
}
#pragma endregion

#pragma region PRIVATE FNC
static Texture2D ResourceManagerTextureLoad(ResourceManagerTexture* manager, char* textureLocation) {
    strcpy(manager->textures[manager->textureCount].path, textureLocation);
    char bufferPath[64];
    strcpy(bufferPath, textureLocation);
    strcat(bufferPath, ".png");
    manager->textures[manager->textureCount].texture = LoadTexture(bufferPath);

    return manager->textures[manager->textureCount++].texture;
}
#pragma endregion

