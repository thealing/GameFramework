#pragma once

#include "engine/graphics.h"

#include "engine/window.h"

#define TOUCH_COUNT 12

typedef struct Touch Touch;

struct Touch
{
	double x;

	double y;

	bool down;
};

void input_update();

Touch* input_get_touch(int index);

bool* input_get_key(int index);

Vector input_project_touch(const Touch* touch);

bool input_is_rect_down(const Rect* rect);

bool input_is_segment_down(const Segment* segment);

bool input_is_circle_down(const Circle* circle);

bool input_is_polygon_down(const Polygon* polygon);

bool input_is_shape_down(const Shape* shape);

bool input_is_rect_pressed(const Rect* rect);

bool input_is_segment_pressed(const Segment* segment);

bool input_is_circle_pressed(const Circle* circle);

bool input_is_polygon_pressed(const Polygon* polygon);

bool input_is_shape_pressed(const Shape* shape);

bool input_is_rect_released(const Rect* rect);

bool input_is_segment_released(const Segment* segment);

bool input_is_circle_released(const Circle* circle);

bool input_is_polygon_released(const Polygon* polygon);

bool input_is_shape_released(const Shape* shape);

bool input_is_key_down(Window_Key key);

bool input_is_key_pressed(Window_Key key);

bool input_is_key_released(Window_Key key);
