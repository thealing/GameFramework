#include "sounds.h"

Sounds g_sounds;

void sounds_init()
{
	g_sounds.intro = sound_load("sounds/intro.mp3");

	g_sounds.music = sound_load("sounds/music.mp3");

	g_sounds.airhorn = sound_load("sounds/airhorn.mp3");

	g_sounds.applause = sound_load("sounds/applause.mp3");

	g_sounds.blip = sound_load("sounds/blip.mp3");

	g_sounds.click = sound_load("sounds/click.mp3");

	g_sounds.click2 = sound_load("sounds/click2.mp3");

	g_sounds.laser = sound_load("sounds/laser.mp3");

	g_sounds.water = sound_load("sounds/water.mp3");

	g_sounds.beep = sound_load("sounds/beep.mp3");

	g_sounds.explosion = sound_load("sounds/explosion.mp3");

	g_sounds.saw = sound_load("sounds/saw.mp3");
}

void sounds_pause_all()
{
	for (int i = 0; i < sizeof(g_sounds) / sizeof(Sound*); i++)
	{
		Sound* sound = ((Sound**)&g_sounds)[i];

		sound_pause(sound);
	}
}

void sounds_resume_all()
{
	for (int i = 0; i < sizeof(g_sounds) / sizeof(Sound*); i++)
	{
		Sound* sound = ((Sound**)&g_sounds)[i];

		sound_resume(sound);
	}
}

void sounds_stop_all()
{
	for (int i = 0; i < sizeof(g_sounds) / sizeof(Sound*); i++)
	{
		Sound* sound = ((Sound**)&g_sounds)[i];

		sound_stop(sound);
	}
}
