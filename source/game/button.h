#pragma once

#include "textures.h"

#include "sounds.h"

#include "util.h"

#include "engine/input.h"

#define BUTTON_KEYBIND_COUNT_MAX 4

typedef struct Button Button;

struct Button
{
	Shape* shape;

	Texture* texture_pressed;

	Texture* texture_released;

	Vector position;

	double scale;

	Window_Key keybinds[BUTTON_KEYBIND_COUNT_MAX];

	int keybind_count;

	bool down;

	bool clicked;
};

Button* button_create(const Shape* shape, Texture* texture_pressed, Texture* texture_released, Vector position, double scale);

void button_destroy(Button* button);

void button_update(Button* button);

void button_render(Button* button);

void button_bind_key(Button* button, Window_Key key);

bool button_was_clicked(Button* button);

