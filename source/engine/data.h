#pragma once

#include "config.h"

#include <stdio.h>

#define DATA_PATH_LENGTH 512

int data_write(const char* path, void* buffer, int buffer_size);

int data_read(const char* path, void* buffer, int buffer_size);
