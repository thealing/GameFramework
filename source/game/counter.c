#include "counter.h"

Counter* counter_create(Vector position, Vector half_size, int min, int max, int* value)
{
	Counter* counter = HEAPALLOC(sizeof(Counter));

	Vector left_center = vector_subtract_xy(position, half_size.x + half_size.y, 0);

	Vector right_center = vector_add_xy(position, half_size.x + half_size.y, 0);
	
	counter->left_arrow = (Rect){ vector_subtract_value(left_center, half_size.y), vector_add_value(left_center, half_size.y) };

	counter->right_arrow = (Rect){ vector_subtract_value(right_center, half_size.y), vector_add_value(right_center, half_size.y) };

	counter->digits = (Rect){ vector_subtract(position, half_size), vector_add(position, half_size) };

	counter->min = min;

	counter->max = max;

	counter->value = value;

	return counter;
}

void counter_destroy(Counter* counter)
{
	free(counter);
}

void counter_update(Counter* counter)
{
	if (*counter->value > counter->min && input_is_rect_released(&counter->left_arrow))
	{
		(*counter->value)--;

		sound_play(g_sounds.click2);
	}

	if (*counter->value < counter->max && input_is_rect_released(&counter->right_arrow))
	{
		(*counter->value)++;

		sound_play(g_sounds.click2);
	}
}

void counter_render(Counter* counter)
{
	set_texture_and_color(NULL, &(Color){ 0, 0, 0, 1 });

	graphics_draw_format_in_rect(&counter->digits, ALIGNMENT_RIGHT, "%i", *counter->value);

	Texture* left_arrow_texture = g_textures.ui_arrow_released;

	if (input_is_rect_down(&counter->left_arrow))
	{
		left_arrow_texture = g_textures.ui_arrow_pressed;
	}

	if (*counter->value <= counter->min)
	{
		left_arrow_texture = g_textures.ui_arrow_disabled;
	}

	set_texture_and_color(left_arrow_texture, NULL);

	graphics_draw_texture_in_rect(&(Rect){ counter->left_arrow.max, counter->left_arrow.min });

	Texture* right_arrow_texture = g_textures.ui_arrow_released;

	if (input_is_rect_down(&counter->right_arrow))
	{
		right_arrow_texture = g_textures.ui_arrow_pressed;
	}

	if (*counter->value >= counter->max)
	{
		right_arrow_texture = g_textures.ui_arrow_disabled;
	}

	set_texture_and_color(right_arrow_texture, NULL);

	graphics_draw_texture_in_rect(&counter->right_arrow);
}
