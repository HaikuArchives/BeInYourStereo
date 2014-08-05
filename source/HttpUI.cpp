/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include <Application.h>
#include <Roster.h>
#include <VolumeRoster.h>
#include <Volume.h>
#include <UTF8.h>
#include <Alert.h>
#include <NetEndpoint.h>
#include <fs_attr.h>
#include <Path.h>
#include "HttpUI.h"
#include "Http-thread.h"
#include "MP3Query.h"
#include "HTTPRequest.h"
#include "pluginproto.h"

PluginHandle::PluginHandle(SoundPlayController *controller)
	:	  SLooper("BIYS"),
	 ctrl(controller)
{
	listener= -1;
	endpoint=NULL;
	
	run_own_webserver = true;
	port = 8080;
	producer_absorbs_author = false;
	
	last_status = B_NO_ERROR;

	mp3Data = NULL;
	build_index();
	// start listening for requests
	
	Run();
}

void PluginHandle ::Quit() {
	BLooper::Lock();
	BLooper::Quit();
}

PluginHandle::~PluginHandle() {
	stop_listening(); // in case it hasn't been stopped already
	mp3Data->Quit();
}

void PluginHandle::ReadConfig(BMessage *config) {
	printf("Read config.\n");
	run_own_webserver = true;
	if (config->HasBool("run_own_webserver")) 
		run_own_webserver = config->FindBool("run_own_webserver");

	if (run_own_webserver) {

		int newPort = 8080;
		if (config->HasInt32("port")) 
			newPort = config->FindInt32("port");

		if (port != newPort) {
			stop_listening();
			port = newPort;
		}
		start_listening();
	} else {
		stop_listening();
	}

	if (config->HasBool("producer_absorbs_author")) {
		bool newSetting = config->FindBool("producer_absorbs_author");
		if (newSetting != producer_absorbs_author) {
			producer_absorbs_author = newSetting;
			build_index();
		}
	}
}

status_t PluginHandle::start_listening() {
	status_t rc = B_BUSY;

	// don't do anything if there's already a listener
	if (endpoint) return last_status;

	// default to 8080
	if (port == 0) port=8080;

	endpoint=new BNetEndpoint(SOCK_STREAM);
	if(!endpoint || ((rc = endpoint->InitCheck()) != B_NO_ERROR)) { goto bomb; }
	
	rc = endpoint->Bind(BNetAddress(INADDR_ANY,port));
	if (rc != B_NO_ERROR) { goto bomb; }
		
	rc = endpoint->Listen();
	if (rc != B_NO_ERROR) { goto bomb; }
	
	listener=spawn_thread(_Listen,"BIYS HTTP listener",B_NORMAL_PRIORITY,this);
	if(listener>=0)
	{
			resume_thread(listener);
			last_status = B_OK;
			return B_OK;
	}

	rc = B_BUSY;
	
 bomb:
	delete endpoint;
	endpoint = NULL;
	last_status = rc;

	if (rc != B_NO_ERROR) {
		BAlert *alert = new BAlert("Failed to start Be in your Stereo's web server", strerror(rc), "Yuck.", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT);
		alert->SetLook(B_TITLED_WINDOW_LOOK);
		alert->SetFlags(B_NOT_RESIZABLE);
		alert->Go();
	}

	return rc;
}

void PluginHandle::stop_listening() {
	if (endpoint) {
		endpoint->Close();
		if(listener>=0)
		{
			status_t dummy;
			wait_for_thread(listener,&dummy);
		}
		delete endpoint;
		endpoint = NULL;
	}
}

int32 PluginHandle::_Listen(void *data) {
	return ((PluginHandle*)data)->Listen();
}

int32 PluginHandle::Listen()
{
	BNetEndpoint *remote;
	
	while((remote=endpoint->Accept())!=NULL)
	{
		if(B_OK==remote->InitCheck())
		{
			HTTPRequest *remotehandle = new HTTPRequest;
			remotehandle->ctrl = ctrl;
			remotehandle->mp3Data = mp3Data;
			remotehandle->endpoint=remote;
			
			// spawn a thread to do the actual communication.
			// I should be keeping track of all the threads I create, but I'm too lazy
			thread_id worker=spawn_thread(http_handler,"BIYS HTTP worker",B_NORMAL_PRIORITY,remotehandle);
			if(worker>=0)
				resume_thread(worker);
		}
	}

	return B_OK;
}

void PluginHandle::build_index() {
	MP3Query *oldData = mp3Data;
	mp3Data = new MP3Query(producer_absorbs_author);
	if (oldData) 
		oldData->Quit();
	
		
	BVolumeRoster roster;
	BVolume vol;
	roster.Rewind();
	while (roster.GetNextVolume(&vol) == B_OK) {
		if (vol.KnowsQuery()) 
			mp3Data->QueryOn(&vol);
	}
}


static AttributeList genres;
static AttributeList artists;
static AttributeList albums;
static AttributeList producers;
static AttributeList years;
static mp3set matching;

