#include "common.h"
#include "util.h"
#include "relation.h"
#include <cstdlib>
#include "game.h"
#include "council.h"
#include "archspace.h"

TZone gCouncilRelationZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCouncilRelation),
	0,
	0,
	NULL,
	"Zone CCouncilRelation"
};

int CCouncilRelation::mMaxID = 0;

CCouncilRelation::CCouncilRelation():CRelation()
{
	mCouncil1 = mCouncil2 = NONE;
}

CCouncilRelation::CCouncilRelation(MYSQL_ROW aRow)
{
	enum
	{
		FIELD_ID = 0,
		FIELD_COUNCIL1,
		FIELD_COUNCIL2,
		FIELD_RELATION,
		FIELD_TIME
	};

	mID = atoi(aRow[FIELD_ID]);
	set_council1_id(atoi(aRow[FIELD_COUNCIL1]));
	set_council2_id(atoi(aRow[FIELD_COUNCIL2]));
	mRelation = (ERelation)atoi(aRow[FIELD_RELATION]);
	mTime = (time_t)atoi(aRow[FIELD_TIME]);

	if (mMaxID < mID) mMaxID = mID;
}

CCouncilRelation::CCouncilRelation(
		CCouncil* aCouncil1, CCouncil* aCouncil2, 
		ERelation aRelation)
{
	mMaxID++;
	
	mID = (mMaxID) ? mMaxID:1;
	mCouncil1 = aCouncil1;
	mCouncil2 = aCouncil2;
	mRelation = aRelation;
	mTime = CGame::get_game_time();
}

CCouncilRelation::~CCouncilRelation()
{
}

CString &
CCouncilRelation::query()
{
	static CString
		Query;

	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query = "INSERT INTO council_relation "
					"(id, council1, council2, relation, time) VALUES ";
			Query.format("( %d, %d, %d, %d, %d)",
					mID, mCouncil1->get_id(), mCouncil2->get_id(), 
					mRelation, mTime);
			break;

		case QUERY_UPDATE:
			if (mRelation == RELATION_SUBORDINARY)
				Query.format("UPDATE council_relation SET "
							"relation = %d, time = %d, "
							"council1 = %d, council2 = %d WHERE id = %d",
							mRelation, mTime, 
							mCouncil1->get_id(), mCouncil2->get_id(), mID);
			else 
				Query.format("UPDATE council_relation SET "
							"relation = %d, time = %d WHERE id = %d",
									mRelation, mTime, mID);
			break;

		case QUERY_DELETE:
			Query.format("DELETE FROM council_relation WHERE id = %d", mID);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

void
CCouncilRelation::set_council1_id(int aCouncilID)
{
	mCouncil1 = COUNCIL_TABLE->get_by_id(aCouncilID);
}

void
CCouncilRelation::set_council2_id(int aCouncilID)
{
	mCouncil2 = COUNCIL_TABLE->get_by_id(aCouncilID);
}

CCouncil* 
CCouncilRelation::get_council1()
{
	return mCouncil1;
}

CCouncil* 
CCouncilRelation::get_council2()
{
	return mCouncil2;
}

void 
CCouncilRelation::set_council1(CCouncil* aCouncil)
{
	mCouncil1 = aCouncil;
}

void 
CCouncilRelation::set_council2(CCouncil* aCouncil)
{
	mCouncil2 = aCouncil;
}
