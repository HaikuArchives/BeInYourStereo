/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include <stdio.h>
#include <assert.h>
#include "bestereo.h"

#include <Application.h>

int main(int argc, char **argv) {
	BApplication foo("application/x-vnd.svanegmond.sender");

	int playlists = b_count_playlists();
	printf("There are %d playlists going.\n", playlists);
	
	int32 added_playlist = b_create_playlist();
	printf("Added one; now there's %d.\n", added_playlist);
	assert(added_playlist = playlists+1);
	added_playlist -= 1; // to refer to it
		
	printf("Adding %s to the first playlist\n", argv[1]);
	b_add(added_playlist, argv[1], 0);

	b_play(added_playlist);
		
	return 0;
}
