#include "objects.h"

static const Vector s_mine[] = {{-23,-10},{-9,8},{9,8},{23,-10}};

static const Vector s_lifter_base[] = {{-55,1},{-44,10},{44,10},{54,1}};

static const Vector s_lifter_sensor[] = {{-37,10},{39,10},{58,196},{-56,196}};

static const Vector s_booster_top[] = {{-30,6},{30,6},{50,-5},{-50,-5}};

static const Vector s_booster_bottom[] = {{0,-2},{-61,-7},{61,-7}};

static bool saw_collision_callback(Physics_Collider* saw_collider, Physics_Collider* other_collider)
{
	UNUSED(saw_collider);

	if (other_collider->flags & FLAG_HEAD)
	{
		Car* car = other_collider->data;

		return !car->done;
	}

	return false;
}

static bool mine_collision_callback(Physics_Collider* mine_collider, Physics_Collider* other_collider)
{
	UNUSED(other_collider);

	Mine* mine = mine_collider->data;

	if (mine->active || mine_collider->flags & FLAG_TOUCHED)
	{
		if (mine->state == 0)
		{
			mine->state = 1;

			mine->count_time = 0;
		}
	}

	return true;
}

static bool lifter_collision_callback(Physics_Collider* lifter_collider, Physics_Collider* other_collider)
{
	Physics_Body* lifter_body = lifter_collider->body;

	Physics_Body* other_body = other_collider->body;

	if (other_body != lifter_collider->body && other_body->type == PHYSICS_BODY_TYPE_DYNAMIC)
	{
		Vector closest_point = vector_create(INFINITY, INFINITY);

		double closest_distance = INFINITY;

		bool is_head = false;

		for (List_Node* collider_node = other_body->collider_list.first; collider_node != NULL; collider_node = collider_node->next)
		{
			Physics_Collider* collider = collider_node->item;

			Vector point = shape_project_point(collider->world_shape, lifter_body->position);

			double distance = vector_distance_squared(point, lifter_body->position);

			if (distance < closest_distance)
			{
				closest_point = point;

				closest_distance = distance;
			}	
			
			if (collider->flags & FLAG_HEAD)
			{
				is_head = true;
			}
		}

		if (!is_head)
		{
			Vector vector = vector_subtract(closest_point, lifter_body->position);

			Vector normal = vector_rotate(vector_create(0, 1), lifter_body->angle);

			double impulse = 2e5 / vector_dot(normal, vector);

			physics_body_apply_impulse_at_world_point(other_body, closest_point, vector_multiply(normal, impulse));
		}
	}

	return true;
}

static bool booster_collision_callback(Physics_Collider* booster_collider, Physics_Collider* other_collider)
{
	if (booster_collider->flags & FLAG_SAFE)
	{
		if (other_collider->flags & FLAG_WHEEL)
		{
			Booster* booster = booster_collider->data;

			Wheel* wheel = other_collider->data;

			double multiplier = booster->reversed ? -1 : 1;

			Vector boost = vector_rotate(vector_create(BOOSTER_STRENGTH * multiplier, 0), booster->body->angle);

			if (wheel->side == booster->reversed)
			{
				wheel->boost_forward = vector_add(wheel->boost_forward, boost);
			}
			else
			{
				wheel->boost_backward = vector_add(wheel->boost_backward, boost);
			}
		}

		return other_collider->filter_group == GROUP_LEVEL;
	}
	else
	{
		return other_collider->filter_group != GROUP_LEVEL;
	}
}

Object* object_create_saw(Physics_World* world, Vector position, double radius, bool reversed)
{
	Saw* saw = calloc(1, sizeof(Saw));

	saw->type = OBJECT_TYPE_SAW;

	saw->body = physics_body_create(world, PHYSICS_BODY_TYPE_KINEMATIC);

	saw->body->position = position;

	saw->body->angular_velocity = reversed ? -3 : 3;

	Shape* shape = shape_create_circle(vector_create(0, 0), radius);

	Physics_Collider* collider = physics_collider_create(saw->body, move_shape(shape), 1);

	collider->collision_callback = saw_collision_callback;

	collider->data = saw;

	collider->flags |= FLAG_OBJECT;

	saw->radius = radius;

	return saw;
}

