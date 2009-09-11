<h1>Be in your Stereo 2.0</h1>

<img src="3dzooicon.gif">

There are <? echo biys_count_playlists(); ?> playlists.  Want to <A href="playlist-create.php">create a new one?</a>.

<ul>
<? for ($i=0; $i<biys_count_playlists(); $i++) {
	echo "<li><a href=\"playlist-one.php?pid=$i\">Playlist $i</a>";
}
 ?>
</ul>

<P>What does <a href="info.php">phpinfo</a> say?

<P><a href="list.php">Take a look at your music collection.</a>

<form action="get-attributes.php">Type in a path, and I'll see
what its attributes are: <input type=text name=path></form>
