<? $title = "Attribute Editing"; include "head.h"; ?>

<h1>Attribute editing</h1>
File: <? echo $path = biys_get_path($id); ?>
<P>

<form action="edit-2.php" method=post>
<input type=hidden name="path" value="<? echo $path ?>">

<?
	$atts = biys_get_attributes_for_id($id);
	$atts['Audio:Genre'] .= '';
	$atts['Audio:Artist'] .= '';
	$atts['Audio:Album'] .= '';
	$atts['Audio:Year'] .= '';
	$atts['Audio:Title'] .= '';
	$atts['Audio:Track'] .= '';
	
	foreach ($atts AS $key => $value) {
		if (substr($key, 0, 5) == "Audio") {
			if ($key == "Audio:Bitrate" || $key =="Audio:Length" || $key=="Audio:Frequency") continue; 
			if ($key == "Audio:Genre") {
				$genres = biys_list_genres();
				echo 'Genre: <select name="Audio:Genre"><option name="">';
				foreach ($genres AS $g) {
					echo '<option ';
					if ($value == $g) {
						echo " SELECTED ";
					}
					echo "value=\"$g\">$g\n";
				}
				echo '</select> or type one in: <input type=text name="genre2"><br>';
			} else {
				echo substr($key, 6).': ';
				echo '<input type=text name="'.urlencode($key).'" value="'.$value.'"><br>';
			}
		}
	}
?>

<P>
<input type=submit value="Make Changes">
</pre>

<? include "tail.h"; ?>
