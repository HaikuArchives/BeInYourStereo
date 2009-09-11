/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include "MP3Query.h"
#include "mp3file.h"

#include <Messenger.h>
#include <Query.h>
#include <Entry.h>
#include <Node.h>
#include <Path.h>
#include <NodeMonitor.h>
#include <Volume.h>
#include <OS.h>
#include <fs_attr.h>
#include <stdio.h>

MP3Query::MP3Query(bool absorb) 
	: BLooper("live query BLooper") {

	producer_absorbs_author = absorb;
	Run();
}

MP3Query::~MP3Query() {
	set<thread_id>::iterator threadIter;
	status_t ignored;
	for (threadIter=queryThreads.begin(); threadIter != queryThreads.end(); threadIter++) {
		wait_for_thread(*threadIter, &ignored);
	}

	set<BQuery*>::iterator iter;
	for (iter=queryList.begin(); iter != queryList.end(); iter++) {
		(*iter)->Clear();
		delete *iter;
	}
	
	RemoveAllRefs();
}

void MP3Query::Quit() {
	BLooper::Lock();
	BLooper::Quit();
}


void MP3Query::MessageReceived(BMessage *message) {
	if (message->what != B_QUERY_UPDATE) return;
	int32 opcode;
	message->FindInt32("opcode", &opcode);
	
	switch (opcode) {
	case B_ENTRY_CREATED: {
		entry_ref ref;
		ino_t node;
		const char *name;
		message->FindInt32("device", &ref.device); 
		message->FindInt64("directory", &ref.directory);
		message->FindString("name", &name);
		ref.set_name(name);
		message->FindInt64("node", &node);
		printf("Adding %s\n", name);
		AddRef(ref, node, true);
		break;
	}

	case B_ENTRY_REMOVED: {
		mp3file t;

		message->FindInt32("device", &t.ref.device); 
		message->FindInt64("directory", &t.ref.directory);
		message->FindInt64("node", &t.node);

		mp3set::iterator iter = queryResults.find(&t);
		if (iter==queryResults.end()) return;
		printf("Removing %s\n", (*iter)->ref.name);
		RemoveRef(iter);
		break;
	}

	default: {
		message->PrintToStream();
	}	
	
	}
}


typedef struct {
	MP3Query *object;
	BVolume *volume;
} MP3QueryThreadData;


int32 MP3Query::StartQueryThread(void *d) {
	MP3QueryThreadData *data = (MP3QueryThreadData *)d;
	data->object->RunQueryOn(data->volume);
	delete data->volume;
	delete data;
	return 0;
}

void MP3Query::QueryOn(BVolume *volume) {
	MP3QueryThreadData *data = new MP3QueryThreadData;
	data->object = this;
	data->volume = new BVolume(*volume); // copy it because it might be gone when it wakes up
	
	thread_id id = spawn_thread(StartQueryThread, "BIYS query thread", B_NORMAL_PRIORITY, data);
	queryThreads.insert(id);
	resume_thread(id);
}	


void MP3Query::RunQueryOn( BVolume *volume) {	
	BQuery *query = new BQuery;
	queryList.insert(query);

	bigtime_t startTime = system_time();
	
	query->SetVolume(volume);

	query->PushAttr("Audio:Genre");
	query->PushString("\"\"");
	query->PushOp(B_NE);

	query->PushAttr("Audio:Album");
	query->PushString("\"\"");
	query->PushOp(B_NE);

	query->PushOp(B_OR);

	query->PushAttr("Audio:Artist");
	query->PushString("\"\"");
	query->PushOp(B_NE);

	query->PushOp(B_OR);

	query->PushAttr("Audio:Producer");
	query->PushString("\"\"");
	query->PushOp(B_NE);

	query->PushOp(B_OR);

	query->PushAttr("Audio:Year");
	query->PushInt32(0);
	query->PushOp(B_GT);

	query->PushOp(B_OR);

	query->SetTarget(this);
	query->Fetch();

	entry_ref ref;
	int found=0;
	while (query->GetNextRef(&ref)==B_OK) {
		found++;
		node_ref node;
		BNode n(&ref);
		n.GetNodeRef(&node);

		AddRef(ref, node.node);
	}
	
	char throwaway[1024];
	volume->GetName(throwaway);
	thread_info info;
	get_thread_info(find_thread(NULL), &info);
	printf("Scanning %s: %d found. %f (user time), %f (kernel time), %f (clock time) seconds.\n", throwaway, found, info.user_time/1000000.0, info.kernel_time/1000000.0, (system_time()-startTime)/1000000.0);
}

