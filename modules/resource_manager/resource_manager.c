/**
 * resource_manager.c
 * komatr 2.6.2025
 * main module file for resource management, avoid load duplication, load an UNLOAD resources
 * !!! resources must be managed carefully, you cant unload resources, when someone else use them !!!
 */
#include "resource_manager.h"
#include "string.h"

#define MAX_PATH_LENGTH 128     // maybe public for entire project???

#pragma region TEXTURES TODO: solo file/s
#define MAX_TEXTURES 32

/**
 * contain path and texture
 * used for collection of loaded textures => low speed dictionary
 */
typedef struct{
    char path[MAX_PATH_LENGTH];
    Texture2D texture;
} TextureResource;

static TextureResource textures[MAX_TEXTURES];
static int textureCount;

static Texture2D res_LoadTexture(char* path);

Texture2D GetTexture(char* path)
{
    for (int i = 0; i < textureCount; i++)
    {
        if(strcmp(textures[i].path, path) == 0) return textures[i].texture;
    }
    return res_LoadTexture(path);
}

static Texture2D res_LoadTexture(char* path)
{
    strcpy(textures[textureCount].path, path);
    textures[textureCount].texture = LoadTexture(path);
    return textures[textureCount++].texture;
}
#pragma endregion