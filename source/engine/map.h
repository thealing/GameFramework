#pragma once

#include <stdlib.h>

#include <stdint.h>

#include <stdbool.h>

#define MAP_INITIAL_CAPACITY 8

#define MAP_KEY_EMPTY (UINT64_MAX - 1)

#define MAP_KEY_DELETED (UINT64_MAX - 2)

typedef uint64_t Map_Key;

typedef uint32_t Map_Value;

typedef uint32_t Map_Hash;

typedef struct Map Map;

Map* map_create();

void map_destroy(Map* map);

void map_insert(Map* map, Map_Key key, Map_Value value);

bool map_get(Map* map, Map_Key key, Map_Value* value);

bool map_remove(Map* map, Map_Key key);

void map_clear(Map* map);
