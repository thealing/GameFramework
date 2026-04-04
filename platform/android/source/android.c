#include "engine/window.h"

#include "platform.h"

static bool s_started;

static pthread_t s_thread;

static pthread_mutex_t s_mutex;

static AInputQueue* s_input_queue;

static Window_Event_Node* s_event_first;

static Window_Event_Node* s_event_last;

static bool s_window_active;

static float s_prev_touch_dist;

static void push_event(Window_Event* event)
{
	if (s_event_last == NULL) 
	{
		s_event_first = HEAPALLOC(sizeof(Window_Event_Node));

		s_event_last = s_event_first;
	}
	else 
	{
		s_event_last->next = HEAPALLOC(sizeof(Window_Event_Node));

		s_event_last = s_event_last->next;
	}

	s_event_last->event = *event;

	s_event_last->next = NULL;
}

static void collect_input_events()
{
	if (s_input_queue == NULL) 
	{
		return;
	}

	while (AInputQueue_hasEvents(s_input_queue) > 0) 
	{
		AInputEvent* input_event;

		AInputQueue_getEvent(s_input_queue, &input_event);

		bool handled = true;

		switch (AInputEvent_getType(input_event)) 
		{
			case AINPUT_EVENT_TYPE_MOTION: 
			{
				size_t pointer_count = AMotionEvent_getPointerCount(input_event);

				if (pointer_count == 2 && config_get_value(CONFIG_KEY_TOUCH_ZOOM_ENABLED) != NULL)
				{
					float x1 = AMotionEvent_getX(input_event, 0);

					float y1 = AMotionEvent_getY(input_event, 0);

					float x2 = AMotionEvent_getX(input_event, 1);

					float y2 = AMotionEvent_getY(input_event, 1);

					float dx = x1 - x2;

					float dy = y1 - y2;

					float current_dist = sqrtf(dx * dx + dy * dy);

					if (s_prev_touch_dist > 0)
					{
						float delta = (current_dist - s_prev_touch_dist) * 0.01f;

						if (fabsf(delta) > 0.001f)
						{
							Window_Event zoom_event = { 0 };

							zoom_event.type = WINDOW_EVENT_ZOOM;

							zoom_event.zoom_event.amount = (double)delta;

							push_event(&zoom_event);
						}
					}

					s_prev_touch_dist = current_dist;

					break;
				}
				else
				{
					s_prev_touch_dist = -1.0f;
				}

				int action = AMotionEvent_getAction(input_event) & AMOTION_EVENT_ACTION_MASK;

				Window_Event_Type type = WINDOW_EVENT_UNKNOWN;

				switch (action)
				{
					case AMOTION_EVENT_ACTION_DOWN:
					case AMOTION_EVENT_ACTION_POINTER_DOWN:
					case AMOTION_EVENT_ACTION_BUTTON_PRESS:
					{
						type = WINDOW_EVENT_TOUCH_DOWN;

						break;
					}
					case AMOTION_EVENT_ACTION_UP:
					case AMOTION_EVENT_ACTION_POINTER_UP:
					case AMOTION_EVENT_ACTION_BUTTON_RELEASE:
					case AMOTION_EVENT_ACTION_CANCEL:
					{
						type = WINDOW_EVENT_TOUCH_UP;

						break;
					}
					case AMOTION_EVENT_ACTION_MOVE:
					{
						type = WINDOW_EVENT_TOUCH_MOVE;

						break;
					}
				}

				switch (type)
				{
					case WINDOW_EVENT_TOUCH_DOWN:
					case WINDOW_EVENT_TOUCH_UP:
					{
						int pointer = AMotionEvent_getAction(input_event) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

						int index = AMotionEvent_getPointerId(input_event, pointer);

						double x = AMotionEvent_getX(input_event, pointer);

						double y = AMotionEvent_getY(input_event, pointer);

						push_event(&(Window_Event){ .type = type, .touch_event = { .index = index, .x = x, .y = y } });

						break;
					}
					case WINDOW_EVENT_TOUCH_MOVE:
					{
						int pointer_count = AMotionEvent_getPointerCount(input_event);

						for (int pointer = 0; pointer < pointer_count; pointer++)
						{
							int index = AMotionEvent_getPointerId(input_event, pointer);

							double x = AMotionEvent_getX(input_event, pointer);

							double y = AMotionEvent_getY(input_event, pointer);

							push_event(&(Window_Event){ .type = type, .touch_event = { .index = index, .x = x, .y = y } });
						}

						break;
					}
				}

				break;
			}
			case AINPUT_EVENT_TYPE_KEY:
			{
				int action = AKeyEvent_getAction(input_event);

				Window_Event_Type type = WINDOW_EVENT_UNKNOWN;

				switch (action)
				{
					case AKEY_EVENT_ACTION_DOWN:
					{
						type = WINDOW_EVENT_KEY_DOWN;

						break;
					}
					case AKEY_EVENT_ACTION_UP:
					{
						type = WINDOW_EVENT_KEY_UP;

						break;
					}
				}

				int key_code = AKeyEvent_getKeyCode(input_event);

				Window_Key key = WINDOW_KEY_UNKNOWN;

				switch (key_code)
				{
					case AKEYCODE_VOLUME_DOWN:
					case AKEYCODE_VOLUME_UP:
					{
						handled = false;

						break;
					}
					case AKEYCODE_BACK:
					{
						key = WINDOW_KEY_BACK;

						break;
					}
					case AKEYCODE_ENTER:
					case AKEYCODE_DPAD_CENTER:
					{
						key = WINDOW_KEY_ENTER;

						break;
					}
					case AKEYCODE_SPACE:
					{
						key = WINDOW_KEY_SPACE;

						break;
					}
					case AKEYCODE_DEL:
					{
						key = WINDOW_KEY_BACKSPACE;

						break;
					}
					case AKEYCODE_DPAD_LEFT:
					{
						key = WINDOW_KEY_LEFT;

						break;
					}
					case AKEYCODE_DPAD_RIGHT:
					{
						key = WINDOW_KEY_RIGHT;

						break;
					}
					case AKEYCODE_DPAD_UP:
					{
						key = WINDOW_KEY_UP;

						break;
					}
					case AKEYCODE_DPAD_DOWN:
					{
						key = WINDOW_KEY_DOWN;

						break;
					}
					default:
					{
						if (key_code >= AKEYCODE_0 && key_code <= AKEYCODE_9)
						{
							key = (Window_Key)('0' + key_code - AKEYCODE_0);
						}

						if (key_code >= AKEYCODE_A && key_code <= AKEYCODE_Z)
						{
							key = (Window_Key)('A' + key_code - AKEYCODE_A);
						}
					}
				}

				push_event(&(Window_Event){ .type = type, .key_event = { .key = key } });

				break;
			}
		}

		AInputQueue_finishEvent(s_input_queue, input_event, handled);
	}
}

