#ifndef TMX_H
#define TMX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for Tileset within a Map
typedef struct Tileset {
    int firstgid;
    int source;
    // ... other Tileset attributes ...
    struct Tileset *next;
} Tileset;

// Structure for Layer within a Map
typedef struct Layer {
    int id;
    char *name;
    int width;
    int height;
    int *data;
    // ... other Layer attributes ...
    struct Layer *next;
} Layer;

// Structure for the Map
typedef struct Map {
    char *version;
    char *orientation;
    int width;
    int height;
    int tilewidth;
    int tileheight;
    Tileset *tilesets;
    Layer *layers;
    // ... other Map attributes ...
} Map;


#endif //TMX_H