#include "council.h"
#include <cstdlib>
#include "archspace.h"
#include "game.h"

CAdmission::CAdmission()
{
	mPlayerID = 0;
	mCouncilID = 0;
	mTime = 0;
}

CAdmission::CAdmission(MYSQL_ROW aRow)
{
	enum
	{
		FIELD_PLAYER_ID = 0,
		FIELD_COUNCIL_ID,
		FIELD_TIME,
		FIELD_CONTENT
	};

	mPlayerID = atoi(aRow[FIELD_PLAYER_ID]);
	mCouncilID = atoi(aRow[FIELD_COUNCIL_ID]);

	mTime = atoi(aRow[FIELD_TIME]);

	CPlayer *
		Player = PLAYER_TABLE->get_by_game_id(mPlayerID);
	CCouncil *
		Council = COUNCIL_TABLE->get_by_id(mCouncilID);
	mContent = aRow[FIELD_CONTENT];

	if (Player && Council)
	{
		Player->set_admission(this);
		Council->add_admission(this);
	} else {
		SLOG( "WARNING : wrong admission "
					"player[%d] council[%d] time[%d]", 
					mPlayerID, mCouncilID, mTime );
	}
}

CAdmission::~CAdmission()
{
}

CPlayer *
CAdmission::get_player()
{
	return PLAYER_TABLE->get_by_game_id(mPlayerID);
}

CCouncil *
CAdmission::get_council()
{
	return COUNCIL_TABLE->get_by_id(mCouncilID);
}

void 
CAdmission::set_player(CPlayer* aPlayer)
{
	assert(aPlayer);
	mPlayerID = aPlayer->get_game_id(); 
}

void
CAdmission::set_council(CCouncil* aCouncil)
{
	assert(aCouncil);
	mCouncilID = aCouncil->get_id();
}

CString &
CAdmission::query()
{
	static CString
		Query;

	Query.clear();

	switch( type() ){
		case QUERY_INSERT :
			if (add_slashes(mContent).length() == 0)
			{
				Query.format("INSERT INTO admission (player, council, time, content) "
								"VALUES ( %d, %d, %d, '' )", 
								mPlayerID,
								mCouncilID,
								mTime);
			}
			else
			{
				Query.format("INSERT INTO admission (player, council, time, content) "
								"VALUES ( %d, %d, %d, '%s' )", 
								mPlayerID,
								mCouncilID,
								mTime,
								(char*)add_slashes(mContent));
			}

			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM admission "
						"WHERE player = %d AND council = %d", 
							mPlayerID,
							mCouncilID );
			break;
		default:
			SLOG("WARNING: unknown query type in CAdmission(%d)", type());
	}

	mStoreFlag.clear();

	return Query;
}

const char*
CAdmission::get_time_string()
{
	static char TimeString[50];
	struct tm *Time;

	Time = localtime(&mTime);
	strftime(TimeString, 50, "%Y/%m/%d/%H:%M", Time);

	return TimeString;	
}
