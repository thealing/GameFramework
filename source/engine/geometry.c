#include "geometry.h"

Vector vector_create(double x, double y)
{
	Vector v = { x, y };

	return v;
}

bool vector_equal(Vector v, Vector w)
{
	return v.x == w.x && v.y == w.y;
}

Vector vector_negate(Vector v)
{
	return vector_create(-v.x, -v.y);
}

Vector vector_add(Vector v, Vector w)
{
	return vector_create(v.x + w.x, v.y + w.y);
}

Vector vector_add_value(Vector v, double w)
{
	return vector_create(v.x + w, v.y + w);
}

Vector vector_add_xy(Vector v, double x, double y)
{
	return vector_create(v.x + x, v.y + y);
}

Vector vector_subtract(Vector v, Vector w)
{
	return vector_create(v.x - w.x, v.y - w.y);
}

Vector vector_subtract_value(Vector v, double w)
{
	return vector_create(v.x - w, v.y - w);
}

Vector vector_subtract_xy(Vector v, double x, double y)
{
	return vector_create(v.x - x, v.y - y);
}

Vector vector_multiply(Vector v, double s)
{
	return vector_create(v.x * s, v.y * s);
}

Vector vector_divide(Vector v, double s)
{
	return vector_create(v.x / s, v.y / s);
}

Vector vector_scale(Vector v, Vector s)
{
	return vector_create(v.x * s.x, v.y * s.y);
}

Vector vector_downscale(Vector v, Vector s)
{
	return vector_create(v.x / s.x, v.y / s.y);
}

Vector vector_rotate(Vector v, double a)
{
	double s = sin(a);

	double c = cos(a);

	return vector_create(c * v.x - s * v.y, s * v.x + c * v.y);
}

Vector vector_left(Vector v)
{
	return vector_create(-v.y, v.x);
}

Vector vector_right(Vector v)
{
	return vector_create(v.y, -v.x);
}

Vector vector_normalize(Vector v)
{
	return vector_divide(v, vector_length(v));
}

double vector_length(Vector v)
{
	return sqrt(vector_length_squared(v));
}

double vector_length_squared(Vector v)
{
	return square(v.x) + square(v.y);
}

double vector_distance(Vector v, Vector w)
{
	return sqrt(vector_distance_squared(v, w));
}

double vector_distance_squared(Vector v, Vector w)
{
	return square(v.x - w.x) + square(v.y - w.y);
}

double vector_dot(Vector v, Vector w)
{
	return v.x * w.x + v.y * w.y;
}

double vector_cross(Vector v, Vector w)
{
	return v.x * w.y - v.y * w.x;
}

Vector vector_middle(Vector v, Vector w)
{
	return vector_multiply(vector_add(v, w), 0.5);
}

Transform transform_create(Vector translation, double rotation)
{
	Transform transform;

	transform.x = translation.x;

	transform.y = translation.y;

	transform.s = sin(rotation);

	transform.c = cos(rotation);

	return transform;
}

Transform transform_invert(Transform transform)
{
	Transform result;

	result.x = -transform.c * transform.x - transform.s * transform.y;

	result.y = transform.s * transform.x - transform.c * transform.y;

	result.s = -transform.s;

	result.c = transform.c;

	return result;
}

Vector transform_apply(Transform transform, Vector vector)
{
	Vector result;

	result.x = transform.c * vector.x - transform.s * vector.y + transform.x;

	result.y = transform.s * vector.x + transform.c * vector.y + transform.y;

	return result;
}

Vector segment_get_centroid(const Segment* segment)
{
	return vector_middle(segment->a, segment->b);
}

double segment_get_linear_mass_factor(const Segment* segment)
{
	return vector_distance(segment->a, segment->b);
}

double segment_get_angular_mass_factor(const Segment* segment)
{
	return vector_distance_squared(segment->a, segment->b) / 12.0;
}

