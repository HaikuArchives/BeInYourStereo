<h2>Rename</h2>

This just renames the file as SoundPlay shows it.  It doesn't rename the file on disk.
<P>

<form action="rename-2.php" method=GET>
<input type=hidden name="pid" value="<? echo $pid ?>">
<input type=hidden name="id" value="<? echo $id ?>">
New name:<input type=text name="newname" size=60 value="<? echo biys_get_name($id, $pid); ?>">
</form>
