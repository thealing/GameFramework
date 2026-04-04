#pragma once

#include "textures.h"

#include "util.h"

#include "defines.h"

typedef enum Wheel_Type Wheel_Type;

typedef struct Wheel Wheel;

enum Wheel_Type
{
	WHEEL_TYPE_SMALL,

	WHEEL_TYPE_MEDIUM,

	WHEEL_TYPE_MONSTER,

	WHEEL_TYPE_CYCLE,

	WHEEL_TYPE_COUNT
};

struct Wheel
{
	int side;

	double size;

	double acceleration;

	double max_speed;

	Physics_Body* body;

	Texture* texture;

	Vector boost_forward;

	Vector boost_backward;
};

Wheel* wheel_create(Wheel_Type type, Physics_Body* chassis_body, Vector chassis_offset, int group, double size, double acceleration, double max_speed, double grip, double density);

void wheel_destroy(Wheel* wheel);

void wheel_update(Wheel* wheel, bool forward, bool backward);

void wheel_render(Wheel* wheel);
