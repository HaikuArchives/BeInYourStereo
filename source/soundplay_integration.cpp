/*
 * Copyright 2000-2001, Stephen van Egmond, svanegmond@tinyplanet.ca.
 * Distributed under the terms of the MIT License.
 */

#include <Resources.h>
#include <Alert.h>

#include "pluginproto.h"
#include "ConfigView.h"
#include "HttpUI.h"

const char *PLUGINID="sve-http-playlist";
BResources resources;

// =======================================================================
// implementation of interface-specific plugin operations
static void setconfig(void *data,BMessage *config)
{
	PluginHandle *net=(PluginHandle*)data;
	net->ReadConfig(config);
}

static void getconfig(void *data,BMessage *config)
{
}


static interface_plugin_ops plugin_ops={
	PLUGIN_INTERFACE_MAGIC, PLUGIN_INTERFACE_VERSION,
	NULL,	// no ui to show
	NULL,	// no ui to hide
	setconfig,
	getconfig
};




// =======================================================================
// implementation of plugin operations
static void about() {
	char buffer[1024];
	sprintf(buffer, "Be in your Stereo's lame About Box\n\nVersion %d.%d\n", APP_MAJOR_VERSION, APP_MINOR_VERSION);
	
	BAlert *about = new BAlert("biys about", buffer, "Thanks for the info", NULL, NULL, B_WIDTH_AS_USUAL, B_INFO_ALERT);
	about->Go();
}

static BView* configure(BMessage *config)
{
	return new ConfigView(config);
}

static void *getplugin(void **data,const char *name, const char *, uint32, plugin_info *info)
{
	PluginHandle *net=new PluginHandle(info->controller);
	*data=net;

	return &plugin_ops;
}

static void destroyplugin(void *,void *data)
{
	PluginHandle *net=(PluginHandle*)data;
	net->Quit();
}






static plugin_descriptor plugin={
	PLUGIN_DESCRIPTOR_MAGIC,PLUGIN_DESCRIPTOR_VERSION,
	PLUGINID,2, PLUGIN_IS_INTERFACE,

	"Be in your Stereo",
	"by Stephen van Egmond.\n\n"
	"Displays the playlist as a webpage, with facilities for manipulating the playlist and playback. "
	"Connect your browser to port 8080\n",
	&about,
	&configure,
	&getplugin,
	&destroyplugin
};

static plugin_descriptor *plugs[]={
	&plugin,0
};

plugin_descriptor **get_plugin_list(void)
{
	return plugs;
}
