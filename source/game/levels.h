#pragma once

#include "textures.h"

#include "sounds.h"

#include "util.h"

#include "objects.h"

#include "engine/random.h"

#include "engine/time.h"

#include <stdarg.h>

#define OBJECT_COUNT_MAX 1000

#define RANDOM_LOCATION_COUNT_MAX 6

typedef enum Level_Type Level_Type;

typedef enum Armageddon_Type Armageddon_Type;

typedef struct Location Location;

typedef struct Level Level;

enum Level_Type
{
	LEVEL_TYPE_CAVE,

	LEVEL_TYPE_TUNNEL,

	LEVEL_TYPE_DESERT_CAVE,

	LEVEL_TYPE_DESERT_TUNNEL,

	LEVEL_TYPE_RAMP,

	LEVEL_TYPE_GAP,

	LEVEL_TYPE_BUMP,

	LEVEL_TYPE_DOUBLE_BUMP,

	LEVEL_TYPE_TRIPLE_BUMP,

	LEVEL_TYPE_SMILEY_FACE,

	LEVEL_TYPE_FROWNY_FACE,

	LEVEL_TYPE_NEUTRAL_FACE,

	LEVEL_TYPE_BLADES,

	LEVEL_TYPE_COUNT
};

enum Armageddon_Type
{
	ARMAGEDDON_TYPE_LASER_UP,

	ARMAGEDDON_TYPE_LASER_DOWN,

	ARMAGEDDON_TYPE_WATER_RISE,

	ARMAGEDDON_TYPE_MINE_RAIN,

	ARMAGEDDON_TYPE_COUNT
};

struct Location
{
	Vector position;

	bool types[OBJECT_TYPE_COUNT];
};

struct Level
{
	Physics_Body* body;

	Texture* texture;

	Vector blue_spawn;

	Vector red_spawn;

	double time;

	Location random_locations[RANDOM_LOCATION_COUNT_MAX];

	int random_location_count;

	struct Object* objects[OBJECT_COUNT_MAX];

	int object_count;

	Armageddon_Type armageddon_type;

	bool armageddon_active;

	Physics_Body* laser_body;

	Physics_Body* water_body;

	double mine_spawn_time;
};

Level* level_create(Level_Type type, Physics_World* world, int group);

void level_destroy(Level* level);

void level_update(Level* level, double delta_time);

void level_render(Level* level);

Vector level_project_point(Level* level, Vector point);

void level_add_random_objects(Level* level);

void level_start_armageddon(Level* level);

void level_stop_armageddon(Level* level);