static void DoCrossReference(AttributeIndex &index, BMessage *msg) {
	string xref = msg->FindString("xref");
	index.CrossReference(xref, &matching, &albums, &artists, &genres, &producers, &years);

	for (AttributeList::iterator i=genres.begin(); i != genres.end(); ++i) {
		msg->AddString("genre", i->c_str());
	}

	for (AttributeList::iterator i=producers.begin(); i != producers.end(); ++i) {
		msg->AddString("producer", i->c_str());
	}

	for (AttributeList::iterator i=years.begin(); i != years.end(); ++i) {
		msg->AddString("year", i->c_str());
	}
	
	for (AttributeList::iterator i=albums.begin(); i != albums.end(); ++i) {
		msg->AddString("album", i->c_str());
	}

	for (AttributeList::iterator i=artists.begin(); i != artists.end(); ++i) {
		msg->AddString("artist", i->c_str());
	}

	for (mp3set::iterator i=matching.begin(); i != matching.end(); ++i) {
		msg->AddString("matching", (*i)->path.c_str());
	}

	albums.clear();
	artists.clear();
	genres.clear();
	producers.clear();
	years.clear();
	matching.clear();
}

/**
	Crawls the given path (maybe a file, maybe a shoutcast)
	and adds the attibutes it can figure out to the given BMessage.
*/
static void AddFileAttributes(const char *path, BMessage *msg) {
	// not much to do in this case
	if (!path)
		return;
		
	msg->AddString("name", path);
	BNode node(path);
	if (node.InitCheck()!=B_OK) return;
	
	node.RewindAttrs();
	char attribute[B_ATTR_NAME_LENGTH];
	union {
		int32 integer;
		float floating;
		char str[65536];
	} attributeValue;
	
	while (node.GetNextAttrName(attribute) == B_NO_ERROR) {
		attr_info info;
		node.GetAttrInfo(attribute, &info);
		node.ReadAttr(attribute, B_ANY_TYPE, 0, &attributeValue, sizeof(attributeValue));
	
		switch (info.type) {
		case B_INT32_TYPE:
			msg->AddInt32(attribute, attributeValue.integer);
			break;
			
		case B_FLOAT_TYPE:
			msg->AddInt32(attribute, attributeValue.floating);
			break;
			
		case B_STRING_TYPE:
			msg->AddString(attribute, attributeValue.str);
			break;
		}
	}
}

