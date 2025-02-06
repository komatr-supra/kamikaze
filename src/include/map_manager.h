#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 

// Function to read the XML file into a string
bool ReadFile(const char filename[]) {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
        printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    
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

    char* buffer = (char* )malloc(fileSize + 1);
    if (buffer == NULL) {
        TraceLog(LOG_ERROR, "Memory allocation failed!");
        fclose(file);
        return false;
    }

    while ((read = getline(&line, &len, file)) != -1) {
        TraceLog(LOG_INFO, TextFormat("Retrieved line of length %zu:", read));
        TraceLog(LOG_INFO, TextFormat("%s", line));
    }

    fclose(file);
    if (line)
        free(line);

    return true;
}

bool MapParser(int mapIndex){
    // Clear Map

    // Load Map
    if (!ReadFile("./resources/maps/test_map.tmx")) {
        TraceLog(LOG_WARNING, "Map not loaded");
        return false;
    }
    TraceLog(LOG_INFO, "Map loaded");
    return true;
}
#endif //MAP_MANAGER_H