#pragma once

#include "math.h"

#include "macros.h"

#include <stdbool.h>

#include <stdlib.h>

#include <string.h>

typedef enum Shape_Type Shape_Type;

typedef struct Vector Vector;

typedef struct Transform Transform;

typedef struct Segment Segment;

typedef struct Circle Circle;

typedef struct Polygon Polygon;

typedef struct Shape Shape;

typedef struct Rect Rect;

typedef struct Collision Collision;

enum Shape_Type
{
	SHAPE_TYPE_SEGMENT,

	SHAPE_TYPE_CIRCLE,

	SHAPE_TYPE_POLYGON,

	SHAPE_TYPE_COUNT
};

struct Vector
{
	double x;

	double y;
};

struct Transform
{
	double x;

	double y;

	double s;

	double c;
};

struct Segment
{
	Vector a;

	Vector b;
};

struct Circle
{
	Vector center;

	double radius;
};

struct Polygon
{
	int point_count;

	Vector points[];
};

struct Shape
{
	Shape_Type type;

	union
	{
		Segment segment;

		Circle circle;

		Polygon polygon;
	};
};

struct Rect
{
	Vector min;

	Vector max;
};

struct Collision
{
	Vector point;

	Vector normal;

	double depth;

	Vector point_2;

	double depth_2;
};

Vector vector_create(double x, double y);

bool vector_equal(Vector v, Vector w);

Vector vector_negate(Vector v);

Vector vector_add(Vector v, Vector w);

Vector vector_add_value(Vector v, double x);

Vector vector_add_xy(Vector v, double x, double y);

Vector vector_subtract(Vector v, Vector w);

Vector vector_subtract_value(Vector v, double x);

Vector vector_subtract_xy(Vector v, double x, double y);

Vector vector_multiply(Vector v, double s);

Vector vector_divide(Vector v, double s);

Vector vector_scale(Vector v, Vector s);

Vector vector_downscale(Vector v, Vector s);

Vector vector_rotate(Vector v, double a);

Vector vector_left(Vector v);

Vector vector_right(Vector v);

Vector vector_normalize(Vector v);

double vector_length(Vector v);

double vector_length_squared(Vector v);

double vector_distance(Vector v, Vector w);

double vector_distance_squared(Vector v, Vector w);

double vector_dot(Vector v, Vector w);

double vector_cross(Vector v, Vector w);

Vector vector_middle(Vector v, Vector w);

Transform transform_create(Vector translation, double rotation);

Transform transform_invert(Transform transform);

Vector transform_apply(Transform transform, Vector vector);

Vector segment_get_centroid(const Segment* segment);

double segment_get_linear_mass_factor(const Segment* segment);

double segment_get_angular_mass_factor(const Segment* segment);

Rect segment_get_bounding_rect(const Segment* segment);

void segment_transform(const Segment* segment, Transform transform, Segment* result);

bool segment_test_point(const Segment* segment, Vector point);

Vector segment_project_point(const Segment* segment, Vector point);

Vector circle_get_centroid(const Circle* circle);

double circle_get_linear_mass_factor(const Circle* circle);

double circle_get_angular_mass_factor(const Circle* circle);

Rect circle_get_bounding_rect(const Circle* circle);

void circle_transform(const Circle* circle, Transform transform, Circle* result);

bool circle_test_point(const Circle* circle, Vector point);

Vector circle_project_point(const Circle* circle, Vector point);

Vector polygon_get_centroid(const Polygon* polygon);

double polygon_get_linear_mass_factor(const Polygon* polygon);

double polygon_get_angular_mass_factor(const Polygon* polygon);

Rect polygon_get_bounding_rect(const Polygon* polygon);

void polygon_transform(const Polygon* polygon, Transform transform, Polygon* result);

bool polygon_test_point(const Polygon* polygon, Vector point);

Vector polygon_project_point(const Polygon* polygon, Vector point);

Shape* shape_create_segment(Vector a, Vector b);

Shape* shape_create_circle(Vector center, double radius);

Shape* shape_create_polygon(int point_count, const Vector points[]);

Shape* shape_clone_segment(const Segment* segment);

Shape* shape_clone_circle(const Circle* circle);

Shape* shape_clone_polygon(const Polygon* polygon);

Shape* shape_clone(const Shape* shape);

void shape_destroy(Shape* shape);

Vector shape_get_centroid(const Shape* shape);

double shape_get_linear_mass_factor(const Shape* shape);

double shape_get_angular_mass_factor(const Shape* shape);

Rect shape_get_bounding_rect(const Shape* shape);

void shape_transform(const Shape* shape, Transform transform, Shape* result);

bool shape_test_point(const Shape* shape, Vector point);

Vector shape_project_point(const Shape* shape, Vector point);

Vector project_onto_segment(Vector a, Vector b, Vector p);

Vector clamp_onto_segment(Vector a, Vector b, Vector p);

bool test_point_rect(Vector point, const Rect* rect);

bool collide_shapes(const Shape* shape_1, const Shape* shape_2, Collision* collision);

bool collide_segments(const Segment* segment_1, const Segment* segment_2, Collision* collision);

bool collide_circles(const Circle* circle_1, const Circle* circle_2, Collision* collision);

bool collide_polygons(const Polygon* polygon_1, const Polygon* polygon_2, Collision* collision);

bool collide_segment_circle(const Segment* segment, const Circle* circle, Collision* collision);

bool collide_segment_polygon(const Segment* segment, const Polygon* polygon, Collision* collision);

bool collide_circle_polygon(const Circle* circle, const Polygon* polygon, Collision* collision);

bool collide_circle_segment(const Circle* circle, const Segment* segment, Collision* collision);

bool collide_polygon_segment(const Polygon* polygon, const Segment* segment, Collision* collision);

bool collide_polygon_circle(const Polygon* polygon, const Circle* circle, Collision* collision);
