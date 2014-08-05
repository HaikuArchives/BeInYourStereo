<?  $what = stripslashes($what);
    $title = "$what";
    include "head.h";

	$xref = biys_xref_genre($what);
	echo "<h1>$what</h1>";
	$count = count($xref['matching']);
?>

There <? echo ($count==1 ? "is one matching file" : "are $count matching files"); ?>.  You can 
<b><a href="xref-add.php?thing=genre&what=<? echo urlencode($what) ?>">add them
all</a></b> or go to the
<b><a href="index.php">playlist</a></b> or <B>
<a href="list.php">music collection</a></b>.

<P>
<?
	render_table($xref['artist'], "Artists", 'xref-artist.php');
	render_table($xref['album'], "Albums", 'xref-album.php');
	render_table($xref['producer'], "Producers", 'xref-producer.php');
	render_table($xref['year'], "Years", 'xref-year.php');
?>

<? if (count($xref['matching']>60)): ?>
	<h3>Matching files</h3><ul>
	<? foreach ($xref['matching'] AS $file): ?>
	<? echo '<li>'.pretty(biys_get_attributes_for_path($file));
	   $url = 'path-add.php?path='.urlencode($file);
	   echo " (<b><a href=\"$url\">add</a></b>) ";
	   echo "(<b><a href=\"$url&play=1\">play</a></b>)\n";
	?>
	<? endforeach ?>
	</ul>
<? endif ?>


<? include "tail.h"; ?>