Object* object_create_mine(Physics_World* world, Vector position, bool active)
{
	Mine* mine = calloc(1, sizeof(Mine));

	mine->type = OBJECT_TYPE_MINE;

	mine->body = physics_body_create(world, PHYSICS_BODY_TYPE_DYNAMIC);

	mine->body->position = position;

	Shape* shape = shape_create_polygon(COUNTOF(s_mine), s_mine);

	Physics_Collider* collider = physics_collider_create(mine->body, move_shape(shape), 5);

	collider->static_friction = 1;

	collider->dynamic_friction = 1;

	collider->collision_callback = mine_collision_callback;

	collider->data = mine;

	collider->flags |= FLAG_OBJECT;

	collider->flags |= FLAG_MINE;

	mine->active = active;

	return mine;
}

Object* object_create_box(Physics_World* world, Vector position, double weight)
{
	Box* box = calloc(1, sizeof(Box));

	box->type = OBJECT_TYPE_BOX;

	box->body = physics_body_create(world, PHYSICS_BODY_TYPE_DYNAMIC);

	box->body->position = position;

	Shape* shape = create_rect_shape(create_isotropic_vector(-20), create_isotropic_vector(20));

	Physics_Collider* collider = physics_collider_create(box->body, move_shape(shape), weight);

	collider->static_friction = 0.5;

	collider->dynamic_friction = 0.5;

	collider->restitution = 0.4;

	collider->data = box;

	collider->flags |= FLAG_OBJECT;

	return box;
}

Object* object_create_tire(Physics_World* world, Vector position, double radius)
{
	Tire* tire = calloc(1, sizeof(Tire));

	tire->type = OBJECT_TYPE_TIRE;

	tire->body = physics_body_create(world, PHYSICS_BODY_TYPE_DYNAMIC);

	tire->body->position = position;

	Shape* shape = shape_create_circle(vector_create(0, 0), radius);

	Physics_Collider* collider = physics_collider_create(tire->body, move_shape(shape), 1);

	collider->static_friction = 1;

	collider->dynamic_friction = 1;

	collider->restitution = 0.75;

	collider->data = tire;

	collider->flags |= FLAG_OBJECT;

	tire->radius = radius;

	return tire;
}

Object* object_create_lifter(Physics_World* world, Vector position)
{
	Lifter* lifter = calloc(1, sizeof(Lifter));

	lifter->type = OBJECT_TYPE_LIFTER;

	lifter->body = physics_body_create(world, PHYSICS_BODY_TYPE_DYNAMIC);

	lifter->body->position = position;

	Shape* base_shape = shape_create_polygon(COUNTOF(s_lifter_base), s_lifter_base);

	Physics_Collider* base_collider = physics_collider_create(lifter->body, move_shape(base_shape), 2);

	base_collider->static_friction = 1;

	base_collider->dynamic_friction = 1;

	base_collider->data = lifter;

	base_collider->flags |= FLAG_OBJECT;

	Shape* sensor_shape = shape_create_polygon(COUNTOF(s_lifter_sensor), s_lifter_sensor);

	Physics_Collider* sensor_collider = physics_collider_create(lifter->body, move_shape(sensor_shape), 0);

	sensor_collider->sensor = true;

	sensor_collider->collision_callback = lifter_collision_callback;

	sensor_collider->data = lifter;

	sensor_collider->flags |= FLAG_SAFE;

	return lifter;
}

