/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include "SLooper.h"

class SoundPlayController;
class MP3Query;
class BNetEndpoint;

/**
This class represents BIYS.  SoundPlay passes around pointers to this
without really knowing what it is.

It brings together several roles:
* the audio file database
* the web server
* the current BIYS settings

One instance is kept around for the plugin.  It is cloned for each HTTP
request that comes in, so that settings etc. are around.
*/
class PluginHandle
	: private SLooper
{
public:
	PluginHandle(SoundPlayController *controller);
	void Quit(); 	// leads to destruction

	/// Called (indirectly) by SoundPlay to inform us of our settings.
	void ReadConfig(BMessage *config);

	enum messages {
		count_playlists,
		create_playlist,
		remove_playlist,
		
		count,
		clear,

		add,
		remove,

		play,
		pause,
		get_pitch,
		set_pitch,
		get_position,
		set_position,
		get_volume,
		set_volume,
		play_file,
		play_next,
		play_previous,
		
		shuffle,
		sort,
		
		get_current_index,
		get_current_id,
		id_for_index,
		get_name,
		get_path,
		set_name,

		trivia_for_id,
		attributes_for_id,
		attributes_for_path,
			
		list_files,
		list_genres,
		list_years,
		list_producers,
		list_artists,
		list_albums,
		
		xref_genre,
		xref_year,
		xref_producer,
		xref_artist,
		xref_album,

		get_genre,
		get_year,
		get_producer,
		get_artist,
		get_album,
	};

private:
	virtual void MessageReceived(BMessage *);
	virtual ~PluginHandle();
	
	/// Talk to SoundPlay through this.
	SoundPlayController *ctrl;

	/// The audio file database is here.
	void build_index();
	MP3Query *mp3Data;	
	
	/// web server properties
		bool run_own_webserver;
		int port;
	
	/// audio database properties
		bool producer_absorbs_author;

	// webserver metadata
	thread_id listener;
	status_t last_status;
	BNetEndpoint *endpoint;
	
	void stop_listening();
	status_t start_listening();
	
	static int32 _Listen(void *data);
	int32 Listen();
};


