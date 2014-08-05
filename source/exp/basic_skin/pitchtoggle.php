<?
	if (biys_get_pitch() != $pitch) {
		biys_set_pitch($pitch);
	} else {
		biys_set_pitch(1.0);
	}
	header("Location: index.php");
?>