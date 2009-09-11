<?
$title = "Preferences";
include "head.h";

include "prefs.h";

$prefs = read_prefs();
?>

<h2>Preferences</h2>

<form action="prefs-2.php" method=POST>

Number of files to add when adding files at random:
<input type=text name="random" size=3 value="<? echo $prefs['random'] ?>">

<P>

On music collection page, show: <br>
<input type=checkbox name="list_year" size=3 value="ues" <? echo ($prefs['list_year'] ? "CHECKED":"") ?>>
Year attribute

<input type=checkbox name="list_genre" size=3 value="yes" <? echo $prefs['list_genre'] ?"CHECKED":"" ?>>
Genre attribute

<input type=checkbox name="list_producer" size=3 value="yes" <? echo $prefs['list_producer'] ? "CHECKED":"" ?>>
Producer attribute


<P>



<P>
<input type=submit>
</form>
	

<hr>
You can <b><a href="customize.php">customize this skin</a></b> even more...

<? include "tail.h"; ?>