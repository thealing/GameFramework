#include "engine/data.h"

#include "platform.h"

static const char* get_data_folder()
{
	static char data_folder[DATA_PATH_LENGTH];

	if (data_folder[0] == '\0')
	{
		PWSTR roaming_path = NULL;

		SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &roaming_path);

		if (roaming_path != NULL)
		{
			char* path_prefix = config_get_value(CONFIG_KEY_FOLDER_NAME);

			snprintf(data_folder, DATA_PATH_LENGTH, "%ls/%s", roaming_path, path_prefix);

			SHCreateDirectoryEx(NULL, data_folder, NULL);

			CoTaskMemFree(roaming_path);
		}
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
