#include "menu.h"

static Physics_World* s_blue_world;

static Physics_World* s_red_world;

static Physics_World* s_level_world;

static Car* s_blue_cars[CAR_TYPE_COUNT];

static Car* s_red_cars[CAR_TYPE_COUNT];

static Level* s_levels[LEVEL_TYPE_COUNT];

static const char* s_car_names[CAR_TYPE_COUNT];

static const char* s_level_names[LEVEL_TYPE_COUNT];

static Checkbox* s_blue_is_bot;

static Checkbox* s_red_is_bot;

static Counter* s_blue_car_index;

static Counter* s_red_car_index;

static Counter* s_level_index;

static Counter* s_scores_to_win;

static Button* s_play;

static Button* s_settings;

static void draw_car(Vector center, Vector bottom, double scale, Car* car)
{
	graphics_save_transform();

	if (car == NULL)
	{
		graphics_translate(center);

		graphics_translate(vector_create(0, -20));

		graphics_scale(vector_create(250, 250));

		graphics_set_color(&(Color){ 0, 0, 0, 1 });

		graphics_draw_string(ALIGNMENT_CENTER, ALIGNMENT_CENTER, "?");
	}
	else
	{
		graphics_translate(center);

		graphics_scale(create_isotropic_vector(scale));

		car_render(car);
	}

	graphics_load_transform();

	graphics_save_transform();

	graphics_translate(bottom);

	graphics_scale(create_isotropic_vector(64));

	graphics_set_color(&(Color){ 0, 0, 0, 1 });

	graphics_draw_string(ALIGNMENT_CENTER, ALIGNMENT_CENTER, car == NULL ? "RANDOM" : s_car_names[car->type]);

	graphics_load_transform();
}

static void draw_level(Vector center, Vector bottom, double scale)
{
	graphics_save_transform();

	if (g_save.selected_level == 0)
	{
		graphics_translate(center);

		graphics_translate(vector_create(0, -20));

		graphics_scale(vector_create(250, 250));

		graphics_set_color(&(Color){ 0, 0, 0, 1 });

		graphics_draw_string(ALIGNMENT_CENTER, ALIGNMENT_CENTER, "?");
	}
	else
	{
		graphics_translate(vector_subtract(center, vector_create(scale * 640, scale * 360)));

		graphics_scale(create_isotropic_vector(scale));

		level_render(s_levels[g_save.selected_level - 1]);

		set_texture_and_color(NULL, &(Color){ 0, 0, 0, 1 });

		graphics_set_line_width(10);

		graphics_set_line_cap(LINE_CAP_SQUARE);

		graphics_draw_line(vector_create(0, 0), vector_create(1280, 0));

		graphics_draw_line(vector_create(1280, 0), vector_create(1280, 720));

		graphics_draw_line(vector_create(1280, 720), vector_create(0, 720));

		graphics_draw_line(vector_create(0, 720), vector_create(0, 0));
	}

	graphics_load_transform();

	graphics_save_transform();

	graphics_translate(bottom);

	graphics_scale(create_isotropic_vector(64));

	graphics_set_color(&(Color){ 0, 0, 0, 1 });

	graphics_draw_string(ALIGNMENT_CENTER, ALIGNMENT_CENTER, g_save.selected_level == 0 ? "RANDOM" : s_level_names[g_save.selected_level - 1]);

	graphics_load_transform();
}

void menu_init()
{
	g_menu.enter = menu_enter;

	g_menu.leave = menu_leave;

	g_menu.pause = menu_pause;

	g_menu.resume = menu_resume;

	g_menu.update = menu_update;

	g_menu.render = menu_render;

	s_blue_world = physics_world_create();

	s_red_world = physics_world_create();

	s_level_world = physics_world_create();

	for (int i = 0; i < CAR_TYPE_COUNT; i++)
	{
		s_blue_cars[i] = car_create(i, s_blue_world, vector_create(0, 0), 0);

		s_red_cars[i] = car_create(i, s_red_world, vector_create(0, 0), 1);
	}

	for (int i = 0; i < LEVEL_TYPE_COUNT; i++)
	{
		s_levels[i] = level_create(i, s_level_world, 0);
	}

	s_car_names[CAR_TYPE_MONSTER_TRUCK] = "MONSTER TRUCK";

	s_car_names[CAR_TYPE_DRAGSTER] = "DRAGSTER";

	s_car_names[CAR_TYPE_GO_CART] = "GO-CART";

	s_car_names[CAR_TYPE_MINIBUS] = "MINIBUS";

	s_car_names[CAR_TYPE_TRACTOR] = "TRACTOR";

	s_car_names[CAR_TYPE_TIMBER_LORRY] = "TIMBER LORRY";

	s_car_names[CAR_TYPE_GARBAGE_TRUCK] = "GARBAGE TRUCK";

	s_level_names[LEVEL_TYPE_CAVE] = "CAVE";

	s_level_names[LEVEL_TYPE_TUNNEL] = "TUNNEL";

	s_level_names[LEVEL_TYPE_DESERT_CAVE] = "DESERT CAVE";

	s_level_names[LEVEL_TYPE_DESERT_TUNNEL] = "DESERT TUNNEL";

	s_level_names[LEVEL_TYPE_RAMP] = "RAMP";

	s_level_names[LEVEL_TYPE_GAP] = "GAP";

	s_level_names[LEVEL_TYPE_BUMP] = "BUMP";

	s_level_names[LEVEL_TYPE_DOUBLE_BUMP] = "DOUBLE BUMP";

	s_level_names[LEVEL_TYPE_TRIPLE_BUMP] = "TRIPLE BUMP";

	s_level_names[LEVEL_TYPE_SMILEY_FACE] = "SMILEY FACE";

	s_level_names[LEVEL_TYPE_FROWNY_FACE] = "FROWNY FACE";

	s_level_names[LEVEL_TYPE_NEUTRAL_FACE] = "NEUTRAL FACE";

	s_level_names[LEVEL_TYPE_BLADES] = "BLADES";

	sounds_stop_all();
}

