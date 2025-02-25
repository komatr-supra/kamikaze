// File to be deleted in the future (doing some speed coding)
#ifndef MAICON_MAIN_H
#define MAICON_MAIN_H
#include "map_manager.h"

Map* map;

void MaiconInit(){
    map = MapParser(0, map);
    if(!map){
        TraceLog(LOG_ERROR, "Map Loader Error");
    }
}

void MaiconUpdate(float delta_time){

}

void MaiconDraw(){
    DrawTextureV(map->tilesets[0].tileTexture, (Vector2){0}, WHITE);
}

void MaiconUnload(){
    if(map) MapFree(map);
}

#endif //MAICON_MAIN_H