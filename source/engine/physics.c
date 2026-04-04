#include "physics.h"

Physics_World* physics_world_create()
{
	Physics_World* world = calloc(1, sizeof(Physics_World));

	world->list_pool = pool_create(PHYSICS_OBJECT_CAPACITY * 3, sizeof(List_Node));

	world->body_pool = pool_create(PHYSICS_OBJECT_CAPACITY, sizeof(Physics_Body));

	world->collider_pool = pool_create(PHYSICS_OBJECT_CAPACITY, sizeof(Physics_Collider));

	world->joint_pool = pool_create(PHYSICS_OBJECT_CAPACITY, sizeof(Physics_Joint));

	return world;
}

void physics_world_destroy(Physics_World* world)
{
	list_set_pool(world->list_pool);

	for (List_Node* body_node = world->body_list.first; body_node != NULL; )
	{
		Physics_Body* body = body_node->item;

		body_node = body_node->next;

		physics_body_destroy(body);
	}

	for (List_Node* collider_node = world->collider_list.first; collider_node != NULL; )
	{
		Physics_Collider* collider = collider_node->item;

		collider_node = collider_node->next;

		physics_collider_destroy(collider);
	}

	for (List_Node* joint_node = world->joint_list.first; joint_node != NULL; )
	{
		Physics_Joint* joint = joint_node->item;

		joint_node = joint_node->next;

		physics_joint_destroy(joint);
	}

	list_set_pool(NULL);

	pool_destroy(world->list_pool);

	pool_destroy(world->body_pool);

	pool_destroy(world->collider_pool);

	pool_destroy(world->joint_pool);

	if (world->collisions != NULL)
	{
		free(world->collisions);
	}

	if (world->saved_collisions != NULL)
	{
		free(world->saved_collisions);
	}

	if (world->collision_map != NULL)
	{
		map_destroy(world->collision_map);
	}

	free(world);
}

