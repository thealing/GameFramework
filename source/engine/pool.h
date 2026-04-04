#pragma once

#include <stdlib.h>

#include <malloc.h>

typedef struct Pool Pool;

Pool* pool_create(size_t capacity, size_t size);

void pool_destroy(Pool* pool);

void* pool_alloc(Pool* pool);

void pool_free(Pool* pool, void* object);