Rect segment_get_bounding_rect(const Segment* segment)
{
	Rect rect;

	rect.min = vector_create(fmin(segment->a.x, segment->b.x), fmin(segment->a.y, segment->b.y));

	rect.max = vector_create(fmax(segment->a.x, segment->b.x), fmax(segment->a.y, segment->b.y));

	return rect;
}

void segment_transform(const Segment* segment, Transform transform, Segment* result)
{
	result->a = transform_apply(transform, segment->a);

	result->b = transform_apply(transform, segment->b);
}

bool segment_test_point(const Segment* segment, Vector point)
{
	Vector ab = vector_subtract(segment->b, segment->a);

	return vector_dot(ab, point) >= vector_dot(ab, segment->a) && vector_dot(ab, point) <= vector_dot(ab, segment->b) && vector_cross(ab, point) == vector_cross(ab, segment->a);
}

Vector segment_project_point(const Segment* segment, Vector point)
{
	return project_onto_segment(segment->a, segment->b, point);
}

Vector circle_get_centroid(const Circle* circle)
{
	return circle->center;
}

double circle_get_linear_mass_factor(const Circle* circle)
{
	return square(circle->radius) * M_PI;
}

double circle_get_angular_mass_factor(const Circle* circle)
{
	return square(circle->radius) / 2.0;
}

Rect circle_get_bounding_rect(const Circle* circle)
{
	Rect rect;

	rect.min = vector_subtract_value(circle->center, circle->radius);

	rect.max = vector_add_value(circle->center, circle->radius);

	return rect;
}

void circle_transform(const Circle* circle, Transform transform, Circle* result)
{
	result->center = transform_apply(transform, circle->center);

	result->radius = circle->radius;
}

bool circle_test_point(const Circle* circle, Vector point)
{
	return vector_distance_squared(circle->center, point) <= square(circle->radius);
}

Vector circle_project_point(const Circle* circle, Vector point)
{
	return vector_add(circle->center, vector_multiply(vector_normalize(vector_subtract(point, circle->center)), circle->radius));
}

Vector polygon_get_centroid(const Polygon* polygon)
{
	Vector centroid = { 0.0, 0.0 };

	double weight = 0.0;

	for (int i = polygon->point_count - 1, j = 0; j < polygon->point_count; i = j, j++)
	{
		Vector a = polygon->points[i];

		Vector b = polygon->points[j];

		double side_length = vector_distance(a, b);

		centroid = vector_add(centroid, vector_multiply(vector_middle(a, b), side_length));

		weight += side_length;
	}

	return vector_divide(centroid, weight);
}

double polygon_get_linear_mass_factor(const Polygon* polygon)
{
	double area = 0.0;

	for (int i = polygon->point_count - 1, j = 0; j < polygon->point_count; i = j, j++)
	{
		Vector a = polygon->points[i];

		Vector b = polygon->points[j];

		area += vector_cross(a, b);
	}

	area /= 2.0;

	return area;
}

double polygon_get_angular_mass_factor(const Polygon* polygon)
{
	double numerator = 0.0;

	double denominator = 0.0;

	for (int i = polygon->point_count - 1, j = 0; j < polygon->point_count; i = j, j++)
	{
		Vector a = polygon->points[i];

		Vector b = polygon->points[j];

		numerator += vector_cross(a, b) * (vector_dot(a, a) + vector_dot(a, b) + vector_dot(b, b));

		denominator += vector_cross(a, b) * 6.0;
	}

	return numerator / denominator - vector_length_squared(polygon_get_centroid(polygon));
}

Rect polygon_get_bounding_rect(const Polygon* polygon)
{
	Rect rect;

	rect.min.x = INFINITY;

	rect.min.y = INFINITY;

	rect.max.x = -INFINITY;

	rect.max.y = -INFINITY;

	for (int i = 0; i < polygon->point_count; i++)
	{
		rect.min.x = fmin(rect.min.x, polygon->points[i].x);

		rect.min.y = fmin(rect.min.y, polygon->points[i].y);

		rect.max.x = fmax(rect.max.x, polygon->points[i].x);

		rect.max.y = fmax(rect.max.y, polygon->points[i].y);
	}

	return rect;
}

