#include "engine/input.h"

static Touch s_current_touches[TOUCH_COUNT];

static Touch s_previous_touches[TOUCH_COUNT];

static bool s_current_keys[WINDOW_KEY_MAX];

static bool s_previous_keys[WINDOW_KEY_MAX];

void input_update()
{
	memcpy(s_previous_touches, s_current_touches, sizeof(s_previous_touches));

	memcpy(s_previous_keys, s_current_keys, sizeof(s_previous_keys));

	if (!window_is_active())
	{
		memset(s_current_touches, 0, sizeof(s_current_touches));

		memset(s_current_keys, 0, sizeof(s_current_keys));
	}
}

Touch* input_get_touch(int index)
{
	return &s_current_touches[index];
}

bool* input_get_key(int index)
{
	return &s_current_keys[index];
}

Vector input_project_touch(const Touch* touch)
{
	Vector point = vector_create(touch->x, touch->y);

	graphics_unproject(&point);

	return point;
}

bool input_is_rect_down(const Rect* rect)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && test_point_rect(input_project_touch(&s_current_touches[i]), rect))
		{
			return true;
		}
	}

	return false;
}

bool input_is_segment_down(const Segment* segment)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && segment_test_point(segment, input_project_touch(&s_current_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_circle_down(const Circle* circle)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && circle_test_point(circle, input_project_touch(&s_current_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_polygon_down(const Polygon* polygon)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && polygon_test_point(polygon, input_project_touch(&s_current_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_shape_down(const Shape* shape)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && shape_test_point(shape, input_project_touch(&s_current_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_rect_pressed(const Rect* rect)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && !s_previous_touches[i].down && test_point_rect(input_project_touch(&s_current_touches[i]), rect))
		{
			return true;
		}
	}

	return false;
}

bool input_is_segment_pressed(const Segment* segment)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && !s_previous_touches[i].down && segment_test_point(segment, input_project_touch(&s_current_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_circle_pressed(const Circle* circle)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && !s_previous_touches[i].down && circle_test_point(circle, input_project_touch(&s_current_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_polygon_pressed(const Polygon* polygon)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && !s_previous_touches[i].down && polygon_test_point(polygon, input_project_touch(&s_current_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_shape_pressed(const Shape* shape)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (s_current_touches[i].down && !s_previous_touches[i].down && shape_test_point(shape, input_project_touch(&s_current_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_rect_released(const Rect* rect)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (!s_current_touches[i].down && s_previous_touches[i].down && test_point_rect(input_project_touch(&s_previous_touches[i]), rect))
		{
			return true;
		}
	}

	return false;
}

bool input_is_segment_released(const Segment* segment)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (!s_current_touches[i].down && s_previous_touches[i].down && segment_test_point(segment, input_project_touch(&s_previous_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_circle_released(const Circle* circle)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (!s_current_touches[i].down && s_previous_touches[i].down && circle_test_point(circle, input_project_touch(&s_previous_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_polygon_released(const Polygon* polygon)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (!s_current_touches[i].down && s_previous_touches[i].down && polygon_test_point(polygon, input_project_touch(&s_previous_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_shape_released(const Shape* shape)
{
	for (int i = 0; i < TOUCH_COUNT; i++)
	{
		if (!s_current_touches[i].down && s_previous_touches[i].down && shape_test_point(shape, input_project_touch(&s_previous_touches[i])))
		{
			return true;
		}
	}

	return false;
}

bool input_is_key_down(Window_Key key)
{
	return s_current_keys[key];
}

bool input_is_key_pressed(Window_Key key)
{
	return s_current_keys[key] && !s_previous_keys[key];
}

bool input_is_key_released(Window_Key key)
{
	return !s_current_keys[key] && s_previous_keys[key];
}
