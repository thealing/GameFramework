#include "pool.h"

struct Pool
{
	size_t object_size;

	void* object_list;

	void** free_list;

	size_t free_count;
};

Pool* pool_create(size_t capacity, size_t size)
{
	Pool* pool = calloc(1, sizeof(Pool));

	pool->object_size = size;

	pool->object_list = calloc(capacity, size);

	pool->free_list = calloc(capacity, sizeof(void*));

	while (pool->free_count < capacity)
	{
		pool->free_list[pool->free_count] = (char*)pool->object_list + pool->free_count * size;

		pool->free_count++;
	}

	return pool;
}

void pool_destroy(Pool* pool)
{
	free(pool->object_list);

	free(pool->free_list);

	free(pool);
}

void* pool_alloc(Pool* pool)
{
	pool->free_count--;

	return pool->free_list[pool->free_count];
}

void pool_free(Pool* pool, void* object)
{
	pool->free_list[pool->free_count] = object;

	pool->free_count++;
}
