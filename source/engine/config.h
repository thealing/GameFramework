#pragma once

typedef enum Config_Key Config_Key;

enum Config_Key
{
	CONFIG_KEY_WINDOW_TITLE,

	CONFIG_KEY_WINDOW_CLASS,

	CONFIG_KEY_FOLDER_NAME,

	CONFIG_KEY_TOUCH_ZOOM_ENABLED,

	CONFIG_KEY_COUNT
};

void* config_get_value(Config_Key key);

void config_set_value(Config_Key key, void* value);