void physics_world_step(Physics_World* world, double delta_time)
{
	for (List_Node* body_node = world->body_list.first; body_node != NULL; body_node = body_node->next)
	{
		Physics_Body* body = body_node->item;

		physics_body_update_world_transform(body);

		switch (body->type)
		{
			case PHYSICS_BODY_TYPE_DYNAMIC:
			{
				body->inverse_linear_mass = body->real_inverse_linear_mass;

				body->inverse_angular_mass = body->real_inverse_angular_mass;

				body->linear_velocity = vector_add(body->linear_velocity, vector_multiply(world->gravity, delta_time));

				body->linear_velocity = vector_add(body->linear_velocity, vector_multiply(body->linear_force, body->inverse_linear_mass * delta_time));

				body->angular_velocity += body->angular_force * body->inverse_angular_mass * delta_time;

				break;
			}
			case PHYSICS_BODY_TYPE_KINEMATIC:
			{
				body->inverse_linear_mass = 0.0;

				body->inverse_angular_mass = 0.0;

				break;
			}
			case PHYSICS_BODY_TYPE_STATIC:
			{
				body->inverse_linear_mass = 0.0;

				body->inverse_angular_mass = 0.0;

				body->linear_velocity = vector_create(0.0, 0.0);

				body->angular_velocity = 0.0;

				break;
			}
		}

		body->linear_force = vector_create(0.0, 0.0);

		body->angular_force = 0.0;
	}

	for (List_Node* collider_node = world->collider_list.first; collider_node != NULL; )
	{
		List_Node* next = collider_node->next;

		while (collider_node->prev != NULL && ((Physics_Collider*)collider_node->item)->world_bounding_rect.min.y < ((Physics_Collider*)collider_node->prev->item)->world_bounding_rect.min.y)
		{
			list_node_swap_with_prev(collider_node);
		}

		collider_node = next;
	}

	for (List_Node* joint_node = world->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
	{
		Physics_Joint* joint = joint_node->item;

		joint->total_impulse = 0;
	}

	if (world->collision_map == NULL)
	{
		world->collision_map = map_create();
	}

	if (world->collisions == NULL)
	{
		world->collisions = HEAPALLOC(PHYSICS_COLLISION_COUNT_MAX * sizeof(Physics_Collision));
	}

	if (world->saved_collisions == NULL)
	{
		world->saved_collisions = HEAPALLOC(PHYSICS_COLLISION_COUNT_MAX * sizeof(Physics_Collision));
	}

	Physics_Collision* collisions = world->saved_collisions;

	world->saved_collisions = world->collisions;

	world->collisions = collisions;

	int collision_count = 0;

	for (List_Node* collider_node_1 = world->collider_list.first; collider_node_1 != NULL; collider_node_1 = collider_node_1->next)
	{
		Physics_Collider* collider_1 = collider_node_1->item;

		if (!collider_1->enabled)
		{
			continue;
		}

		for (List_Node* collider_node_2 = collider_node_1->next; collider_node_2 != NULL; collider_node_2 = collider_node_2->next)
		{
			Physics_Collider* collider_2 = collider_node_2->item;

			if (!collider_2->enabled)
			{
				continue;
			}

			if (collider_2->world_bounding_rect.min.y > collider_1->world_bounding_rect.max.y)
			{
				break;
			}

			if (collider_2->world_bounding_rect.min.x > collider_1->world_bounding_rect.max.x || collider_1->world_bounding_rect.min.x > collider_2->world_bounding_rect.max.x)
			{
				continue;
			}

			if (collider_1->body == collider_2->body)
			{
				continue;
			}

			if (collider_1->body->type != PHYSICS_BODY_TYPE_DYNAMIC && collider_2->body->type != PHYSICS_BODY_TYPE_DYNAMIC)
			{
				continue;
			}

			if (!physics_can_collide(collider_1, collider_2))
			{
				continue;
			}

			memset(&collisions[collision_count], 0, sizeof(Physics_Collision));

			bool collided = physics_collide(collider_1, collider_2, &collisions[collision_count]);

			if (collided)
			{
				if (world->collision_callback != NULL)
				{
					collided &= world->collision_callback(collider_1, collider_2);

					collided &= world->collision_callback(collider_2, collider_1);
				}

				if (collider_1->collision_callback != NULL)
				{
					collided &= collider_1->collision_callback(collider_1, collider_2);
				}

				if (collider_2->collision_callback != NULL)
				{
					collided &= collider_2->collision_callback(collider_2, collider_1);
				}

				if (collided && !collider_1->sensor && !collider_2->sensor && collision_count + 2 <= PHYSICS_COLLISION_COUNT_MAX)
				{
					collision_count++;

					Collision collision = collisions[collision_count - 1].collision;

					if (collision.depth_2 != 0)
					{
						collision.point = collision.point_2;

						collision.depth = collision.depth_2;

						collisions[collision_count] = collisions[collision_count - 1];

						collisions[collision_count].collision = collision;

						collisions[collision_count].second = true;

						collision_count++;
					}
				}
			}
		}
	}

	world->collision_count = collision_count;

	for (int i = 0; i < collision_count; i++)
	{
		Collision collision = collisions[i].collision;

		Physics_Collider* collider_1 = collisions[i].collider_1;

		Physics_Collider* collider_2 = collisions[i].collider_2;

		Physics_Body* body_1 = collider_1->body;

		Physics_Body* body_2 = collider_2->body;

		Vector tangent_1 = vector_left(vector_subtract(collision.point, body_1->position));

		Vector tangent_2 = vector_left(vector_subtract(collision.point, body_2->position));

		Vector contact_velocity_1 = vector_add(body_1->linear_velocity, vector_multiply(tangent_1, body_1->angular_velocity));

		Vector contact_velocity_2 = vector_add(body_2->linear_velocity, vector_multiply(tangent_2, body_2->angular_velocity));

		Vector relative_velocity = vector_subtract(contact_velocity_2, contact_velocity_1);

		double normal_velocity = vector_dot(collision.normal, relative_velocity);

		double combined_restitution = fmax(collider_1->restitution, collider_2->restitution);

		double normal_inverse_mass_1 = body_1->inverse_linear_mass + body_1->inverse_angular_mass * square(vector_dot(collision.normal, tangent_1));

		double normal_inverse_mass_2 = body_2->inverse_linear_mass + body_2->inverse_angular_mass * square(vector_dot(collision.normal, tangent_2));

		collisions[i].inverse_normal_mass = 1 / (normal_inverse_mass_1 + normal_inverse_mass_2);

		collisions[i].target_velocity = -fmin(normal_velocity, 0) * combined_restitution;

		collisions[i].normal_impulse = 0;

		Vector collision_tangent = vector_right(collision.normal);

		collisions[i].static_friction = sqrt(collider_1->static_friction * collider_2->static_friction);

		collisions[i].dynamic_friction = sqrt(collider_1->dynamic_friction * collider_2->dynamic_friction);

		double tangent_inverse_mass_1 = body_1->inverse_linear_mass + body_1->inverse_angular_mass * square(vector_dot(collision_tangent, tangent_1));

		double tangent_inverse_mass_2 = body_2->inverse_linear_mass + body_2->inverse_angular_mass * square(vector_dot(collision_tangent, tangent_2));

		collisions[i].inverse_tangent_mass = 1 / (tangent_inverse_mass_1 + tangent_inverse_mass_2);
	}

	if (PHYSICS_USE_WARM_STARTING)
	{
		for (int i = 0; i < collision_count; i++)
		{
			Collision collision = collisions[i].collision;

			Physics_Collider* collider_1 = collisions[i].collider_1;

			Physics_Collider* collider_2 = collisions[i].collider_2;

			Map_Key key = (Map_Key)collider_1->index | (Map_Key)collider_2->index << 30 | (Map_Key)collisions[i].second << 60;

			Map_Value value = 0;

			if (map_get(world->collision_map, key, &value))
			{
				Physics_Collision* saved_collision = &world->saved_collisions[value];

				double collision_impulse = saved_collision->normal_impulse;

				collisions[i].normal_impulse = collision_impulse;

				Physics_Body* body_1 = collider_1->body;

				Physics_Body* body_2 = collider_2->body;

				Vector tangent_1 = vector_left(vector_subtract(collision.point, body_1->position));

				Vector tangent_2 = vector_left(vector_subtract(collision.point, body_2->position));

				body_1->linear_velocity = vector_subtract(body_1->linear_velocity, vector_multiply(collision.normal, collision_impulse * body_1->inverse_linear_mass));

				body_2->linear_velocity = vector_add(body_2->linear_velocity, vector_multiply(collision.normal, collision_impulse * body_2->inverse_linear_mass));

				body_1->angular_velocity -= vector_dot(collision.normal, tangent_1) * collision_impulse * body_1->inverse_angular_mass;

				body_2->angular_velocity += vector_dot(collision.normal, tangent_2) * collision_impulse * body_2->inverse_angular_mass;
			}
		}
	}

	for (int t = 0; t < PHYSICS_VELOCITY_ITERATION_COUNT; t++)
	{
		for (int i = 0; i < collision_count; i++)
		{
			Collision collision = collisions[i].collision;

			Physics_Collider* collider_1 = collisions[i].collider_1;

			Physics_Collider* collider_2 = collisions[i].collider_2;

			Physics_Body* body_1 = collider_1->body;

			Physics_Body* body_2 = collider_2->body;

			Vector tangent_1 = vector_left(vector_subtract(collision.point, body_1->position));

			Vector tangent_2 = vector_left(vector_subtract(collision.point, body_2->position));

			Vector contact_velocity_1 = vector_add(body_1->linear_velocity, vector_multiply(tangent_1, body_1->angular_velocity));

			Vector contact_velocity_2 = vector_add(body_2->linear_velocity, vector_multiply(tangent_2, body_2->angular_velocity));

			Vector relative_velocity = vector_subtract(contact_velocity_2, contact_velocity_1);

			double normal_velocity = vector_dot(collision.normal, relative_velocity);

			double impulse = (collisions[i].target_velocity - normal_velocity) * collisions[i].inverse_normal_mass;

			double total_impulse = fmax(collisions[i].normal_impulse + impulse, 0);

			double collision_impulse = total_impulse - collisions[i].normal_impulse;

			collisions[i].normal_impulse = total_impulse;

			body_1->linear_velocity = vector_subtract(body_1->linear_velocity, vector_multiply(collision.normal, collision_impulse * body_1->inverse_linear_mass));

			body_2->linear_velocity = vector_add(body_2->linear_velocity, vector_multiply(collision.normal, collision_impulse * body_2->inverse_linear_mass));

			body_1->angular_velocity -= vector_dot(collision.normal, tangent_1) * collision_impulse * body_1->inverse_angular_mass;

			body_2->angular_velocity += vector_dot(collision.normal, tangent_2) * collision_impulse * body_2->inverse_angular_mass;

			Vector collision_tangent = vector_right(collision.normal);

			double tangent_velocity = vector_dot(collision_tangent, relative_velocity);

			double total_tangent_impulse = collisions[i].tangent_impulse - tangent_velocity * collisions[i].static_friction * collisions[i].inverse_tangent_mass;

			double max_impulse_magnitude = collisions[i].normal_impulse * collisions[i].dynamic_friction;

			total_tangent_impulse = clamp(total_tangent_impulse, -max_impulse_magnitude, max_impulse_magnitude);

			double tangent_impulse = total_tangent_impulse - collisions[i].tangent_impulse;

			collisions[i].tangent_impulse = total_tangent_impulse;

			body_1->linear_velocity = vector_subtract(body_1->linear_velocity, vector_multiply(collision_tangent, tangent_impulse * body_1->inverse_linear_mass));

			body_2->linear_velocity = vector_add(body_2->linear_velocity, vector_multiply(collision_tangent, tangent_impulse * body_2->inverse_linear_mass));

			body_1->angular_velocity -= vector_dot(collision_tangent, tangent_1) * tangent_impulse * body_1->inverse_angular_mass;

			body_2->angular_velocity += vector_dot(collision_tangent, tangent_2) * tangent_impulse * body_2->inverse_angular_mass;
		}

		for (List_Node* joint_node = world->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
		{
			Physics_Joint* joint = joint_node->item;

			Physics_Body* body_1 = joint->body_1;

			Physics_Body* body_2 = joint->body_2;

			Vector displacement = vector_subtract(joint->world_anchor_2, joint->world_anchor_1);

			double distance = vector_length(displacement);

			if (distance != 0)
			{
				Vector normal = vector_divide(displacement, distance);

				Vector lever_1 = vector_subtract(joint->world_anchor_1, body_1->position);

				Vector lever_2 = vector_subtract(joint->world_anchor_2, body_2->position);

				Vector tangent_1 = vector_left(lever_1);

				Vector tangent_2 = vector_left(lever_2);

				double inverse_mass_1 = body_1->inverse_linear_mass + body_1->inverse_angular_mass * square(vector_dot(normal, tangent_1));

				double inverse_mass_2 = body_2->inverse_linear_mass + body_2->inverse_angular_mass * square(vector_dot(normal, tangent_2));

				double normal_velocity_1 = vector_dot(body_1->linear_velocity, normal) + vector_cross(lever_1, normal) * body_1->angular_velocity;

				double normal_velocity_2 = vector_dot(body_2->linear_velocity, normal) + vector_cross(lever_2, normal) * body_2->angular_velocity;

				double normal_velocity = normal_velocity_2 - normal_velocity_1;

				double correction_impulse = normal_velocity / (inverse_mass_1 + inverse_mass_2);

				body_1->linear_velocity = vector_add(body_1->linear_velocity, vector_multiply(normal, correction_impulse * body_1->inverse_linear_mass));

				body_2->linear_velocity = vector_subtract(body_2->linear_velocity, vector_multiply(normal, correction_impulse * body_2->inverse_linear_mass));

				body_1->angular_velocity += vector_dot(normal, tangent_1) * correction_impulse * body_1->inverse_angular_mass;

				body_2->angular_velocity -= vector_dot(normal, tangent_2) * correction_impulse * body_2->inverse_angular_mass;
			}

			if (joint->type == PHYSICS_JOINT_TYPE_FIXED)
			{
				double relative_angle = body_2->angle - body_1->angle;

				if (relative_angle != 0.0)
				{
					double relative_angular_velocity = body_2->angular_velocity - body_1->angular_velocity;

					double correction_angular_impulse = relative_angular_velocity / (body_1->inverse_angular_mass + body_2->inverse_angular_mass);

					body_1->angular_velocity += correction_angular_impulse * body_1->inverse_angular_mass;

					body_2->angular_velocity -= correction_angular_impulse * body_2->inverse_angular_mass;
				}
			}
		}
	}

	for (List_Node* body_node = world->body_list.first; body_node != NULL; body_node = body_node->next)
	{
		Physics_Body* body = body_node->item;

		body->position_change = vector_multiply(body->linear_velocity, delta_time);

		body->angle_change = body->angular_velocity * delta_time;
	}

	for (int t = 0; t < PHYSICS_POSITION_ITERATION_COUNT; t++)
	{
		for (int i = 0; i < collision_count; i++)
		{
			Collision collision = collisions[i].collision;

			Physics_Collider* collider_1 = collisions[i].collider_1;

			Physics_Collider* collider_2 = collisions[i].collider_2;

			Physics_Body* body_1 = collider_1->body;

			Physics_Body* body_2 = collider_2->body;

			Vector tangent_1 = vector_left(vector_subtract(collision.point, body_1->position));

			Vector tangent_2 = vector_left(vector_subtract(collision.point, body_2->position));

			double linear_contribution = vector_dot(collision.normal, vector_subtract(body_2->position_change, body_1->position_change));

			double angular_contribution = body_2->angle_change * vector_dot(collision.normal, tangent_2) - body_1->angle_change * vector_dot(collision.normal, tangent_1);

			double current_depth = collision.depth - (linear_contribution + angular_contribution);

			double collision_impulse = current_depth * PHYSICS_CORRECTION_FACTOR * collisions[i].inverse_normal_mass;

			body_1->position_change = vector_subtract(body_1->position_change, vector_multiply(collision.normal, collision_impulse * body_1->inverse_linear_mass));

			body_2->position_change = vector_add(body_2->position_change, vector_multiply(collision.normal, collision_impulse * body_2->inverse_linear_mass));

			body_1->angle_change -= vector_dot(collision.normal, tangent_1) * collision_impulse * body_1->inverse_angular_mass;

			body_2->angle_change += vector_dot(collision.normal, tangent_2) * collision_impulse * body_2->inverse_angular_mass;
		}

		for (List_Node* joint_node = world->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
		{
			Physics_Joint* joint = joint_node->item;

			Physics_Body* body_1 = joint->body_1;

			Physics_Body* body_2 = joint->body_2;

			Vector lever_1 = vector_rotate(joint->local_anchor_1, body_1->angle + body_1->angle_change);

			Vector lever_2 = vector_rotate(joint->local_anchor_2, body_2->angle + body_2->angle_change);

			Vector world_anchor_1 = vector_add(vector_add(body_1->position, body_1->position_change), lever_1);

			Vector world_anchor_2 = vector_add(vector_add(body_2->position, body_2->position_change), lever_2);

			Vector displacement = vector_subtract(world_anchor_2, world_anchor_1);

			double distance = vector_length(displacement);

			if (distance != 0)
			{
				Vector normal = vector_divide(displacement, distance);

				Vector tangent_1 = vector_left(lever_1);

				Vector tangent_2 = vector_left(lever_2);

				double inverse_mass_1 = body_1->inverse_linear_mass + body_1->inverse_angular_mass * square(vector_dot(normal, tangent_1));

				double inverse_mass_2 = body_2->inverse_linear_mass + body_2->inverse_angular_mass * square(vector_dot(normal, tangent_2));

				double correction_impulse = distance * PHYSICS_CORRECTION_FACTOR / (inverse_mass_1 + inverse_mass_2);

				body_1->position_change = vector_add(body_1->position_change, vector_multiply(normal, correction_impulse * body_1->inverse_linear_mass));

				body_2->position_change = vector_subtract(body_2->position_change, vector_multiply(normal, correction_impulse * body_2->inverse_linear_mass));

				body_1->angle_change += vector_dot(normal, tangent_1) * correction_impulse * body_1->inverse_angular_mass;

				body_2->angle_change -= vector_dot(normal, tangent_2) * correction_impulse * body_2->inverse_angular_mass;
			}

			if (joint->type == PHYSICS_JOINT_TYPE_FIXED)
			{
				double relative_angle = body_2->angle - body_1->angle + body_2->angle_change - body_1->angle_change;

				if (relative_angle != 0.0)
				{
					double correction_angular_impulse = relative_angle * PHYSICS_CORRECTION_FACTOR / (body_1->inverse_angular_mass + body_2->inverse_angular_mass);

					body_1->angle_change += correction_angular_impulse * body_1->inverse_angular_mass;

					body_2->angle_change -= correction_angular_impulse * body_2->inverse_angular_mass;
				}
			}
		}
	}

	if (PHYSICS_BACKFEED_POSITIONS)
	{
		for (List_Node* body_node = world->body_list.first; body_node != NULL; body_node = body_node->next)
		{
			Physics_Body* body = body_node->item;

			body->linear_velocity = vector_multiply(body->position_change, 1 / delta_time);

			body->angular_velocity = body->angle_change / delta_time;
		}
	}

	for (List_Node* body_node = world->body_list.first; body_node != NULL; body_node = body_node->next)
	{
		Physics_Body* body = body_node->item;

		Vector position_change = body->position_change;

		double angle_change = body->angle_change;

		body->position = vector_add(body->position, position_change);

		body->angle += angle_change;

		body->world_transform_is_dirty |= position_change.x != 0.0;

		body->world_transform_is_dirty |= position_change.y != 0.0;

		body->world_transform_is_dirty |= angle_change != 0.0;

		physics_body_update_world_transform(body);
	}

	if (PHYSICS_USE_WARM_STARTING)
	{
		map_clear(world->collision_map);

		for (int i = 0; i < collision_count; i++)
		{
			Physics_Collider* collider_1 = collisions[i].collider_1;

			Physics_Collider* collider_2 = collisions[i].collider_2;

			Map_Key key = (Map_Key)collider_1->index | (Map_Key)collider_2->index << 30 | (Map_Key)collisions[i].second << 60;

			map_insert(world->collision_map, key, i);
		}
	}

	world->elapsed_time += delta_time;
}

Physics_Body* physics_body_create(Physics_World* world, Physics_Body_Type type)
{
	Physics_Body* body = pool_alloc(world->body_pool);

	body->type = type;

	body->world = world;

	list_set_pool(world->list_pool);

	body->node_in_world = list_insert_last_item(&world->body_list, body);

	list_set_pool(NULL);

	body->world->body_count++;

	return body;
}

void physics_body_destroy(Physics_Body* body)
{
	body->world->body_count--;

	physics_body_destroy_all_colliders(body);

	physics_body_destroy_all_joints(body);

	list_set_pool(body->world->list_pool);

	list_node_destroy(body->node_in_world);

	list_set_pool(NULL);

	pool_free(body->world->body_pool, body);
}

void physics_body_destroy_all_colliders(Physics_Body* body)
{
	list_set_pool(body->world->list_pool);

	for (List_Node* collider_node = body->collider_list.first; collider_node != NULL; )
	{
		Physics_Collider* collider = collider_node->item;

		collider_node = collider_node->next;

		physics_collider_destroy(collider);
	}

	list_set_pool(NULL);
}

void physics_body_destroy_all_joints(Physics_Body* body)
{
	list_set_pool(body->world->list_pool);

	for (List_Node* joint_node = body->joint_list.first; joint_node != NULL; )
	{
		Physics_Joint* joint = joint_node->item;

		joint_node = joint_node->next;

		physics_joint_destroy(joint);
	}

	list_set_pool(NULL);
}

Transform physics_body_get_transform(const Physics_Body* body)
{
	return transform_create(body->position, body->angle);
}

Transform physics_body_get_inverse_transform(const Physics_Body* body)
{
	return transform_invert(transform_create(body->position, body->angle));
}

void physics_body_apply_speed_at_local_point(Physics_Body* body, Vector local_point, Vector speed)
{
	body->linear_velocity = vector_add(body->linear_velocity, speed);

	body->angular_velocity += vector_cross(local_point, speed);
}

void physics_body_apply_speed_at_world_point(Physics_Body* body, Vector world_point, Vector speed)
{
	body->linear_velocity = vector_add(body->linear_velocity, speed);

	body->angular_velocity += vector_cross(vector_subtract(world_point, body->position), speed);
}

void physics_body_apply_impulse_at_local_point(Physics_Body* body, Vector local_point, Vector impulse)
{
	body->linear_velocity = vector_add(body->linear_velocity, vector_multiply(vector_rotate(impulse, body->angle), body->inverse_linear_mass));

	body->angular_velocity += vector_cross(local_point, impulse) * body->inverse_angular_mass;
}

void physics_body_apply_impulse_at_world_point(Physics_Body* body, Vector world_point, Vector impulse)
{
	body->linear_velocity = vector_add(body->linear_velocity, vector_multiply(impulse, body->inverse_linear_mass));

	body->angular_velocity += vector_cross(vector_subtract(world_point, body->position), impulse) * body->inverse_angular_mass;
}

void physics_body_apply_force_at_local_point(Physics_Body* body, Vector local_point, Vector force)
{
	body->linear_force = vector_add(body->linear_force, vector_rotate(force, body->angle));

	body->angular_force += vector_cross(local_point, force);
}

void physics_body_apply_force_at_world_point(Physics_Body* body, Vector world_point, Vector force)
{
	body->linear_force = vector_add(body->linear_force, force);

	body->angular_force += vector_cross(vector_subtract(world_point, body->position), force);
}

void physics_body_update_world_transform(Physics_Body* body)
{
	if (!body->world_transform_is_dirty)
	{
		return;
	}

	Transform body_transform = physics_body_get_transform(body);

	for (List_Node* collider_node = body->collider_list.first; collider_node != NULL; collider_node = collider_node->next)
	{
		Physics_Collider* collider = collider_node->item;

		shape_transform(collider->local_shape, body_transform, collider->world_shape);

		collider->world_bounding_rect = shape_get_bounding_rect(collider->world_shape);
	}

	for (List_Node* joint_node = body->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
	{
		Physics_Joint* joint = joint_node->item;

		if (joint->body_1 == body)
		{
			joint->world_anchor_1 = transform_apply(body_transform, joint->local_anchor_1);
		}

		if (joint->body_2 == body)
		{
			joint->world_anchor_2 = transform_apply(body_transform, joint->local_anchor_2);
		}
	}

	body->world_transform_is_dirty = false;
}

void physics_body_add_collider_mass(Physics_Body* body, Physics_Collider* collider)
{
	Vector body_center_of_mass = body->center_of_mass;

	double body_linear_mass = body->real_inverse_linear_mass == 0.0 ? 0.0 : 1.0 / body->real_inverse_linear_mass;

	double body_angular_mass = body->real_inverse_angular_mass == 0.0 ? 0.0 : 1.0 / body->real_inverse_angular_mass;

	Vector collider_center_of_mass;

	double collider_linear_mass;

	double collider_angular_mass;

	physics_collider_get_mass(collider, &collider_center_of_mass, &collider_linear_mass, &collider_angular_mass);

	Vector new_body_center_of_mass = vector_divide(vector_add(vector_multiply(body_center_of_mass, body_linear_mass), vector_multiply(collider_center_of_mass, collider_linear_mass)), body_linear_mass + collider_linear_mass);

	double new_body_linear_mass = body_linear_mass + collider_linear_mass;

	double new_body_angular_mass = body_angular_mass + body_linear_mass * vector_distance_squared(body_center_of_mass, new_body_center_of_mass) + collider_angular_mass + collider_linear_mass * vector_distance_squared(collider_center_of_mass, new_body_center_of_mass);

	body->center_of_mass = new_body_center_of_mass;

	body->real_inverse_linear_mass = 1.0 / new_body_linear_mass;

	body->real_inverse_angular_mass = 1.0 / new_body_angular_mass;
}

void physics_body_subtract_collider_mass(Physics_Body* body, Physics_Collider* collider)
{
	Vector body_center_of_mass = body->center_of_mass;

	double body_linear_mass = body->real_inverse_linear_mass == 0.0 ? 0.0 : 1.0 / body->real_inverse_linear_mass;

	double body_angular_mass = body->real_inverse_angular_mass == 0.0 ? 0.0 : 1.0 / body->real_inverse_angular_mass;

	Vector collider_center_of_mass;

	double collider_linear_mass;

	double collider_angular_mass;

	physics_collider_get_mass(collider, &collider_center_of_mass, &collider_linear_mass, &collider_angular_mass);

	Vector new_body_center_of_mass = vector_divide(vector_add(vector_multiply(body_center_of_mass, body_linear_mass), vector_multiply(collider_center_of_mass, collider_linear_mass)), body_linear_mass - collider_linear_mass);

	double new_body_linear_mass = body_linear_mass - collider_linear_mass;

	double new_body_angular_mass = body_angular_mass + body_linear_mass * vector_distance_squared(body_center_of_mass, new_body_center_of_mass) - collider_angular_mass - collider_linear_mass * vector_distance_squared(collider_center_of_mass, new_body_center_of_mass);

	if (new_body_linear_mass == 0.0)
	{
		body->center_of_mass = vector_create(0.0, 0.0);

		body->real_inverse_linear_mass = 0.0;

		body->real_inverse_angular_mass = 0.0;
	}
	else
	{
		body->center_of_mass = new_body_center_of_mass;

		body->real_inverse_linear_mass = 1.0 / new_body_linear_mass;

		body->real_inverse_angular_mass = 1.0 / new_body_angular_mass;
	}
}

Physics_Collider* physics_collider_create(Physics_Body* body, const Shape* shape, double density)
{
	Physics_Collider* collider = pool_alloc(body->world->collider_pool);

	collider->local_shape = shape_clone(shape);

	collider->world_shape = shape_clone(shape);

	collider->density = density;

	collider->filter_mask_1 = 0xFFFFFFFF;

	collider->filter_mask_2 = 0xFFFFFFFF;

	collider->filter_group = 0;

	collider->enabled = true;

	collider->body = body;

	list_set_pool(body->world->list_pool);

	collider->node_in_body = list_insert_last_item(&body->collider_list, collider);

	collider->node_in_world = list_insert_last_item(&body->world->collider_list, collider);

	list_set_pool(NULL);

	physics_body_add_collider_mass(body, collider);

	body->world_transform_is_dirty = true;

	body->world->collider_count++;

	collider->index = body->world->collider_index++;

	return collider;
}

void physics_collider_destroy(Physics_Collider* collider)
{
	Physics_Body* body = collider->body;

	body->world->collider_count--;

	physics_body_subtract_collider_mass(body, collider);

	shape_destroy(collider->local_shape);

	shape_destroy(collider->world_shape);

	list_set_pool(body->world->list_pool);

	list_node_destroy(collider->node_in_body);

	list_node_destroy(collider->node_in_world);

	list_set_pool(NULL);

	pool_free(body->world->collider_pool, collider);
}

void physics_collider_get_mass(const Physics_Collider* collider, Vector* center_of_mass, double* linear_mass, double* angular_mass)
{
	*center_of_mass = shape_get_centroid(collider->local_shape);

	*linear_mass = collider->density * shape_get_linear_mass_factor(collider->local_shape);

	*angular_mass = *linear_mass * shape_get_angular_mass_factor(collider->local_shape);
}

Physics_Joint* physics_joint_create(Physics_Joint_Type type, Physics_Body* body_1, Vector local_anchor_1, Vector world_anchor_1, Physics_Body* body_2, Vector local_anchor_2, Vector world_anchor_2)
{
	Physics_Joint* joint = pool_alloc(body_1->world->joint_pool);

	joint->type = type;

	joint->local_anchor_1 = local_anchor_1;

	joint->local_anchor_2 = local_anchor_2;

	joint->world_anchor_1 = world_anchor_1;

	joint->world_anchor_2 = world_anchor_2;

	joint->body_1 = body_1;

	joint->body_2 = body_2;

	list_set_pool(body_1->world->list_pool);

	joint->node_in_body_1 = list_insert_last_item(&body_1->joint_list, joint);

	joint->node_in_body_2 = list_insert_last_item(&body_2->joint_list, joint);

	joint->node_in_world = list_insert_last_item(&body_1->world->joint_list, joint);

	list_set_pool(NULL);

	body_1->world->joint_count++;

	return joint;
}

Physics_Joint* physics_joint_create_local(Physics_Joint_Type type, Physics_Body* body_1, Vector local_anchor_1, Physics_Body* body_2, Vector local_anchor_2)
{
	Vector world_anchor_1 = transform_apply(physics_body_get_transform(body_1), local_anchor_1);

	Vector world_anchor_2 = transform_apply(physics_body_get_transform(body_2), local_anchor_2);

	return physics_joint_create(type, body_1, local_anchor_1, world_anchor_1, body_2, local_anchor_2, world_anchor_2);
}

Physics_Joint* physics_joint_create_world(Physics_Joint_Type type, Physics_Body* body_1, Vector world_anchor_1, Physics_Body* body_2, Vector world_anchor_2)
{
	Vector local_anchor_1 = transform_apply(physics_body_get_inverse_transform(body_1), world_anchor_1);

	Vector local_anchor_2 = transform_apply(physics_body_get_inverse_transform(body_2), world_anchor_2);

	return physics_joint_create(type, body_1, local_anchor_1, world_anchor_1, body_2, local_anchor_2, world_anchor_2);
}

void physics_joint_destroy(Physics_Joint* joint)
{
	joint->body_1->world->joint_count--;

	joint->body_2->world->joint_count--;

	list_set_pool(joint->body_1->world->list_pool);

	list_node_destroy(joint->node_in_body_1);

	list_node_destroy(joint->node_in_body_2);

	list_node_destroy(joint->node_in_world);

	list_set_pool(NULL);

	pool_free(joint->body_1->world->joint_pool, joint);
}

bool physics_can_collide(const Physics_Collider* collider_1, const Physics_Collider* collider_2)
{
	if (collider_1->filter_group == collider_2->filter_group)
	{
		if (collider_1->filter_group > 0)
		{
			return true;
		}

		if (collider_1->filter_group < 0)
		{
			return false;
		}
	}

	return (collider_1->filter_mask_1 & collider_2->filter_mask_2) != 0 && (collider_1->filter_mask_2 & collider_2->filter_mask_1) != 0;
}

bool physics_collide(const Physics_Collider* collider_1, const Physics_Collider* collider_2, Physics_Collision* collision)
{
	if (collide_shapes(collider_1->world_shape, collider_2->world_shape, &collision->collision))
	{
		collision->collider_1 = (Physics_Collider*)collider_1;

		collision->collider_2 = (Physics_Collider*)collider_2;

		return true;
	}

	return false;
}
