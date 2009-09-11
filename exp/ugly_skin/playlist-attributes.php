<?
	$xref = stripslashes($xref);	# urlencode adds \ before ' - this takes it out.
	
	echo "<h1>Full playlist attributes</h1>";
	$result = biys_get_playlist_attributes($pid);

	echo 'Raw result:<pre>';
	print_r($result);
	echo '</pre>';
?>