void polygon_transform(const Polygon* polygon, Transform transform, Polygon* result)
{
	for (int i = 0; i < polygon->point_count; i++)
	{
		result->points[i] = transform_apply(transform, polygon->points[i]);
	}
}

bool polygon_test_point(const Polygon* polygon, Vector point)
{
	for (int i = polygon->point_count - 1, j = 0; j < polygon->point_count; i = j, j++)
	{
		Vector a = polygon->points[i];

		Vector b = polygon->points[j];

		if (vector_cross(vector_subtract(b, a), vector_subtract(point, a)) < 0)
		{
			return false;
		}
	}

	return true;
}

Vector polygon_project_point(const Polygon* polygon, Vector point)
{
	double min_distance = INFINITY;

	Vector closest_point = polygon->points[0];

	for (int i = polygon->point_count - 1, j = 0; j < polygon->point_count; i = j, j++)
	{
		Vector a = polygon->points[i];

		Vector b = polygon->points[j];

		Vector projected_point = project_onto_segment(a, b, point);

		double distance = vector_distance_squared(projected_point, point);

		if (distance < min_distance)
		{
			min_distance = distance;

			closest_point = projected_point;
		}
	}

	return closest_point;
}

Shape* shape_create_segment(Vector a, Vector b)
{
	Shape* shape = HEAPALLOC(sizeof(Shape));

	shape->type = SHAPE_TYPE_SEGMENT;

	shape->segment.a = a;

	shape->segment.b = b;

	return shape;
}

Shape* shape_create_circle(Vector center, double radius)
{
	Shape* shape = HEAPALLOC(sizeof(Shape));

	shape->type = SHAPE_TYPE_CIRCLE;

	shape->circle.center = center;

	shape->circle.radius = radius;

	return shape;
}

Shape* shape_create_polygon(int point_count, const Vector points[])
{
	Shape* shape = HEAPALLOC(sizeof(Shape) + sizeof(Vector) * point_count);

	shape->type = SHAPE_TYPE_POLYGON;

	shape->polygon.point_count = point_count;

	memcpy(shape->polygon.points, points, sizeof(Vector) * point_count);

	if (polygon_get_linear_mass_factor(&shape->polygon) < 0)
	{
		for (int i = 0; i < point_count; i++)
		{
			shape->polygon.points[i] = points[point_count - 1 - i];
		}
	}

	return shape;
}

Shape* shape_clone_segment(const Segment* segment)
{
	return shape_create_segment(segment->a, segment->b);
}

Shape* shape_clone_circle(const Circle* circle)
{
	return shape_create_circle(circle->center, circle->radius);
}

Shape* shape_clone_polygon(const Polygon* polygon)
{
	return shape_create_polygon(polygon->point_count, polygon->points);
}

Shape* shape_clone(const Shape* shape)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			return shape_clone_segment(&shape->segment);
		}
		case SHAPE_TYPE_CIRCLE:
		{
			return shape_clone_circle(&shape->circle);
		}
		case SHAPE_TYPE_POLYGON:
		{
			return shape_clone_polygon(&shape->polygon);
		}
	}

	UNREACHABLE();
}

void shape_destroy(Shape* shape)
{
	free(shape);
}

Vector shape_get_centroid(const Shape* shape)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			return segment_get_centroid(&shape->segment);
		}
		case SHAPE_TYPE_CIRCLE:
		{
			return circle_get_centroid(&shape->circle);
		}
		case SHAPE_TYPE_POLYGON:
		{
			return polygon_get_centroid(&shape->polygon);
		}
	}

	UNREACHABLE();
}

