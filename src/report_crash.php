<?
$TO = "someone@example.com";
$PRE_S = "AS Crash Detected at ";
$TIME = shell_exec('date');
$PRE_B = "Contents:\n";
$CONTENTS = shell_exec("cat /var/log/archspace/lastcrash.log");
//echo "$TO  $PRE_S  $TIME  $PRE_B $CONTENTS";
mail($TO,"$PRE_S$TIME","$PRE_B$CONTENTS", "From: AS Monitor <root@as.kenisware.org>\r\n");
?>
