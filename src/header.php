<HTML>
<HEAD>
</HEAD>
<?
include "libportal.php";
$is_admin = get_is_admin ( "localhost 5000");

if ($is_admin != "YES")
{
$msg = "You do not have the proper permissions to view this page";
echo <<<HTML
<BODY BGCOLOR="000000" LINK="999999" VLINK="999999" ALINK="999999">
<TABLE WIDTH="610" BORDER="0" CELLSPACING="0" CELLPADDING="0">
  <TR ALIGN="CENTER">
    <TD>&nbsp;</TD>
  </TR>
  <TR ALIGN="CENTER">
    <TD>&nbsp;</TD>
  </TR>
  <TR>
    <TD>&nbsp;</TD>
  </TR>
  <TR>
    <TD>&nbsp;</TD>
  </TR>
  <TR ALIGN="CENTER">
    <TD CLASS="maintext"><IMG SRC="/image/as_login/result/error.gif" WIDTH="314" HEIGHT="153">
    </TD>
  </TR>
  <TR ALIGN="CENTER">
    <TD>&nbsp; </TD>
  </TR>
  <TR ALIGN="CENTER">
    <TD><FONT face="verdana, arial, helvetica" color=white><small>{$msg}</small></FONT></TD>
  </TR>
  <TR ALIGN="CENTER">
    <TD>&nbsp; </TD>
  </TR>
  <TR ALIGN="CENTER">
    <TD><A HREF="/main.php"><IMG SRC="/image/as_login/bu_back.gif" BORDER="0"></A></TD>
  </TR>
  <TR ALIGN="CENTER">
    <TD>&nbsp; </TD>
  </TR>
</TABLE>
</BODY>
</HTML>
HTML;
exit();
}
else
{
 echo "<LINK REL=STYLESHEET HREF=\"doxygen.css\" TYPE=\"text/css\"><BODY>";
}
?>
