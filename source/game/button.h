#pragma once

#include "util.h"

#include "engine/input.h"

#define BUTTON_KEYBIND_COUNT_MAX 4

typedef struct Button Button;

struct Button
{
	Shape* shape;

	Shape* current_shape;

	const char* text;

	Vector position;

	bool down;

	bool held;

	bool clicked;

	bool toggle;
};

Button* button_create(const char* text, Vector position, Vector scale);

void button_destroy(Button* button);

void button_update(Button* button);

void button_render(Button* button);

bool button_was_clicked(Button* button);

