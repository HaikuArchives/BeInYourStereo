/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#define BUILDING_BESTEREO 1
#include "bestereo.h"
#include "HttpUI.h"

#include <Message.h>
#include <Messenger.h>

static BMessenger *messenger = SLooper::FindOtherEnd("BIYS");
BMessage reply;

/* int b_count_playlists() {
	BMessage msg(PluginHandle::count_playlists);
	messenger->SendMessage(&msg, &reply);
	return reply.FindInt32("c");
}*/

/*
int b_create_playlist() {
	BMessage msg(PluginHandle::create_playlist);
	messenger->SendMessage(&msg, &reply);
	return reply.FindInt32("i");
}
*/

/*
int32 b_count(int32 playlist) {
	BMessage msg(PluginHandle::count);
	
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg, &reply);
	return reply.FindInt32("c");
} */

/*
void b_clear(int32 playlist) {
	BMessage msg(PluginHandle::clear);
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg);
}
*/

void b_add(int32 playlist, const char * path, int32 index=-1) {
	if (path == NULL)
		return;
		
	BMessage msg(PluginHandle::add);
	
	msg.AddInt32("pl", playlist);
	msg.AddString("path", path);
	msg.AddInt32("i", index);

	messenger->SendMessage(&msg);
}

void b_remove(int32 playlist, int32 ID) {
	BMessage msg(PluginHandle::remove);
	
	msg.AddInt32("pl", playlist);
	msg.AddInt32("id", ID);
	
	messenger->SendMessage(&msg);
}

/*
void b_play(int32 playlist) {
	BMessage msg(PluginHandle::play);
	
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg);
}

void b_pause(int32 playlist) {
	BMessage msg(PluginHandle::pause);
	
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg);
}
*/

/*
float b_get_pitch(int32 playlist) {
	BMessage msg(PluginHandle::get_pitch);
	
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg, &reply);

	return reply.FindFloat("pitch");
}

void b_set_pitch(int32 playlist, float pitch) {
	BMessage msg(PluginHandle::set_pitch);
	
	msg.AddInt32("pl", playlist);
	msg.AddFloat("pitch", pitch);
	messenger->SendMessage(&msg);
}
*/

/*
float b_get_position(int32 playlist) {
	BMessage msg(PluginHandle::get_position);
	
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg, &reply);

	return reply.FindFloat("pos");
}

void b_set_position(int32 playlist, float position) {
	BMessage msg(PluginHandle::set_position);

	msg.AddInt32("pl", playlist);
	msg.AddFloat("pos", position);
	messenger->SendMessage(&msg);
}

float b_get_volume(int32 playlist) {
	BMessage msg(PluginHandle::get_volume);
	
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg, &reply);

	return reply.FindFloat("vol");
}

void b_set_volume(int32 playlist, float volume) {
	BMessage msg(PluginHandle::set_volume);

	msg.AddInt32("pl", playlist);
	msg.AddFloat("vol", volume);
	messenger->SendMessage(&msg);
}
*/

/* void b_play_file(int32 playlist, int32 ID) {
	BMessage msg(PluginHandle::play_file);
	msg.AddInt32("pl", playlist);
	msg.AddInt32("id", ID);
	messenger->SendMessage(&msg);
}
*/

/*
void b_play_next(int32 playlist) {
	BMessage msg(PluginHandle::play_next);
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg);
}

void b_play_previous(int32 playlist) {
	BMessage msg(PluginHandle::play_previous);
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg);
}
*/
/* int32 b_get_current_id(int32 playlist) {
	BMessage msg(PluginHandle::get_current_id);
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg, &reply);
	return reply.FindInt32("id");
}

int32 b_get_current_index(int32 playlist) {
	BMessage msg(PluginHandle::get_current_index);
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg, &reply);
	return reply.FindInt32("i");
}
*/
/*
int32 b_get_id_for_index(int32 playlist, int32 index) {
	BMessage msg(PluginHandle::id_for_index);
	
	msg.AddInt32("pl", playlist);
	msg.AddInt32("i", index);
	messenger->SendMessage(&msg, &reply);

	return reply.FindInt32("id");
}
*/


/*

void b_shuffle(int32 playlist) {
	BMessage msg(PluginHandle::shuffle);
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg);
}


void b_sort(int32 playlist) {
	BMessage msg(PluginHandle::sort);
	msg.AddInt32("pl", playlist);
	messenger->SendMessage(&msg);
}

*/



/* const char* b_get_name(int32 playlist, int32 ID) {
	BMessage msg(PluginHandle::get_name);
	
	msg.AddInt32("pl", playlist);
	msg.AddInt32("id", ID);
	messenger->SendMessage(&msg, &reply);

	return reply.FindString("n");
}

*/


/*
void b_set_name(int32 playlist, int32 ID, const char *newname) {
	BMessage msg(PluginHandle::set_name);
	
	msg.AddInt32("pl", playlist);
	msg.AddInt32("id", ID);
	msg.AddString("n", newname);
	
	messenger->SendMessage(&msg);
}
*/
