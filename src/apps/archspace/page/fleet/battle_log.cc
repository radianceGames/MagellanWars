bool
CPageBattleLog::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CString
		buf,
		buf2;
	QUERY( "LOG_ID", logIDstring );
	if( logIDstring ) {
		//���⿡ �α� ID�˻��ϴ� ��ƾ.
		ITEM( "LOG_ID", logIDstring );
	}

//	system_log("end page handler %s", get_name());

	return output( "fleet/battle_log.html" );
}

/* end of file battle_log.cc */