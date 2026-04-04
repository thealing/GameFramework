#include "cars.h"

static const Vector s_monster_truck[][5] = {{{-68,12},{-64,-5},{-19,13},{-23,25}},{{-36,7},{-16,-4},{59,9},{-24,12}},{{31,23},{29,12},{38,11}},{{39,6},{59,-6},{63,-7},{63,6}},{{27,4},{13,-15},{-11,-16},{-18,-3}}};

static const Vector s_dragster[][5] = {{{-76,22},{-70,15},{-44,15},{-51,22}},{{-65,14},{-47,-5},{-40,-5},{-40,0},{-54,15}},{{-40,-1},{67,-1},{72,-6},{-40,-6}},{{-26,1},{-26,9},{-21,9},{-16,0}}};

static const Vector s_go_cart[][5] = {{{-23,-6},{-21,1},{16,0},{22,-6}},{{-21,1},{-25,7},{-23,-5}}};

static const Vector s_minibus[][7] = {{{-49,25},{-54,19},{-54,-2},{18,-3},{14,20},{10,24}},{{-54,-3},{-54,-21},{-48,-25},{52,-26},{56,-21},{56,-3}},{{56,-1},{52,5},{48,-1}}};

static const Vector s_tractor[][5] = {{{-43,-15},{-43,10},{42,8},{42,-16}},{{-18,11},{-14,15},{42,14},{42,8}},{{22,29},{20,29},{20,15},{22,15}}};

static const Vector s_timber_lorry[][6] = {{{-69,-17},{-64,-11},{38,-11},{38,-16}},{{-4,-11},{-4,32},{2,32},{10,9}},{{10,9},{36,8},{38,7},{38,-11},{-5,-11}}};

static const Vector s_timber_holder[][5] = {{{-25,25},{27,25},{27,23},{-25,23}},{{-25,-18},{27,-19},{27,-15},{-25,-15}},{{24,-16},{23,25},{27,24},{27,-16}}};

static const Vector s_plank[][5] = {{{-20,4},{21,4},{21,-4},{-20,-4}}};

static const Vector s_garbage_truck[][7] = {{{-43,33},{-43,28},{22,27},{18,33}},{{23,28},{14,28},{12,-27},{21,-27}},{{23,-19},{-51,-20},{-52,-26},{22,-28}},{{28,10},{22,-26},{32,-27},{34,1}},{{34,1},{59,0},{63,-4},{63,-23},{58,-28},{33,-27}}};

static const Vector s_garbage_lid[][5] = {{{6,31},{-4,-32},{-8,-31},{2,31}}};

static const Vector s_garbage[][7] = {{{-1,-5},{9,-2},{9,4},{0,7},{-9,4},{-9,-1}}};

static bool head_collision_callback(Physics_Collider* head_collider, Physics_Collider* other_collider)
{
	Car* car = head_collider->data;

	if (car->done)
	{
		return true;
	}

	if (other_collider->flags & FLAG_SAFE)
	{
		return true;
	}
	
	if (other_collider->flags & FLAG_WATER)
	{
		car->done = true;
	}
	else
	{
		car_kill(car);
	}

	return true;
}

static void create_chassis(Car* car, int index, Physics_World* world, Vector position, int group, double density, const Vector* hitbox, int hitbox_count, int hitbox_length)
{
	Physics_Body* body = physics_body_create(world, PHYSICS_BODY_TYPE_DYNAMIC);

	body->position = position;

	for (int i = 0; i < hitbox_count; i++)
	{
		int length = 0;

		while (hitbox[i * hitbox_length + length].x || hitbox[i * hitbox_length + length].y)
		{
			length++;
		}

		Shape* shape = shape_create_polygon(length, hitbox + i * hitbox_length);

		Physics_Collider* collider = physics_collider_create(body, move_shape(shape), density);

		collider->static_friction = 0.6;

		collider->dynamic_friction = 0.5;

		collider->filter_group = group;

		collider->flags |= FLAG_CAR;

		collider->flags |= FLAG_CHASSIS;
	}

	car->bodies[index] = body;

	car->chassis_body = body;
}

