bool
CPageDummyBattleLog::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CString
		buf = "battlefield/Penthouse
		fleetgroupname/Tare Pandas
		fleetgroupadmiral/out42der
		endturn/400
		map/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/0/1/2/-1/-1/-1/-1/-1/-1/-1/3/4/5/-1/-
		1/-1/-1/-1/-1/-1/6/7/8/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/
		-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/9/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/
		-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1/-1
		fleetleader/1/Tare Leader/out42der/Tare/40/2000/5000/80/20/0
		fleet/2/Tare Jacos/out52der/Tare/40/2000/8000/20/20/0
		fleetleader/-1/Tare Leader/firiel/Tare/40/8000/5000/15/20/0
		fleet/-2/Tare Jacos/firiel/Tare/40/7000/2000/15/20/0
		event/100/1/degree/5
		event/105/1/degree/10
		event/110/-1/degree/170
		event/115/2/degree/350
		event/120/-2/degree/190
		weapon/100/1/-1/0/120
		weapon/105/-2/2/1/125";
	ITEM( "BATTLELOG", buf );

//	system_log("end page handler %s", get_name());

	return output( "fleet/dummy_battle_log.html" );
}

/* end of file dummy_battle_log.cc */
