<?  $what = stripslashes($what);
    $title = "$what album";
    include "head.h";

	$xref = biys_xref_album($what);
	echo "<h1>$what</h1>";
	if ($xref['artist'][0]) {
		echo 'Artist: <a href="xref-artist.php?what='.urlencode($xref['artist'][0]).'">'.$xref['artist'][0].'</a> | ';
	}
	if ($xref['genre'][0]) {
		echo 'Genre: <a href="xref-genre.php?what='.urlencode($xref['genre'][0]).'">'.$xref['genre'][0].'</a> | ';
	}
	if ($xref['year'][0]) {
		echo 'Released: <a href="xref-year.php?what='.urlencode($xref['year'][0]).'">'.$xref['year'][0].'</a><p>';
	}
	$count = count($xref['matching']);
?>

There <? echo ($count==1 ? "is one matching file" : "are $count matching files"); ?>.  You can 
<b><a href="xref-add.php?thing=album&what=<? echo urlencode($what) ?>">add them
all</a></b> or go to the <B><a href="index.php">playlist</a></b> or <b><a href="list.php">music
collection</a></b>.
<P>

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
