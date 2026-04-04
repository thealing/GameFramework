#include "config.h"

static void* s_config_values[CONFIG_KEY_COUNT];

void* config_get_value(Config_Key key)
{
	return s_config_values[key];
}

void config_set_value(Config_Key key, void* value)
{
	s_config_values[key] = value;
}
