/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "php_biys.h"
#include "ext/standard/php_standard.h"
}

/* If you declare any globals in php_biys.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(biys)
*/

/* True global resources - no need for thread safety here */
static int le_biys;

/* Every user visible function must have an entry in biys_functions[].
*/
extern "C" {
function_entry biys_functions[] = {
	PHP_FE(biys_count_playlists,	NULL)    //
	PHP_FE(biys_create_playlist,	NULL)    //
	PHP_FE(biys_remove_playlist,	NULL)    //
	PHP_FE(biys_add,	NULL)				//
	PHP_FE(biys_remove,	NULL)			//
	PHP_FE(biys_clear,	NULL)               //
	PHP_FE(biys_count,	NULL)				//
	PHP_FE(biys_shuffle,	NULL)		//
	PHP_FE(biys_sort,	NULL)			//
	PHP_FE(biys_play,	NULL)			//	
	PHP_FE(biys_pause,	NULL)			//
	PHP_FE(biys_get_pitch,	NULL)		//
	PHP_FE(biys_set_pitch,	NULL)		//
	PHP_FE(biys_get_position,	NULL)	//
	PHP_FE(biys_set_position,	NULL)	//
	PHP_FE(biys_get_volume,	NULL)	//
	PHP_FE(biys_set_volume,	NULL)	//
	PHP_FE(biys_play_file,	NULL)		//
	PHP_FE(biys_play_next,	NULL)		//
	PHP_FE(biys_play_previous,	NULL)	//
	PHP_FE(biys_get_current_id,	NULL)	//
	PHP_FE(biys_get_current_index,	NULL)	//
	PHP_FE(biys_get_attributes_for_id,	NULL)	//
	PHP_FE(biys_get_trivia_for_id,	NULL)	//
	PHP_FE(biys_get_attributes_for_path,	NULL) //

	PHP_FE(biys_set_name,	NULL)           //
	PHP_FE(biys_get_name,	NULL)			//
	PHP_FE(biys_get_path,	NULL)			//
	PHP_FE(biys_get_id_for_index,	NULL)		//

	PHP_FE(biys_list_files,	NULL)	//
	PHP_FE(biys_list_genres,	NULL)	//
	PHP_FE(biys_list_producers,	NULL)	//
	PHP_FE(biys_list_artists,	NULL)	//
	PHP_FE(biys_list_albums,	NULL)	//
	PHP_FE(biys_list_years,	NULL)		//

	PHP_FE(biys_xref_genre,	NULL)		//
	PHP_FE(biys_xref_producer,	NULL)	//
	PHP_FE(biys_xref_artist,	NULL)	//
	PHP_FE(biys_xref_album,	NULL)		//
	PHP_FE(biys_xref_year,	NULL)		//

	{NULL, NULL, NULL}	/* Must be the last line in biys_functions[] */
};

zend_module_entry biys_module_entry = {
	"Be in your Stereo",
	biys_functions,
	PHP_MINIT(biys),
	PHP_MSHUTDOWN(biys),
	NULL,
	NULL,
	PHP_MINFO(biys),
	STANDARD_MODULE_PROPERTIES
};
}

#ifdef COMPILE_DL_BIYS
ZEND_GET_MODULE(biys)
#endif




//---------------------------
// beginning of implementation

// high-level stuff used throughout the biys module

#include "HttpUI.h"
#include <Message.h>
#include <Messenger.h>

BMessage reply;
static BMessenger *messenger = NULL;


// Grovels through the Reply, packs the response into an array and returns it.
static inline void return_list(INTERNAL_FUNCTION_PARAMETERS, char *name) {
	if (array_init(return_value) == FAILURE) RETURN_FALSE;	
	
	int32 count;
	type_code type;

	reply.GetInfo(name, &type, &count);
	for (int i=0; i<count; i++) {
		char *item = (char*)reply.FindString(name, i);
		if (item==NULL || item[0]==0) continue;

		add_next_index_string(return_value, item, 1);
	}		
}

/**
	Grovels through the reply message, and packs everything it can into a hash.
*/
static inline void return_hash(INTERNAL_FUNCTION_PARAMETERS) {
	if (array_init(return_value) == FAILURE) RETURN_FALSE;	

	int index=0;
	char *name;
	type_code type;
	while (reply.GetInfo(B_ANY_TYPE, index++, &name, &type) == B_NO_ERROR) {
		switch (type) {
		case B_STRING_TYPE:
			add_assoc_string(return_value, name, (char*)reply.FindString(name), 1);
			break;
		case B_INT32_TYPE:
			add_assoc_long(return_value, name, reply.FindInt32(name));
			break;
		case B_FLOAT_TYPE:
			add_assoc_double(return_value, name, reply.FindFloat(name));
			break;
		}
	}
}

