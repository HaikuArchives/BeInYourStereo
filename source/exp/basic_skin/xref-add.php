<?
	$function = "biys_xref_$thing";
	$what = stripslashes($what);
	$result = $function($what);
	
	foreach ($result['matching'] AS $path) {
		biys_add($path);
	}
	header("Location: index.php");
?>
