#pragma once

#include "engine/sound.h"

typedef struct Sounds Sounds;

struct Sounds
{
	Sound* intro;

	Sound* music;

	Sound* airhorn;

	Sound* applause;

	Sound* blip;

	Sound* click;

	Sound* click2;

	Sound* laser;

	Sound* water;

	Sound* beep;

	Sound* explosion;

	Sound* saw;
};

extern Sounds g_sounds;

void sounds_init();

void sounds_pause_all();

void sounds_resume_all();

void sounds_stop_all();
