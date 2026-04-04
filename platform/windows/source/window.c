#include "engine/window.h"

#include "platform.h"

#include "resources.h"

static Window_Event_Node* s_event_first;

static Window_Event_Node* s_event_last;

static bool s_window_closed;

static bool s_assets_extracted;

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

static Window_Key map_key(WPARAM wparam)
{
	if (wparam >= 'A' && wparam <= 'Z' || wparam >= '0' && wparam <= '9')
	{
		return (Window_Key)wparam;
	}

	switch (wparam)
	{
		case VK_BACK:
		{
			return WINDOW_KEY_BACKSPACE;
		}
		case VK_ESCAPE:
		{
			return WINDOW_KEY_BACK;
		}
		case VK_RETURN:
		{
			return WINDOW_KEY_ENTER;
		}
		case VK_SPACE:
		{
			return WINDOW_KEY_SPACE;
		}
		case VK_LEFT:
		{
			return WINDOW_KEY_LEFT;
		}
		case VK_RIGHT:
		{
			return WINDOW_KEY_RIGHT;
		}
		case VK_UP:
		{
			return WINDOW_KEY_UP;
		}
		case VK_DOWN:
		{
			return WINDOW_KEY_DOWN;
		}
	}

	return WINDOW_KEY_UNKNOWN;
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Window_Event event = { 0 };

	switch (msg)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wparam) == WA_INACTIVE)
			{
				event.type = WINDOW_EVENT_PAUSED;

				push_event(&event);
			}
			else
			{
				event.type = WINDOW_EVENT_RESUMED;

				push_event(&event);
			}

			break;
		}
		case WM_SIZE:
		{
			if (wparam == SIZE_MINIMIZED)
			{
				event.type = WINDOW_EVENT_PAUSED;

				push_event(&event);
			}

			glViewport(0, 0, LOWORD(lparam), HIWORD(lparam));

			break;
		}
		case WM_LBUTTONDOWN:
		{
			event.type = WINDOW_EVENT_TOUCH_DOWN;

			event.touch_event.x = (double)LOWORD(lparam);

			event.touch_event.y = (double)HIWORD(lparam);

			push_event(&event);

			break;
		}
		case WM_LBUTTONUP:
		{
			event.type = WINDOW_EVENT_TOUCH_UP;

			event.touch_event.x = (double)LOWORD(lparam);

			event.touch_event.y = (double)HIWORD(lparam);

			push_event(&event);

			break;
		}
		case WM_MOUSEMOVE:
		{
			event.type = WINDOW_EVENT_TOUCH_MOVE;

			event.touch_event.x = (double)LOWORD(lparam);

			event.touch_event.y = (double)HIWORD(lparam);

			push_event(&event);

			break;
		}
		case WM_KEYDOWN:
		{
			event.type = WINDOW_EVENT_KEY_DOWN;

			event.key_event.key = map_key(wparam);

			push_event(&event);

			break;
		}
		case WM_KEYUP:
		{
			event.type = WINDOW_EVENT_KEY_UP;

			event.key_event.key = map_key(wparam);

			push_event(&event);

			break;
		}
		case WM_CLOSE:
		{
			event.type = WINDOW_EVENT_WINDOW_DESTROYED;

			event.state_event.window = hwnd;

			push_event(&event);

			s_window_closed = true;

			break;
		}
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void window_create(int width, int height)
{
	if (!s_assets_extracted)
	{
		s_assets_extracted = true;

		extract_assets(config_get_value(CONFIG_KEY_FOLDER_NAME));
	}

	HINSTANCE instance = GetModuleHandle(NULL);

	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = window_proc;

	wc.hInstance = instance;

	wc.lpszClassName = config_get_value(CONFIG_KEY_WINDOW_CLASS);

	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	char* title = config_get_value(CONFIG_KEY_WINDOW_TITLE);

	RECT wr = { 0, 0, width, height };

	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	HWND window = CreateWindowEx(0, wc.lpszClassName, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, instance, NULL);

	Window_Event event = { 0 };

	event.type = WINDOW_EVENT_WINDOW_CREATED;

	event.state_event.window = window;

	push_event(&event);
}

bool window_is_open()
{
	return !s_window_closed;
}

bool window_is_active()
{
	return GetActiveWindow() != NULL;
}

bool window_poll_event(Window_Event* event)
{
	MSG msg = { 0 };

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);

		DispatchMessage(&msg);
	}

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

	return result;
}

int window_get_width(void* window)
{
	RECT rect = { 0 };

	GetClientRect(window, &rect);

	return rect.right - rect.left;
}

int window_get_height(void* window)
{
	RECT rect = { 0 };

	GetClientRect(window, &rect);

	return rect.bottom - rect.top;
}

int main();

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show_mode)
{
	UNUSED(instance);

	UNUSED(previous_instance);

	UNUSED(command_line);

	UNUSED(show_mode);

	return main();
}
