<?
function fix_path($path) {
	$path = stripslashes($path);
	$path = str_replace("`", "\`", $path);
	return $path;
}

function pretty($array) {
	if ($array['Audio:Producer'] != "") {
		$first = $array['Audio:Producer'].' - ';
	} elseif ($array['Audio:Artist']) {
		$first = $array['Audio:Artist'].' - ';
	}

	if ($array['Audio:Album']) {
		$first .= $array['Audio:Album'].' - ';
	}
	
	if ($array['Audio:Title']) {
		$first .= $array['Audio:Title'];
	}

	if ($first) return $first;

	$path = $array['name'];
	if (substr($path, 4) == "http") {
		return $path;
	} else {
		return basename($path);
	}
}

?>