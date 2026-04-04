#include "button.h"

Button* button_create(const char* text, Vector position, Vector scale)
{
	Button* button = calloc(1, sizeof(Button));

	button->shape = create_rect_shape(vector_multiply(scale, -0.5), vector_multiply(scale, 0.5));

	button->current_shape = shape_clone(button->shape);

	button->text = text;

	button->position = position;

	return button;
}

void button_destroy(Button* button)
{
	shape_destroy(button->shape);

	shape_destroy(button->current_shape);

	free(button);
}

void button_update(Button* button)
{
	shape_transform(button->shape, transform_create(button->position, 0), button->current_shape);

	button->down = input_is_shape_down(button->current_shape);

	if (input_is_shape_pressed(button->current_shape))
	{
		button->held = true;
	}

	if (!button->toggle && button->clicked)
	{
		button->clicked = false;
	}

	if (button->held && input_is_shape_released(button->current_shape))
	{
		button->clicked = button->toggle ? !button->clicked : true;
	}
	else if (!input_is_rect_down(&(Rect) { -INFINITY, -INFINITY, INFINITY, INFINITY }))
	{
		button->held = false;
	}
}

void button_render(Button* button)
{
	graphics_save_transform();

	graphics_translate(button->position);

	set_texture_and_color(NULL, &(Color){ 1.0, 1.0, 0.0, 1 });

	graphics_draw_shape(button->shape, false);

	set_texture_and_color(NULL, &(Color){ 1.0, 1.0, 0.0, (button->down && button->held || button->clicked) * 0.5 });

	graphics_draw_shape(button->shape, true);

	set_texture_and_color(NULL, &(Color){ 1.0, 1.0, 0.0, 1 });

	graphics_scale_uniformly(30);

	graphics_draw_string(ALIGNMENT_CENTER, ALIGNMENT_CENTER, button->text);

	graphics_load_transform();
}

bool button_was_clicked(Button* button)
{
	bool clicked = button->clicked;

	return clicked;
}
