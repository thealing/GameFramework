#include "engine/data.h"

#include "platform.h"

#include <stdio.h>

#include <string.h>

static const char* get_data_folder()
{
	static const char* data_folder;

	if (data_folder == NULL)
	{
		jobject file = (*env)->CallObjectMethod(env, activity_instance, Activity_getFilesDir);

		jstring path = (*env)->CallObjectMethod(env, file, File_getPath);

		data_folder = (*env)->GetStringUTFChars(env, path, NULL);
	}

	return data_folder;
}

static const char* get_full_path(const char* path)
{
	static char full_path[DATA_PATH_LENGTH];

	snprintf(full_path, DATA_PATH_LENGTH, "%s/%s", get_data_folder(), path);

	return full_path;
}

int data_write(const char* path, void* buffer, int buffer_size)
{
	FILE* file = fopen(get_full_path(path), "wb");

	if (file == NULL)
	{
		return -1;
	}

	int result = (int)fwrite(buffer, 1, buffer_size, file);

	fclose(file);

	return result;
}

int data_read(const char* path, void* buffer, int buffer_size)
{
	FILE* file = fopen(get_full_path(path), "rb");

	if (file == NULL)
	{
		return -1;
	}

	int result = (int)fread(buffer, 1, buffer_size, file);

	fclose(file);

	return result;
}