double shape_get_linear_mass_factor(const Shape* shape)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			return segment_get_linear_mass_factor(&shape->segment);
		}
		case SHAPE_TYPE_CIRCLE:
		{
			return circle_get_linear_mass_factor(&shape->circle);
		}
		case SHAPE_TYPE_POLYGON:
		{
			return polygon_get_linear_mass_factor(&shape->polygon);
		}
	}

	UNREACHABLE();
}

double shape_get_angular_mass_factor(const Shape* shape)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			return segment_get_angular_mass_factor(&shape->segment);
		}
		case SHAPE_TYPE_CIRCLE:
		{
			return circle_get_angular_mass_factor(&shape->circle);
		}
		case SHAPE_TYPE_POLYGON:
		{
			return polygon_get_angular_mass_factor(&shape->polygon);
		}
	}

	UNREACHABLE();
}

Rect shape_get_bounding_rect(const Shape* shape)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			return segment_get_bounding_rect(&shape->segment);
		}
		case SHAPE_TYPE_CIRCLE:
		{
			return circle_get_bounding_rect(&shape->circle);
		}
		case SHAPE_TYPE_POLYGON:
		{
			return polygon_get_bounding_rect(&shape->polygon);
		}
	}

	UNREACHABLE();
}

void shape_transform(const Shape* shape, Transform transform, Shape* result)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			segment_transform(&shape->segment, transform, &result->segment);

			return;
		}
		case SHAPE_TYPE_CIRCLE:
		{
			circle_transform(&shape->circle, transform, &result->circle);

			return;
		}
		case SHAPE_TYPE_POLYGON:
		{
			polygon_transform(&shape->polygon, transform, &result->polygon);

			return;
		}
	}

	UNREACHABLE();
}

bool shape_test_point(const Shape* shape, Vector point)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			return segment_test_point(&shape->segment, point);
		}
		case SHAPE_TYPE_CIRCLE:
		{
			return circle_test_point(&shape->circle, point);
		}
		case SHAPE_TYPE_POLYGON:
		{
			return polygon_test_point(&shape->polygon, point);
		}
	}

	UNREACHABLE();
}

Vector shape_project_point(const Shape* shape, Vector point)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			return segment_project_point(&shape->segment, point);
		}
		case SHAPE_TYPE_CIRCLE:
		{
			return circle_project_point(&shape->circle, point);
		}
		case SHAPE_TYPE_POLYGON:
		{
			return polygon_project_point(&shape->polygon, point);
		}
	}

	UNREACHABLE();
}

Vector project_onto_segment(Vector a, Vector b, Vector p)
{
	Vector ab = vector_subtract(b, a);

	double t = vector_dot(ab, vector_subtract(p, a)) / vector_length_squared(ab);

	if (t <= 0.0)
	{
		return a;
	}

	if (t >= 1.0)
	{
		return b;
	}

	return vector_add(a, vector_multiply(ab, t));
}

Vector clamp_onto_segment(Vector a, Vector b, Vector p)
{
	Vector ab = vector_subtract(b, a);

	double t = vector_dot(ab, vector_subtract(p, a)) / vector_length_squared(ab);

	if (t < 0.0)
	{
		return vector_subtract(p, vector_multiply(ab, t));
	}

	if (t > 1.0)
	{
		return vector_subtract(p, vector_multiply(ab, t - 1.0));
	}

	return p;
}

bool test_point_rect(Vector point, const Rect* rect)
{
	return point.x >= rect->min.x && point.y >= rect->min.y && point.x <= rect->max.x && point.y <= rect->max.y;
}

