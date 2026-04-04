#pragma once

#include "engine/data.h"

#include <stdbool.h>

#include <memory.h>

typedef struct Save Save;

struct Save
{
	bool play_music;

	bool play_sounds;

	bool random_objects;

	bool same_random_car;

	int sudden_death_time;

	bool debug_hud;

	int sudden_death_countdown;

	int max_score;

	int selected_level;

	int selected_blue_car;

	int selected_red_car;

	bool blue_is_bot;

	bool red_is_bot;
};

extern Save g_save;

void save_save();

void save_load();
