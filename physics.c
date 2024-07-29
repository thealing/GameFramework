#include "physics.h"

Physics_World* physics_world_create()
{
	Physics_World* world = calloc(1, sizeof(Physics_World));

	return world;
}

void physics_world_destroy(Physics_World* world)
{
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
		
		body->correction_linear_velocity = vector_create(0.0, 0.0);

		body->correction_angular_velocity = 0.0;
	}

	for (List_Node* collider_node = world->collider_list.first; collider_node != NULL; )
	{
		List_Node* next = collider_node->next;

		while (collider_node->prev != NULL && ((Physics_Collider*)collider_node->item)->world_bounding_rect.min.x < ((Physics_Collider*)collider_node->prev->item)->world_bounding_rect.min.x)
		{
			list_node_swap_with_prev(collider_node);
		}

		collider_node = next;
	}

	int collision_count_limit = imin(isquare(world->collider_list.size), PHYSICS_COLLISION_COUNT_MAX);

	Physics_Collision* collisions = malloc(collision_count_limit * sizeof(Physics_Collision));

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

			if (collider_2->world_bounding_rect.min.x > collider_1->world_bounding_rect.max.x)
			{
				break;
			}

			if (collider_2->world_bounding_rect.min.y > collider_1->world_bounding_rect.max.y || collider_1->world_bounding_rect.min.y > collider_2->world_bounding_rect.max.y)
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

				if (collided && !collider_1->sensor && !collider_2->sensor)
				{
					collision_count++;
				}
			}
		}
	}

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

		if (normal_velocity >= 0.0)
		{
			continue;
		}

		double combined_restitution = fmax(collider_1->restitution, collider_2->restitution);

		double normal_inverse_mass_1 = body_1->inverse_linear_mass + body_1->inverse_angular_mass * square(vector_dot(collision.normal, tangent_1));

		double normal_inverse_mass_2 = body_2->inverse_linear_mass + body_2->inverse_angular_mass * square(vector_dot(collision.normal, tangent_2));

		double collision_impulse = -normal_velocity * (1 + combined_restitution) / (normal_inverse_mass_1 + normal_inverse_mass_2);

		body_1->linear_velocity = vector_subtract(body_1->linear_velocity, vector_multiply(collision.normal, collision_impulse * body_1->inverse_linear_mass));

		body_2->linear_velocity = vector_add(body_2->linear_velocity, vector_multiply(collision.normal, collision_impulse * body_2->inverse_linear_mass));

		body_1->angular_velocity -= vector_dot(collision.normal, tangent_1) * collision_impulse * body_1->inverse_angular_mass;

		body_2->angular_velocity += vector_dot(collision.normal, tangent_2) * collision_impulse * body_2->inverse_angular_mass;

		Vector collision_tangent = vector_right(collision.normal);

		double tangent_velocity = vector_dot(collision_tangent, relative_velocity);

		double combined_static_friction = sqrt(collider_1->static_friction * collider_2->static_friction);

		double combined_dynamic_friction = sqrt(collider_1->dynamic_friction * collider_2->dynamic_friction);

		double tangent_inverse_mass_1 = body_1->inverse_linear_mass + body_1->inverse_angular_mass * square(vector_dot(collision_tangent, tangent_1));

		double tangent_inverse_mass_2 = body_2->inverse_linear_mass + body_2->inverse_angular_mass * square(vector_dot(collision_tangent, tangent_2));

		double friction_impulse = -tangent_velocity * combined_static_friction / (tangent_inverse_mass_1 + tangent_inverse_mass_2);

		if (fabs(friction_impulse) > fabs(collision_impulse) * combined_static_friction)
		{
			friction_impulse = signum(friction_impulse) * fabs(collision_impulse) * combined_dynamic_friction;
		}

		body_1->linear_velocity = vector_subtract(body_1->linear_velocity, vector_multiply(collision_tangent, friction_impulse * body_1->inverse_linear_mass));

		body_2->linear_velocity = vector_add(body_2->linear_velocity, vector_multiply(collision_tangent, friction_impulse * body_2->inverse_linear_mass));

		body_1->angular_velocity -= vector_dot(collision_tangent, tangent_1) * friction_impulse * body_1->inverse_angular_mass;

		body_2->angular_velocity += vector_dot(collision_tangent, tangent_2) * friction_impulse * body_2->inverse_angular_mass;
	}

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

		double correction_impulse = collision.depth / delta_time - normal_velocity;

		if (correction_impulse <= 0.0)
		{
			continue;
		}

		double inverse_mass_1 = body_1->inverse_linear_mass + body_1->inverse_angular_mass * square(vector_dot(collision.normal, tangent_1));

		double inverse_mass_2 = body_2->inverse_linear_mass + body_2->inverse_angular_mass * square(vector_dot(collision.normal, tangent_2));

		correction_impulse /= inverse_mass_1 + inverse_mass_2;

		physics_body_apply_correction_impulse(body_1, collision.point, vector_multiply(collision.normal, -correction_impulse));

		physics_body_apply_correction_impulse(body_2, collision.point, vector_multiply(collision.normal, correction_impulse));
	}

	free(collisions);

	for (List_Node* joint_node = world->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
	{
		Physics_Joint* joint = joint_node->item;

		switch (joint->type)
		{
			case PHYSICS_JOINT_TYPE_FIXED:
			{
				Vector displacement = vector_subtract(joint->world_anchor_2, joint->world_anchor_1);
				
				Vector normal = vector_normalize(displacement);
				
				double distance = vector_length(displacement);

				if (distance != 0.0)
				{
					Vector tangent_1 = vector_left(vector_subtract(joint->world_anchor_1, joint->body_1->position));

					Vector tangent_2 = vector_left(vector_subtract(joint->world_anchor_2, joint->body_2->position));

					Vector velocity_1 = vector_add(joint->body_1->linear_velocity, vector_multiply(tangent_1, joint->body_1->angular_velocity));

					Vector velocity_2 = vector_add(joint->body_2->linear_velocity, vector_multiply(tangent_2, joint->body_2->angular_velocity));

					Vector relative_velocity = vector_subtract(velocity_2, velocity_1);

					double normal_velocity = vector_dot(normal, relative_velocity);

					double correction_impulse = distance / delta_time - normal_velocity;

					double inverse_mass_1 = joint->body_1->inverse_linear_mass + joint->body_1->inverse_angular_mass * square(vector_dot(normal, tangent_1));

					double inverse_mass_2 = joint->body_2->inverse_linear_mass + joint->body_2->inverse_angular_mass * square(vector_dot(normal, tangent_2));

					correction_impulse /= inverse_mass_1 + inverse_mass_2;

					physics_body_apply_correction_impulse(joint->body_1, joint->world_anchor_1, vector_multiply(normal, correction_impulse));

					physics_body_apply_correction_impulse(joint->body_2, joint->world_anchor_2, vector_multiply(normal, -correction_impulse));
				}

				double relative_angle = joint->body_2->angle - joint->body_1->angle;

				if (relative_angle != 0.0)
				{
					double relative_angular_velocity = joint->body_2->angular_velocity - joint->body_1->angular_velocity;

					double correction_angular_impulse = relative_angle / delta_time - relative_angular_velocity;

					double combined_inverse_angular_mass = joint->body_1->inverse_angular_mass + joint->body_2->inverse_angular_mass;

					joint->body_1->correction_angular_velocity += correction_angular_impulse * joint->body_1->inverse_angular_mass / combined_inverse_angular_mass;
					
					joint->body_2->correction_angular_velocity -= correction_angular_impulse * joint->body_2->inverse_angular_mass / combined_inverse_angular_mass;
				}

				break;
			}
			case PHYSICS_JOINT_TYPE_PIN:
			{
				Vector displacement = vector_subtract(joint->world_anchor_2, joint->world_anchor_1);
				
				Vector normal = vector_normalize(displacement);
				
				double distance = vector_length(displacement);

				if (distance != 0.0)
				{
					Vector tangent_1 = vector_left(vector_subtract(joint->world_anchor_1, joint->body_1->position));
				
					Vector tangent_2 = vector_left(vector_subtract(joint->world_anchor_2, joint->body_2->position));

					Vector velocity_1 = vector_add(joint->body_1->linear_velocity, vector_multiply(tangent_1, joint->body_1->angular_velocity));
					
					Vector velocity_2 = vector_add(joint->body_2->linear_velocity, vector_multiply(tangent_2, joint->body_2->angular_velocity));

					Vector relative_velocity = vector_subtract(velocity_2, velocity_1);

					double normal_velocity = vector_dot(normal, relative_velocity);

					double correction_impulse = distance / delta_time - normal_velocity;

					double inverse_mass_1 = joint->body_1->inverse_linear_mass + joint->body_1->inverse_angular_mass * square(vector_dot(normal, tangent_1));
					
					double inverse_mass_2 = joint->body_2->inverse_linear_mass + joint->body_2->inverse_angular_mass * square(vector_dot(normal, tangent_2));
					
					correction_impulse /= inverse_mass_1 + inverse_mass_2;

					physics_body_apply_correction_impulse(joint->body_1, joint->world_anchor_1, vector_multiply(normal, correction_impulse));
					
					physics_body_apply_correction_impulse(joint->body_2, joint->world_anchor_2, vector_multiply(normal, -correction_impulse));
				}

				break;
			}
		}
	}

	for (List_Node* body_node = world->body_list.first; body_node != NULL; body_node = body_node->next)
	{
		Physics_Body* body = body_node->item;

		Vector position_change = vector_multiply(vector_add(body->linear_velocity, body->correction_linear_velocity), delta_time);

		double angle_change = (body->angular_velocity + body->correction_angular_velocity) * delta_time;
		
		body->position = vector_add(body->position, position_change);

		body->angle += angle_change;
		
		body->linear_velocity = vector_add(body->linear_velocity, vector_multiply(body->correction_linear_velocity, PHYSICS_CORRECTION_VELOCITY_GAIN));
		
		body->angular_velocity += body->correction_angular_velocity * PHYSICS_CORRECTION_VELOCITY_GAIN;

		body->world_transform_is_dirty |= position_change.x != 0.0;

		body->world_transform_is_dirty |= position_change.y != 0.0;

		body->world_transform_is_dirty |= angle_change != 0.0;

		physics_body_update_world_transform(body);
	}
}

