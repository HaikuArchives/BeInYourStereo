<?
include "prefs.h";

$prefs = read_prefs();

foreach ($prefs AS $key=>$ignored) {
	$prefs[$key] = $$key;
}

write_prefs($prefs);

header("Location: index.php");
?>
