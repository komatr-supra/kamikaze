#ifndef LIST_MODULE_H
#define LIST_MODULE_H

#include <stdlib.h>
#include "raylib.h"

typedef struct ListInt{
    int count;
    int maxSize;
    bool isSorted;
    int* list;
} ListInt;

static ListInt* ListIntCreate(int size, bool isSorted)
{
    void* allocatedMemory = malloc(sizeof(ListInt) + sizeof(int) * size);
    ListInt* list = (ListInt*)allocatedMemory;
    list->count = 0;
    list->maxSize = size;
    list->isSorted = isSorted;
    list->list = (int*)(list + 1);
    return list;
}

static void ListIntAdd(ListInt* list, int value)
{
    if(list->count + 1 >= list->maxSize)
    {
        TraceLog(LOG_DEBUG, "too much item in list");
        return;
    }
    if(!list->isSorted)
    {
        list->list[list->count] = value;
        list->count++;
    }
    else
    {
        int index = list->count - 1;
        while (index > 0 && list->list[index] > value)
        {
            list->list[index + 1] = list->list[index];
            index--;
        }
        list->count++;
        index++;
        list->list[index] = value;
    }
}

static void ListIntRemove(ListInt* list, int index)
{
    if(index >= 0 && index < list->count)
    {
        TraceLog(LOG_DEBUG, "given index dont exist");
        return;
    }
    if(!list->isSorted)
    {
        list->list[index] = list->list[list->count - 1];
    }
    else
    {
        for (int i = index; i < list->count - 1; i++)
        {
            list->list[i] = list->list[i + 1];
        }
    }
    list->count--;
}

typedef struct ListFree{
    int count;
    int maxSize;
    void* collection;
} ListFree;

#endif