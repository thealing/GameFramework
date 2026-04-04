#include "scene.h"

Scene g_menu;

Scene g_settings;

Scene g_battle;

static Scene* s_scene;

static Button* s_buttons[BUTTON_COUNT_MAX];

static int s_button_count;

static Checkbox* s_checkboxes[CHECKBOX_COUNT_MAX];

static int s_checkbox_count;

static Counter* s_counters[COUNTER_COUNT_MAX];

static int s_counter_count;

void scene_change(Scene* scene)
{
	if (s_scene != NULL)
	{
		s_scene->leave();

		s_button_count = 0;

		s_checkbox_count = 0;
		
		s_counter_count = 0;
	}

	s_scene = scene;

	if (s_scene != NULL)
	{
		s_scene->enter();
	}
}

void scene_pause()
{
	s_scene->pause();
}

void scene_resume()
{
	s_scene->resume();
}

void scene_update(double delta_time)
{
	if (s_scene != NULL)
	{
		s_scene->update(delta_time);

		for (int i = 0; i < s_button_count; i++)
		{
			button_update(s_buttons[i]);
		}

		for (int i = 0; i < s_checkbox_count; i++)
		{
			checkbox_update(s_checkboxes[i]);
		}

		for (int i = 0; i < s_counter_count; i++)
		{
			counter_update(s_counters[i]);
		}

		if (input_is_key_pressed('B'))
		{
			g_save.debug_hud = !g_save.debug_hud;

			sound_play(g_sounds.blip);
		}
	}

	save_save();
}

void scene_render()
{
	if (s_scene != NULL)
	{
		s_scene->render();

		for (int i = 0; i < s_button_count; i++)
		{
			button_render(s_buttons[i]);
		}

		for (int i = 0; i < s_checkbox_count; i++)
		{
			checkbox_render(s_checkboxes[i]);
		}

		for (int i = 0; i < s_counter_count; i++)
		{
			counter_render(s_counters[i]);
		}
	}
}

void scene_add_button(Button* button)
{
	s_buttons[s_button_count++] = button;
}

void scene_remove_button(Button* button)
{
	for (int i = 0; i < s_button_count; i++)
	{
		if (s_buttons[i] == button)
		{
			s_buttons[i] = s_buttons[--s_button_count];
			
			return;
		}
	}
}

void scene_add_checkbox(Checkbox* checkbox)
{
	s_checkboxes[s_checkbox_count++] = checkbox;
}

void scene_remove_checkbox(Checkbox* checkbox)
{
	for (int i = 0; i < s_checkbox_count; i++)
	{
		if (s_checkboxes[i] == checkbox)
		{
			s_checkboxes[i] = s_checkboxes[--s_checkbox_count];
			
			return;
		}
	}
}

void scene_add_counter(Counter* counter)
{
	s_counters[s_counter_count++] = counter;
}

void scene_remove_counter(Counter* counter)
{
	for (int i = 0; i < s_counter_count; i++)
	{
		if (s_counters[i] == counter)
		{
			s_counters[i] = s_counters[--s_counter_count];
			
			return;
		}
	}
}

