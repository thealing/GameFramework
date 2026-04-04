#include "map.h"

#define FALSE 0

#define TRUE 1

#define COUNT_MAX SIZE_MAX

#define INDEX_MAX SIZE_MAX

#define ADDRESS_MAX UINTPTR_MAX

#define OFFSET_MAX PTRDIFF_MAX

typedef bool Bool;

typedef uint8_t Byte;

typedef int32_t Int;

typedef size_t Size;

typedef size_t Count;

typedef size_t Index;

typedef uintptr_t Address;

typedef ptrdiff_t Offset;

struct Map
{
	Count total_count;

	Count deleted_count;

	Count capacity;

	Map_Key* keys;

	Map_Value* values;
};

Map_Hash _map_get_hash(Map_Key key)
{
	return key + 1;
}

Map_Hash _map_step_hash(Map_Hash hash)
{
	hash ^= hash << 13;

	hash ^= hash >> 17;

	hash ^= hash << 5;

	return hash;
}

Index _map_get_hash_index(Map_Hash hash, Count capacity)
{
	return hash & capacity;
}

void _map_clear_keys(Map* map)
{
	Count actual_capacity = map->capacity + 1;

	for (Index index = 0; index < actual_capacity; index++)
	{
		map->keys[index] = MAP_KEY_EMPTY;
	}
}

void _map_insert_no_grow(Map* map, Map_Key key, Map_Value value)
{
	Map_Hash hash = _map_get_hash(key);

	while (TRUE)
	{
		Index index = _map_get_hash_index(hash, map->capacity);

		Map_Key actual_key = map->keys[index];

		if (actual_key == MAP_KEY_EMPTY)
		{
			map->total_count++;

			map->keys[index] = key;

			map->values[index] = value;

			return;
		}

		if (actual_key == MAP_KEY_DELETED)
		{
			map->deleted_count--;

			map->keys[index] = key;

			map->values[index] = value;

			return;
		}

		hash = _map_step_hash(hash);
	}
}

Index _map_get_key_index(Map* map, Map_Key key)
{
	Map_Hash hash = _map_get_hash(key);

	while (TRUE)
	{
		Index index = _map_get_hash_index(hash, map->capacity);

		Map_Key actual_key = map->keys[index];

		if (actual_key == key)
		{
			return index;
		}

		if (actual_key == MAP_KEY_EMPTY)
		{
			return INDEX_MAX;
		}

		hash = _map_step_hash(hash);
	}
}

void _map_resize(Map* map, Count new_capacity)
{
	Map_Key* keys = map->keys;

	Map_Value* values = map->values;

	Count actual_capacity = map->capacity + 1;

	map->total_count = 0;

	map->deleted_count = 0;

	map->keys = calloc(new_capacity, sizeof(Map_Key));

	map->values = calloc(new_capacity, sizeof(Map_Value));

	map->capacity = new_capacity - 1;

	_map_clear_keys(map);

	for (Index index = 0; index < actual_capacity; index++)
	{
		Map_Key key = keys[index];

		if (key != MAP_KEY_EMPTY && key != MAP_KEY_DELETED)
		{
			_map_insert_no_grow(map, key, values[index]);
		}
	}

	free(keys);

	free(values);
}

void _map_grow(Map* map)
{
	Count actual_capacity = map->capacity + 1;

	if (map->total_count * 2 + 1 >= actual_capacity)
	{
		Count new_capacity = actual_capacity << 1;

		_map_resize(map, new_capacity);
	}
}

void _map_shrink(Map* map)
{
	Count actual_capacity = map->capacity + 1;

	if (map->deleted_count >= actual_capacity / 4)
	{
		Count new_capacity = actual_capacity >> 1;

		_map_resize(map, new_capacity);
	}
}

Map* map_create()
{
	Map* map = calloc(1, sizeof(Map));

	Count initial_capacity = MAP_INITIAL_CAPACITY;

	map->capacity = initial_capacity - 1;

	map->keys = calloc(initial_capacity, sizeof(Map_Key));

	map->values = calloc(initial_capacity, sizeof(Map_Value));

	_map_clear_keys(map);

	return map;
}

void map_destroy(Map* map)
{
	free(map->keys);

	free(map->values);

	free(map);
}

Count map_size(Map* map)
{
	return map->total_count - map->deleted_count;
}

void map_insert(Map* map, Map_Key key, Map_Value value)
{
	_map_grow(map);

	_map_insert_no_grow(map, key, value);
}

Bool map_get(Map* map, Map_Key key, Map_Value* value)
{
	Index index = _map_get_key_index(map, key);

	if (index == INDEX_MAX)
	{
		return FALSE;
	}

	*value = map->values[index];

	return TRUE;
}

Bool map_remove(Map* map, Map_Key key)
{
	Index index = _map_get_key_index(map, key);

	if (index == INDEX_MAX)
	{
		return FALSE;
	}

	map->keys[index] = MAP_KEY_DELETED;

	map->deleted_count++;

	_map_shrink(map);

	return TRUE;
}

void map_clear(Map* map)
{
	map->total_count = 0;

	map->deleted_count = 0;

	_map_clear_keys(map);
}