/**
	Takes care of functions that look like
		void somefunction(int playlist=0);
*/
static inline void handle_1param(INTERNAL_FUNCTION_PARAMETERS, int32 msgwhat, bool expectReply=false) {
	if (!messenger) return;
	zval **playlist_arg;

	int args = ZEND_NUM_ARGS();
	if (args>1  || zend_get_parameters_ex(args, &playlist_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	BMessage msg(msgwhat);
	if (args==1) {
		convert_to_long_ex(playlist_arg);
		msg.AddInt32("pl", Z_LVAL_PP(playlist_arg));
	} else {
		msg.AddInt32("pl", 0);
	}
	
	if (expectReply) {
		reply.MakeEmpty();
		messenger->SendMessage(&msg, &reply);
	} else {
		messenger->SendMessage(&msg);
	}
}


/**
	Takes care of functions that look like
		void somefunction(int ID, int playlist=0);
*/
static inline void handle_2param(INTERNAL_FUNCTION_PARAMETERS, int32 msgwhat, bool expectReply=false, char *parameterName=NULL, bool firstParamMayBeZero=false) {
	if (!messenger) return;
	zval **ID_arg, **playlist_arg;

	int args = ZEND_NUM_ARGS();
	if (args==0 || args>2  || zend_get_parameters_ex(args, &ID_arg, &playlist_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(ID_arg);

	if (!firstParamMayBeZero && (ZVAL_IS_NULL(*ID_arg) || (Z_LVAL_PP(ID_arg))==0)) {
		php_error(E_ERROR, "ID can not be null or zero");
		return;
	}

	BMessage msg(msgwhat);
	if (args==2) {
		convert_to_long_ex(playlist_arg);
		msg.AddInt32("pl", Z_LVAL_PP(playlist_arg));
	} else {
		msg.AddInt32("pl", 0);
	}
	
	msg.AddInt32(parameterName ? parameterName : "id", Z_LVAL_PP(ID_arg));

	if (expectReply) {
		reply.MakeEmpty();
		messenger->SendMessage(&msg, &reply);
	} else {
		messenger->SendMessage(&msg);
	}
}

/**
	Takes care of functions that look like
		void somefunction(double something, int playlist=0);
*/
static inline void handle_2param_double(INTERNAL_FUNCTION_PARAMETERS, int32 msgwhat, char *parameterName) {
	if (!messenger) return;
	zval **playlist_arg, **double_arg;
	
	int args = ZEND_NUM_ARGS();
	if (args==0 || args>2  || zend_get_parameters_ex(args, &double_arg, &playlist_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_double_ex(double_arg);

	BMessage msg(msgwhat);
	if (args==2) {
		convert_to_long_ex(playlist_arg);
		msg.AddInt32("pl", Z_LVAL_PP(playlist_arg));
	} else {
		msg.AddInt32("pl", 0);
	}
	msg.AddFloat(parameterName, Z_DVAL_PP(double_arg));
	messenger->SendMessage(&msg);
}

static inline void handle_xref(INTERNAL_FUNCTION_PARAMETERS, int32 what) {
	if (!messenger) return;

	zval **xref_arg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &xref_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(xref_arg);
	BMessage msg(what);
	msg.AddString("xref", Z_STRVAL_PP(xref_arg));
	reply.MakeEmpty();
	messenger->SendMessage(&msg, &reply);
	
	// we have the xref back
	if (array_init(return_value) == FAILURE) RETURN_FALSE;	
	zval *val;
	char *name; 
	uint32 type; 
	int32 count;

	// genres
	MAKE_STD_ZVAL(val);
	array_init(val);
	add_assoc_zval(return_value, "genre", val);
	reply.GetInfo("genre", &type, &count);
	for (int j=0; j<count; j++) {
		const char *name = reply.FindString("genre", j);
		add_next_index_string(val, (char*)name, 1);
	}

	// artists
	MAKE_STD_ZVAL(val);
	array_init(val);
	add_assoc_zval(return_value, "artist", val);
	reply.GetInfo("artist", &type, &count);
	for (int j=0; j<count; j++) {
		const char *name = reply.FindString("artist", j);
		add_next_index_string(val, (char*)name, 1);
	}

	// producers
	MAKE_STD_ZVAL(val);
	array_init(val);
	add_assoc_zval(return_value, "producer", val);
	reply.GetInfo("producer", &type, &count);
	for (int j=0; j<count; j++) {
		const char *name = reply.FindString("producer", j);
		add_next_index_string(val, (char*)name, 1);
	}

	// albums
	MAKE_STD_ZVAL(val);
	array_init(val);
	add_assoc_zval(return_value, "album", val);
	reply.GetInfo("album", &type, &count);
	for (int j=0; j<count; j++) {
		const char *name = reply.FindString("album", j);
		add_next_index_string(val, (char*)name, 1);
	}

	// years
	MAKE_STD_ZVAL(val);
	array_init(val);
	add_assoc_zval(return_value, "year", val);
	reply.GetInfo("year", &type, &count);
	for (int j=0; j<count; j++) {
		const char *name = reply.FindString("year", j);
		add_next_index_string(val, (char*)name, 1);
	}

	// matching files
	MAKE_STD_ZVAL(val);
	array_init(val);
	add_assoc_zval(return_value, "matching", val);
	reply.GetInfo("matching", &type, &count);
	for (int j=0; j<count; j++) {
		const char *name = reply.FindString("matching", j);
		add_next_index_string(val, (char*)name, 1);
	}
}

// end of high level stuff

PHP_MINIT_FUNCTION(biys)
{
	messenger = SLooper::FindOtherEnd("BIYS");
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(biys)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(biys)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "biys support", "enabled");
	php_info_print_table_end();
}

/* {{{ proto int biys_count_playlists()
    */
PHP_FUNCTION(biys_count_playlists)
{
	if (!messenger) return;
	
	BMessage msg(PluginHandle::count_playlists);
	reply.MakeEmpty();
	messenger->SendMessage(&msg, &reply);

	RETURN_LONG(reply.FindInt32("c"));
}
/* }}} */

/* {{{ proto int biys_create_playlist()
    */
PHP_FUNCTION(biys_create_playlist)
{
	if (!messenger) return;

	BMessage msg(PluginHandle::create_playlist);
	reply.MakeEmpty();
	messenger->SendMessage(&msg, &reply);

	RETURN_LONG(reply.FindInt32("i"));
}
/* }}} */

/* {{{ proto void biys_remove_playlist(int playlist=0)
    */
PHP_FUNCTION(biys_remove_playlist)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::remove_playlist);
}
/* }}} */

/* {{{ proto void biys_add(string path, int index=-1, int playlist=0)
    */
PHP_FUNCTION(biys_add)
{
	if (!messenger) return;
	zval **playlist_arg;
	zval **path_arg;
	zval **index_arg;

	BMessage msg(PluginHandle::add);

	int args = ZEND_NUM_ARGS();
	if (args==1 && zend_get_parameters_ex(1, &path_arg) != FAILURE) {
		convert_to_string_ex(path_arg);
		msg.AddString("path", Z_STRVAL_PP(path_arg));
		msg.AddInt32("i", -1);
		msg.AddInt32("pl", 0);
		messenger->SendMessage(&msg);
	} else if (args==2 && zend_get_parameters_ex(2, &path_arg, &index_arg) != FAILURE) {
		convert_to_long_ex(index_arg);
		convert_to_string_ex(path_arg);
		msg.AddString("path", Z_STRVAL_PP(path_arg));
		msg.AddInt32("i", Z_LVAL_PP(index_arg));
		msg.AddInt32("pl", 0);
		messenger->SendMessage(&msg);
	} else if (args==3 && zend_get_parameters_ex(3, &path_arg, &index_arg, &playlist_arg) != FAILURE) {
		convert_to_long_ex(playlist_arg);
		convert_to_string_ex(path_arg);
		convert_to_long_ex(index_arg);
		msg.AddInt32("pl", Z_LVAL_PP(playlist_arg));
		msg.AddString("path", Z_STRVAL_PP(path_arg));
		msg.AddInt32("i", Z_LVAL_PP(index_arg));
		messenger->SendMessage(&msg);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto void biys_remove(int id, int playlist=0)
    */
PHP_FUNCTION(biys_remove)
{
	handle_2param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::remove);
}
/* }}} */

/* {{{ proto void biys_clear(int playlist=0)
    */
PHP_FUNCTION(biys_clear)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::clear);
}
/* }}} */

/* {{{ proto void biys_count(int playlist=0)
    */
PHP_FUNCTION(biys_count)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::count, true);
	RETURN_LONG(reply.FindInt32("c"));
}
/* }}} */