static void create_head(Car* car, int index, Physics_World* world, Vector position, int group, Vector offset)
{
	Physics_Body* body = physics_body_create(world, PHYSICS_BODY_TYPE_DYNAMIC);

	body->position = vector_add(position, offset);

	Shape* shape = shape_create_circle(vector_create(0, 0), 10);

	Physics_Collider* collider = physics_collider_create(body, move_shape(shape), 3.75);

	collider->static_friction = 1;

	collider->dynamic_friction = 1;

	collider->filter_group = group;

	collider->collision_callback = head_collision_callback;

	collider->data = car;

	collider->flags |= FLAG_CAR;

	collider->flags |= FLAG_HEAD;

	physics_joint_create_world(PHYSICS_JOINT_TYPE_FIXED, car->chassis_body, body->position, body, body->position);

	car->bodies[index] = body;

	car->head_body = body;
}

static void create_extra_body(Car* car, int index, Physics_World* world, Vector position, int group, Vector offset, double density, double friction, const Vector* hitbox, int hitbox_count, int hitbox_length, int flags)
{
	Physics_Body* body = physics_body_create(world, PHYSICS_BODY_TYPE_DYNAMIC);

	body->position = vector_add(position, offset);

	for (int i = 0; i < hitbox_count; i++)
	{
		int length = 0;

		while (hitbox[i * hitbox_length + length].x || hitbox[i * hitbox_length + length].y)
		{
			length++;
		}

		Shape* shape = shape_create_polygon(length, hitbox + i * hitbox_length);

		Physics_Collider* collider = physics_collider_create(body, move_shape(shape), density);

		collider->static_friction = friction;

		collider->dynamic_friction = friction;

		collider->filter_group = group;

		collider->flags |= FLAG_CAR;

		collider->flags |= flags;
	}

	car->bodies[index] = body;
}

