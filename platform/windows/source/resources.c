#include "generated/resources.h"

#include "platform.h"

static char s_asset_root[MAX_PATH];

static char s_asset_files[ASSET_COUNT][MAX_PATH];

static void delete_assets(void)
{
	for (int i = 0; i < ASSET_COUNT; i++)
	{
		DeleteFile(s_asset_files[i]);
	}
}

void extract_assets(const char* folder_name)
{
	char temp_root[MAX_PATH] = { 0 };

	GetTempPath(MAX_PATH, temp_root);

	snprintf(s_asset_root, MAX_PATH, "%s%s", temp_root, folder_name);

	HMODULE module = GetModuleHandle(NULL);

	for (int i = 0; i < ASSET_COUNT; i++)
	{
		HRSRC resource = FindResource(module, MAKEINTRESOURCE(i + 1), RT_RCDATA);

		DWORD size = SizeofResource(module, resource);

		void* data = LockResource(LoadResource(module, resource));

		char* asset_path = s_asset_files[i];

		snprintf(asset_path, MAX_PATH, "%s\\%s", s_asset_root, g_asset_paths[i]);

		char asset_directory[MAX_PATH] = { 0 };

		strcpy(asset_directory, asset_path);

		PathRemoveFileSpec(asset_directory);

		SHCreateDirectoryEx(NULL, asset_directory, NULL);

		HANDLE handle = CreateFile(asset_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

		DWORD written = 0;

		WriteFile(handle, data, size, &written, NULL);

		CloseHandle(handle);
	}

	SetCurrentDirectory(s_asset_root);

	atexit(delete_assets);
}
