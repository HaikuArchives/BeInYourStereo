/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

class SoundPlayController;
class MP3Query;
class BNetEndpoint;

typedef struct {
	SoundPlayController *ctrl;
	BNetEndpoint *endpoint;
	MP3Query *mp3Data;
} HTTPRequest;
