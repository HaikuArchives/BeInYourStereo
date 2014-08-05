/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include <Message.h>
#include <Locker.h>
#include <string>
#include "hash_string.h"  // defines hash<string>
#include "mp3file.h"


class AttributeIndex {
public:
	void AddAssociation(const string&from, mp3file *to);
	int CrossReference(const string&from, mp3set *matching,
							AttributeList *albumIndex=NULL,
							AttributeList *artistIndex=NULL,
							AttributeList *genreIndex = NULL,
							AttributeList *producerIndex = NULL,
							AttributeList *yearIndex = NULL);
	
	// returns true if that was the last mapping
	bool RemoveAssociation(const string&from, mp3file *to);
	
private:
	BLocker locker;

	typedef hash_multimap<string, mp3file*, hash<string> > MapType;
	MapType map;
};

