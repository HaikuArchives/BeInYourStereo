<?
	$newplaylist = biys_create_playlist();
?>

There, I created a new playlist.  It shall be known as number <? echo $newplaylist; ?>.
<a href="playlist-one.php?pid=<? echo $newplaylist ?>">Wanna see?</a>
