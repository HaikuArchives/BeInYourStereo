<?
	biys_add(stripslashes($path));
	if ($play) {
		biys_play_file(biys_get_id_for_index(biys_count()-1));
	}
	
	header("Location: index.php");	
?>

