/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include <stdio.h>
#include <assert.h>
#include "bestereo.h"

#include <Application.h>

#include <iostream.h>
#define expect(x,y,message) { snooze(50000); if ((x) != (y)) { cout << "Test failed while " << message << ": " << (x) << "!=" << (y) << endl; } }
#define expect_gte(x,y,message) { snooze(50000); if ((x) < (y)) { cout << "Test failed while " << message << ": " << (x) << "<" << (y) << endl; } }
#define expect_str(x,y,message) if (strcmp(x,y)) { cout << "Test failed while " << message << ": " << x << "!=" << y << endl; }

void dump_playlist(int32 pid) {
	int32 count = b_count(pid);
	
	for (int32 i=0; i<count; i++) {
		cout << i << ": " << b_get_name(pid, b_id_for_index(pid, i)) << endl;
	}
	cout << endl;
}

int main(int argc, char **argv) {
	BApplication foo("application/x-vnd.svanegmond.sender");

	char *fname1,*fname2;
	if (argc>=2) fname1 = argv[1];
	else fname1 = "/clarke/incoming/Trance-John Digweed - Bedrock Disc 1.mp3";
	
	if (argc>=3) fname2 = argv[2];
	else fname2 = "/clarke/mp3s/The Orb/Little Fluffy Clouds/orb.little-fluffy-clouds.05.one-true-parker-mix.mp3";
	
	int playlists = b_count_playlists();

	
	int32 added_playlist = b_create_playlist();
	expect(added_playlist, playlists, "Adding a playlist");

		
	b_add(added_playlist, fname1, 0);

	expect(1, b_count(added_playlist), "Adding a track");



	b_play(added_playlist);
	float pitch = b_get_pitch(added_playlist);
	expect (pitch, 1.0, "Starting playback and reading the pitch");

	b_set_pitch(added_playlist, 0.5);
	expect (b_get_pitch(added_playlist), 0.5, "Setting pitch to .5");

	b_set_position(added_playlist, 0.5);
	expect_gte(b_get_position(added_playlist), 0.5, "Setting position to 50%");

	b_set_position(added_playlist, 0.65);
	expect_gte(b_get_position(added_playlist), 0.65, "Setting position to 65%");
	
	
	b_set_pitch(added_playlist, -1.0);
	expect (b_get_pitch(added_playlist), -1.0, "Setting pitch to -1");

	b_set_pitch(added_playlist, 1.0);
	expect (b_get_pitch(added_playlist), 1.0, "Setting pitch to +1");

	b_pause(added_playlist);
	expect(b_get_pitch(added_playlist), 0.0, "Pausing");

	b_play(added_playlist);
	expect(b_get_pitch(added_playlist), 1.0, "Unpausing");

	b_add(added_playlist, fname2, 0);
	int32 count = b_count(added_playlist);
	expect(count, 2, "Adding another track");

	cout << "Before shuffling: " << endl;
	dump_playlist(added_playlist);
	
	cout << "After shuffling: " << endl;
	b_shuffle(added_playlist);
	dump_playlist(added_playlist);

	cout << "After sorting: " << endl;
	b_sort(added_playlist);
	dump_playlist(added_playlist);
	
	int32 item0 = b_id_for_index(added_playlist, 0);
	int32 item1 = b_id_for_index(added_playlist, 1);
	
	b_set_name(added_playlist, item0, "zzzzebra");
	expect_str("zzzzebra", b_get_name(added_playlist, item0), "Renaming item to zzzzebra");

	b_play(added_playlist);
	
	b_play_file(added_playlist, item0);
	expect(b_get_current_id(added_playlist), item0, "play_file");
	expect(b_get_current_index(added_playlist), 0, "play_file while checking the index");
	
	b_play_next(added_playlist);
	expect(b_get_current_id(added_playlist), item1, "play_next");
	expect(b_get_current_index(added_playlist), 1, "play_next while checking the index");
	expect(b_get_current_index(added_playlist), 1, "play_next while checking the index");
	expect(b_get_current_index(added_playlist), 1, "play_next while checking the index");
	expect(b_get_current_index(added_playlist), 1, "play_next while checking the index");
	
	b_play_previous(added_playlist);
	expect(b_get_current_id(added_playlist), item0, "play_previous");
	expect(b_get_current_index(added_playlist), 0, "play_previous while checking the index");
	

	b_remove(added_playlist, item0);
	expect (b_count(added_playlist), 1, "Removing an item");

	b_clear(added_playlist);
	expect (b_count(added_playlist), 0, "Clearing a playlist");
	
	
	return 0;
}
