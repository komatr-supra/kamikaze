#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// #include <dirent.h> 
#include <limits.h>
#include <errno.h>
#include "raylib.h"

#define MAX_TILESETS 5 // Adjust as needed
#define MAX_LAYERS 10 // Adjust as needed

typedef struct TilesetTile {
    char* name; // original image name with path, but without extension (”cliffs/cliff2”) => fake folder structure
    float x; // position on X, where the sprite starts on the texture
    float y; // position on Y, where the sprite starts on the texture
    float width; // width of the sprite in texture
    float height; // height of the sprite in texture
    float offsetX; // X offset position, relative to original top left - images are trimmed, so we need a original start
    float offsetY; // Y offset position, relative to original top left - images are trimmed, so we need a original start
    int id;
} TilesetTile;

typedef struct Tileset {
    Texture2D tileTexture;
    TilesetTile* tilesetTile;
    char* name;
    char* tileParent;
    char* source;
    int firstgid;
    int numTilesetTiles;
    int tilecount;
} Tileset;

typedef struct Layer {
    char* name;
    int* data;
    int id;
    int height;
    int width;
} Layer;

typedef struct Map {
    Layer layers[MAX_LAYERS];
    Tileset* tilesets;
    int width;
    int height;
    int tilewidth;
    int tileheight;
    int numTilesets;
    int numLayers;
} Map;
/*
* May change until the final version
* Map length 352
* Tileset length 72
* Layer length 32
* TilesetTile length 40
**/
int GetIntFromString(const char* origin, const char* start){
    char* path_copy = strdup(origin);
    char* extract = strstr(path_copy, start);
    int result = 0;
    if (extract) {
        result = atoi(extract + strlen(start));
    }
    free(path_copy);
    return result;
}

char* GetStringFromString(const char* origin, const char* start, const char* end){
    char* path_copy = strdup(origin);
    char* result = NULL;

    char* extract = strstr(origin, start);
    if (extract) {
        extract += strlen(start);
        char* endExtract = strchr(extract, *end);
        if (endExtract) {
            size_t extractLen = endExtract - extract;
            result = malloc(extractLen+1);
            strncpy(result, extract, extractLen);
            result[extractLen] = '\0';
        }
    }
    free(path_copy);
    return result;
}


void SkipWhitespace(char **xml) {
    while (isspace((unsigned char)**xml)) {
        (*xml)++;
    }
}

char* ResourcePath(char *path, const char* target) {
    if (path == NULL) {
        return NULL;
    }

    const char* replacement = "./resources";
    size_t repl_len = strlen(replacement);
    size_t target_len = strlen(target);

    // Create a copy of the path to work with (since we'll modify it)
    char *path_copy = strdup(path);
    if (path_copy == NULL) {
        TraceLog(LOG_ERROR, "strdup failed");
        return NULL; // Or handle memory allocation error appropriately
    }

    char* result = (char *)malloc(strlen(path_copy) * 2 + 1); // Allocate enough space. (+1 for null terminator)
    if(result == NULL)
    {
        TraceLog(LOG_ERROR, "malloc failed");
        free(path_copy);
        return NULL;
    }

    char* current_pos = path_copy;
    char* write_pos = result;

    size_t prefix_len = 0;
    while ((current_pos = strstr(current_pos, target)) != NULL) {
        // Copy everything up to the ".."
        prefix_len = current_pos - path_copy;
        strncpy(write_pos, path_copy, prefix_len);
        write_pos += prefix_len;

        // Copy the replacement string
        strncpy(write_pos, replacement, repl_len);
        write_pos += repl_len;

        // Move past the ".." in the original string
        current_pos += target_len;
        path_copy = current_pos; // Important: Update path_copy
    }

    // Copy the remaining part of the string (after the last "..")
    strcpy(write_pos, path_copy);

    return result;
}
bool TilesetTileParser(TilesetTile* tilesetTile, FILE* file){
    if(!tilesetTile || !file) return false;

    // Load Tileset
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    long fileSize = ftell(file);
    rewind(file);
    const char* delimiters = ",";

    while ((read = getline(&line, &len, file)) != -1) {
        int infoIndex = 0;
        char* sourceStr = strstr(line, tilesetTile->name);
        if (sourceStr) {
            char* token = strtok(line, delimiters);
            while (token != NULL) {
                char *endptr;
                errno = 0;
                int num = strtol(token, &endptr, 10);
                if (endptr != token && errno == 0 && num >= INT_MIN && num <= INT_MAX) {
                    switch (infoIndex)
                    {
                        case 0:
                            tilesetTile->x = num;
                            break;
                        case 1:
                            tilesetTile->y = num;
                            break;
                        case 2:
                            tilesetTile->width = num;
                            break;
                        case 3:
                            tilesetTile->height = num;
                            break;
                        case 4:
                            tilesetTile->offsetX = num;
                            break;
                        case 5:
                            tilesetTile->offsetY = num;
                            break;
                        
                        default:
                            break;
                    }
                    infoIndex++;
                }

                token = strtok(NULL, delimiters); // I Dont get it but they say to use it to loop
            }
        }
    }
    return true;

}

