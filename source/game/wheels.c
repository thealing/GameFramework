#include "wheels.h"

Wheel* wheel_create(Wheel_Type type, Physics_Body* chassis_body, Vector chassis_offset, int group, double size, double acceleration, double max_speed, double grip, double density)
{
	Wheel* wheel = calloc(1, sizeof(Wheel));

	wheel->side = group & 1;

	wheel->size = size;

	wheel->acceleration = acceleration;

	wheel->max_speed = max_speed;

	wheel->body = physics_body_create(chassis_body->world, PHYSICS_BODY_TYPE_DYNAMIC);

	wheel->body->position = vector_add(chassis_body->position, chassis_offset);

	Shape* shape = shape_create_circle(vector_create(0, 0), size);

	Physics_Collider* collider = physics_collider_create(wheel->body, move_shape(shape), density);

	collider->static_friction = 1;

	collider->dynamic_friction = grip;

	collider->filter_group = group;

	collider->data = wheel;

	collider->flags |= FLAG_CAR;

	collider->flags |= FLAG_WHEEL;

	physics_joint_create_world(PHYSICS_JOINT_TYPE_PIN, wheel->body, wheel->body->position, chassis_body, wheel->body->position);

	switch (type)
	{
		case WHEEL_TYPE_SMALL:
		{
			wheel->texture = g_textures.wheel_small[wheel->side];

			break;
		}
		case WHEEL_TYPE_MEDIUM:
		{
			wheel->texture = g_textures.wheel_medium[wheel->side];

			break;
		}
		case WHEEL_TYPE_MONSTER:
		{
			wheel->texture = g_textures.wheel_monster[wheel->side];

			break;
		}
		case WHEEL_TYPE_CYCLE:
		{
			wheel->texture = g_textures.wheel_cycle[wheel->side];

			break;
		}
	}

	return wheel;
}

void wheel_destroy(Wheel* wheel)
{
	physics_body_destroy(wheel->body);

	free(wheel);
}

void wheel_update(Wheel* wheel, bool forward, bool backward)
{
	double multiplier = wheel->side ? -1 : 1;

	if (forward && backward)
	{
		wheel->body->angular_velocity = 0;
	}
	else if (forward)
	{
		wheel->body->angular_velocity -= wheel->acceleration * multiplier;
	}
	else if (backward)
	{
		wheel->body->angular_velocity += wheel->acceleration * multiplier;
	}
	else 
	{
		wheel->body->angular_velocity *= 0.99;
	}

	wheel->body->angular_velocity = clamp(wheel->body->angular_velocity, -wheel->max_speed, wheel->max_speed);
}

void wheel_render(Wheel* wheel)
{
	set_texture_and_color(wheel->texture, NULL);

	graphics_save_transform();

	graphics_translate(wheel->body->position);

	graphics_rotate(wheel->body->angle);

	graphics_scale_uniformly(wheel->size / 64.0);

	graphics_draw_texture();

	graphics_load_transform();
}