/* {{{ proto void biys_shuffle(int playlist=0)
    */
PHP_FUNCTION(biys_shuffle)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::shuffle);
}
/* }}} */

/* {{{ proto void biys_sort(int playlist=0)
    */
PHP_FUNCTION(biys_sort)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::sort);
}
/* }}} */


/* {{{ proto void biys_play(int playlist=0)
    */
PHP_FUNCTION(biys_play)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::play);
}
/* }}} */

/* {{{ proto void biys_pause(int playlist=0)
    */
PHP_FUNCTION(biys_pause)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::pause);
}
/* }}} */

/* {{{ proto double biys_get_pitch(int playlist=0)
    */
PHP_FUNCTION(biys_get_pitch)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::get_pitch, true);
	RETURN_DOUBLE(reply.FindFloat("pitch"));
}
/* }}} */

/* {{{ proto void biys_set_pitch(double pitch, int playlist=0)
    */
PHP_FUNCTION(biys_set_pitch)
{
	handle_2param_double(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::set_pitch, "pitch");
}
/* }}} */

/* {{{ proto double biys_get_position(int playlist=0)
    */
PHP_FUNCTION(biys_get_position)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::get_position, true);
	RETURN_DOUBLE(reply.FindFloat("pos"));
}
/* }}} */

/* {{{ proto void biys_set_position(double position, int playlist=0)
    */
