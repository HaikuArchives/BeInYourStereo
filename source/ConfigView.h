/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include "VGroup.h"
#include <Entry.h>

class MTextControl;
class MPopup;

class ConfigView: public VGroup
{
public:
	ConfigView(BMessage *_config);
	~ConfigView();
	
	virtual void MessageReceived(BMessage *mes);
	virtual void AttachedToWindow();

private:
	BMessage *config;
	MTextControl *hackTextControl[1];
	
	// watch messages get sent to this messenger
		BMessenger *messenger;
		
	void ScanForSkins();
	MPopup *skinPopup;
};
