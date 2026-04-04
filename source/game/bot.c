#include "bot.h"

Bot* bot_create()
{
	Bot* bot = calloc(1, sizeof(Bot));

	return bot;
}

void bot_destroy(Bot* bot)
{
	free(bot);
}

void bot_reset(Bot* bot)
{
	bot->forward = false;

	bot->backward = false;

	bot->update_time = 0.4;
}

void bot_update(Bot* bot, bool in_air, double bot_location, double bot_angle, Vector bot_velocity, double enemy_location, double time)
{
	UNUSED(bot_velocity);

	if (time > bot->update_time)
	{
		if (enemy_location > bot_location)
		{
			bot->forward = true;

			bot->backward = false;
		}

		if (enemy_location < bot_location)
		{
			bot->forward = false;

			bot->backward = true;
		}

		double threshold = in_air ? -0.1 : 0.3;

		if (bot->forward && bot_angle > M_PI * threshold)
		{
			bot->forward = false;

			bot->backward = true;
		}

		if (bot->backward && bot_angle < -M_PI * threshold)
		{
			bot->forward = true;

			bot->backward = false;
		}

		bot->update_time = time + random_real_below(0.24);
	}

	if (bot_angle > M_PI * 0.45)
	{
		bot->forward = false;

		bot->backward = true;

		bot->update_time = time;
	}

	if (bot_angle < -M_PI * 0.45)
	{
		bot->forward = true;

		bot->backward = false;

		bot->update_time = time;
	}
}
