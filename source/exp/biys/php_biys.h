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

#ifndef PHP_BIYS_H
#define PHP_BIYS_H

extern zend_module_entry biys_module_entry;
#define phpext_biys_ptr &biys_module_entry

#ifdef PHP_WIN32
#define PHP_BIYS_API __declspec(dllexport)
#else
#define PHP_BIYS_API
#endif

PHP_MINIT_FUNCTION(biys);
PHP_MSHUTDOWN_FUNCTION(biys);
PHP_RINIT_FUNCTION(biys);
PHP_RSHUTDOWN_FUNCTION(biys);
PHP_MINFO_FUNCTION(biys);

PHP_FUNCTION(biys_count_playlists);
PHP_FUNCTION(biys_create_playlist);
PHP_FUNCTION(biys_remove_playlist);
PHP_FUNCTION(biys_add);
PHP_FUNCTION(biys_remove);
PHP_FUNCTION(biys_clear);
PHP_FUNCTION(biys_count);
PHP_FUNCTION(biys_shuffle);
PHP_FUNCTION(biys_sort);
PHP_FUNCTION(biys_play);
PHP_FUNCTION(biys_pause);
PHP_FUNCTION(biys_get_pitch);
PHP_FUNCTION(biys_set_pitch);
PHP_FUNCTION(biys_get_position);
PHP_FUNCTION(biys_set_position);
PHP_FUNCTION(biys_get_volume);
PHP_FUNCTION(biys_set_volume);
PHP_FUNCTION(biys_play_file);
PHP_FUNCTION(biys_play_next);
PHP_FUNCTION(biys_play_previous);
PHP_FUNCTION(biys_get_current_id);
PHP_FUNCTION(biys_get_current_index);
PHP_FUNCTION(biys_get_id_for_index);
PHP_FUNCTION(biys_get_trivia_for_id);
PHP_FUNCTION(biys_get_attributes_for_id);
PHP_FUNCTION(biys_get_attributes_for_path);
PHP_FUNCTION(biys_set_name);
PHP_FUNCTION(biys_get_name);
PHP_FUNCTION(biys_get_path);
PHP_FUNCTION(biys_list_files);
PHP_FUNCTION(biys_list_genres);
PHP_FUNCTION(biys_list_producers);
PHP_FUNCTION(biys_list_artists);
PHP_FUNCTION(biys_list_albums);
PHP_FUNCTION(biys_list_years);
PHP_FUNCTION(biys_xref_genre);
PHP_FUNCTION(biys_xref_producer);
PHP_FUNCTION(biys_xref_artist);
PHP_FUNCTION(biys_xref_album);
PHP_FUNCTION(biys_xref_year);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(biys)
	int global_variable;
ZEND_END_MODULE_GLOBALS(biys)
*/

/* In every function that needs to use variables in php_biys_globals,
   do call BIYSLS_FETCH(); after declaring other variables used by
   that function, and always refer to them as BIYSG(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define BIYSG(v) (biys_globals->v)
#define BIYSLS_FETCH() php_biys_globals *biys_globals = ts_resource(biys_globals_id)
#else
#define BIYSG(v) (biys_globals.v)
#define BIYSLS_FETCH()
#endif

#endif	/* PHP_BIYS_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
