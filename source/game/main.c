#include "main.h"

static double s_update_fps;

static double s_render_fps;

int main()
{
	random_set_seed(time(NULL));

	void* window = NULL;

	bool started = false;

	double update_delta = 1.0 / 200.0;

	double render_delta = 1.0 / 60.0;

	double max_latency = 0.1;

	double measure_delta = 1.0;

	double start_time = get_time();

	double last_update_time = start_time;

	double last_render_time = start_time;

	double last_measure_time = start_time;

	bool paused = false;

	int update_count = 0;

	int render_count = 0;

	Window_Event event = { 0 };

	config_set_value(CONFIG_KEY_WINDOW_TITLE, "Rivet Rival");

	config_set_value(CONFIG_KEY_WINDOW_CLASS, "RivetRivalWindowClass");

	config_set_value(CONFIG_KEY_FOLDER_NAME, "RivetRival");

	window_create(1280, 720);

	while (window_is_open())
	{
		double current_time = get_time();

		if (current_time > last_update_time + update_delta)
		{
			input_update();

			while (window_poll_event(&event))
			{
				switch (event.type)
				{
					case WINDOW_EVENT_RESUMED:
					{
						if (started && paused)
						{
							paused = false;

							scene_resume();
						}

						break;
					}
					case WINDOW_EVENT_PAUSED:
					{
						if (started && !paused)
						{
							paused = true;

							scene_pause();
						}

						break;
					}
					case WINDOW_EVENT_WINDOW_CREATED:
					{
						window = event.state_event.window;

						graphics_init(window);

						textures_init();

						graphics_set_font(g_textures.font);

						graphics_set_camera(&(Rect){ 0, 0, 1280, 720 });

						if (!started)
						{
							sounds_init();

							menu_init();

							settings_init();

							battle_init();

							scene_change(&g_settings);

							started = true;
						}

						break;
					}
					case WINDOW_EVENT_WINDOW_DESTROYED:
					{
						graphics_uninit(window);

						window = NULL;

						break;
					}
					case WINDOW_EVENT_TOUCH_MOVE:
					{
						if (window != NULL)
						{
							double width = window_get_width(window);

							double height = window_get_height(window);

							Touch* touch = input_get_touch(event.touch_event.index);

							touch->x = event.touch_event.x / width;

							touch->y = 1 - event.touch_event.y / height;
						}

						break;
					}
					case WINDOW_EVENT_TOUCH_DOWN:
					{
						double width = window_get_width(window);

						double height = window_get_height(window);

						Touch* touch = input_get_touch(event.touch_event.index);

						touch->x = event.touch_event.x / width;

						touch->y = 1 - event.touch_event.y / height;

						touch->down = true;

						break;
					}
					case WINDOW_EVENT_TOUCH_UP:
					{
						double width = window_get_width(window);

						double height = window_get_height(window);

						Touch* touch = input_get_touch(event.touch_event.index);

						touch->x = event.touch_event.x / width;

						touch->y = 1 - event.touch_event.y / height;

						touch->down = false;

						break;
					}
					case WINDOW_EVENT_KEY_DOWN:
					{
						bool* key = input_get_key(event.key_event.key);

						*key = true;

						break;
					}
					case WINDOW_EVENT_KEY_UP:
					{
						bool* key = input_get_key(event.key_event.key);

						*key = false;

						break;
					}
				}
			}

			if (!paused)
			{
				update(update_delta);
			}

			last_update_time = fmax(last_update_time + update_delta, current_time - max_latency);

			update_count++;
		}

		if (current_time > last_render_time + render_delta)
		{
			graphics_clear(&(Color){ 1.0, 0.0, 1.0, 1.0 });

			render();

			graphics_display();

			last_render_time = current_time;

			render_count++;
		}

		if (current_time > last_measure_time + measure_delta)
		{
			s_update_fps = update_count / (current_time - last_measure_time);

			s_render_fps = render_count / (current_time - last_measure_time);

			update_count = 0;

			render_count = 0;

			last_measure_time = current_time;
		}

		double sleep_duration = fmin(last_update_time + update_delta - current_time, last_render_time + render_delta - current_time);

		sleep(sleep_duration);
	}

	return 0;
}

void update(double delta_time)
{
	scene_update(delta_time);
}

void render()
{
	scene_render();

	if (g_save.debug_hud)
	{
		graphics_draw_format_in_rect(&(Rect){ 0, 0, 0, 30 }, ALIGNMENT_LEFT, "%5d %3d", lround(s_update_fps), lround(s_render_fps));
	}
}