void menu_enter()
{
	s_blue_is_bot = checkbox_create(vector_create(580, 500), 36, &g_save.blue_is_bot);

	s_red_is_bot = checkbox_create(vector_create(700, 500), 36, &g_save.red_is_bot);

	scene_add_checkbox(s_blue_is_bot);

	scene_add_checkbox(s_red_is_bot);

	s_blue_car_index = counter_create(vector_create(700, 310), vector_create(30, 24), 0, CAR_TYPE_COUNT, &g_save.selected_blue_car);

	s_red_car_index = counter_create(vector_create(700, 230), vector_create(30, 24), 0, CAR_TYPE_COUNT, &g_save.selected_red_car);

	s_level_index = counter_create(vector_create(700, 150), vector_create(30, 24), 0, LEVEL_TYPE_COUNT, &g_save.selected_level);

	s_scores_to_win = counter_create(vector_create(700, 70), vector_create(30, 24), 1, 10, &g_save.max_score);

	scene_add_counter(s_blue_car_index);

	scene_add_counter(s_red_car_index);

	scene_add_counter(s_level_index);

	scene_add_counter(s_scores_to_win);

	s_play = button_create(move_shape(shape_create_circle(vector_create(1200, 80), 64)), g_textures.ui_play_pressed, g_textures.ui_play_released, vector_create(1200, 80), 120);

	s_settings = button_create(move_shape(shape_create_circle(vector_create(1200, 300), 64)), g_textures.ui_settings_pressed, g_textures.ui_settings_released, vector_create(1200, 300), 120);

	button_bind_key(s_play, WINDOW_KEY_ENTER);

	scene_add_button(s_play);

	scene_add_button(s_settings);
}

void menu_leave()
{
	checkbox_destroy(s_blue_is_bot);

	checkbox_destroy(s_red_is_bot);

	counter_destroy(s_scores_to_win);

	button_destroy(s_play);

	button_destroy(s_settings);
}

void menu_pause()
{
	sounds_pause_all();
}

void menu_resume()
{
	sounds_resume_all();
}

void menu_update(double delta_time)
{
	UNUSED(delta_time);

	if (button_was_clicked(s_play))
	{
		sounds_stop_all();

		scene_change(&g_battle);

		return;
	}

	if (button_was_clicked(s_settings))
	{
		scene_change(&g_settings);

		return;
	}

	if (!sound_is_playing(g_sounds.intro))
	{
		sound_play(g_sounds.intro);
	}
}

void menu_render()
{
	set_texture_and_color(NULL, &(Color){ 0.9, 1, 0.9, 1 });

	graphics_draw_rect(&(Rect){ 0, 0, 1280, 720 }, true);

	draw_car(vector_create(280, 570), vector_create(280, 440), 1.8, g_save.selected_blue_car == 0 ? NULL : s_blue_cars[g_save.selected_blue_car - 1]);

	draw_car(vector_create(1000, 570), vector_create(1000, 440), 1.8, g_save.selected_red_car == 0 ? NULL : s_red_cars[g_save.selected_red_car - 1]);

	draw_level(vector_create(280, 210), vector_create(280, 80), 0.25);

	set_texture_and_color(g_textures.overlay, NULL);

	graphics_draw_texture_in_rect(&(Rect){ 0, 0, 1280, 720 });

	graphics_set_color(&(Color){ 0, 0, 0, 1 });

	graphics_draw_string_in_rect(&(Rect){ 0, 550, 1280, 670 }, ALIGNMENT_CENTER, "BOT");

	graphics_draw_string_in_rect(&(Rect){ 810, 290, 0, 330 }, ALIGNMENT_LEFT, "BLUE CAR");

	graphics_draw_string_in_rect(&(Rect){ 810, 210, 0, 250 }, ALIGNMENT_LEFT, "RED CAR");

	graphics_draw_string_in_rect(&(Rect){ 810, 130, 0, 170 }, ALIGNMENT_LEFT, "LEVEL");

	graphics_draw_string_in_rect(&(Rect){ 810, 50, 0, 90 }, ALIGNMENT_LEFT, "SCORES TO WIN");
}
