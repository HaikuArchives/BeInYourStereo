/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include <Looper.h>
#include <list>
#include "AttributeIndex.h"

#ifdef __POWERPC__
#include <hashmap.h>
#else
#include <hash_map>
#endif

class Query;

class MP3Query: private BLooper {
public:
	MP3Query(bool producer_absorbs_author);

	virtual void Quit();	// implicitly leads to destruction

	void QueryOn(BVolume *volume);

	// the mp3 catalogue
	AttributeList genres;
	AttributeList artists;
	AttributeList albums;
	AttributeList years;
	AttributeList producers;

	AttributeIndex genreIndex;
	AttributeIndex artistIndex;
	AttributeIndex albumIndex;
	AttributeIndex yearIndex;	
	AttributeIndex producerIndex;
	
	mp3set queryResults;

private:	
	~MP3Query();
	virtual void MessageReceived(BMessage *message);

	bool producer_absorbs_author;

	set<thread_id> queryThreads;
	static int32 StartQueryThread(void *data);
	void RunQueryOn(BVolume *);

	set<BQuery*> queryList;

	void AddRef(entry_ref &ref, ino_t node, bool reindex=false);
	void RemoveAllRefs();

	void RemoveRef(mp3set::iterator &forWhat);
};
