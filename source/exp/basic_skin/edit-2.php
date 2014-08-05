<?
	include_once "utils.h";
	
	$path = fix_path($path);
	
	foreach ($HTTP_GET_VARS AS $key => $value) {
		$key = urldecode($key);
		$value = stripslashes($value);
		if (substr($key, 0, 5) != "Audio") continue;
		
		if ($key == "Audio:Genre") {
			if ($genre2 != "") $value = $genre2;
		}
		system("addattr $key \"$value\" \"$path\"");
	}
	header("Location: index.php");
?>	