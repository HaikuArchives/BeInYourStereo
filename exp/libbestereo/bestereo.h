/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#ifndef _BESTEREO_H
#define _BESTEREO_H

#if BUILDING_BESTEREO
	#define EXPORT __declspec(dllexport) 
#else 
	#define EXPORT __declspec(dllimport) 
#endif

#include <SupportDefs.h>

extern "C" {

EXPORT int b_count_playlists();
EXPORT int b_create_playlist();

EXPORT int32 b_count(int32 playlist);
EXPORT void b_clear(int32 playlist);

EXPORT void b_add(int32 playlist, const char * path, int32 index=-1);
EXPORT void b_remove(int32 playlist, int32 ID);

EXPORT void b_play(int32 playlist);
EXPORT void b_pause(int32 playlist);
EXPORT float b_get_pitch(int32 playlist);
EXPORT void b_set_pitch(int32 playlist, float pitch);
EXPORT float b_get_position(int32 playlist);
EXPORT void b_set_position(int32 playlist, float position);
EXPORT float b_get_volume(int32 playlist);
EXPORT void b_set_volume(int32 playlist, float volume);
EXPORT void b_play_file(int32 playlist, int32 ID);
EXPORT void b_play_next(int32 playlist);
EXPORT void b_play_previous(int32 playlist);

EXPORT void b_shuffle(int32 playlist);
EXPORT void b_sort(int32 playlist);

EXPORT int32 b_get_current_id(int32 playlist);
EXPORT int32 b_get_current_index(int32 playlist);
EXPORT int32 b_id_for_index(int32 playlist, int32 index);
EXPORT const char* b_get_name(int32 playlist, int32 ID=0);
EXPORT const char* b_get_path(int32 playlist, int32 ID=0);
EXPORT void b_set_name(int32 playlist, int32 ID, const char *newname);

}

#endif
