#pragma once

#include "scene.h"

#include "levels.h"

#include "cars.h"

#include "engine/sound.h"

void menu_init();

void menu_enter();

void menu_leave();

void menu_pause();

void menu_resume();

void menu_update(double delta_time);

void menu_render();
