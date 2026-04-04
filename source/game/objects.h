#pragma once

#include "util.h"

#include "defines.h"

#include "sounds.h"

#include "cars.h"

typedef enum Object_Type Object_Type;

typedef struct Object Object;

typedef struct Saw Saw;

typedef struct Mine Mine;

typedef struct Box Box;

typedef struct Tire Tire;

typedef struct Lifter Lifter;

typedef struct Booster Booster;

enum Object_Type
{
	OBJECT_TYPE_NONE,

	OBJECT_TYPE_SAW,

	OBJECT_TYPE_MINE,

	OBJECT_TYPE_BOX,

	OBJECT_TYPE_TIRE,

	OBJECT_TYPE_LIFTER,

	OBJECT_TYPE_BOOSTER,

	OBJECT_TYPE_COUNT
};

struct Object
{
	Object_Type type;
};

struct Saw
{
	Object_Type type;

	Physics_Body* body;

	double radius;
};

struct Mine
{
	Object_Type type;

	Physics_Body* body;

	bool active;

	int state;

	double count_time;

	int counter;
};

struct Box
{
	Object_Type type;

	Physics_Body* body;
};

struct Tire
{
	Object_Type type;

	Physics_Body* body;

	double radius;
};

struct Lifter
{
	Object_Type type;

	Physics_Body* body;

	double time;
};

struct Booster
{
	Object_Type type;

	Physics_Body* body;

	bool reversed;

	double time;
};

Object* object_create_saw(Physics_World* world, Vector position, double radius, bool reversed);

Object* object_create_mine(Physics_World* world, Vector position, bool active);

Object* object_create_box(Physics_World* world, Vector position, double weight);

Object* object_create_tire(Physics_World* world, Vector position, double radius);

Object* object_create_lifter(Physics_World* world, Vector position);

Object* object_create_booster(Physics_World* world, Vector position, double angle, bool reversed);

void object_destroy(Object* object);

void object_update(Object* object, double delta_time);

void object_render(Object* object);
