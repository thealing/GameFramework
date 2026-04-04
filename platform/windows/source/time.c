#include "engine/time.h"

#include "platform.h"

double get_time()
{
	LARGE_INTEGER performance_count = { 0 };

	QueryPerformanceCounter(&performance_count);

	LARGE_INTEGER frequency = { 0 };

	QueryPerformanceFrequency(&frequency);

	return (double)performance_count.QuadPart / (double)frequency.QuadPart;
}

void sleep(double duration)
{
	typedef NTSTATUS (NTAPI* NtSetTimerResolution)(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);

	typedef NTSTATUS (NTAPI* NtDelayExecution)(BOOLEAN Alertable, PLARGE_INTEGER DelayInterval);

	static NtDelayExecution ntDelayExecution;

	if (ntDelayExecution == NULL)
	{
		HMODULE ntdll = GetModuleHandle("ntdll.dll");

		NtSetTimerResolution ntSetTimerResolution = (NtSetTimerResolution)GetProcAddress(ntdll, "NtSetTimerResolution");

		ULONG currentResolution = 0;

		ntSetTimerResolution(0, TRUE, &currentResolution);

		ntDelayExecution = (NtDelayExecution)GetProcAddress(ntdll, "NtDelayExecution");
	}

	if (duration <= 0.0)
	{
		return;
	}

	LARGE_INTEGER large_duration = { 0 };

	large_duration.QuadPart = -10000000 * duration;

	ntDelayExecution(FALSE, &large_duration);
}
