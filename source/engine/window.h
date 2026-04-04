#pragma once

#include "config.h"

#include <stdbool.h>

typedef enum Window_Event_Type Window_Event_Type;

typedef enum Window_Key Window_Key;

typedef struct Window_Activity_Event Window_Activity_Event;

typedef struct Window_State_Event Window_State_Event;

typedef struct Window_Touch_Event Window_Touch_Event;

typedef struct Window_Key_Event Window_Key_Event;

typedef struct Window_Event Window_Event;

typedef struct Window_Event_Node Window_Event_Node;

enum Window_Event_Type 
{
	WINDOW_EVENT_UNKNOWN,

	WINDOW_EVENT_RESUMED,

	WINDOW_EVENT_PAUSED,

	WINDOW_EVENT_WINDOW_CREATED,

	WINDOW_EVENT_WINDOW_DESTROYED,

	WINDOW_EVENT_TOUCH_DOWN,

	WINDOW_EVENT_TOUCH_UP,

	WINDOW_EVENT_TOUCH_MOVE,

	WINDOW_EVENT_KEY_DOWN,

	WINDOW_EVENT_KEY_UP,

	WINDOW_EVENT_COUNT
};

enum Window_Key
{
	WINDOW_KEY_UNKNOWN,

	WINDOW_KEY_BACK,

	WINDOW_KEY_ENTER,

	WINDOW_KEY_SPACE,

	WINDOW_KEY_BACKSPACE,

	WINDOW_KEY_LEFT,

	WINDOW_KEY_RIGHT,

	WINDOW_KEY_UP,

	WINDOW_KEY_DOWN,

	WINDOW_KEY_MAX = 256
};

struct Window_State_Event
{
	void* window;
};

struct Window_Touch_Event
{
	int index;

	double x;

	double y;
};

struct Window_Key_Event
{
	Window_Key key;
};

struct Window_Event
{
	Window_Event_Type type;

	union
	{
		Window_State_Event state_event;

		Window_Touch_Event touch_event;

		Window_Key_Event key_event;
	};
};

struct Window_Event_Node
{
	Window_Event event;

	Window_Event_Node* next;
};

void window_create(int width, int height);

bool window_is_open();

bool window_is_active();

int window_get_width(void* window);

int window_get_height(void* window);

bool window_poll_event(Window_Event* event);
