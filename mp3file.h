/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#ifndef __mp3file_h
#define __mp3file_h

#include <Entry.h>
#include <set>
#include <string>

#include <stdio.h>

typedef set<string> AttributeList;

typedef struct mp3file {
	entry_ref ref;
	ino_t node;
	string path;
	
	AttributeList::iterator genre;
	AttributeList::iterator artist;
	AttributeList::iterator album;
	AttributeList::iterator year;
	AttributeList::iterator producer;
} mp3file;

struct entryref_lt {
	bool operator()(const mp3file *e1, const mp3file *e2) const {
		if (e1->node<e2->node) {
			return 1;
		} else if (e1->node > e2->node) {
			return 0;
		}
		
		if (e1->ref.device < e2->ref.device) {
			return 1;
		}

		return 0;
	}
};

typedef set<mp3file *, entryref_lt> mp3set;

#endif
