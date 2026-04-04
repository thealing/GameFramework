#include "engine/sound.h"

#include "platform.h"

struct Sound
{
	IGraphBuilder* graph;

	IMediaControl* control;

	IMediaEvent* event;

	IMediaSeeking* seeking;

	IBasicAudio* audio;
};

Sound* sound_load(const char* path)
{
	CoInitialize(NULL);

	Sound* sound = (Sound*)HEAPALLOC(sizeof(Sound));

	CoCreateInstance(&CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, &IID_IGraphBuilder, (void**)&sound->graph);

	sound->graph->lpVtbl->QueryInterface(sound->graph, &IID_IMediaControl, (void**)&sound->control);

	sound->graph->lpVtbl->QueryInterface(sound->graph, &IID_IMediaEvent, (void**)&sound->event);

	sound->graph->lpVtbl->QueryInterface(sound->graph, &IID_IMediaSeeking, (void**)&sound->seeking);

	sound->graph->lpVtbl->QueryInterface(sound->graph, &IID_IBasicAudio, (void**)&sound->audio);

	wchar_t wide_path[MAX_PATH] = { 0 };

	MultiByteToWideChar(CP_ACP, 0, path, -1, wide_path, MAX_PATH);

	sound->graph->lpVtbl->RenderFile(sound->graph, wide_path, NULL);

	return sound;
}

void sound_destroy(Sound* sound)
{
	sound->control->lpVtbl->Stop(sound->control);

	sound->audio->lpVtbl->Release(sound->audio);

	sound->seeking->lpVtbl->Release(sound->seeking);

	sound->event->lpVtbl->Release(sound->event);

	sound->control->lpVtbl->Release(sound->control);

	sound->graph->lpVtbl->Release(sound->graph);

	free(sound);

	CoUninitialize();
}

bool sound_is_playing(Sound* sound)
{
	LONGLONG duration = 0;

	sound->seeking->lpVtbl->GetDuration(sound->seeking, &duration);

	LONGLONG current = 0;

	sound->seeking->lpVtbl->GetCurrentPosition(sound->seeking, &current);

	if (current >= duration)
	{
		return false;
	}

	OAFilterState state = 0;

	sound->control->lpVtbl->GetState(sound->control, 0, &state);

	return state == State_Running;
}

void sound_set_volume(Sound* sound, double volume)
{
	long millibel = (long)(2000.0 * log10(volume));

	if (millibel < -10000)
	{
		millibel = -10000;
	}

	sound->audio->lpVtbl->put_Volume(sound->audio, millibel);
}

void sound_play(Sound* sound)
{
	LONGLONG start = 0;

	sound->seeking->lpVtbl->SetPositions(sound->seeking, &start, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);

	sound->control->lpVtbl->Run(sound->control);
}

void sound_stop(Sound* sound)
{
	sound->control->lpVtbl->Stop(sound->control);

	LONGLONG start = 0;

	sound->seeking->lpVtbl->SetPositions(sound->seeking, &start, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
}

void sound_pause(Sound* sound)
{
	OAFilterState state = 0;

	sound->control->lpVtbl->GetState(sound->control, 0, &state);

	if (state == State_Running)
	{
		sound->control->lpVtbl->Pause(sound->control);
	}
}

void sound_resume(Sound* sound)
{
	OAFilterState state = 0;

	sound->control->lpVtbl->GetState(sound->control, 0, &state);

	if (state == State_Paused)
	{
		sound->control->lpVtbl->Run(sound->control);
	}
}
