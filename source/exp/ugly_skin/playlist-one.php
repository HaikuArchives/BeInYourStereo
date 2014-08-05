<?
	$count = biys_count($pid);
	$pidvar = "?pid=$pid";
?>

<h1>Playlist #<? echo 1+$pid ?></h1>

|<a href="playlist-clear.php<? echo $pidvar ?>">clear</a>|
 |<a href="playlist-sort.php<? echo $pidvar ?>">sort</a>|
 |<a href="playlist-shuffle.php<? echo $pidvar ?>">shuffle</a>|

<br>

|<a href="playlist-previous.php<? echo $pidvar ?>">previous</a>|
|<a href="play.php<? echo $pidvar ?>">play</a>|
|<a href="pause.php<? echo $pidvar ?>">pause</a> |
|<a href="playlist-next.php<? echo $pidvar ?>">next</a>|

<table><tr><td>
	<form action="setpitch.php" method=get>
	<input type=hidden name="pid" value=<? echo $pid ?>>
	Pitch: <input type=text name="pitch" size=3 value="<? echo biys_get_pitch($pid); ?>">
	</form>
</td><td>
	<form action="setposition.php" method=get>
	<input type=hidden name="pid" value=<? echo $pid ?>>
	Track position: <input type=text name="position" size=8 value="<? echo biys_get_position($pid); ?>">
	</form>
</td><td>
	<form action="setvolume.php" method=get>
	<input type=hidden name="pid" value=<? echo $pid ?>>
	Volume: <input type=text name="volume" size=4 value="<? echo biys_get_volume($pid); ?>">
	</form>
</td></tr></table>
<P><? echo $count; ?> entries.

<ul>
<? for ($i=0; $i<$count; $i++): ?>
<li><?
		$id = biys_get_id_for_index($i, $pid);
		echo biys_get_name($id, $pid);
		echo "  (<a href=\"remove.php?pid=$pid&id=$id\">remove</a>)";				
		echo "  (<a href=\"rename.php?pid=$pid&id=$id\">rename</a>)";
		echo "  (<a href=\"playfile.php?pid=$pid&id=$id\">play</a>)";				
		echo "  (move <a href=\"moveup.php?pid=$pid&index=$i\">up</a> <a href=\"movedown.php?pid=$pid&index=$i\">down</a>)";				
		echo "  (<a href=\"trivia.php?pid=$pid&id=$id\">trivia</a>)";
?>
<? endfor ?>
</ul>

<form action="add.php" method=get>
<input type=hidden name="pid" value="<? echo $pid; ?>">
Add a file, directory or shoutcast URL to this playlist: <input type=text name="path">
</form>

Unimportant nonsense:  Current index=<? $index = biys_get_current_index($pid); echo $index; ?>
 Current ID=<? $id = biys_get_current_id($pid); echo $id; ?>

<P>

<a href="/">Back to the index.</a>