Car* car_create(Car_Type type, Physics_World* world, Vector position, int group)
{
	Car* car = calloc(1, sizeof(Car));
	
	car->type = type;

	car->side = group & 1;
	
	switch (type)
	{
		case CAR_TYPE_MONSTER_TRUCK:
		{
			create_chassis(car, 1, world, position, group, 1, s_monster_truck, COUNTOF(s_monster_truck), COUNTOF(s_monster_truck[0]));

			create_head(car, 0, world, position, group, vector_create(8, 30));

			car->textures[1] = g_textures.car_monster_truck[car->side];

			car->textures[0] = g_textures.head_neck[car->side];

			car->wheels[0] = wheel_create(WHEEL_TYPE_MONSTER, car->chassis_body, vector_create(-37, -25), group, 22, 2, 80, 3, 1);

			car->wheels[1] = wheel_create(WHEEL_TYPE_MONSTER, car->chassis_body, vector_create(40, -25), group, 22, 2, 80, 3, 1);
			
			break;
		}
		case CAR_TYPE_DRAGSTER:
		{
			create_chassis(car, 1, world, position, group, 0.8, s_dragster, COUNTOF(s_dragster), COUNTOF(s_dragster[0]));

			create_head(car, 0, world, position, group, vector_create(-7, 14));

			car->textures[1] = g_textures.car_dragster[car->side];

			car->textures[0] = g_textures.head_neck[car->side];

			car->wheels[0] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(-43, -3), group, 15, 10, 200, 10, 0.75);

			car->wheels[1] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(60, -9), group, 9, 6, 200, 2, 0.5);

			break;
		}
		case CAR_TYPE_GO_CART:
		{
			create_chassis(car, 1, world, position, group, 10, s_go_cart, COUNTOF(s_go_cart), COUNTOF(s_go_cart[0]));

			create_head(car, 0, world, position, group, vector_create(-2, 14));

			car->textures[1] = g_textures.car_go_cart[car->side];

			car->textures[0] = g_textures.head_neck[car->side];

			car->wheels[0] = wheel_create(WHEEL_TYPE_SMALL, car->chassis_body, vector_create(-15, -9), group, 7, 20, 200, 10, 10);

			car->wheels[1] = wheel_create(WHEEL_TYPE_SMALL, car->chassis_body, vector_create(15, -9), group, 7, 20, 200, 10, 10);

			break;
		}
		case CAR_TYPE_MINIBUS:
		{
			create_chassis(car, 1, world, position, group, 0.6, s_minibus, COUNTOF(s_minibus), COUNTOF(s_minibus[0]));

			create_head(car, 0, world, position, group, vector_create(33, 16));

			car->textures[1] = g_textures.car_minibus[car->side];

			car->textures[0] = g_textures.head_neck[car->side];

			car->wheels[0] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(-37, -30), group, 14, 5, 80, 5, 2);

			car->wheels[1] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(39, -30), group, 14, 5, 80, 5, 2);

			break;
		}
		case CAR_TYPE_TRACTOR:
		{
			create_chassis(car, 1, world, position, group, 1.2, s_tractor, COUNTOF(s_tractor), COUNTOF(s_tractor[0]));

			create_head(car, 0, world, position, group, vector_create(-31, 28));

			car->textures[1] = g_textures.car_tractor[car->side];

			car->textures[0] = g_textures.head_neck[car->side];

			car->wheels[0] = wheel_create(WHEEL_TYPE_MONSTER, car->chassis_body, vector_create(-40, -18), group, 22, 4, 70, 4, 1);

			car->wheels[1] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(33, -26), group, 14, 4, 70, 4, 3);

			break;
		}
		case CAR_TYPE_TIMBER_LORRY:
		{
			create_chassis(car, 1, world, position, group, 2, s_timber_lorry, COUNTOF(s_timber_lorry), COUNTOF(s_timber_lorry[0]));

			create_head(car, 0, world, position, group, vector_create(22, 26));

			car->textures[1] = g_textures.car_timber_lorry[car->side];

			car->textures[0] = g_textures.head_neck[car->side];

			car->wheels[0] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(23, -23), group, 14, 10, 60, 10, 3);

			car->wheels[1] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(-50, -23), group, 14, 10, 60, 10, 3);

			for (int i = 2, j = 0; i <= 5; i++, j++)
			{
				create_extra_body(car, i, world, position, 0, vector_create(-38, -1 + 10 * j), 5, 2, s_plank, COUNTOF(s_plank), COUNTOF(s_plank[0]), FLAG_OBJECT);

				car->textures[i] = g_textures.plank;
			}

			create_extra_body(car, 6, world, position, 0, vector_create(-36, 11), 1.7, 0.5, s_timber_holder, COUNTOF(s_timber_holder), COUNTOF(s_timber_holder[0]), 0);

			Vector joint_position = vector_add_xy(position, -65, -7);

			physics_joint_create_world(PHYSICS_JOINT_TYPE_PIN, car->chassis_body, joint_position, car->bodies[6], joint_position);

			car->textures[6] = g_textures.timber_holder[car->side];

			break;
		}
		case CAR_TYPE_GARBAGE_TRUCK:
		{
			create_chassis(car, 7, world, position, group, 0.8, s_garbage_truck, COUNTOF(s_garbage_truck), COUNTOF(s_garbage_truck[0]));

			create_head(car, 6, world, position, group, vector_create(44, 15));

			car->textures[7] = g_textures.car_garbage_truck[car->side];

			car->textures[6] = g_textures.head_neck[car->side];

			car->wheels[0] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(-33, -33), group, 14, 10, 60, 5, 3);

			car->wheels[1] = wheel_create(WHEEL_TYPE_MEDIUM, car->chassis_body, vector_create(46, -33), group, 14, 10, 60, 5, 3);

			create_extra_body(car, 8, world, position, 0, vector_create(-52, 3), 3, 10, s_garbage_lid, COUNTOF(s_garbage_lid), COUNTOF(s_garbage_lid[0]), 0);

			Vector joint_position = vector_add_xy(position, -45, 33);

			physics_joint_create_world(PHYSICS_JOINT_TYPE_PIN, car->chassis_body, joint_position, car->bodies[8], joint_position);

			car->textures[8] = g_textures.garbage_lid[car->side];

			for (int i = 0; i < 6; i++)
			{
				create_extra_body(car, i, world, position, 0, vector_create(-32 + i * 8, -17), 0.3, 0.9, s_garbage, COUNTOF(s_garbage), COUNTOF(s_garbage[0]), FLAG_OBJECT);

				car->textures[i] = g_textures.garbage;
			}

			break;
		}
	}

	if (car->side)
	{
		for (int i = 0; car->bodies[i] != NULL; i++)
		{
			if (car->bodies[i] != car->chassis_body)
			{
				car->bodies[i]->position.x = car->chassis_body->position.x * 2 - car->bodies[i]->position.x;
			}

			for (List_Node* collider_node = car->bodies[i]->collider_list.first; collider_node != NULL; collider_node = collider_node->next)
			{
				Physics_Collider* collider = collider_node->item;

				switch (collider->local_shape->type)
				{
					case SHAPE_TYPE_SEGMENT:
					{
						collider->local_shape->segment.a.x *= -1;

						collider->local_shape->segment.b.x *= -1;

						break;
					}
					case SHAPE_TYPE_CIRCLE:
					{
						collider->local_shape->circle.center.x *= -1;

						break;
					}
					case SHAPE_TYPE_POLYGON:
					{
						Polygon* polygon = &collider->local_shape->polygon;

						for (int j = 0, k = polygon->point_count - 1; j <= k; j++, k--)
						{
							Vector a = polygon->points[j];

							Vector b = polygon->points[k];

							polygon->points[j] = vector_create(-b.x, b.y);

							polygon->points[k] = vector_create(-a.x, a.y);
						}

						break;
					}
				}
			}

			for (List_Node* joint_node = car->bodies[i]->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
			{
				Physics_Joint* joint = joint_node->item;

				if (joint->body_1 == car->bodies[i])
				{
					joint->local_anchor_1.x *= -1;
				}

				if (joint->body_2 == car->bodies[i])
				{
					joint->local_anchor_2.x *= -1;
				}
			}
		}

		for (int i = 0; i < 2; i++)
		{
			car->wheels[i]->body->position.x = car->chassis_body->position.x * 2 - car->wheels[i]->body->position.x;

			for (List_Node* joint_node = car->wheels[i]->body->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
			{
				Physics_Joint* joint = joint_node->item;

				if (joint->body_1 == car->wheels[i]->body)
				{
					joint->local_anchor_1.x *= -1;
				}

				if (joint->body_2 == car->wheels[i]->body)
				{
					joint->local_anchor_2.x *= -1;
				}
			}
		}
	}

	return car;
}

void car_destroy(Car* car)
{
	for (int i = 0; car->bodies[i] != NULL; i++)
	{
		physics_body_destroy(car->bodies[i]);
	}

	for (int i = 0; i < 2; i++)
	{
		wheel_destroy(car->wheels[i]);
	}
	
	free(car);
}

void car_update(Car* car, bool forward, bool backward)
{
	double multiplier = car->side ? -1 : 1;

	if (forward)
	{
		car->chassis_body->angular_force += 2e8 * multiplier;
	}

	if (backward)
	{
		car->chassis_body->angular_force -= 2e8 * multiplier;
	}

	car->chassis_body->angular_velocity = clamp(car->chassis_body->angular_velocity, -3, 3);
	
	for (int i = 0; i < 2; i++)
	{
		wheel_update(car->wheels[i], forward, backward);

		if (forward && !backward)
		{
			physics_body_apply_speed_at_local_point(car->chassis_body, vector_create(0, 0), car->wheels[i]->boost_forward);
		}

		if (backward && !forward)
		{
			physics_body_apply_speed_at_local_point(car->chassis_body, vector_create(0, 0), car->wheels[i]->boost_backward);
		}

		car->wheels[i]->boost_forward = vector_create(0, 0);

		car->wheels[i]->boost_backward = vector_create(0, 0);
	}

	if (!test_point_rect(car->head_body->position, &(Rect){ -10, -10, 1290, 2000 }))
	{
		car->done = true;
	}
}

void car_render(Car* car)
{
	for (int i = 0; car->textures[i] != NULL; i++)
	{
		set_texture_and_color(car->textures[i], NULL);

		graphics_draw_texture_at(car->bodies[i]->position, car->bodies[i]->angle);
	}
	
	for (int i = 0; i < 2; i++)
	{
		wheel_render(car->wheels[i]);
	}
}

void car_kill(Car* car)
{
	if (car->done)
	{
		return;
	}

	car->done = true;

	physics_body_destroy_all_joints(car->head_body);

	for (int i = 0; car->textures[i] != NULL; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (car->textures[i] == g_textures.head_neck[j])
			{
				car->textures[i] = g_textures.head_only[j];
			}
		}
	}
}
