#include "../../pages.h"
#include <cstdlib>
#include "../../battle.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageBattleReport2::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CString
		buf;

	QUERY( "LOG_ID", logIDstring );
	if( logIDstring ) {
		int logID = as_atoi( logIDstring ) ;
		CBattleRecord
			*record = (CBattleRecord*)BATTLE_RECORD_TABLE->get_by_id(logID);
		if( record != NULL )
		{
			ITEM( "LOG_URL", record->get_record_file());
			ITEM("PLAYER_ID", aPlayer->get_game_id());
		} else
		{
			ITEM( "ERROR_MESSAGE", "Battle report you requested not exist" );
			return output( "error.html" );
		}

	}

//	system_log("end page handler %s", get_name());

	return output( "war/battle_report2.html" );
}