bool collide_shapes(const Shape* shape_1, const Shape* shape_2, Collision* collision)
{
	switch (shape_1->type * SHAPE_TYPE_COUNT + shape_2->type)
	{
		case SHAPE_TYPE_SEGMENT * SHAPE_TYPE_COUNT + SHAPE_TYPE_SEGMENT:
		{
			return collide_segments(&shape_1->segment, &shape_2->segment, collision);
		}
		case SHAPE_TYPE_SEGMENT * SHAPE_TYPE_COUNT + SHAPE_TYPE_CIRCLE:
		{
			return collide_segment_circle(&shape_1->segment, &shape_2->circle, collision);
		}
		case SHAPE_TYPE_SEGMENT * SHAPE_TYPE_COUNT + SHAPE_TYPE_POLYGON:
		{
			return collide_segment_polygon(&shape_1->segment, &shape_2->polygon, collision);
		}
		case SHAPE_TYPE_CIRCLE * SHAPE_TYPE_COUNT + SHAPE_TYPE_SEGMENT:
		{
			return collide_circle_segment(&shape_1->circle, &shape_2->segment, collision);
		}
		case SHAPE_TYPE_CIRCLE * SHAPE_TYPE_COUNT + SHAPE_TYPE_CIRCLE:
		{
			return collide_circles(&shape_1->circle, &shape_2->circle, collision);
		}
		case SHAPE_TYPE_CIRCLE * SHAPE_TYPE_COUNT + SHAPE_TYPE_POLYGON:
		{
			return collide_circle_polygon(&shape_1->circle, &shape_2->polygon, collision);
		}
		case SHAPE_TYPE_POLYGON * SHAPE_TYPE_COUNT + SHAPE_TYPE_SEGMENT:
		{
			return collide_polygon_segment(&shape_1->polygon, &shape_2->segment, collision);
		}
		case SHAPE_TYPE_POLYGON * SHAPE_TYPE_COUNT + SHAPE_TYPE_CIRCLE:
		{
			return collide_polygon_circle(&shape_1->polygon, &shape_2->circle, collision);
		}
		case SHAPE_TYPE_POLYGON * SHAPE_TYPE_COUNT + SHAPE_TYPE_POLYGON:
		{
			return collide_polygons(&shape_1->polygon, &shape_2->polygon, collision);
		}
	}

	UNREACHABLE();
}

bool collide_segments(const Segment* segment_1, const Segment* segment_2, Collision* collision)
{
	UNUSED(segment_1);

	UNUSED(segment_2);

	UNUSED(collision);

	return false;
}

bool collide_circles(const Circle* circle_1, const Circle* circle_2, Collision* collision)
{
	if (vector_distance_squared(circle_1->center, circle_2->center) > square(circle_1->radius + circle_2->radius))
	{
		return false;
	}

	if (vector_equal(circle_1->center, circle_2->center))
	{
		return false;
	}

	collision->point = vector_middle(circle_1->center, circle_2->center);

	collision->normal = vector_normalize(vector_subtract(circle_2->center, circle_1->center));

	collision->depth = circle_1->radius + circle_2->radius - vector_distance(circle_1->center, circle_2->center);

	return true;
}

bool collide_polygons(const Polygon* polygon_1, const Polygon* polygon_2, Collision* collision)
{
	collision->depth = INFINITY;

	Vector point_2 = vector_create(0, 0);

	double depth_2 = 0;

	for (int t = 0; t < 2; t++)
	{
		for (int i = polygon_1->point_count - 1, j = 0; j < polygon_1->point_count; i = j, j++)
		{
			Vector a = polygon_1->points[i];

			Vector b = polygon_1->points[j];

			Vector axis = vector_normalize(vector_right(vector_subtract(b, a)));

			double offset = vector_dot(a, axis);

			double depth_max = -INFINITY;

			double depth_max_2 = -INFINITY;

			Vector deepest_point = vector_create(0, 0);

			Vector deepest_point_2 = vector_create(0, 0);

			for (int k = 0; k < polygon_2->point_count; k++)
			{
				Vector point = polygon_2->points[k];

				double depth = offset - vector_dot(point, axis);

				if (depth > depth_max)
				{
					depth_max_2 = depth_max;

					deepest_point_2 = deepest_point;

					depth_max = depth;

					deepest_point = point;
				}
				else if (depth > depth_max_2)
				{
					depth_max_2 = depth;

					deepest_point_2 = point;
				}
			}

			if (depth_max < 0.0)
			{
				return false;
			}

			if (depth_max < collision->depth)
			{
				collision->depth = depth_max;

				collision->point = deepest_point;

				collision->normal = axis;

				if (depth_max_2 < 0.0)
				{
					depth_2 = 0;
				}
				else
				{
					point_2 = clamp_onto_segment(a, b, deepest_point_2);

					depth_2 = depth_max_2;
				}
			}
		}

		const Polygon* polygon = polygon_1;

		polygon_1 = polygon_2;

		polygon_2 = polygon;

		collision->normal = vector_negate(collision->normal);
	}

	collision->point_2 = point_2;

	collision->depth_2 = depth_2;

	return true;
}

