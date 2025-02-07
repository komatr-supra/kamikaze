/**
 * resource_manager.h
 * komatr 6.2.2025
 * this header file is an interface for resource manager
 */

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "raylib.h"

#define MAX_PATH_LENGTH 128
#define MAX_TEXTURES 32

typedef struct {
    char path[MAX_PATH_LENGTH];
    Texture2D texture;
} TextureResource;

typedef struct {
    TextureResource textures[MAX_TEXTURES];
    int textureCount;
} ResourceManager;

// Resource Manager API
void ResourceManagerInit(ResourceManager* manager);
Texture2D ResourceManagerGetTexture(ResourceManager* manager, const char* path);
void ResourceManagerUnloadAll(ResourceManager* manager);

#endif