PHP_FUNCTION(biys_set_position)
{
	handle_2param_double(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::set_position, "pos");
}
/* }}} */

/* {{{ proto double biys_get_volume(int playlist=0)
    */
PHP_FUNCTION(biys_get_volume)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::get_volume, true);
	RETURN_DOUBLE(reply.FindFloat("vol"));
}
/* }}} */

/* {{{ proto void biys_set_volume(double volume, int playlist=0)
    */
PHP_FUNCTION(biys_set_volume)
{
	handle_2param_double(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::set_volume, "vol");
}
/* }}} */

/* {{{ proto void biys_play_file(int ID, int playlist=0)
    */
PHP_FUNCTION(biys_play_file)
{
	handle_2param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::play_file);
}
/* }}} */

/* {{{ proto void biys_play_next(int playlist=0)
    */
PHP_FUNCTION(biys_play_next)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::play_next);
}
/* }}} */

/* {{{ proto void biys_play_previous(int playlist=0)
    */
PHP_FUNCTION(biys_play_previous)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::play_previous);
}
/* }}} */

/* {{{ proto int biys_get_current_id(int playlist=0)
    */
PHP_FUNCTION(biys_get_current_id)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::get_current_id, true);
	RETURN_LONG(reply.FindInt32("id"));
}
/* }}} */

/* {{{ proto int biys_get_current_index(int playlist=0)
    */
PHP_FUNCTION(biys_get_current_index)
{
	handle_1param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::get_current_index, true);
	RETURN_LONG(reply.FindInt32("i"));
}
/* }}} */

/* {{{ proto int biys_get_id_for_index(int index, int playlist=0)
    */
PHP_FUNCTION(biys_get_id_for_index)
{
	handle_2param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::id_for_index, true, "i", true);
	RETURN_LONG(reply.FindInt32("id"));
}
/* }}} */

/* {{{ proto array biys_get_attributes_for_id(int ID, int playlist=0)
    */
PHP_FUNCTION(biys_get_attributes_for_id)
{
	handle_2param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::attributes_for_id, true);

	return_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto array biys_get_trivia_for_id(int ID, int playlist=0)
    */
PHP_FUNCTION(biys_get_trivia_for_id)
{
	handle_2param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::trivia_for_id, true);

	return_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */


/* {{{ proto void biys_set_name(int ID, string newName, int playlist=0)
    */
