#include "resource_manager.h"
#include <string.h>

static Texture2D ResourceManagerLoadTexture(ResourceManager* manager, const char* path);

void ResourceManagerInit(ResourceManager* manager) {
    manager->textureCount = 0;
}

Texture2D ResourceManagerGetTexture(ResourceManager* manager, const char* path) {
    for (int i = 0; i < manager->textureCount; i++) {
        if (strcmp(manager->textures[i].path, path) == 0) {
            return manager->textures[i].texture;
        }
    }
    return ResourceManagerLoadTexture(manager, path);
}

static Texture2D ResourceManagerLoadTexture(ResourceManager* manager, const char* path) {
    strcpy(manager->textures[manager->textureCount].path, path);
    //char bufferPath[64] = "../resources/characters/";
    //manager->textures[manager->textureCount].texture = LoadTexture(path);
    manager->textures[manager->textureCount].texture = LoadTexture("../resources/characters/knight/knight-0.png");

    return manager->textures[manager->textureCount++].texture;
}

void ResourceManagerUnloadAll(ResourceManager* manager) {
    for (int i = 0; i < manager->textureCount; i++) {
        UnloadTexture(manager->textures[i].texture);
    }
    manager->textureCount = 0;
}