static void on_destroy(ANativeActivity* activity)
{
	pthread_mutex_lock(&s_mutex);

	push_event(&(Window_Event){ .type = WINDOW_EVENT_PAUSED });

	pthread_mutex_unlock(&s_mutex);
}

static void on_pause(ANativeActivity* activity)
{
	pthread_mutex_lock(&s_mutex);

	push_event(&(Window_Event){ .type = WINDOW_EVENT_PAUSED });

	s_window_active = false;

	pthread_mutex_unlock(&s_mutex);
}

static void on_resume(ANativeActivity* activity)
{
	pthread_mutex_lock(&s_mutex);

	push_event(&(Window_Event){ .type = WINDOW_EVENT_RESUMED });

	s_window_active = true;

	pthread_mutex_unlock(&s_mutex);
}

static void on_native_window_created(ANativeActivity* activity, ANativeWindow* window)
{
	pthread_mutex_lock(&s_mutex);

	push_event(&(Window_Event){ .type = WINDOW_EVENT_WINDOW_CREATED, .state_event = { .window = window } });

	s_window_active = true;

	pthread_mutex_unlock(&s_mutex);
}

static void on_native_window_destroyed(ANativeActivity* activity, ANativeWindow* window)
{
	pthread_mutex_lock(&s_mutex);

	push_event(&(Window_Event){ .type = WINDOW_EVENT_WINDOW_DESTROYED, .state_event = { .window = window } });

	pthread_mutex_unlock(&s_mutex);
}

static void on_input_queue_created(ANativeActivity* activity, AInputQueue* queue)
{
	pthread_mutex_lock(&s_mutex);

	s_input_queue = queue;

	pthread_mutex_unlock(&s_mutex);
}

static void on_input_queue_destroyed(ANativeActivity* activity, AInputQueue* queue)
{
	pthread_mutex_lock(&s_mutex);

	s_input_queue = NULL;

	pthread_mutex_unlock(&s_mutex);
}

int main();

static void* entry(void* context)
{
	ANativeActivity* activity = (ANativeActivity*)context;

	platform_init(activity);

	main();

	return NULL;
}

__attribute__((visibility("default"))) void ANativeActivity_onCreate(ANativeActivity* activity, void* saved_state, size_t saved_state_size)
{
	activity->callbacks->onDestroy = on_destroy;

	activity->callbacks->onPause = on_pause;

	activity->callbacks->onResume = on_resume;

	activity->callbacks->onNativeWindowCreated = on_native_window_created;

	activity->callbacks->onNativeWindowDestroyed = on_native_window_destroyed;

	activity->callbacks->onInputQueueCreated = on_input_queue_created;

	activity->callbacks->onInputQueueDestroyed = on_input_queue_destroyed;

	if (!s_started)
	{
		s_started = true;

		pthread_mutex_init(&s_mutex, NULL);

		pthread_create(&s_thread, NULL, entry, activity);
	}
}

bool window_poll_event(Window_Event* event)
{
	pthread_mutex_lock(&s_mutex);

	collect_input_events();

	bool result = s_event_first != NULL;

	if (result) 
	{
		*event = s_event_first->event;

		Window_Event_Node* next = s_event_first->next;

		free(s_event_first);

		if (next == NULL)
		{
			s_event_first = NULL;

			s_event_last = NULL;
		}
		else
		{
			s_event_first = next;
		}
	}

	pthread_mutex_unlock(&s_mutex);

	return result;
}

void window_create(int width, int height)
{
}

bool window_is_open()
{
	return true;
}

bool window_is_active()
{
	return s_window_active;
}

int window_get_width(void* window)
{
	return ANativeWindow_getWidth(window);
}

int window_get_height(void* window)
{
	return ANativeWindow_getHeight(window);
}
