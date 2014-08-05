<?
	$xref = stripslashes($xref);	# urlencode adds \ before ' - this takes it out.
	
	$function = "biys_xref_".$what;
	echo "<h1>Cross-reference $what '$xref'</h1>";
	$result = $function($xref);
	print $result['matching']." matching tracks.<P>";

	$inspect = array('genre', 'year', 'artist', 'producer', 'album');
	foreach ($inspect AS $thing) {
		print "<h2>Matching $thing</h2>";
		foreach ($result[$thing] AS $name) {
			print "$name<br>";
		}
	}

	echo 'Raw result:<pre>';
	print_r($result);
	echo '</pre>';
?>