Object* object_create_booster(Physics_World* world, Vector position, double angle, bool reversed)
{
	Booster* booster = calloc(1, sizeof(Booster));

	booster->type = OBJECT_TYPE_BOOSTER;

	booster->body = physics_body_create(world, PHYSICS_BODY_TYPE_DYNAMIC);

	booster->body->position = position;

	booster->body->angle = angle;

	Shape* top_shape = shape_create_polygon(COUNTOF(s_booster_top), s_booster_top);

	Physics_Collider* top_collider = physics_collider_create(booster->body, move_shape(top_shape), 3);

	top_collider->static_friction = 0.9;

	top_collider->dynamic_friction = 0.9;

	top_collider->collision_callback = booster_collision_callback;

	top_collider->data = booster;

	top_collider->flags |= FLAG_SAFE;

	Shape* bottom_shape = shape_create_polygon(COUNTOF(s_booster_bottom), s_booster_bottom);

	Physics_Collider* bottom_collider = physics_collider_create(booster->body, move_shape(bottom_shape), 4);

	bottom_collider->static_friction = 0.9;

	bottom_collider->dynamic_friction = 0.9;

	bottom_collider->collision_callback = booster_collision_callback;

	bottom_collider->data = booster;

	bottom_collider->flags |= FLAG_OBJECT;

	booster->reversed = reversed;

	return booster;
}

void object_destroy(Object* object)
{
	switch (object->type)
	{
		case OBJECT_TYPE_SAW:
		{
			Saw* saw = object;

			physics_body_destroy(saw->body);

			break;
		}
		case OBJECT_TYPE_MINE:
		{
			Mine* mine = object;

			physics_body_destroy(mine->body);

			sound_stop(g_sounds.beep);

			sound_stop(g_sounds.explosion);

			break;
		}
		case OBJECT_TYPE_BOX:
		{
			Box* box = object;

			physics_body_destroy(box->body);

			break;
		}
		case OBJECT_TYPE_TIRE:
		{
			Tire* tire = object;

			physics_body_destroy(tire->body);

			break;
		}
		case OBJECT_TYPE_LIFTER:
		{
			Lifter* lifter = object;

			physics_body_destroy(lifter->body);

			break;
		}
		case OBJECT_TYPE_BOOSTER:
		{
			Booster* booster = object;

			physics_body_destroy(booster->body);

			break;
		}
	}

	free(object);
}

void object_update(Object* object, double delta_time)
{
	switch (object->type)
	{
		case OBJECT_TYPE_SAW:
		{
			if (!sound_is_playing(g_sounds.saw))
			{
				sound_play(g_sounds.saw);
			}

			break;
		}
		case OBJECT_TYPE_MINE:
		{
			Mine* mine = object;

			if (mine->state == 1)
			{
				if (mine->count_time / 0.3 >= mine->counter)
				{
					sound_play(g_sounds.beep);

					mine->counter++;
				}

				if (mine->counter >= 4)
				{
					mine->state = 2;

					mine->count_time = 0;

					sound_stop(g_sounds.beep);

					sound_play(g_sounds.explosion);
				}

				if (mine->counter >= 2)
				{
					for (List_Node* collider_node = mine->body->world->collider_list.first; collider_node != NULL; collider_node = collider_node->next)
					{
						Physics_Collider* collider = collider_node->item;

						if (collider->flags & FLAG_MINE)
						{
							Mine* other_mine = collider->data;

							if (other_mine->state == 0 && vector_distance_squared(mine->body->position, other_mine->body->position) < square(MINE_RANGE))
							{
								other_mine->state = 1;

								other_mine->count_time = 0;
							}
						}
					}
				}
			}

			if (mine->state == 2)
			{
				Physics_World* world = mine->body->world;

				for (List_Node* body_node = world->body_list.first; body_node != NULL; body_node = body_node->next)
				{
					Physics_Body* body = body_node->item;

					if (body != mine->body && body->type == PHYSICS_BODY_TYPE_DYNAMIC)
					{
						Vector closest_point = vector_create(INFINITY, INFINITY);

						double closest_distance = INFINITY;

						bool is_car = false;

						for (List_Node* collider_node = body->collider_list.first; collider_node != NULL; collider_node = collider_node->next)
						{
							Physics_Collider* collider = collider_node->item;

							if (collider->flags & FLAG_CHASSIS || collider->flags & FLAG_OBJECT && !(collider->flags & FLAG_MINE))
							{
								Vector point = shape_project_point(collider->world_shape, mine->body->position);

								double distance = vector_distance_squared(point, mine->body->position);

								if (distance < closest_distance)
								{
									closest_point = point;

									closest_distance = distance;
								}
							}

							if (collider->flags & FLAG_CHASSIS)
							{
								is_car = true;
							}
						}

						Vector vector = vector_subtract(closest_point, mine->body->position);

						if (closest_distance < square(MINE_RANGE))
						{
							closest_distance = fmin(square(MINE_RANGE / 2), closest_distance);

							if (is_car)
							{
								physics_body_apply_speed_at_world_point(body, closest_point, vector_multiply(vector, 2700 / closest_distance));
							}
							else
							{
								physics_body_apply_impulse_at_world_point(body, closest_point, vector_multiply(vector, 1e6 / closest_distance));
							}
						}
					}
				}

				if (mine->count_time > MINE_EXPLOSION_DURATION)
				{
					mine->state = 3;

					mine->body->position = vector_create(10000, 10000);
				}
			}

			mine->count_time += delta_time;

			break;
		}
		case OBJECT_TYPE_BOX:
		{
			break;
		}
		case OBJECT_TYPE_TIRE:
		{
			break;
		}
		case OBJECT_TYPE_LIFTER:
		{
			Lifter* lifter = object;

			lifter->time += delta_time;

			break;
		}
		case OBJECT_TYPE_BOOSTER:
		{
			Booster* booster = object;

			booster->time += delta_time;

			break;
		}
	}
}

