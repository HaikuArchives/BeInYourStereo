<?
	$genres = biys_list_genres();
	$artists = biys_list_artists();
	$producers = biys_list_producers();
	$years = biys_list_years();
	$albums = biys_list_albums();

?>

<h2>Genres</h2>
<? foreach ($genres AS $x): ?>
<? 	echo '<a href="xref.php?what=genre&xref='.urlencode($x)."\">$x</a>"; ?> | 
<? endforeach ?>



<h2>Artists</h2>
<? foreach ($artists AS $x): ?>
<? 	echo '<a href="xref.php?what=artist&xref='.urlencode($x)."\">$x</a>"; ?> | 
<? endforeach ?>



<h2>Producers</h2>
<? foreach ($producers AS $x): ?>
<? 	echo '<a href="xref.php?what=producer&xref='.urlencode($x)."\">$x</a>"; ?> | 
<? endforeach ?>



<h2>Years</h2>
<? foreach ($years AS $x): ?>
<? 	echo '<a href="xref.php?what=year&xref='.urlencode($x)."\">$x</a>"; ?> | 
<? endforeach ?>

<h2>Albums</h2>
<? foreach ($albums AS $x): ?>
<? 	echo '<a href="xref.php?what=album&xref='.urlencode($x)."\">$x</a>"; ?> | 
<? endforeach ?>


