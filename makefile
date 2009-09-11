## Application Specific Settings ---------------------------------------------

# specify the name of the binary
NAME=Be\ in\ your\ Stereo

TYPE= SHARED

#%{
# @src->@ 

SRCS=AttributeIndex.cpp  ConfigView.cpp  Http-thread.cpp  HttpUI.cpp  MP3Query.cpp soundplay_integration.cpp SLooper.cpp
RSRCS= icon.rsrc

# @<-src@ 
#%}

LIBS= be root layout textencoding  nework _APP_ stdc++.r4
LIBPATHS= /boot/common/lib/ /boot/develop/headers/soundplay

SYSTEM_INCLUDE_PATHS = /boot/develop/headers/soundplay /boot/common/include/liblayout
LOCAL_INCLUDE_PATHS = 

## Build setting ---------------------------------------------
#	specify the level of optimization that you desire
#	NONE, SOME, FULL
OPTIMIZE= FULL

APP_MAJOR_VERSION = 2
APP_MINOR_VERSION = 0
APP_RELEASE_STATUS = d    # d=dev, a=alpha, b=beta, g=gamma, f=final

#	specify any preprocessor symbols to be defined.  The symbols will not
#	have their values set automatically; you must supply the value (if any)
#	to use.  For example, setting DEFINES to "DEBUG=1" will cause the
#	compiler option "-DDEBUG=1" to be used.  Setting DEFINES to "DEBUG"
#	would pass "-DDEBUG" on the compiler's command line.
DEFINES = NDEBUG APP_MAJOR_VERSION=$(APP_MAJOR_VERSION) APP_MINOR_VERSION=$(APP_MINOR_VERSION)

WARNINGS = ALL

#	specify whether image symbols will be created
#	so that stack crawls in the debugger are meaningful
#	if TRUE symbols will be created
SYMBOLS = TRUE

#	specify debug settings
#	if TRUE will allow application to be run from a source-level
#	debugger.  Note that this will disable all optimzation.
DEBUGGER = FALSE

COMPILER_FLAGS = -fomit-frame-pointer
LINKER_FLAGS =

DISTDIR = /tmp/bestereo-$(VERSION)
VERSION = $(APP_MAJOR_VERSION).$(APP_MINOR_VERSION)

## include the makefile-engine
include $(BUILDHOME)/etc/makefile-engine

# Post-build specializations	
#install: default
#	mv obj.x86/Be* /boot/apps/SoundPlay/Plugins
	
dist: default
	rm -rf /tmp/bestereo-$(VERSION)
	cp etc/AddProducerAttribute obj.x86   #x86 only!
	cp etc/reindex obj.x86                #x86 only!
	cp ~/config/bin/php obj.x86           #x86 only!
	rm -rf /tmp/bestereo*
	mkdir $(DISTDIR)
	cp -r manual $(DISTDIR)
	cp -r exp/basic_skin $(DISTDIR)/basic_skin
	cp -r manual/api $(DISTDIR)/basic_skin/
	
	chmod -R u-x $(DISTDIR)/basic_skin
	chmod -R o+x $(DISTDIR)/basic_skin
	ln -s manual/index.html $(DISTDIR)/manual.html 
	cp -r obj.* $(DISTDIR)
	cp Installer $(DISTDIR)
	rm $(DISTDIR)/obj.*/*.o

	rm -r `find $(DISTDIR) -type d | grep .svn`
	/system/Tracker $(DISTDIR) 
	cd /tmp; zip -ry9 $(DISTDIR).zip bestereo-$(VERSION)


