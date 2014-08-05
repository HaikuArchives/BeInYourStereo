<?  $what = stripslashes($what);
    $title = "$what";
    include "head.h";

	$xref = biys_xref_producer($what);
	echo "<h1>$what</h1>";
	$count = count($xref['matching']);
?>

There <? echo ($count==1 ? "is one matching file" : "are $count matching files"); ?>.  You can 
<b><a href="xref-add.php?thing=producer&what=<? echo urlencode($what) ?>">add them
all</a></b> or go to the <B><a href="index.php">playlist</a></b> or <b><a href="list.php">music
collection</a></b>.

Check out their listing <B><a href="xref-artist.php?what=<? echo urlencode($what) ?>">as
artist</a></b>.<P>


<P>
<?
	render_table($xref['artist'], "Has remixed these artists", 'xref-artist.php');
	render_table($xref['album'], "Produced these albums", 'xref-album.php');
	render_table($xref['genre'], "Genres", 'xref-genre.php');
	render_table($xref['year'], "Years", 'xref-year.php');
?>

<? if (count($xref['matching']>60)): ?>
	<h3>Matching files</h3><ul>
	<? foreach ($xref['matching'] AS $file): ?>
	<? echo '<li>'.pretty(biys_get_attributes_for_path($file));
	   $url = 'path-add.php?path='.urlencode($file);
	   echo " (<b><a href=\"$url\">add</a></b>) ";
	   echo "(<b><a href=\"$url&play=1\">play</a>)</b>\n";
	?>
	<? endforeach ?>
	</ul>
<? endif ?>


<? include "tail.h"; ?>
