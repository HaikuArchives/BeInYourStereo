## Application Specific Settings ---------------------------------------------

# specify the name of the binary
NAME=tester

TYPE= APP

#%{
# @src->@ 

SRCS=tester.cpp
RSRCS=

# @<-src@ 
#%}

LIBS= be root bestereo
LIBPATHS= /boot/home/config/lib

SYSTEM_INCLUDE_PATHS =
LOCAL_INCLUDE_PATHS = ../libbestereo

APP_MAJOR_VERSION = 2
APP_MINOR_VERSION = 0
APP_RELEASE_STATUS = d    # d=dev, a=alpha, b=beta, g=gamma, f=final

## Build setting ---------------------------------------------
#	specify the level of optimization that you desire
#	NONE, SOME, FULL
OPTIMIZE= NONE

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

COMPILER_FLAGS =
LINKER_FLAGS =


## include the makefile-engine
include $(BUILDHOME)/etc/makefile-engine