void PluginHandle::MessageReceived(BMessage *msg) {
	PlaylistPtr ptr;
	ctrl->Lock();

	bool ptrSet = false;
	if (msg->HasInt32("pl")) {
		int32 pl;
		msg->FindInt32("pl", &pl);
		ptr = ctrl->PlaylistAt(pl);
		if (!ptr->IsValid()) {
			int32 count = ctrl->CountPlaylists();
			if (count==0) {
				ptr = ctrl->AddPlaylist();
				if (!ptr->IsValid()) {
					ctrl->Unlock();
					msg->SendReply(msg);
					return;
				}
			} else {
				ptr = ctrl->PlaylistAt(0);
			}
		}
		ptrSet = true;
		ptr->Lock();
	}

	int32 index = -1;
	index = msg->FindInt32("i");
	
	int32 id = 0;
	id = msg->FindInt32("id");
	int reply_expected = 1;
	file_info info; // common temp var
	
	switch (msg->what) {
	case count_playlists:
		msg->AddInt32("c", ctrl->CountPlaylists());
		break;

	case create_playlist:
		ptr = ctrl->AddPlaylist();
		msg->AddInt32("i", ctrl->CountPlaylists()-1);
		break;

	case remove_playlist:
		ctrl->RemovePlaylist(ptr);
		reply_expected = 0;
		break;
		
	case count:
		msg->AddInt32("c", ptr->CountItems());
		break;

	case clear:
		ptr->MakeEmpty();
		reply_expected = 0;
		break;

	case add: {
		const char *path;
		
		msg->FindString("path", &path);
		if (index<-1 || index>ptr->CountItems()) index = -1;
			
		BEntry entry(path);
		if (entry.IsDirectory()) {
			ptr->AddDir(path,index);
		} else {
			ptr->Add(path, index);
		}
		reply_expected = 0;
		break;
		}

	case remove:
		if (id == ptr->CurrentID()) {
			if (ptr->CountItems()==1) {
				// we'll remove the playlist later (to get it to stop playing)
			} else if (ptr->CurrentIndex() == ptr->CountItems()-1) {
				ptr->PlayPrevious();
			} else {
				ptr->PlayNext();
			}			
		}
		ptr->Remove(id);

	
		if (ptr->CountItems()==0) {
			ctrl->RemovePlaylist(ptr);
		}

		reply_expected = 0;
		break;
	
	case play:
		ptr->Play();
		reply_expected = 0;
		break;

	case pause:
		ptr->Pause();
		reply_expected = 0;
		break;

	case shuffle:
		ptr->Shuffle();
		reply_expected = 0;
		break;

	case sort:
		ptr->Sort();
		reply_expected = 0;
		break;

		
	case get_pitch:
		msg->AddFloat("pitch", ptr->Pitch());
		break;

	case set_pitch:
		ptr->SetPitch(msg->FindFloat("pitch"));
		// argh, this seems asynchronous
		snooze(250000);
		reply_expected = 0;
		break;

	case get_position:
		ptr->GetInfo(ptr->CurrentID(), &info);
		msg->AddFloat("pos",
						ptr->Position()/info.framecount*info.samplerate);
		break;

	case set_position:
		ptr->GetInfo(ptr->CurrentID(), &info);
		ptr->SetPosition(msg->FindFloat("pos")*info.framecount/info.samplerate);
		reply_expected = 0;
		break;

	case get_volume:
		msg->AddFloat("vol", ptr->Volume());
		break;

	case set_volume:
		ptr->SetVolume(msg->FindFloat("vol"));
		reply_expected = 0;
		break;

	case play_file:
		ptr->PlayFile(id);
		reply_expected = 0;
		break;

	case play_next:
		ptr->PlayNext();
		reply_expected = 0;
		break;

	case play_previous:
		ptr->PlayPrevious();
		reply_expected = 0;
		break;

	case get_current_id:
		msg->AddInt32("id", ptr->CurrentID());
		break;

	case get_current_index:
		msg->AddInt32("i", ptr->CurrentIndex());
		break;

	case trivia_for_id: {
		file_info info;
		if (ptr->GetInfo(id, &info) != B_NO_ERROR) {
			ctrl->Unlock();
			return;
		}

		msg->AddString("pretty_name", info.name);
		msg->AddString("mimetype", info.mimetype);
		msg->AddString("pretty_type", info.typedesc);
		
		msg->AddFloat("samplerate", info.samplerate);
		msg->AddFloat("bitrate", info.bitrate);
		msg->AddInt32("numchannels", info.numchannels);
		msg->AddInt32("granularity", info.granularity);
		msg->AddInt32("framecount", info.framecount);
		msg->AddInt32("samplesize", info.samplesize);
		msg->AddInt32("byteorder", info.byteorder);
		msg->AddInt32("sampleformat", info.sampleformat);
		break;
	}
		
	case attributes_for_id: {
		AddFileAttributes(ptr->PathForItem(id), msg);
		break;
	}

	case id_for_index:
		msg->AddInt32("id", ptr->IDForItemAt(index));
		break;

	case get_path:
		msg->AddString("path", ptr->PathForItem(id));
		break;

	case get_name:
		msg->AddString("n", ptr->NameForItem(id));
		break;

	case set_name:
		ptr->SetName(id, msg->FindString("n"));
		reply_expected = 0;
		break;

	case attributes_for_path: {
		AddFileAttributes(msg->FindString("path"), msg);
		break;
	}	
		

	case list_files: {
		for (mp3set::iterator i=mp3Data->queryResults.begin(); i != mp3Data->queryResults.end(); ++i) {
			msg->AddString("files", (*i)->path.c_str());
		}
		break;
	}

	case list_genres: {
		for (AttributeList::iterator i=mp3Data->genres.begin(); i != mp3Data->genres.end(); ++i) {
			msg->AddString("genre", i->c_str());
		}
		break;
	}
	
	case list_years: {
		for (AttributeList::iterator i=mp3Data->years.begin(); i != mp3Data->years.end(); ++i) {
			msg->AddString("year", i->c_str());
		}
		break;
	}

	case list_producers: {
		for (AttributeList::iterator i=mp3Data->producers.begin(); i != mp3Data->producers.end(); ++i) {
			msg->AddString("producer", i->c_str());
		}
		break;
	}

	case list_artists: {
		for (AttributeList::iterator i=mp3Data->artists.begin(); i != mp3Data->artists.end(); ++i) {
			msg->AddString("artist", i->c_str());
		}
		break;
	}

	case list_albums: {
		for (AttributeList::iterator i=mp3Data->albums.begin(); i != mp3Data->albums.end(); ++i) {
			msg->AddString("album", i->c_str());
		}
		break;
	}

	case xref_genre: {
		DoCrossReference(mp3Data->genreIndex, msg);
		break;
	}

	case xref_year: {
		DoCrossReference(mp3Data->yearIndex, msg);
		break;
	}

	case xref_producer: {
		DoCrossReference(mp3Data->producerIndex, msg);
		break;
	}

	case xref_artist: {
		DoCrossReference(mp3Data->artistIndex, msg);
		break;
	}


	case xref_album: {
		DoCrossReference(mp3Data->albumIndex, msg);
		break;
	}
	
	default:
		printf("Uh.  Unimplemented function, dude\n");
		msg->PrintToStream();
	}

	if (ptrSet)
		ptr->Unlock();

	ctrl->Unlock();

	if (reply_expected) {
		msg->SendReply(msg);
	}	
}
