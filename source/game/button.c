#include "button.h"

Button* button_create(const Shape* shape, Texture* texture_pressed, Texture* texture_released, Vector position, double scale)
{
	Button* button = calloc(1, sizeof(Button));

	button->shape = shape_clone(shape);

	button->texture_pressed = texture_pressed;

	button->texture_released = texture_released;

	button->position = position;

	button->scale = scale;

	return button;
}

void button_destroy(Button* button)
{
	shape_destroy(button->shape);

	free(button);
}

void button_update(Button* button)
{
	button->down = input_is_shape_down(button->shape);

	if (input_is_shape_released(button->shape))
	{
		button->clicked = true;
	}

	for (int i = 0; i < button->keybind_count; i++)
	{
		if (input_is_key_down(button->keybinds[i]))
		{
			button->down = true;
		}

		if (input_is_key_released(button->keybinds[i]))
		{
			button->clicked = true;
		}
	}
}

void button_render(Button* button)
{
	Texture* texture = button->down ? button->texture_pressed : button->texture_released;

	set_texture_and_color(texture, NULL);

	graphics_save_transform();

	graphics_translate(button->position);

	double size = fmax(texture->size.x, texture->size.y);

	graphics_scale(vector_divide(vector_create(button->scale, button->scale), size));

	graphics_draw_texture();

	graphics_load_transform();
}

void button_bind_key(Button* button, Window_Key key)
{
	button->keybinds[button->keybind_count++] = key;
}

bool button_was_clicked(Button* button)
{
	bool clicked = button->clicked;

	button->clicked = false;

	return clicked;
}
