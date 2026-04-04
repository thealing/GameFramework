#pragma once

#include "engine/physics.h"

#include "engine/graphics.h"

#define COUNTOF(a) (sizeof(a) / sizeof((a)[0]))

Shape* create_rect_shape(Vector min, Vector max);

Shape* move_shape(Shape* shape);

void set_texture_and_color(const Texture* texture, const Color* color);

void draw_physics_world(const Physics_World* world);

void draw_physics_body(const Physics_Body* body);

void draw_physics_joint(const Physics_Joint* joint);
