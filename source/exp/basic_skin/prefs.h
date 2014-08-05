<?
# If you're looking at this file, then you're probably wondering
# how to do preferences yourself.
#
# To use it for yourself, decide:
#   a) what your skin's parameters are
#   b) what the default values are
# Then place those defaults in the defaults array below.
# Then drop prefs.php from this directory into your skin, editing
#   it to exhibit your preference settings.
# Don't forget prefs-2.php. It will probably be sufficient for strings, checkboxes, and
# radio button preferences.
#
# Usage from your skin:
# The way to implement persistent skin preferences.
# Usage:
#   $p = read_prefs();
#   $p['some_int'] = 5;
#   $p['some_string'] = "this is a string";
#   $p['some_array'] = array("this", "is", "an", "array");
#   write_prefs($p);
#

function read_prefs() {
	if (!file_exists("skin_prefs.bin")) {
		return array();
	}

	$fd = fopen("skin_prefs.bin", 'r');

	$sz = filesize('skin_prefs.bin');
	$ser = fread($fd, $sz);
	$prefs = unserialize($ser);

	fclose($fd);
	
	# define the defaults
	$defaults = array(
		'random' => 50,
		
		'list_year' => '0',
		'list_genre' => '1',
		'list_producer' => '1'
	);

	# check the defaults
	foreach ($defaults AS $item => $value) {
		if (in_array($item, $prefs)) {
			print "Setting $item = $value";
			$prefs[$item] = $value;
		}
	}

	return $prefs;
}

function write_prefs($prefs) {
	$fd = fopen('skin_prefs.bin', 'w');
	fwrite($fd, serialize($prefs));
	fclose($fd);
}
