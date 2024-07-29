#pragma once

#include "geometry.h"

#include "list.h"

#define PHYSICS_COLLISION_COUNT_MAX 100000

#define PHYSICS_CORRECTION_VELOCITY_GAIN 0.1

typedef enum Physics_Body_Type Physics_Body_Type;

typedef enum Physics_Joint_Type Physics_Joint_Type;

typedef struct Physics_World Physics_World;

typedef struct Physics_Body Physics_Body;

typedef struct Physics_Collider Physics_Collider;

typedef struct Physics_Joint Physics_Joint;

typedef struct Physics_Collision Physics_Collision;

typedef bool (* Physics_Collision_Callback)(Physics_Collider* collider, Physics_Collider* other);

enum Physics_Body_Type
{
	PHYSICS_BODY_TYPE_DYNAMIC,

	PHYSICS_BODY_TYPE_KINEMATIC,

	PHYSICS_BODY_TYPE_STATIC,

	PHYSICS_BODY_TYPE_COUNT
};

enum Physics_Joint_Type
{
	PHYSICS_JOINT_TYPE_FIXED,

	PHYSICS_JOINT_TYPE_PIN,

	PHYSICS_JOINT_TYPE_COUNT
};

struct Physics_World
{
	Vector gravity;

	List body_list;

	List collider_list;

	List joint_list;

	Physics_Collision_Callback collision_callback;
};

struct Physics_Body
{
	Physics_Body_Type type;

	Vector center_of_mass;

	double inverse_linear_mass;

	double inverse_angular_mass;

	double real_inverse_linear_mass;

	double real_inverse_angular_mass;

	Vector position;

	double angle;

	Vector linear_velocity;

	double angular_velocity;

	Vector linear_force;

	double angular_force;

	Vector correction_linear_velocity;

	double correction_angular_velocity;

	bool world_transform_is_dirty;

	List collider_list;

	List joint_list;

	Physics_World* world;

	List_Node* node_in_world;
};

struct Physics_Collider
{
	Shape* local_shape;

	Shape* world_shape;

	Rect world_bounding_rect;

	double density;

	double restitution;

	double static_friction;

	double dynamic_friction;

	int filter_mask_1;

	int filter_mask_2;

	int filter_group;

	bool enabled;

	bool sensor;

	Physics_Body* body;

	List_Node* node_in_body;

	List_Node* node_in_world;

	Physics_Collision_Callback collision_callback;

	void* data;
};

struct Physics_Joint
{
	Physics_Joint_Type type;

	Vector local_anchor_1;

	Vector local_anchor_2;

	Vector world_anchor_1;

	Vector world_anchor_2;

	Physics_Body* body_1;

	Physics_Body* body_2;

	List_Node* node_in_body_1;

	List_Node* node_in_body_2;

	List_Node* node_in_world;
};

struct Physics_Collision
{
	Collision collision;

	Physics_Collider* collider_1;

	Physics_Collider* collider_2;
};

Physics_World* physics_world_create();

void physics_world_destroy(Physics_World* world);

void physics_world_step(Physics_World* world, double delta_time);

Physics_Body* physics_body_create(Physics_World* world, Physics_Body_Type type);

void physics_body_destroy(Physics_Body* body);

void physics_body_destroy_all_colliders(Physics_Body* body);

void physics_body_destroy_all_joints(Physics_Body* body);

Transform physics_body_get_transform(const Physics_Body* body);

Transform physics_body_get_inverse_transform(const Physics_Body* body);

void physics_body_apply_impulse_at_local_point(Physics_Body* body, Vector local_point, Vector impulse);

void physics_body_apply_impulse_at_world_point(Physics_Body* body, Vector world_point, Vector impulse);

void physics_body_apply_force_at_local_point(Physics_Body* body, Vector local_point, Vector force);

void physics_body_apply_force_at_world_point(Physics_Body* body, Vector world_point, Vector force);

void physics_body_apply_correction_impulse(Physics_Body* body, Vector point, Vector impulse);

void physics_body_update_world_transform(Physics_Body* body);

void physics_body_add_collider_mass(Physics_Body* body, Physics_Collider* collider);

void physics_body_subtract_collider_mass(Physics_Body* body, Physics_Collider* collider);

Physics_Collider* physics_collider_create(Physics_Body* body, const Shape* shape, double density);

void physics_collider_destroy(Physics_Collider* collider);

void physics_collider_get_mass(const Physics_Collider* collider, Vector* center_of_mass, double* linear_mass, double* angular_mass);

Physics_Joint* physics_joint_create(Physics_Joint_Type type, Physics_Body* body_1, Vector local_anchor_1, Vector world_anchor_1, Physics_Body* body_2, Vector local_anchor_2, Vector world_anchor_2);

Physics_Joint* physics_joint_create_local(Physics_Joint_Type type, Physics_Body* body_1, Vector local_anchor_1, Physics_Body* body_2, Vector local_anchor_2);

Physics_Joint* physics_joint_create_world(Physics_Joint_Type type, Physics_Body* body_1, Vector world_anchor_1, Physics_Body* body_2, Vector world_anchor_2);

void physics_joint_destroy(Physics_Joint* joint);

bool physics_can_collide(const Physics_Collider* collider_1, const Physics_Collider* collider_2);

bool physics_collide(const Physics_Collider* collider_1, const Physics_Collider* collider_2, Physics_Collision* collision);

