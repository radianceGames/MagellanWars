#include "common.h"
#include "util.h"
#include "relation.h"
#include <cstdlib>
#include "player.h"
#include "archspace.h"
#include "game.h"

TZone gPlayerRelationZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPlayerRelation),
	0,
	0,
	NULL,
	"Zone CPlayerRelation"
};

int CPlayerRelation::mMaxID = 0;

CPlayerRelation::CPlayerRelation()
{
	mPlayer1 = mPlayer2 = NONE;
}

CPlayerRelation::CPlayerRelation(MYSQL_ROW aRow)
{
	enum
	{
		FIELD_ID,
		FIELD_PLAYER1,
		FIELD_PLAYER2,
		FIELD_RELATION,
		FIELD_TIME
	};

	mID = atoi(aRow[FIELD_ID]);
	set_player1_id(atoi(aRow[FIELD_PLAYER1]));
	set_player2_id(atoi(aRow[FIELD_PLAYER2]));
	mRelation = (ERelation)atoi(aRow[FIELD_RELATION]);
	mTime = (time_t)atoi(aRow[FIELD_TIME]);

	if (mMaxID < mID) mMaxID = mID;
}

CPlayerRelation::CPlayerRelation(CPlayer* aPlayer1, CPlayer* aPlayer2,
		ERelation aRelation)
{
	mMaxID++;

	mID = (mMaxID) ? mMaxID:1;
	mPlayer1 = aPlayer1;
	mPlayer2 = aPlayer2;
	mRelation = aRelation;
	mTime = CGame::get_game_time();
}

CPlayerRelation::~CPlayerRelation()
{
}

CString &
CPlayerRelation::query()
{
	static CString
		Query;

	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query = "INSERT INTO player_relation "
					"(id, player1, player2, relation, time) VALUES ";
			Query.format("( %d, %d, %d, %d, %d )",
					mID,
					mPlayer1->get_game_id(), mPlayer2->get_game_id(),
					mRelation, mTime);
			break;

		case QUERY_UPDATE:

			Query.format("UPDATE player_relation SET relation = %d, time = %d WHERE id = %d",
							mRelation, mTime, mID);
			break;

		case QUERY_DELETE:
			Query.format("DELETE FROM player_relation WHERE id = %d",
									mID);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

void
CPlayerRelation::set_player1_id(int aPlayerID)
{
	mPlayer1 = PLAYER_TABLE->get_by_game_id(aPlayerID);
}

void
CPlayerRelation::set_player2_id(int aPlayerID)
{
	mPlayer2 = PLAYER_TABLE->get_by_game_id(aPlayerID);
}

CPlayer*
CPlayerRelation::get_player1()
{
	return mPlayer1;
}

CPlayer*
CPlayerRelation::get_player2()
{
	return mPlayer2;
}

inline void
CPlayerRelation::set_player1(CPlayer *aPlayer)
{
	mPlayer1 = aPlayer;
}

inline void
CPlayerRelation::set_player2(CPlayer *aPlayer)
{
	mPlayer2 = aPlayer;
}
