#include "engine/graphics.h"

#include "platform.h"

#include <math.h>

#include <stdlib.h>

#include <string.h>

#include <stdarg.h>

static EGLDisplay s_display;

static EGLSurface s_surface;

static EGLContext s_context;

static State s_state;

static State s_state_history[GRAPHICS_HISTORY_SIZE];

static int s_state_history_index;

static Rect s_camera;

void texture_create_from_image(Texture** texture_pointer, const Image* image)
{
	if (*texture_pointer == NULL)
	{
		*texture_pointer = HEAPALLOC(sizeof(Texture));
	}

	Texture* texture = *texture_pointer;

	texture->size = vector_create(image->width, image->height);

	texture->center = vector_create(image->width / 2.0, image->height / 2.0);

	texture->scale = vector_create(1.0, 1.0);

	glGenTextures(1, &texture->id);

	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_create_from_file(Texture** texture_pointer, const char* path)
{
	Image* image = image_load(path);

	texture_create_from_image(texture_pointer, image);

	image_destroy(image);
}

void texture_destroy(Texture* texture)
{
	glDeleteTextures(1, &texture->id);

	free(texture);
}

void texture_scale(Texture* texture, Vector scale)
{
	texture->size = vector_scale(texture->size, scale);

	texture->center = vector_scale(texture->center, scale);
}

void texture_resize(Texture* texture, Vector size)
{
	texture->center = vector_scale(texture->center, vector_downscale(size, texture->size));

	texture->size = size;
}

void texture_align(Texture* texture, Alignment horizontal_alignment, Alignment vertical_alignment)
{
	switch (horizontal_alignment)
	{
		case ALIGNMENT_LEFT:
		{
			texture->center.x = 0;

			break;
		}
		case ALIGNMENT_RIGHT:
		{
			texture->center.x = texture->size.x;

			break;
		}
		case ALIGNMENT_CENTER:
		{
			texture->center.x = texture->size.x / 2;

			break;
		}
	}

	switch (vertical_alignment)
	{
		case ALIGNMENT_BOTTOM:
		{
			texture->center.y = 0;

			break;
		}
		case ALIGNMENT_TOP:
		{
			texture->center.y = texture->size.y;

			break;
		}
		case ALIGNMENT_CENTER:
		{
			texture->center.y = texture->size.y / 2;

			break;
		}
	}
}

void graphics_init(void* window)
{
	s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	EGLint major = 0;
	
	EGLint minor = 0;

	eglInitialize(s_display, &major, &minor);

	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_NONE,
	};

	EGLConfig config = 0;

	int num_config = 0;

	eglChooseConfig(s_display, attribs, &config, 1, &num_config);

	s_surface = eglCreateWindowSurface(s_display, config, window, NULL);

	s_context = eglCreateContext(s_display, config, NULL, NULL);

	eglMakeCurrent(s_display, s_surface, s_surface, s_context);

	glEnable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnableClientState(GL_VERTEX_ARRAY);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void graphics_uninit(void* window)
{
	eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	eglDestroyContext(s_display, s_context);

	eglDestroySurface(s_display, s_surface);

	eglTerminate(s_display);

	s_display = EGL_NO_DISPLAY;

	s_context = EGL_NO_CONTEXT;

	s_surface = EGL_NO_SURFACE;
}

void graphics_display()
{
	eglSwapBuffers(s_display, s_surface);
}

void graphics_clear(const Color* color)
{
	glClearColor(color->red, color->green, color->blue, color->alpha);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void graphics_unproject(Vector* point)
{
	*point = vector_add(s_camera.min, vector_scale(vector_subtract(s_camera.max, s_camera.min), *point));
}

void graphics_set_camera(const Rect* rect)
{
	s_camera = *rect;

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glOrthof(rect->min.x, rect->max.x, rect->min.y, rect->max.y, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
}

void graphics_set_color(const Color* color)
{
	s_state.color = *color;

	glColor4f(color->red, color->green, color->blue, color->alpha);
}

void graphics_set_texture(const Texture* texture)
{
	if (s_state.texture == texture) 
	{
		return;
	}

	s_state.texture = texture;

	if (texture != NULL)
	{
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}
	else 
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void graphics_set_font(const Texture* font)
{
	s_state.font = font;
}

void graphics_set_line_width(double line_width)
{
	s_state.line_width = line_width;
}

void graphics_set_line_cap(Line_Cap line_cap)
{
	s_state.line_cap = line_cap;
}

void graphics_save_state()
{
	s_state_history[s_state_history_index] = s_state;

	s_state_history_index++;
}

void graphics_load_state()
{
	s_state_history_index--;

	State state = s_state_history[s_state_history_index];

	graphics_set_color(&state.color);

	graphics_set_texture(state.texture);

	graphics_set_font(state.font);

	graphics_set_line_width(state.line_width);

	graphics_set_line_cap(state.line_cap);
}

void graphics_save_transform()
{
	glPushMatrix();
}

void graphics_load_transform()
{
	glPopMatrix();
}

void graphics_translate(Vector translation)
{
	glTranslatef(translation.x, translation.y, 0.0);
}

void graphics_rotate(double angle)
{
	glRotatef(angle * 180.0 / M_PI, 0.0, 0.0, 1.0);
}

void graphics_scale(Vector factor)
{
	glScalef(factor.x, factor.y, 0.0);
}

void graphics_scale_uniformly(double factor)
{
	glScalef(factor, factor, 1.0);
}

void graphics_draw_segment(const Segment* segment, bool fill)
{
	GLfloat vertices[2][2] = { 
		{ segment->a.x, segment->a.y },
		{ segment->b.x, segment->b.y },
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glDrawArrays(GL_LINES, 0, 2);
}

void graphics_draw_circle(const Circle* circle, bool fill)
{
	GLfloat vertices[GRAPHICS_CIRCLE_RESOLUTION + 2][2] = { 0 };

	vertices[0][0] = circle->center.x;

	vertices[0][1] = circle->center.y;

	for (int i = 0; i <= GRAPHICS_CIRCLE_RESOLUTION; i++) 
	{
		double angle = 2.0 * M_PI * i / GRAPHICS_CIRCLE_RESOLUTION;

		vertices[i + 1][0] = circle->center.x + circle->radius * cos(angle);

		vertices[i + 1][1] = circle->center.y + circle->radius * sin(angle);
	}

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glDrawArrays(fill ? GL_TRIANGLE_FAN : GL_LINE_STRIP, 0, GRAPHICS_CIRCLE_RESOLUTION + 2);
}

void graphics_draw_polygon(const Polygon* polygon, bool fill)
{
	GLfloat vertices[polygon->point_count][2];

	for (int i = 0; i < polygon->point_count; i++) 
	{
		vertices[i][0] = polygon->points[i].x;

		vertices[i][1] = polygon->points[i].y;
	}

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glDrawArrays(fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, polygon->point_count);
}

void graphics_draw_shape(const Shape* shape, bool fill)
{
	switch (shape->type) 
	{
		case SHAPE_TYPE_SEGMENT: 
		{
			graphics_draw_segment(&shape->segment, fill);

			break;
		}
		case SHAPE_TYPE_CIRCLE: 
		{
			graphics_draw_circle(&shape->circle, fill);

			break;
		}
		case SHAPE_TYPE_POLYGON: 
		{
			graphics_draw_polygon(&shape->polygon, fill);

			break;
		}
	}
}

void graphics_draw_rect(const Rect* rect, bool fill)
{
	GLfloat vertices[4][2] = {
		{ rect->min.x, rect->min.y },
		{ rect->max.x, rect->min.y },
		{ rect->max.x, rect->max.y },
		{ rect->min.x, rect->max.y },
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glDrawArrays(fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);
}

void graphics_draw_quad(const Vector quad[4], bool fill)
{
	GLfloat vertices[4][2] = {
		{ quad[0].x, quad[0].y },
		{ quad[1].x, quad[1].y },
		{ quad[2].x, quad[2].y },
		{ quad[3].x, quad[3].y },
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glDrawArrays(fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);
}

void graphics_draw_texture()
{
	if (s_state.texture == NULL)
	{
		return;
	}

	Vector center = s_state.texture->center;

	Vector size = s_state.texture->size;

	GLfloat vertices[] = {
		-center.x, -center.y,
		-center.x + size.x, -center.y,
		-center.x + size.x, -center.y + size.y,
		-center.x, -center.y + size.y
	};

	GLfloat texture_coords[] = {
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glTexCoordPointer(2, GL_FLOAT, 0, texture_coords);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void graphics_draw_texture_at(Vector position, double rotation)
{
	if (s_state.texture == NULL)
	{
		return;
	}

	graphics_save_transform();

	graphics_translate(position);

	graphics_rotate(rotation);

	graphics_draw_texture();

	graphics_load_transform();
}

void graphics_draw_texture_in_rect(const Rect* rect)
{
	if (s_state.texture == NULL)
	{
		return;
	}

	GLfloat vertices[] = {
		rect->min.x, rect->min.y,
		rect->max.x, rect->min.y,
		rect->max.x, rect->max.y,
		rect->min.x, rect->max.y,
	};

	GLfloat texture_coords[] = {
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glTexCoordPointer(2, GL_FLOAT, 0, texture_coords);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void graphics_draw_texture_in_quad(const Vector quad[4])
{
	if (s_state.texture == NULL)
	{
		return;
	}

	GLfloat vertices[] = {
		quad[0].x, quad[0].y,
		quad[1].x, quad[1].y,
		quad[2].x, quad[2].y,
		quad[3].x, quad[3].y,
	};

	GLfloat texture_coords[] = {
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glTexCoordPointer(2, GL_FLOAT, 0, texture_coords);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void graphics_draw_texture_rect_in_rect(const Rect* texture_rect, const Rect* rect)
{
	if (s_state.texture == NULL)
	{
		return;
	}

	GLfloat vertices[] = {
		rect->min.x, rect->min.y,
		rect->max.x, rect->min.y,
		rect->max.x, rect->max.y,
		rect->min.x, rect->max.y,
	};

	GLfloat texture_coords[] = {
		texture_rect->min.x, texture_rect->min.y,
		texture_rect->max.x, texture_rect->min.y,
		texture_rect->max.x, texture_rect->max.y,
		texture_rect->min.x, texture_rect->max.y,
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glTexCoordPointer(2, GL_FLOAT, 0, texture_coords);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void graphics_draw_texture_rect_in_quad(const Rect* texture_rect, const Vector quad[4])
{
	if (s_state.texture == NULL)
	{
		return;
	}

	GLfloat vertices[] = {
		quad[0].x, quad[0].y,
		quad[1].x, quad[1].y,
		quad[2].x, quad[2].y,
		quad[3].x, quad[3].y,
	};

	GLfloat texture_coords[] = {
		texture_rect->min.x, texture_rect->min.y,
		texture_rect->max.x, texture_rect->min.y,
		texture_rect->max.x, texture_rect->max.y,
		texture_rect->min.x, texture_rect->max.y,
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glTexCoordPointer(2, GL_FLOAT, 0, texture_coords);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void graphics_draw_texture_quad_in_quad(const Vector texture_quad[4], const Vector quad[4])
{
	if (s_state.texture == NULL)
	{
		return;
	}

	GLfloat vertices[] = {
		quad[0].x, quad[0].y,
		quad[1].x, quad[1].y,
		quad[2].x, quad[2].y,
		quad[3].x, quad[3].y,
	};

	GLfloat texture_coords[] = {
		texture_quad[0].x, texture_quad[0].y,
		texture_quad[1].x, texture_quad[1].y,
		texture_quad[2].x, texture_quad[2].y,
		texture_quad[3].x, texture_quad[3].y,
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);

	glTexCoordPointer(2, GL_FLOAT, 0, texture_coords);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void graphics_draw_character_in_rect(const Rect* rect, char character)
{
	if (s_state.font == NULL)
	{
		return;
	}

	graphics_set_texture(s_state.font);

	graphics_draw_texture_rect_in_rect(&(Rect){ character / 128.0, 0.0, (character + 1) / 128.0, 1.0 }, rect);
}

void graphics_draw_string_in_rect(const Rect* rect, Alignment alignment, const char* string)
{
	if (s_state.font == NULL)
	{
		return;
	}

	int string_length = strlen(string);

	double character_width = (rect->max.y - rect->min.y) / s_state.font->size.y * (s_state.font->size.x / 128.0);

	switch (alignment)
	{
		case ALIGNMENT_LEFT:
		{
			for (int i = 0; i < string_length; i++)
			{
				graphics_draw_character_in_rect(&(Rect){ { rect->min.x + character_width * i, rect->min.y }, { rect->min.x + character_width * (i + 1), rect->max.y } }, string[i]);
			}

			break;
		}
		case ALIGNMENT_RIGHT:
		{
			for (int i = 0; i < string_length; i++)
			{
				graphics_draw_character_in_rect(&(Rect){ { rect->max.x - character_width * (string_length - i), rect->min.y }, { rect->max.x - character_width * (string_length - i - 1), rect->max.y } }, string[i]);
			}

			break;
		}
		case ALIGNMENT_CENTER:
		{
			double start_x = (rect->min.x + rect->max.x) / 2.0 - character_width * string_length / 2.0;

			for (int i = 0; i < string_length; i++)
			{
				graphics_draw_character_in_rect(&(Rect){ { start_x + character_width * i, rect->min.y }, { start_x + character_width * (i + 1), rect->max.y } }, string[i]);
			}

			break;
		}
	}
}

void graphics_draw_format_in_rect(const Rect* rect, Alignment alignment, const char* format, ...)
{
	if (s_state.font == NULL)
	{
		return;
	}

	char string[GRAPHICS_STRING_BUFFER_SIZE] = { 0 };

	va_list args;

	va_start(args, format);

	vsprintf(string, format, args);

	va_end(args);

	graphics_draw_string_in_rect(rect, alignment, string);
}

void graphics_draw_string(Alignment horizontal_alignment, Alignment vertical_alignment, const char* string)
{
	if (s_state.font == NULL)
	{
		return;
	}

	int string_length = strlen(string);

	double character_width = (s_state.font->size.x / 128.0) / s_state.font->size.y;

	double start_x = 0.0;

	double lower_y = 0.0;

	double upper_y = 0.0;

	switch (horizontal_alignment)
	{
		case ALIGNMENT_LEFT:
		{
			start_x = 0.0;

			break;
		}
		case ALIGNMENT_RIGHT:
		{
			start_x = character_width * -string_length;

			break;
		}
		case ALIGNMENT_CENTER:
		{
			start_x = character_width * -string_length / 2.0;

			break;
		}
	}

	switch (vertical_alignment)
	{
		case ALIGNMENT_BOTTOM:
		{
			lower_y = 0.0;

			upper_y = 1.0;

			break;
		}
		case ALIGNMENT_TOP:
		{
			lower_y = -1.0;

			upper_y = 0.0;

			break;
		}
		case ALIGNMENT_CENTER:
		{
			lower_y = -0.5;

			upper_y = 0.5;

			break;
		}
	}

	for (int i = 0; i < string_length; i++)
	{
		graphics_draw_character_in_rect(&(Rect){ { start_x + character_width * i, lower_y }, { start_x + character_width * (i + 1), upper_y } }, string[i]);
	}
}

void graphics_draw_format(Alignment horizontal_alignment, Alignment vertical_alignment, const char* format, ...)
{
	if (s_state.font == NULL)
	{
		return;
	}

	char string[GRAPHICS_STRING_BUFFER_SIZE] = { 0 };

	va_list args;

	va_start(args, format);

	vsprintf(string, format, args);

	va_end(args);

	graphics_draw_string(horizontal_alignment, vertical_alignment, string);
}

void graphics_draw_line(Vector start, Vector end)
{
	double radius = s_state.line_width;

	Vector direction = vector_multiply(vector_normalize(vector_subtract(end, start)), radius);

	Vector normal = vector_left(direction);

	if (s_state.line_cap == LINE_CAP_SQUARE)
	{
		start = vector_subtract(start, direction);

		end = vector_add(end, direction);
	}

	Vector a = vector_subtract(start, normal);

	Vector b = vector_subtract(end, normal);

	Vector c = vector_add(end, normal);

	Vector d = vector_add(start, normal);

	graphics_draw_quad((Vector[4]){ a, b, c, d }, true);

	if (s_state.line_cap == LINE_CAP_ROUND)
	{
		graphics_draw_circle(&(Circle){ start, radius }, true);

		graphics_draw_circle(&(Circle){ end, radius }, true);
	}
}
