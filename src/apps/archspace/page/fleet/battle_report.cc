bool
CPageBattleReport::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());
	CString
		buf,
		buf2;
	// 여기에 목록을 가져와서 디스플레이하는게 들어가야 함.
	buf2.format( "<TR> <TD>%s</TD> <TD>when</TD> <TD>siege</TD> <TD>attack</TD> <TD>win</TD> <TD><FORM ACTION=\"battle_report2.as\" METHOD=POST><INPUT TYPE=HIDDEN NAME=LOG_ID VALUE=1><INPUT TYPE=SUBMIT></FORM></TD> <TD></TD> </TR>", aPlayer->get_name() );

	buf2 = "<TABLE>\n<TR> <TH>Opponent</TH> <TH>Date</TH> <TH>Type</TH> <TH>Side</TH> <TH>Result</TH> <TH>View</TH> <TH>Delete</TH> </TR>\n" + buf2 + "</TABLE>\n";
	ITEM( "BATTLE_LIST", buf2 );	

//	system_log("end page handler %s", get_name());

	return output( "fleet/battle_report.html" );
}

/* end of file battle_report.cc */
