#pragma once

#include "button.h"

#include "checkbox.h"

#include "counter.h"

#include "save.h"

#include <stddef.h>

#define BUTTON_COUNT_MAX 20

#define CHECKBOX_COUNT_MAX 20

#define COUNTER_COUNT_MAX 20

typedef struct Scene Scene;

struct Scene
{
	void (* enter)();
	
	void (* leave)();

	void (* pause)();

	void (* resume)();
	
	void (* update)(double delta_time);
	
	void (* render)();
};

extern Scene g_menu;

extern Scene g_settings;

extern Scene g_battle;

void scene_change(Scene* scene);

void scene_pause();

void scene_resume();

void scene_update(double delta_time);

void scene_render();

void scene_add_button(Button* button);

void scene_remove_button(Button* button);

void scene_add_checkbox(Checkbox* checkbox);

void scene_remove_checkbox(Checkbox* checkbox);

void scene_add_counter(Counter* counter);

void scene_remove_counter(Counter* counter);