bool TilesetParser(Tileset* tileset){
    // Load Tileset
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen(tileset->source, "r");
    if (file == NULL){
        TraceLog(LOG_ERROR, "File not loaded");
        return false;
    }

    long fileSize = ftell(file);
    rewind(file);
    tileset->numTilesetTiles = 0;
    size_t currentTileIndex = 0;
    bool isCollectingTile = false;
    tileset->tilecount = 0;
    FILE* tilesetFile = NULL;
    while ((read = getline(&line, &len, file)) != -1) {
        SkipWhitespace(&line);
        if (isCollectingTile){
            tileset->tilesetTile[currentTileIndex].name = GetStringFromString(line, "source=\"../../../../tiled/tilesets/", "/");
            TilesetTileParser(&tileset->tilesetTile[currentTileIndex], tilesetFile);

            isCollectingTile = false;
            currentTileIndex++;
        }else if(strstr(line, "<tileset ")){
            tileset->tilecount = GetIntFromString(line, "tilecount=\"");
            tileset->tilesetTile = malloc(sizeof(TilesetTile) * tileset->tilecount);
            tileset->tileParent = GetStringFromString(tileset->source, "./resources/tilesets/", "/");
            // TODO: Change it to a better implementation later
            if (tileset->tileParent) {
                const char resourcesPath[] = "./resources";
                char* filePath = (char *)malloc(strlen("./resources/tilesets//.tpd")+strlen(tileset->tileParent)+strlen(tileset->tileParent)+1); // Allocate enough space. (+1 for null terminator)
                strcpy(filePath, "./resources/tilesets/");
                strcat(filePath, tileset->tileParent);
                strcat(filePath, "/");
                strcat(filePath, tileset->tileParent);
                strcat(filePath, ".tpd");
                
                tilesetFile = fopen(filePath, "r");
                if (tilesetFile == NULL){
                    TraceLog(LOG_ERROR, "File not loaded");
                    fclose(file);
                    return false;
                }
                char* imagePath = (char *)malloc(strlen("./resources/tilesets//.png")+strlen(tileset->tileParent)+strlen(tileset->tileParent)+1);                    strcpy(filePath, "./resources/tilesets/");
                strcpy(imagePath, "./resources/tilesets/");
                strcat(imagePath, tileset->tileParent);
                strcat(imagePath, "/");
                strcat(imagePath, tileset->tileParent);
                strcat(imagePath, ".png");
                Image tileImage = LoadImage(imagePath);
                tileset->tileTexture = LoadTextureFromImage(tileImage);
                UnloadImage(tileImage);
            }
        }else if (strstr(line, "<tile ") && tileset->tilecount != 0) {
            tileset->tilesetTile[currentTileIndex].id = GetIntFromString(line, "id=\"");
            isCollectingTile = true;
        } 
    }
    if(tilesetFile){
        fclose(tilesetFile);
    }
    fclose(file);

    return true;
}

