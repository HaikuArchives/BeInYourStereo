<?
	include "prefs.h";
	$prefs = read_prefs();

	$files = biys_list_files();

	shuffle($files);

	for ($i=0; $i<$prefs[random]; $i++) {
		biys_add($files[$i]);
	}

	if (biys_get_pitch()==0) {
		biys_play(biys_get_id_for_index(0));
	}
	header("Location: index.php");

?>
