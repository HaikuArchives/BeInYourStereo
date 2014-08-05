<?
	$id = biys_get_id_for_index($index, $pid);
	if ($id>0) {
		$path = biys_get_path($id, $pid);
		biys_add($path, $index-1, $pid);
		biys_remove($id, $pid);
	}
	header("Location: playlist-one.php?pid=$pid");
?>
