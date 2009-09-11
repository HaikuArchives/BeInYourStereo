<?
include_once "utils.h";

function render_table($array, $name, $url) {
	if (count($array)) {
		echo '<table border=0 cellspacing="1" cellpadding="4"><tr bgcolor="#101030"><th colspan="6"><big>'.$name.'</big></th></tr>';
		$counter = 0;
		foreach ($array AS $g) {
			if ($counter % 6 == 0) {
				if ($counter % 12 == 6) 
					echo '<tr bgcolor="#103010">';
				else 
					echo '<tr>';
			}

			echo '<td nowrap><a href="'.$url.'?what='.urlencode($g).'">'.$g.'</a></td>';
			if (++$counter % 6 == 0) echo '</tr>';
			if ($counter % 600 == 0) {
				echo '</table><table>';
			}
		}
		
		while ($counter++ % 6 != 0) {
			echo '<td> </td>';
		}
		
		echo '</table><p>';
	}
}
?>
<html><head><title>Be in your Stereo: <? echo $title; ?></title></head>
<body bgcolor="#000000" text="#c0c080" vlink="#e0c080" link="#e0c080">