// Function to read the XML file into a string
bool ReadFile(const char filename[], Map* map) {
    // DIR *d;
    // struct dirent *dir;
    // d = opendir(".");
    // if (d) {
    //     while ((dir = readdir(d)) != NULL) {
    //     printf("%s\n", dir->d_name);
    //     }
    //     closedir(d);
    // }
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen(filename, "r");
    if (file == NULL){
        TraceLog(LOG_ERROR, "File not loaded");
        return false;
    }

    long fileSize = ftell(file);
    rewind(file);

    // initialize Map values
    map->numTilesets = 0;
    map->numLayers = 0;
    bool isCollectingLayerData = false;
    int collectingLayoutDataIndex = 0;
    size_t maxCurrentDataSize = 0;
    size_t currentDataLen = 0; // Start at the beginning
    map->tilesets = malloc(sizeof(Tileset) * MAX_TILESETS);
    while ((read = getline(&line, &len, file)) != -1) {
        SkipWhitespace(&line);
        // TraceLog(LOG_INFO, TextFormat("Retrieved line of length %zu:", read));
        // TraceLog(LOG_INFO, TextFormat("%s", line));
        // Parse line by line
        if (isCollectingLayerData){
            // This does not work yet
            int currentLayer = map->numLayers-1;
            if (*line == '<') {
                isCollectingLayerData = false;
            } else {
                if(map->layers[currentLayer].data){ 
                    char delimiters[1] = ",";
                    char* token = strtok(line, delimiters);
                    
                    while (token != NULL) {
                        char *endptr;
                        errno = 0;
                        int num = strtol(token, &endptr, 10);
                        if (*endptr != '\n' && errno == 0 && num >= INT_MIN && num <= INT_MAX) {
                            map->layers[currentLayer].data[currentDataLen] = num;
                            currentDataLen++;
                        }

                        token = strtok(NULL, delimiters); // I Dont get it but they say to use it to loop
                    }
                }
            }
        }else if (strstr(line, "<map")) {
            map->width = GetIntFromString(line, "width=\"");
            map->height = GetIntFromString(line, "height=\"");
            map->tilewidth = GetIntFromString(line, "tilewidth=\"");
            map->tileheight = GetIntFromString(line, "tileheight=\"");
        } else if (strstr(line, "<tileset")) {
            // Parse tileset attributes (source, firstgid)
            if (map->numTilesets < MAX_TILESETS) {
                map->tilesets[map->numTilesets].source = GetStringFromString(line, "source=\"", "\"");
                map->tilesets[map->numTilesets].source = ResourcePath(map->tilesets[map->numTilesets].source, "..");
                map->tilesets[map->numTilesets].firstgid = GetIntFromString(line, "firstgid=\"");
                TilesetParser(&map->tilesets[map->numTilesets]);
                map->numTilesets++;
            }
        } else if (strstr(line, "<layer")) {
            // Parse layer attributes (name, id, width, height)
            if (map->numLayers < MAX_LAYERS) {
                map->layers[map->numLayers].id = GetIntFromString(line, "id=\"");
                map->layers[map->numLayers].name = GetStringFromString(line, "name=\"", "\"");
                map->layers[map->numLayers].width = GetIntFromString(line, "width=\"");
                map->layers[map->numLayers].height = GetIntFromString(line, "height=\"");

                // Alloc array by layer size
                maxCurrentDataSize = map->layers[map->numLayers].width * map->layers[map->numLayers].height * sizeof(int);
                map->layers[map->numLayers].data = NULL;
                map->layers[map->numLayers].data = (int*)malloc(maxCurrentDataSize);
                if (map->layers[map->numLayers].data == NULL) {
                    TraceLog(LOG_ERROR, "Memory allocation failed for layer data!");
                    fclose(file);
                    return false;
                }
                map->numLayers++;
            }
        } else if (strstr(line, "<data")) {
            isCollectingLayerData = true;
            collectingLayoutDataIndex = 0;
            currentDataLen = 0;
        }
    }

    fclose(file);
    TraceLog(LOG_INFO, TextFormat("Map length %d", sizeof(Map)));
    TraceLog(LOG_INFO, TextFormat("Tileset length %d", sizeof(Tileset)));
    TraceLog(LOG_INFO, TextFormat("Layer length %d", sizeof(Layer)));
    TraceLog(LOG_INFO, TextFormat("TilesetTile length %d", sizeof(TilesetTile)));

    return true;
}
void MapFree(Map* map){
    if(map){
        for(int i=0;i<map->numLayers;i++){
            if(map->layers[i].data){
                free(map->layers[i].data);
            }
        }
        for(int i=0;i<map->numTilesets;i++){
            if(map->tilesets[i].source){
                free(map->tilesets[i].source);
            }
            // if(map->tilesets[i].tileParent){
            //     free(map->tilesets[i].tileParent);
            // }
            if(map->tilesets[i].tileTexture.id > 0){
                UnloadTexture(map->tilesets[i].tileTexture);
            }
            for(int j=0;j<map->tilesets[i].numTilesetTiles;j++){
                if(map->tilesets[i].tilesetTile[j].name){
                    free(map->tilesets[i].tilesetTile[j].name);
                }
            }
        }
        if(map->tilesets){
            free(map->tilesets);
        }
        free(map);
    }
}

Map* MapParser(int mapIndex, Map* currentMap){
    // Clear Map
    if(currentMap){
        MapFree(currentMap);
        currentMap = NULL;
    }
    // Load Map
    currentMap = (Map*) malloc(sizeof(Map));
    if(!currentMap){
        TraceLog(LOG_ERROR, "Memory allocation failed!");
        return NULL;
    }
    // Path is hardcoded to be able to test as fast as possible
    if (!ReadFile("./resources/maps/test_map.tmx", currentMap)) {
        TraceLog(LOG_WARNING, "Map not loaded");
        MapFree(currentMap);
        return NULL;
    }
    TraceLog(LOG_INFO, "Map loaded");
    return currentMap;
}
#endif //MAP_MANAGER_H