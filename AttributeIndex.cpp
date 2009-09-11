/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include "AttributeIndex.h"
#include <TypeConstants.h>
#include "mp3file.h"

bool AttributeIndex::RemoveAssociation(const string&from, mp3file *to) {
	locker.Lock();
	pair<MapType::iterator, MapType::iterator> p = 
	    map.equal_range(from);
	for (MapType::iterator i = p.first; i != p.second; ++i) {
		if ((*i).second == to) {
			map.erase(i);
			if (map.count(from)==0) {
				map.erase(from);
				locker.Unlock();
				return true;
			}
			locker.Unlock();
			return false;
		}
	}
	locker.Unlock();
	return false;
}

void AttributeIndex::AddAssociation(const string&from, mp3file *to) {
	locker.Lock();
	map.insert(MapType::value_type(from, to));
	locker.Unlock();
}

int AttributeIndex::CrossReference(const string&from, mp3set *matching, 
									AttributeList *albumIndex, AttributeList *artistIndex, AttributeList *genreIndex, AttributeList *producerIndex, AttributeList *yearIndex) {
	int count = 0;
	locker.Lock();
	pair<MapType::const_iterator, MapType::const_iterator> p = 
	    map.equal_range(from);
	for (MapType::const_iterator i = p.first; i != p.second; ++i) {
		mp3file *f = (*i).second;
		count++;
		if (matching) matching->insert(f);
		if (albumIndex) albumIndex->insert(*(f->album));
		if (artistIndex) artistIndex->insert(*(f->artist));
		if (genreIndex) genreIndex->insert(*(f->genre));
		if (producerIndex) producerIndex->insert(*(f->producer));
		if (yearIndex) yearIndex->insert(*(f->year));
	}	
	locker.Unlock();
	return count;
}
