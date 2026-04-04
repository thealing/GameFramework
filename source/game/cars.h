#pragma once

#include "util.h"

#include "wheels.h"

#include "defines.h"

typedef enum Car_Type Car_Type;

typedef struct Car Car;

enum Car_Type
{
	CAR_TYPE_MONSTER_TRUCK,

	CAR_TYPE_DRAGSTER,

	CAR_TYPE_GO_CART,

	CAR_TYPE_MINIBUS,

	CAR_TYPE_TRACTOR,

	CAR_TYPE_TIMBER_LORRY,

	CAR_TYPE_GARBAGE_TRUCK,
	
	CAR_TYPE_COUNT
};

struct Car
{
	Car_Type type;

	int side;

	bool done;

	Physics_Body* chassis_body;

	Physics_Body* head_body;

	Physics_Body* bodies[10];

	Texture* textures[10];
	
	Wheel* wheels[2];
};

Car* car_create(Car_Type type, Physics_World* world, Vector position, int group);

void car_destroy(Car* car);

void car_update(Car* car, bool forward, bool backward);

void car_render(Car* car);

void car_kill(Car* car);
