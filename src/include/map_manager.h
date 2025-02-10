#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// #include <dirent.h> 
#include <limits.h>
#include <errno.h>

#define MAX_TILESETS 5 // Adjust as needed
#define MAX_TILESET_TILES 30 // Adjust as needed
#define MAX_LAYERS 10 // Adjust as needed
#define MAX_TILE_DATA_SIZE 1024 // Adjust as needed
#define MAX_NAME_LENGTH 20 // Adjust as needed
#define MAX_SOURCE_LENGTH 64 // Adjust as needed

typedef struct TilesetTile {
    char* source;
    int id;
} TilesetTile;

typedef struct Tileset {
    TilesetTile* tilesetTile;
    char* source;
    char name[MAX_NAME_LENGTH];
    int firstgid;
    int numTilesetTiles;
    int tilecount;
} Tileset;

typedef struct Layer {
    char name[MAX_NAME_LENGTH];
    int* data;
    int id;
    int height;
    int width;
} Layer;

typedef struct Map {
    Tileset* tilesets;
    Layer layers[MAX_LAYERS];
    char version[10];
    char renderorder[20];
    int width;
    int height;
    int tilewidth;
    int tileheight;
    int numTilesets;
    int numLayers;
} Map;
/*
* May change until the final version
* Map length 544
* Tileset length 48
* Layer length 48
* TilesetTile length 16
**/

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
        perror("strdup failed");
        return NULL; // Or handle memory allocation error appropriately
    }

    char* result = (char *)malloc(strlen(path_copy) * 2 + 1); // Allocate enough space. (+1 for null terminator)
    if(result == NULL)
    {
        perror("malloc failed");
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
    while ((read = getline(&line, &len, file)) != -1) {
        SkipWhitespace(&line);
        if (isCollectingTile){
            char* sourceStr = strstr(line, "source=\"");
            if (sourceStr) {
                sourceStr += 8;
                char* sourceEndStr = strchr(sourceStr, '"');
                if (sourceEndStr) {
                    size_t sourceLen = sourceEndStr - sourceStr;
                    if (sourceLen >= MAX_SOURCE_LENGTH) {
                        fclose(file);
                        return false;
                    }
                    tileset->tilesetTile[currentTileIndex].source = malloc(sourceLen+2);
                    strncpy(tileset->tilesetTile[currentTileIndex].source, sourceStr, sourceLen);
                    tileset->tilesetTile[currentTileIndex].source[sourceLen] = '\0';
                    tileset->tilesetTile[currentTileIndex].source = ResourcePath(tileset->tilesetTile[currentTileIndex].source, "../../../..");
                }
            }

            isCollectingTile = false;
            currentTileIndex++;
        }else if(strstr(line, "<tileset ")){
            char* tilecountStr = strstr(line, "tilecount=\"");
            if (tilecountStr) tileset->tilecount = atoi(tilecountStr + 11);
            tileset->tilesetTile = malloc(sizeof(TilesetTile) * tileset->tilecount);
        }else if (strstr(line, "<tile ") && tileset->tilecount != 0) {
            char* idStr = strstr(line, "id=\"");
            if (idStr) tileset->tilesetTile[currentTileIndex].id = atoi(idStr + 4);
            isCollectingTile = true;
        } 
    }

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
            char* widthStr = strstr(line, "width=\"");
            if (widthStr) map->width = atoi(widthStr + 7);
            char* heightStr = strstr(line, "height=\"");
            if (heightStr) 
                map->height = atoi(heightStr + 8);
            char* tilewidthStr = strstr(line, "tilewidth=\"");
            if (tilewidthStr) 
                map->tilewidth = atoi(tilewidthStr + 11);
            char* tileheightStr = strstr(line, "tileheight=\"");
            if (tileheightStr) 
                map->tileheight = atoi(tileheightStr + 12);
        } else if (strstr(line, "<tileset")) {
            // Parse tileset attributes (source, firstgid)
            if (map->numTilesets < MAX_TILESETS) {
                char* sourceStr = strstr(line, "source=\"");
                if (sourceStr) {
                    sourceStr += 8;
                    char* sourceEndStr = strchr(sourceStr, '"');
                    if (sourceEndStr) {
                        size_t sourceLen = sourceEndStr - sourceStr;
                        if (sourceLen >= MAX_SOURCE_LENGTH) {
                            fclose(file);
                            return false;
                        }
                        map->tilesets[map->numTilesets].source = malloc(sourceLen+2);
                        strncpy(map->tilesets[map->numTilesets].source, sourceStr, sourceLen);
                        map->tilesets[map->numTilesets].source[sourceLen] = '\0';
                        map->tilesets[map->numTilesets].source = ResourcePath(map->tilesets[map->numTilesets].source, "..");
                    }
                }
                char* firstgidStr = strstr(line, "firstgid=\"");
                if (firstgidStr) map->tilesets[map->numTilesets].firstgid = atoi(firstgidStr + 10);
                TilesetParser(&map->tilesets[map->numTilesets]);
                map->numTilesets++;
            }
        } else if (strstr(line, "<layer")) {
            // Parse layer attributes (name, id, width, height)
            if (map->numLayers < MAX_LAYERS) {
                char* idStr = strstr(line, "id=\"");
                if (idStr) map->layers[map->numLayers].id = atoi(idStr + 4);
                char* nameStr = strstr(line, "name=\"");
                if (nameStr){
                    nameStr += 6;
                    char* nameEndStr = strchr(nameStr, '"');
                    if (nameEndStr) {
                        size_t nameLen = nameEndStr - nameStr;
                        if (nameLen >= MAX_NAME_LENGTH) {
                            TraceLog(LOG_ERROR, TextFormat("Error: Layer name is too long (max %d characters).\n", MAX_NAME_LENGTH - 1));
                            fclose(file);
                            return false;
                        }
                        strncpy(map->layers[map->numLayers].name, nameStr, nameLen);
                    }
                }
                char* widthStr = strstr(line, "width=\"");
                if (widthStr) map->layers[map->numLayers].width = atoi(widthStr + 7);
                char* heightStr = strstr(line, "height=\"");
                if (heightStr) map->layers[map->numLayers].height = atoi(heightStr + 8);
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
            for(int j=0;j<map->tilesets[i].numTilesetTiles;j++){
                if(map->tilesets[i].tilesetTile[j].source){
                    free(map->tilesets[i].tilesetTile[j].source);
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