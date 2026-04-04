#include "engine/image.h"

#include "platform.h"

#include <stdlib.h>

#include <string.h>

Image* image_load(const char* path)
{
	AAsset* asset = AAssetManager_open(asset_manager, path, AASSET_MODE_BUFFER);

	if (asset == NULL)
	{
		return NULL;
	}

	const void* data = AAsset_getBuffer(asset);

	int length = AAsset_getLength(asset);

	jbyteArray j_byte_array = (*env)->NewByteArray(env, length);

	(*env)->SetByteArrayRegion(env, j_byte_array, 0, length, (jbyte*)data);

	jobject j_bitmap = (*env)->CallStaticObjectMethod(env, BitmapFactory, BitmapFactory_decodeByteArray, j_byte_array, 0, length);

	int width = (*env)->CallIntMethod(env, j_bitmap, Bitmap_getWidth);

	int height = (*env)->CallIntMethod(env, j_bitmap, Bitmap_getHeight);

	int size = width * height;

	jintArray j_pixel_array = (*env)->NewIntArray(env, width * height);

	(*env)->CallVoidMethod(env, j_bitmap, Bitmap_getPixels, j_pixel_array, 0, width, 0, 0, width, height);

	jint* j_pixels = (*env)->GetIntArrayElements(env, j_pixel_array, NULL);

	Image* image = HEAPALLOC(sizeof(Image));

	image->width = width;

	image->height = height;

	image->pixels = HEAPALLOC(size * sizeof(Pixel));

	for (int i = 0; i < height; i++)
	{
		memcpy(image->pixels + i * width, j_pixels + (height - 1 - i) * width, width * sizeof(Pixel));
	}

	for (int i = 0; i < size; i++)
	{
		uint8_t blue = image->pixels[i].red;

		uint8_t red = image->pixels[i].blue;

		image->pixels[i].red = red;

		image->pixels[i].blue = blue;
	}

	(*env)->ReleaseIntArrayElements(env, j_pixel_array, j_pixels, JNI_ABORT);

	(*env)->DeleteLocalRef(env, j_pixel_array);

	(*env)->DeleteLocalRef(env, j_bitmap);

	(*env)->DeleteLocalRef(env, j_byte_array);

	AAsset_close(asset);

	return image;
}

void image_destroy(Image* image)
{
	free(image->pixels);

	free(image);
}