bool collide_segment_circle(const Segment* segment, const Circle* circle, Collision* collision)
{
	Vector point = project_onto_segment(segment->a, segment->b, circle->center);

	if (vector_distance_squared(circle->center, point) <= square(circle->radius))
	{
		collision->point = point;

		collision->normal = vector_normalize(vector_subtract(circle->center, point));

		collision->depth = circle->radius - vector_distance(circle->center, point);

		return true;
	}
	else
	{
		return false;
	}
}

bool collide_segment_polygon(const Segment* segment, const Polygon* polygon, Collision* collision)
{
	Polygon* polygon_of_segment = STACKALLOC(sizeof(Polygon) + 2 * sizeof(Vector));

	polygon_of_segment->point_count = 2;

	polygon_of_segment->points[0] = segment->a;

	polygon_of_segment->points[1] = segment->b;

	return collide_polygons(polygon_of_segment, polygon, collision);
}

bool collide_circle_polygon(const Circle* circle, const Polygon* polygon, Collision* collision)
{
	collision->depth = INFINITY;

	for (int i = polygon->point_count - 1, j = 0; j < polygon->point_count; i = j, j++)
	{
		Vector a = polygon->points[i];

		Vector b = polygon->points[j];

		Vector side = vector_subtract(b, a);

		Vector axis = vector_normalize(vector_left(side));

		double depth = circle->radius + vector_dot(circle->center, axis) - vector_dot(a, axis);

		if (depth < collision->depth)
		{
			if (depth < 0)
			{
				return false;
			}

			collision->depth = depth;

			collision->point = circle->center;

			collision->normal = axis;
		}
	}

	if (collision->depth >= circle->radius)
	{
		return true;
	}

	collision->depth = -INFINITY;

	for (int i = polygon->point_count - 1, j = 0; j < polygon->point_count; i = j, j++)
	{
		Vector a = polygon->points[i];

		Vector b = polygon->points[j];

		Vector point = project_onto_segment(a, b, circle->center);

		Vector axis = vector_normalize(vector_subtract(point, circle->center));

		double depth = circle->radius + vector_dot(circle->center, axis) - vector_dot(point, axis);

		if (depth > collision->depth)
		{
			collision->depth = depth;

			collision->point = point;

			collision->normal = axis;
		}
	}

	if (collision->depth < 0)
	{
		return false;
	}

	return true;
}

bool collide_circle_segment(const Circle* circle, const Segment* segment, Collision* collision)
{
	if (collide_segment_circle(segment, circle, collision))
	{
		collision->normal = vector_negate(collision->normal);

		return true;
	}
	else
	{
		return false;
	}
}

bool collide_polygon_segment(const Polygon* polygon, const Segment* segment, Collision* collision)
{
	if (collide_segment_polygon(segment, polygon, collision))
	{
		collision->normal = vector_negate(collision->normal);

		return true;
	}
	else
	{
		return false;
	}
}

bool collide_polygon_circle(const Polygon* polygon, const Circle* circle, Collision* collision)
{
	if (collide_circle_polygon(circle, polygon, collision))
	{
		collision->normal = vector_negate(collision->normal);

		return true;
	}
	else
	{
		return false;
	}
}