PHP_FUNCTION(biys_set_name)
{
	if (!messenger) return;
	zval **ID_arg, **newName_arg, **playlist_arg;

	int args = ZEND_NUM_ARGS();
	if (args<2 || args>3 || zend_get_parameters_ex(args, &ID_arg, &newName_arg, &playlist_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(ID_arg);
	convert_to_string_ex(newName_arg);

	BMessage msg(PluginHandle::set_name);
	if (args==2) {
		convert_to_long_ex(playlist_arg);
		msg.AddInt32("pl", Z_LVAL_PP(playlist_arg));
	} else {
		msg.AddInt32("pl", 0);
	}
	
	msg.AddInt32("id", Z_LVAL_PP(ID_arg));
	msg.AddString("n", Z_STRVAL_PP(newName_arg));
	messenger->SendMessage(&msg);
}
/* }}} */

/* {{{ proto string biys_get_name(int ID, int playlist=0)
    */
PHP_FUNCTION(biys_get_name)
{
	handle_2param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::get_name, true);
	RETURN_STRING((char*)reply.FindString("n"), 1);
}
/* }}} */

/* {{{ proto string biys_get_path(int playlist, int ID)
    */
PHP_FUNCTION(biys_get_path)
{
	handle_2param(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::get_path, true);
	RETURN_STRING((char*)reply.FindString("path"), 1);
}
/* }}} */


/* {{{ proto array biys_list_files()
    */
PHP_FUNCTION(biys_list_files)
{
	if (!messenger) return;

	reply.MakeEmpty();
	messenger->SendMessage(PluginHandle::list_files, &reply);
	return_list(INTERNAL_FUNCTION_PARAM_PASSTHRU, "files");
}
/* }}} */

/* {{{ proto array biys_list_genres()
    */
PHP_FUNCTION(biys_list_genres)
{
	if (!messenger) return;

	reply.MakeEmpty();
	messenger->SendMessage(PluginHandle::list_genres, &reply);
	return_list(INTERNAL_FUNCTION_PARAM_PASSTHRU, "genre");
}
/* }}} */

/* {{{ proto array biys_list_producers()
    */
PHP_FUNCTION(biys_list_producers)
{
	if (!messenger) return;

	reply.MakeEmpty();
	messenger->SendMessage(PluginHandle::list_producers, &reply);
	return_list(INTERNAL_FUNCTION_PARAM_PASSTHRU, "producer");
}
/* }}} */

/* {{{ proto array biys_list_artists()
    */
PHP_FUNCTION(biys_list_artists)
{
	if (!messenger) return;

	reply.MakeEmpty();
	messenger->SendMessage(PluginHandle::list_artists, &reply);
	return_list(INTERNAL_FUNCTION_PARAM_PASSTHRU, "artist");
}
/* }}} */

/* {{{ proto array biys_list_albums()
    */
PHP_FUNCTION(biys_list_albums)
{
	if (!messenger) return;

	reply.MakeEmpty();
	messenger->SendMessage(PluginHandle::list_albums, &reply);
	return_list(INTERNAL_FUNCTION_PARAM_PASSTHRU, "album");
}
/* }}} */

/* {{{ proto array biys_list_years()
    */
PHP_FUNCTION(biys_list_years)
{
	if (!messenger) return;

	reply.MakeEmpty();
	messenger->SendMessage(PluginHandle::list_years, &reply);
	return_list(INTERNAL_FUNCTION_PARAM_PASSTHRU, "year");
}
/* }}} */

/* {{{ proto array biys_xref_genre(string genre)
    */
PHP_FUNCTION(biys_xref_genre)
{
	handle_xref(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::xref_genre);
}
/* }}} */

/* {{{ proto array biys_xref_producer(string producer)
    */
PHP_FUNCTION(biys_xref_producer)
{
	handle_xref(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::xref_producer);
}
/* }}} */

/* {{{ proto array biys_xref_artist(string artist)
    */
PHP_FUNCTION(biys_xref_artist)
{
	handle_xref(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::xref_artist);
}
/* }}} */

/* {{{ proto array biys_xref_album(string album)
    */
PHP_FUNCTION(biys_xref_album)
{
	handle_xref(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::xref_album);
}
/* }}} */

/* {{{ proto array biys_xref_year(string year)
    */
PHP_FUNCTION(biys_xref_year)
{
	handle_xref(INTERNAL_FUNCTION_PARAM_PASSTHRU, PluginHandle::xref_year);
}
/* }}} */

/* {{{ proto array biys_get_attributes_for_path(string path)
    */
PHP_FUNCTION(biys_get_attributes_for_path)
{
	if (!messenger) return;
	zval **path_arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &path_arg) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(path_arg);
	
	BMessage msg(PluginHandle::attributes_for_path);
	msg.AddString("path", Z_STRVAL_PP(path_arg));
	messenger->SendMessage(&msg, &reply);

	return_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