Physics_Body* physics_body_create(Physics_World* world, Physics_Body_Type type)
{
	Physics_Body* body = calloc(1, sizeof(Physics_Body));

	body->type = type;

	body->world = world;

	body->node_in_world = list_insert_last_item(&world->body_list, body);

	return body;
}

void physics_body_destroy(Physics_Body* body)
{
	physics_body_destroy_all_colliders(body);

	physics_body_destroy_all_joints(body);

	list_node_destroy(body->node_in_world);

	free(body);
}

void physics_body_destroy_all_colliders(Physics_Body* body)
{
	for (List_Node* collider_node = body->collider_list.first; collider_node != NULL; )
	{
		Physics_Collider* collider = collider_node->item;

		collider_node = collider_node->next;

		physics_collider_destroy(collider);
	}
}

void physics_body_destroy_all_joints(Physics_Body* body)
{
	for (List_Node* joint_node = body->joint_list.first; joint_node != NULL; )
	{
		Physics_Joint* joint = joint_node->item;

		joint_node = joint_node->next;

		physics_joint_destroy(joint);
	}
}

Transform physics_body_get_transform(const Physics_Body* body)
{
	return transform_create(body->position, body->angle);
}

Transform physics_body_get_inverse_transform(const Physics_Body* body)
{
	return transform_invert(transform_create(body->position, body->angle));
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

void physics_body_apply_correction_impulse(Physics_Body* body, Vector point, Vector impulse)
{
	body->correction_linear_velocity = vector_add(body->correction_linear_velocity, vector_multiply(impulse, body->inverse_linear_mass));

	body->correction_angular_velocity += vector_cross(vector_subtract(point, body->position), impulse) * body->inverse_angular_mass;
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
	Physics_Collider* collider = calloc(1, sizeof(Physics_Collider));

	collider->local_shape = shape_clone(shape);

	collider->world_shape = shape_clone(shape);

	collider->density = density;

	collider->filter_mask_1 = 0xFFFFFFFF;

	collider->filter_mask_2 = 0xFFFFFFFF;

	collider->filter_group = 0;

	collider->enabled = true;

	collider->body = body;

	collider->node_in_body = list_insert_last_item(&body->collider_list, collider);

	collider->node_in_world = list_insert_last_item(&body->world->collider_list, collider);

	physics_body_add_collider_mass(body, collider);

	body->world_transform_is_dirty = true;

	return collider;
}

void physics_collider_destroy(Physics_Collider* collider)
{
	Physics_Body* body = collider->body;

	physics_body_subtract_collider_mass(body, collider);

	shape_destroy(collider->local_shape);

	shape_destroy(collider->world_shape);

	list_node_destroy(collider->node_in_body);

	list_node_destroy(collider->node_in_world);

	free(collider);
}

void physics_collider_get_mass(const Physics_Collider* collider, Vector* center_of_mass, double* linear_mass, double* angular_mass)
{
	*center_of_mass = shape_get_centroid(collider->local_shape);

	*linear_mass = collider->density * shape_get_linear_mass_factor(collider->local_shape);

	*angular_mass = *linear_mass * shape_get_angular_mass_factor(collider->local_shape);
}

Physics_Joint* physics_joint_create(Physics_Joint_Type type, Physics_Body* body_1, Vector local_anchor_1, Vector world_anchor_1, Physics_Body* body_2, Vector local_anchor_2, Vector world_anchor_2)
{
	Physics_Joint* joint = calloc(1, sizeof(Physics_Joint));

	joint->type = type;

	joint->local_anchor_1 = local_anchor_1;

	joint->local_anchor_2 = local_anchor_2;

	joint->world_anchor_1 = world_anchor_1;

	joint->world_anchor_2 = world_anchor_2;

	joint->body_1 = body_1;

	joint->body_2 = body_2;

	joint->node_in_body_1 = list_insert_last_item(&body_1->joint_list, joint);

	joint->node_in_body_2 = list_insert_last_item(&body_2->joint_list, joint);

	joint->node_in_world = list_insert_last_item(&body_1->world->joint_list, joint);

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
	list_node_destroy(joint->node_in_body_1);

	list_node_destroy(joint->node_in_body_2);

	list_node_destroy(joint->node_in_world);

	free(joint);
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
