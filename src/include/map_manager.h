#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// #include <dirent.h> 

#define MAX_TILESETS 5 // Adjust as needed
#define MAX_LAYERS 10 // Adjust as needed
#define MAX_TILE_DATA_SIZE 1024 // Adjust as needed
#define MAX_NAME_LENGTH 20 // Adjust as needed
#define MAX_SOURCE_LENGTH 64 // Adjust as needed

typedef struct Tileset {
    char source[MAX_SOURCE_LENGTH];
    int firstgid;
} Tileset;

typedef struct Layer {
    char name[MAX_NAME_LENGTH];
    int* data;
    int id;
    int height;
    int width;
} Layer;

typedef struct Map {
    Tileset tilesets[MAX_TILESETS];
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
* Size of Map: 592 bytes
* Size of Layer: 40 bytes
* Size of Tileset: 68 bytes
**/

void SkipWhitespace(char **xml) {
    while (isspace((unsigned char)**xml)) {
        (*xml)++;
    }
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
    while ((read = getline(&line, &len, file)) != -1) {
        SkipWhitespace(&line);
        // TraceLog(LOG_INFO, TextFormat("Retrieved line of length %zu:", read));
        // TraceLog(LOG_INFO, TextFormat("%s", line));
        // Parse line by line
        if (isCollectingLayerData){
            if (*line == '<') {
                isCollectingLayerData = false;
            } else {
                // This does not work yet
                int currentLayer = map->numLayers-1;
                if(map->layers[currentLayer].data){
                    for(int i=0;i< map->layers[currentLayer].width;i++){
                        if(isdigit(*(line + i))){
                            map->layers[currentLayer].data[i] = atoi(line+i);
                        }
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
                    sourceStr += 6;
                    char* sourceEndStr = strchr(sourceStr, '"');
                    if (sourceEndStr) {
                        size_t sourceLen = sourceEndStr - sourceStr;
                        if (sourceLen >= MAX_SOURCE_LENGTH) {
                            fclose(file);
                            return false;
                        }
                        strncpy(map->tilesets[map->numTilesets].source, sourceStr, sourceLen);
                    }
                    strcpy(map->tilesets[map->numTilesets].source, sourceStr + 8);
                }
                char* firstgidStr = strstr(line, "firstgid=\"");
                if (firstgidStr) map->tilesets[map->numTilesets].firstgid = atoi(firstgidStr + 10);
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
                map->layers[map->numLayers].data = (int*)malloc(map->layers[map->numLayers].width * map->layers[map->numLayers].height * sizeof(int));
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
        }
    }

    fclose(file);

    return true;
}
void MapFree(Map* map){
    if(map){
        for(int i=0;i<map->numLayers;i++){
            if(map->layers[i].data){
                free(map->layers[i].data);
            }
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