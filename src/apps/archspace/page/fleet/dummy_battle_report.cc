bool
CPageDummyBattleReport::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

//	system_log("end page handler %s", get_name());

	return output( "fleet/dummy_battle_report.html" );
}

/* end of file dummy_battle_report.cc */
