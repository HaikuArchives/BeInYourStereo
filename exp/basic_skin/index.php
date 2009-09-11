<? $title = "Start"; include "head.h"; ?>

<?
	$id = biys_get_current_id();
	$current = biys_get_attributes_for_id($id);
?>

<TABLE CELLPADDING=1 CELLSPACING=0 BORDER=0  width="33%"><TR><TD BGCOLOR="#80e080" align=center>
<TABLE CELLPADDING=2 CELLSPACING=1 BORDER=0 WIDTH="100%"><TR><TD BGCOLOR="#000000" align=center VALIGN=CENTER WIDTH="100%">
<img src="img/biys.png" alt="Be in your Stereo" width=389 height=60><br>

<? if ($id != -1): ?>
<big><big><? echo pretty($current) ?></big></big><Br>
<?
	$trivia = biys_get_trivia_for_id($id);
	$seconds = round($trivia['framecount'] / $trivia['samplerate']);
	$elapsed = biys_get_position() * $seconds;

	$minutes = floor($seconds / 60);
	$seconds = $seconds % 60;
	$eminutes = floor($elapsed / 60);
	$eseconds = $elapsed % 60;
	
	echo 'Time: '.$eminutes.':'.sprintf('%02d', $eseconds).' / '.$minutes.':'.sprintf('%02d', $seconds);
?>
<br>
<tt><a href="previous.php">|&lt;&lt;</a>
<a href="pitchtoggle.php?pitch=-4">&lt;&lt;</a>
<a href="pitchtoggle.php?pitch=-1">&lt;</a>
<a href="pitchtoggle.php?pitch=0">''</a>
<a href="pitchtoggle.php?pitch=4">&gt;&gt;</a>
<a href="next.php">&gt;&gt;|</a>
</tt>
<? else: ?>
There is nothing playing.
<? endif ?>

</TD></tr></table></td></tr></table>

<? if ($id != -1): ?>
<p><Tt>
&nbsp;Volume : <A href="delta-v.php?percent=-15">&lt;15</a> <?
	$markers = 16;
	$volume = biys_get_volume();
	$position = round($volume * $markers/2);

	for ($i=0; $i<=$markers; $i++) {
		echo '<a href="volume.php?pos='.(200/$markers)*$i.'">';
		if ($i == 8) {
			echo '~';
		} elseif ($i == $position) {
			echo '+';
		} else {
			echo '-';
		}
		echo '</a>';
	}
?>
 <A href="delta-v.php?percent=15">15&gt;</a>
<? echo round($volume*100) ?>%
<br>
Position: <A href="delta-t.php?time=-30">&lt;30</a> <?
	$markers = 16;
	$position = round(biys_get_position() * $markers);
	
	for ($i=0; $i<=$markers; $i++) {
		echo '<a href="position.php?pos='.(100/$markers)*$i.'">';
		if ($i == $position) {
			echo '+';
		} else {
			echo '-';
		}
		echo '</a>';
	}
?> <A href="delta-t.php?time=30">30&gt;</a>
</tt>
<? endif ?>

<P>
<br clear=left clear=right>

<TABLE CELLPADDING=1 CELLSPACING=0 BORDER=0 width="33%"><TR><TD BGCOLOR="#8080c0" align=center>
<TABLE CELLPADDING=2 CELLSPACING=1 BORDER=0 WIDTH="100%"><TR><TD BGCOLOR="#000000" align=center VALIGN=CENTER WIDTH="100%">
<big><big>Playlist</big></big>
<br><a href="clear.php">zap</a> | <a href="sort.php">sort</a> | <a href="shuffle.php">shuffle</a> | <a href="list.php">add files</a>
| <a href="randomfiles.php">random</a> | <a
href="prefs.php">preferences</a>
</TD></tr></table></td></tr></table>

<?
	$limit = 10;
	
	# figure out where we are in the playlist
	$count = biys_count();
	$current = biys_get_current_index();

	# handle scrolling requests and stuff
	if ($start<0) $start=0;
	if ($start>=$count) unset($start);
	
	if (isset($start)) {
		$end = $start + $limit * 2;
	} elseif ($count > $limit*2) {
		$start = $current-$limit;
		$end = min($current + $limit, $count);
	} else {
		$start = 0;
		$end = $count;
	}
	
	# then constrain start and end to fit what's available
	if ($start<0) {
		# slide the bottom down if you're past the top of the list
		$end -= $start;
		$start = 0;
	}
	$end = min($end, $count);

	if ($start != 0) {
		print '.... '.($start).' more files <b><a href="index.php?start='.($start-$limit*2-1).'">above</a></b> ...<br>';
	}	
	for ($i=$start; $i<$end; $i++) {
		$id = biys_get_id_for_index($i);
		$info = biys_get_attributes_for_id($id);


		if ($i != 0) { echo "<a href=\"move.php?id=$id&to=".($i-1)."\"><img border=0 src=\"img/up.png\" alt=\"move up\"></a> "; } else { echo '&nbsp;&nbsp;'; }
		if ($i != $count-1) { echo "<a href=\"move.php?id=$id&to=".($i+1)."\"><img border=0 src=\"img/down.png\" alt=\"move down\"></a> "; } else { echo '&nbsp;&nbsp;'; }

		if ($i == $current) echo "<font color=\"#0aa0ff\">";
		echo "<a href=\"play.php?id=$id\">".pretty($info).'</a> &middot; ';
		if ($i == $current) echo "</font>";

		echo "<a href=\"remove.php?id=$id\">remove</a> &middot; ";
		echo "<a href=\"edit.php?id=$id\">edit</a> &middot; ";

		echo '</b><br>';
	}
	if ($end != $count) {
		print '.... '.($count-$end).' more files <b><a href="index.php?start='.($end-1).'">below</a></b> ...<br>';
	}	
?>
</table>


<? include "tail.h"; ?>

