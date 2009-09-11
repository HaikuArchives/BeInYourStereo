<?
	$path = biys_get_path($id);
	biys_remove($id);
	biys_add($path, $to);
	header("Location: index.php");
?>