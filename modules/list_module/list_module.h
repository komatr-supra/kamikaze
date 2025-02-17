#ifndef LIST_MODULE_H
#define LIST_MODULE_H

#include <stdlib.h>
#include "raylib.h"

typedef struct ListInt{
    int size;           // number of items in list
    int maxSize;        // maximum number of items
    bool isSorted;      // keep sorted this list?
    int* collection;    // values
} ListInt;

static ListInt ListIntCreate(int size)
{
    int* allocatedMemory = (int*)calloc(sizeof(int), sizeof(int) * size);
    ListInt list;
    list.size = 0;
    list.maxSize = allocatedMemory == NULL ? 0 : size;
    list.collection = allocatedMemory;
    return list;
}

static void ListIntAdd(ListInt* list, int value)
{
    if(list->size + 1 >= list->maxSize)
    {
        TraceLog(LOG_DEBUG, "too much item in list");
        return;
    }
    if(!list->isSorted)
    {
        //list is not sorted, just add an element
        list->collection[list->size] = value;
        list->size++;
    }
    else
    {
        //list is sorted, check from end
        int index = list->size - 1;
        while (index > 0 && list->collection[index] > value)
        {
            list->collection[index + 1] = list->collection[index];
            index--;
        }
        list->size++;
        index++;
        list->collection[index] = value;
    }
}

static void ListIntRemove(ListInt* list, int index)
{
    if(index >= 0 && index < list->size)
    {
        TraceLog(LOG_DEBUG, "given index dont exist");
        return;
    }
    if(!list->isSorted)
    {
        list->collection[index] = list->collection[list->size - 1];
    }
    else
    {
        for (int i = index; i < list->size - 1; i++)
        {
            list->collection[i] = list->collection[i + 1];
        }
    }
    list->size--;
}

#endif