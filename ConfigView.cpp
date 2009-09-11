/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include "constants.h"

#include "ConfigView.h"
#include "MCheckBox.h"
#include "MTextControl.h"
#include "MSplitter.h"
#include "VGroup.h"
#include "HGroup.h"
#include "MPopup.h"
#include "MButton.h"
#include <MenuItem.h>
#include <Path.h>

#include <stdio.h>

#include <Application.h>
#include <Clipboard.h>

#include <Roster.h>
#include <Directory.h>
#include <SymLink.h>
#include <string.h>
#include <FindDirectory.h>
#include <NodeMonitor.h>
#include <Window.h>

ConfigView::ConfigView(BMessage *_config)
	: VGroup((MView*)NULL),
	  messenger(NULL)
{
	config=_config;
	
	VGroup *vg = new VGroup();

	// make default config
	if (!config->HasBool("run_own_webserver"))
		config->AddBool("run_own_webserver", true);
	if (!config->HasBool("producer_absorbs_author"))
		config->AddBool("producer_absorbs_author", false);

	int32 port;
	if (!config->HasInt32("port")) {
		config->AddInt32("port", 8080);
	}
	config->FindInt32("port", &port);
	if (port==0) port = 8080;
	char portString[10];
	sprintf(portString, "%ld", port);
	
	// now build UI
	vg->AddChild(skinPopup = new MPopup("Preferred Skin",new BMessage('skin'), this, 0));

	HGroup *hg = new HGroup();
	hg->AddChild(new MButton("Skin Folder", new BMessage('skfo'), this));
	hg->AddChild(new MButton("Download Skins", new BMessage('skdl'), this));
	vg->AddChild(hg);
	
	vg->AddChild(new MSplitter());

	vg->AddChild(new MCheckBox("Use built-in web server",new BMessage('rweb'),this,config->FindBool("run_own_webserver")));
	vg->AddChild(hackTextControl[0]=new MTextControl("Port", portString, new BMessage('port')));

//	vg->AddChild(new MSplitter());

//	vg->AddChild(new MCheckBox("Producer absorbs Author",new BMessage('prod'),this,config->FindBool("producer_absorbs_author")));

	AddChild(vg);
}


void ConfigView::AttachedToWindow() {
	hackTextControl[0]->SetTarget(this);
	
	// now that we're attached to a window, we have a message loop and can get node monitor messages
	status_t ignored;
	messenger = new BMessenger(this, Window(), &ignored);
	
	// figure out where the skin directory is and start watching it.
	node_ref nref;
	BDirectory skinDirectory(BIYS_SKIN_DIRECTORY);
	skinDirectory.GetNodeRef(&nref);

	watch_node(&nref, B_WATCH_DIRECTORY, *messenger);

	// fetch a list of skins
	ScanForSkins();	
}

ConfigView::~ConfigView() {
	if (messenger) {
		stop_watching(*messenger);
		delete messenger;
	}
}

void ConfigView::MessageReceived(BMessage *mes)
{
	switch(mes->what)
	{
		case 'rweb':
			config->ReplaceBool("run_own_webserver",mes->FindInt32("be:value"));
			break;

		case 'prod':
			config->ReplaceBool("producer_absorbs_author",mes->FindInt32("be:value"));
			break;

		case 'port': {
			const char *portString = hackTextControl[0]->Text();
			int32 port;
			sscanf(portString, "%ld", &port);
			config->ReplaceInt32("port", port);
			break;
		}

		case 'sksl': {	// picked a skin
			BMenuItem *item;
			mes->FindPointer("source", (void**) &item);

			BEntry currentSkin(BIYS_SKIN_SELECTION, false);
			currentSkin.Remove();
			
			BDirectory settingsDir(BIYS_SETTINGS_DIRECTORY);
			char newSkin[B_PATH_NAME_LENGTH];
			strcpy(newSkin, BIYS_SKIN_DIRECTORY);
			strcat(newSkin, item->Label());
			BSymLink ignored;
			settingsDir.CreateSymLink("current_skin", newSkin, &ignored);			
			break;
		}
		
		case 'skfo': {	// clicked on 'open skin folder' button
			const char *path = BIYS_SKIN_DIRECTORY;
			be_roster->Launch("application/x-vnd.Be-directory", 1, (char**) &path);

			if (be_clipboard->Lock()) { 
				be_clipboard->Clear();
				BMessage *clip = be_clipboard->Data();
				if (clip) {
					clip->AddData("text/plain", B_MIME_TYPE, BIYS_SKIN_DIRECTORY, strlen(BIYS_SKIN_DIRECTORY)); 
					be_clipboard->Commit(); 
				} 
				be_clipboard->Unlock(); 
			}

			break;
		}

		case 'skdl': {	// clicked on 'open skin folder' button
			const char *url = "http://bang.dhs.org/be/biys/skins/";
			be_roster->Launch("application/x-vnd.Be.URL.http", 1, (char**) &url);
			break;
		}
		
		
		case 'NDMN': {	// node monitor message
			ScanForSkins();
			break;
		}
		
		default:
			mes->PrintToStream();
			break;
	}
}

void ConfigView::ScanForSkins() {
	// get the name of the currently-selected skin
	char skin_preference[B_FILE_NAME_LENGTH];
	BEntry currentSkin(BIYS_SKIN_SELECTION, true);
	currentSkin.GetName(skin_preference);

	// get the directory of skins	
	BDirectory skinNode(BIYS_SKIN_DIRECTORY);

	// empty out the menu
	BMenu *menu = skinPopup->Menu();
	menu->RemoveItems(0, menu->CountItems(), true);
	
	bool wasMarked = false;
	BEntry skin;
	skinNode.Rewind();	
	while (skinNode.GetNextEntry(&skin, false)==B_OK) {
		char skinName[B_FILE_NAME_LENGTH];
		skin.GetName(skinName);
		BMenuItem *item = new BMenuItem(skinName, new BMessage('sksl'));
		if (!strcmp(skin_preference, skinName)) {
			wasMarked = true;
			item->SetMarked(true);
		}
		menu->AddItem(item);
	}

	if (!wasMarked && skinNode.CountEntries()>0) {
		menu->ItemAt(0)->SetMarked(true);	
	}	
}
