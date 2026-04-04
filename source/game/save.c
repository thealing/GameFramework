#include "save.h"

Save g_save;

static Save s_previous_save;

void save_save()
{
	if (memcmp(&g_save, &s_previous_save, sizeof(Save)) == 0)
	{
		return;
	}

	s_previous_save = g_save;

	data_write("save", &g_save, sizeof(g_save));
}

void save_load()
{
	g_save.play_music = true;

	g_save.play_sounds = true;

	g_save.random_objects = true;

	g_save.same_random_car = false;

	g_save.sudden_death_time = 12;

	g_save.debug_hud = false;

	g_save.sudden_death_countdown = 3;

	g_save.max_score = 5;

	g_save.selected_level = 1;

	g_save.selected_blue_car = 1;

	g_save.selected_red_car = 1;

	g_save.blue_is_bot = false;

	g_save.red_is_bot = true;

	data_read("save", &g_save, sizeof(g_save));
}
