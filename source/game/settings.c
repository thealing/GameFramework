#include "settings.h"

static bool s_loaded;

static Checkbox* s_music;

static Checkbox* s_sounds;

static Checkbox* s_random_objects;

static Checkbox* s_same_random_car;

static Counter* s_sudden_death_time;

static Button* s_back;

void settings_init()
{
	g_settings.enter = settings_enter;

	g_settings.leave = settings_leave;

	g_settings.pause = settings_pause;

	g_settings.resume = settings_resume;

	g_settings.update = settings_update;

	g_settings.render = settings_render;
}

void settings_enter()
{
	save_load();

	s_music = checkbox_create(vector_create(120, 630), 32, &g_save.play_music);

	s_sounds = checkbox_create(vector_create(120, 540), 32, &g_save.play_sounds);

	s_random_objects = checkbox_create(vector_create(120, 450), 32, &g_save.random_objects);

	s_same_random_car = checkbox_create(vector_create(120, 360), 32, &g_save.same_random_car);

	scene_add_checkbox(s_music);

	scene_add_checkbox(s_sounds);

	scene_add_checkbox(s_random_objects);

	scene_add_checkbox(s_same_random_car);

	s_sudden_death_time = counter_create(vector_create(120, 270), vector_create(32, 32), 0, 30, &g_save.sudden_death_time);

	scene_add_counter(s_sudden_death_time);

	s_back = button_create(move_shape(shape_create_circle(vector_create(80, 80), 64)), g_textures.ui_back_pressed, g_textures.ui_back_released, vector_create(80, 80), 120);

	button_bind_key(s_back, WINDOW_KEY_BACK);

	button_bind_key(s_back, WINDOW_KEY_BACKSPACE);

	scene_add_button(s_back);
}

void settings_leave()
{
	checkbox_destroy(s_music);

	checkbox_destroy(s_sounds);

	checkbox_destroy(s_random_objects);

	checkbox_destroy(s_same_random_car);

	counter_destroy(s_sudden_death_time);

	button_destroy(s_back);
}

void settings_pause()
{
	sounds_pause_all();
}

void settings_resume()
{
	sounds_resume_all();
}

void settings_update(double delta_time)
{
	UNUSED(delta_time);

	if (g_save.play_music)
	{
		sound_set_volume(g_sounds.intro, 0.6);

		sound_set_volume(g_sounds.music, 1.0);
	}
	else
	{
		sound_set_volume(g_sounds.intro, 0);

		sound_set_volume(g_sounds.music, 0);
	}

	if (g_save.play_sounds)
	{
		sound_set_volume(g_sounds.airhorn, 0.4);

		sound_set_volume(g_sounds.applause, 0.7);

		sound_set_volume(g_sounds.blip, 1.0);

		sound_set_volume(g_sounds.click, 1.0);

		sound_set_volume(g_sounds.click2, 1.0);

		sound_set_volume(g_sounds.laser, 0.7);

		sound_set_volume(g_sounds.water, 0.9);

		sound_set_volume(g_sounds.beep, 1.0);

		sound_set_volume(g_sounds.explosion, 1.0);

		sound_set_volume(g_sounds.saw, 0.4);
	}
	else
	{
		sound_set_volume(g_sounds.airhorn, 0.0);

		sound_set_volume(g_sounds.applause, 0.0);

		sound_set_volume(g_sounds.blip, 0.0);

		sound_set_volume(g_sounds.click, 0.0);

		sound_set_volume(g_sounds.click2, 0.0);

		sound_set_volume(g_sounds.laser, 0.0);

		sound_set_volume(g_sounds.water, 0.0);

		sound_set_volume(g_sounds.beep, 0.0);

		sound_set_volume(g_sounds.explosion, 0.0);

		sound_set_volume(g_sounds.saw, 0.0);
	}

	if (!s_loaded)
	{
		s_loaded = true;

		scene_change(&g_menu);

		return;
	}

	if (input_is_rect_pressed(&(Rect){ 1080, 0, 1280, 200 }))
	{
		g_save.debug_hud = !g_save.debug_hud;

		sound_play(g_sounds.blip);
	}

	if (button_was_clicked(s_back))
	{
		scene_change(&g_menu);

		return;
	}

	if (!sound_is_playing(g_sounds.intro))
	{
		sound_play(g_sounds.intro);
	}
}

void settings_render()
{
	set_texture_and_color(NULL, &(Color){ 0.75, 0.75, 0.75, 1 });

	graphics_draw_rect(&(Rect){ 0, 0, 1280, 720 }, true);

	graphics_set_color(&(Color){ 0, 0, 0, 1 });

	graphics_draw_string_in_rect(&(Rect){ 260, 600, 0, 660 }, ALIGNMENT_LEFT, "MUSIC");

	graphics_draw_string_in_rect(&(Rect){ 260, 510, 0, 570 }, ALIGNMENT_LEFT, "SOUNDS");

	graphics_draw_string_in_rect(&(Rect){ 260, 420, 0, 480 }, ALIGNMENT_LEFT, "RANDOM OBJECTS");

	graphics_draw_string_in_rect(&(Rect){ 260, 330, 0, 390 }, ALIGNMENT_LEFT, "SAME RANDOM CAR");

	graphics_draw_string_in_rect(&(Rect){ 260, 240, 0, 300 }, ALIGNMENT_LEFT, "SUDDEN DEATH TIME");

	if (g_save.debug_hud)
	{
		for (int i = 0; i < WINDOW_KEY_MAX; i++)
		{
			if (input_is_key_down(i))
			{
				graphics_draw_format_in_rect(&(Rect){ 640, 100, 0, 200 }, ALIGNMENT_CENTER, "%i", i);
			}
		}
	}
}
