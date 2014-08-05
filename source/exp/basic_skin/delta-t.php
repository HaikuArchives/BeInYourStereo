<?
	$id = biys_get_current_id();
	$current = biys_get_attributes_for_id($id);

	$duration = round($current['framecount'] / $current['samplerate']);
	$current = biys_get_position() * $duration;
	biys_set_position( ($current + $time) / $duration );

	header("Location: index.php");
?>