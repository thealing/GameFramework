#pragma once

#include <stdint.h>

#include <string.h>

typedef union Pixel Pixel;

typedef struct Image Image;

union Pixel
{
	struct
	{
		uint8_t red;

		uint8_t green;

		uint8_t blue;

		uint8_t alpha;
	};

	uint32_t value;
};

struct Image
{
	int width;

	int height;

	Pixel* pixels;
};

Image* image_load(const char* path);

void image_destroy(Image* image);