void MP3Query::AddRef(entry_ref &ref, ino_t nodenumber, bool reindex) {
	BNode node(&ref);
	if (node.InitCheck() != B_OK || !node.IsFile()) { return; }	

	mp3file *mp3 = new mp3file;
	mp3->ref = ref;
	mp3->node = nodenumber;
	
	BPath path(&ref);
	mp3->path = path.Path();
	
	mp3set::iterator mp3iter = (queryResults.insert(mp3)).first;
	if ((*mp3iter)!=mp3) { // then that file is already indexed!
		if (reindex) {
			RemoveRef(mp3iter);
			mp3iter = (queryResults.insert(mp3)).first;
		} else {
			// 
			return;
		}
	}
			
	char buffer[1024];
	string indexer;
	
	buffer[0]='\0';		
	ssize_t size = node.ReadAttr("Audio:Genre", B_STRING_TYPE, 0, buffer, sizeof(buffer));
	indexer = buffer;
	genreIndex.AddAssociation(indexer, mp3);
	mp3->genre = genres.insert(indexer).first;
	
	buffer[0]='\0';		
	size = node.ReadAttr("Audio:Artist", B_STRING_TYPE, 0, buffer, sizeof(buffer));
	indexer = buffer;
	artistIndex.AddAssociation(indexer, mp3);
	mp3->artist = artists.insert(indexer).first;

	
	buffer[0]='\0';		
	size = node.ReadAttr("Audio:Producer", B_STRING_TYPE, 0, buffer, sizeof(buffer));
	if (producer_absorbs_author && size<=0) {
		// producer takes on author and there's no producer set?
		// retain "indexer" from Audio:Artist, above
	} else {	
		indexer = buffer;
	}
	producerIndex.AddAssociation(indexer, mp3);
	mp3->producer = producers.insert(indexer).first;
	
	buffer[0]='\0';		
	size = node.ReadAttr("Audio:Album", B_STRING_TYPE, 0, buffer, sizeof(buffer));
	indexer = buffer;
	albumIndex.AddAssociation(indexer, mp3);
	mp3->album = albums.insert(indexer).first;
		
	int32 year;
    status_t err;
    attr_info info;

	err = node.GetAttrInfo("Audio:Year", &info);
	if (!err && info.size > 0) {
		switch(info.type) {
		case B_STRING_TYPE:
		size = node.ReadAttr("Audio:Year", B_STRING_TYPE, 0, buffer, sizeof(buffer));
		if (size == 0) {
			sprintf(buffer, "0");
		}
		break;
		
		case B_INT32_TYPE:
			size = node.ReadAttr("Audio:Year", B_INT32_TYPE, 0, &year, sizeof(year));
			if (size == 0) {
				year = 0;
			}
			sprintf(buffer, "%ld", year);
			break;
			
			default:
			sprintf(buffer, "");
			break;
		}
	} else {
		sprintf(buffer, "");
	}

	indexer = buffer;
	yearIndex.AddAssociation(indexer, mp3);
	mp3->year = years.insert(indexer).first;
}

void MP3Query::RemoveAllRefs() {
	for (mp3set::iterator i=queryResults.begin(); i != queryResults.end(); ++i) {
		delete *i;
	}
}

void MP3Query::RemoveRef(mp3set::iterator &forWhat) {
	mp3file *found = *forWhat;

	if (producerIndex.RemoveAssociation(*found->producer, found)) {
		producers.erase(*found->producer);
	}

	if (genreIndex.RemoveAssociation(*found->genre, found)) {
		genres.erase(*found->genre);
	}
	
	if (artistIndex.RemoveAssociation(*found->artist, found)) {
		artists.erase(*found->artist);
	}

	if (albumIndex.RemoveAssociation(*found->album, found)) {
		albums.erase(*found->album);
	}

	if (yearIndex.RemoveAssociation(*found->year, found)) {
		years.erase(*found->year);
	}

	queryResults.erase(forWhat);
	delete found;
}
