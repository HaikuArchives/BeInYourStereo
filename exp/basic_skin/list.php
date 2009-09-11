<? $title = "Your Music Collection"; include "head.h"; ?>


<TABLE CELLPADDING=1 CELLSPACING=0 BORDER=0 align=right width="33%"><TR><TD BGCOLOR="#80e080" align=center>
<TABLE CELLPADDING=2 CELLSPACING=1 BORDER=0 WIDTH="100%"><TR><TD BGCOLOR="#000000" align=center VALIGN=CENTER WIDTH="100%">
<img src="img/biys.png" width=389 height=60></TD></tr></table></td></tr></table>


<h1>Your Music Collection</h1><a href="index.php">Show playlist.</a>
<br clear=right>
<P>

<? include "prefs.h";
   $prefs = read_prefs();
?>

<?
	if ($prefs[list_genre]) {
		$genres = biys_list_genres();
		render_table($genres, "Genres", "xref-genre.php");
	}
	
	if ($prefs[list_year]) {
		$years = biys_list_years();
		render_table($years, "Years", "xref-year.php");
	}

	if ($prefs[list_producer]) {
		$pr = biys_list_producers();
		render_table($pr, "Producers", "xref-producer.php");
	}
	
	$ar = biys_list_artists();
	render_table($ar, "Artists", "xref-artist.php");

	$al = biys_list_albums();
	render_table($al, "Albums", "xref-album.php");

?>

<? include "tail.h"; ?>