void object_render(Object* object)
{
	switch (object->type)
	{
		case OBJECT_TYPE_SAW:
		{
			Saw* saw = object;

			set_texture_and_color(saw->body->angular_velocity > 0 ? g_textures.saw_left : g_textures.saw_right, NULL);

			draw_texture_scaled(saw->body->position, saw->body->angle, saw->radius);

			break;
		}
		case OBJECT_TYPE_MINE:
		{
			Mine* mine = object;

			if (mine->state == 0)
			{
				set_texture_and_color(g_textures.mine_0, NULL);

				graphics_draw_texture_at(mine->body->position, mine->body->angle);
			}

			if (mine->state == 1)
			{
				if (fmod(mine->count_time, 0.3) < 0.1)
				{
					set_texture_and_color(g_textures.mine_1, NULL);
				}
				else
				{
					set_texture_and_color(g_textures.mine_2, NULL);
				}

				graphics_draw_texture_at(mine->body->position, mine->body->angle);
			}

			if (mine->state == 2)
			{
				double progress = mine->count_time / MINE_EXPLOSION_DURATION;

				set_texture_and_color(NULL, &(Color){ 1, 0, 0, 1.2 - progress });

				graphics_draw_circle(&(Circle){ mine->body->position, square(progress) * 0.8 * MINE_RANGE }, true);
			}

			break;
		}
		case OBJECT_TYPE_BOX:
		{
			Box* box = object;

			set_texture_and_color(g_textures.box, NULL);

			graphics_draw_texture_at(box->body->position, box->body->angle);

			break;
		}
		case OBJECT_TYPE_TIRE:
		{
			Tire* tire = object;

			set_texture_and_color(g_textures.tire, NULL);

			draw_texture_scaled(tire->body->position, tire->body->angle, tire->radius);

			break;
		}
		case OBJECT_TYPE_LIFTER:
		{
			Lifter* lifter = object;

			set_texture_and_color(g_textures.lifter_base, NULL);

			graphics_draw_texture_at(lifter->body->position, lifter->body->angle);

			set_texture_and_color(g_textures.lifter_sensor, &(Color){ 1, 1, 1, 0.8 + fabs(fmod(lifter->time / 3, 0.4) - 0.2) });

			graphics_draw_texture_at(lifter->body->position, lifter->body->angle);

			break;
		}
		case OBJECT_TYPE_BOOSTER:
		{
			Booster* booster = object;

			set_texture_and_color(g_textures.booster[(int)(booster->time * 10) % 3], NULL);

			draw_texture_flipped(booster->body->position, booster->body->angle, booster->reversed, false);

			break;
		}
	}
}
