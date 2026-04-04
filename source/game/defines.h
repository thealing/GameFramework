#pragma once

#define DEATH_DELAY 2

#define AIR_TIME_THRESHOLD 0.5

#define MINE_RANGE 250

#define MINE_EXPLOSION_DURATION 0.1

#define BOOSTER_STRENGTH 20

enum
{
	GROUP_LEVEL = -1,

	GROUP_BLUE_CAR = -2,

	GROUP_RED_CAR = -3,
};

enum
{
	FLAG_CAR = (1 << 0),

	FLAG_CHASSIS = (1 << 1),

	FLAG_WHEEL = (1 << 2),

	FLAG_HEAD = (1 << 3),

	FLAG_WATER = (1 << 4),

	FLAG_OBJECT = (1 << 5),

	FLAG_TOUCHED = (1 << 6),

	FLAG_MINE = (1 << 7),

	FLAG_SAFE = (1 << 8),
};
