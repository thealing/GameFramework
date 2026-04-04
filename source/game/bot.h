#pragma once

#include "engine/physics.h"

#include "engine/random.h"

typedef struct Bot Bot;

struct Bot
{
	bool forward;

	bool backward;

	double update_time;
};

Bot* bot_create();

void bot_destroy(Bot* bot);

void bot_reset(Bot* bot);

void bot_update(Bot* bot, bool in_air, double bot_location, double bot_angle, Vector bot_velocity, double enemy_location, double time);
