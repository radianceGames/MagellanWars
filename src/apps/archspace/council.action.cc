#include "common.h"
#include "util.h"
#include "action.h"
#include <cstdlib>
#include "define.h"
#include "archspace.h"
#include "game.h"
#include "council.h"
#include <libintl.h>

TZone gCouncilActionZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCouncilAction),
	0,
	0,
	NULL,
	"Zone CCouncilAction"
};

int CCouncilAction::mMaxID = 0;

CCouncilAction::CCouncilAction():CAction()
{
	mOwner = NULL;
	mMaxID++;
	mID = (mMaxID) ? mMaxID:1;
}

void
CCouncilAction::load_from_database(MYSQL_ROW aRow)
{
	enum
	{
		FIELD_ID = 0,
		FIELD_START_TIME,
		FIELD_TYPE,
		FIELD_OWNER,
		FIELD_ARGUMENT
	};

	mID = atoi(aRow[FIELD_ID]);
	mStartTime = (time_t)atoi(aRow[FIELD_START_TIME]);
	mOwner = COUNCIL_TABLE->get_by_id(atoi(aRow[FIELD_OWNER]));
	mArgument = atoi(aRow[FIELD_ARGUMENT]);

	if (mID > mMaxID) mMaxID = mID;
}

CCouncilAction::~CCouncilAction()
{
}

CString&
CCouncilAction::query()
{
	static CString Query;

	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query = "INSERT INTO council_action "
					"(id, action, start_time, owner, argument) "
					"VALUES ";
			Query.format("( %d, %d, %d, %d, %d )",
					mID, get_type(), mStartTime,
					mOwner->get_id(),
					mArgument);
			break;
		case QUERY_UPDATE:
			break;
		case QUERY_DELETE:
			Query.format("DELETE FROM council_action WHERE id = %d",
																mID);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

time_t
CCouncilActionBreakPact::get_period()
{
	return (CAction::mPeriodPlayerBreakPact * CGame::mSecondPerTurn);
}

void
CCouncilActionBreakPact::feedback()
{
	SLOG("expire action_council_break_pact %s", mOwner->get_nick());
}

void
CCouncilActionDeclareTotalWar::feedback()
{
	SLOG("expire remove_council_declare_total_war %s",
											mOwner->get_nick());
	CCouncil *
		Council = COUNCIL_TABLE->get_by_id(mArgument);
	if (Council == NULL)
	{
		SLOG("ERROR : Council is NULL in CCouncilActionDeclareTotalWar::feedback(), mArgument = %d, mID = %d, mOwner->get_id() = %d", mArgument, mID, mOwner->get_id());
		return;
	}
	CCouncilRelation *
		Relation = mOwner->get_relation(Council);

	if (Relation->get_relation() == CRelation::RELATION_TOTAL_WAR)
	{
		Relation->set_relation(CRelation::RELATION_TRUCE);
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);

		int
			IntervalInHour = get_period() / 60 / 60;
		mOwner->send_time_news_to_all_member(
				(char *)format(GETTEXT("Total war with %1$s has ended after %2$s hours."),
								Council->get_nick(), dec2unit(IntervalInHour)));
	}
}

void
CCouncilActionDeclareWar::feedback()
{
	SLOG("expire remove_council_declare_war %s", mOwner->get_nick());

    CCouncil *
		Council = COUNCIL_TABLE->get_by_id(mArgument);
	if (Council == NULL)
	{
		SLOG("ERROR : Council is NULL in CCouncilActionDeclareWar::feedback(), mArgument = %d, mID = %d, mOwner->get_id() = %d", mArgument, mID, mOwner->get_id());
		return;
	}
	CCouncilRelation *
		Relation = mOwner->get_relation(Council);

	if (Relation->get_relation() == CRelation::RELATION_WAR)
	{
		Relation->set_relation(CRelation::RELATION_TRUCE);
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);

		int
			IntervalInHour = get_period() / 60 / 60;
		mOwner->send_time_news_to_all_member(
				(char *)format(GETTEXT("Your council has grown weary of the war with %1$s and agrees to a temporary ceasefire after %2$s hours."),
								Council->get_nick(), dec2unit(IntervalInHour)));
	}

}

void
CCouncilActionImproveRelation::feedback()
{
	SLOG("expire remove_improve_relation %s", mOwner->get_nick());
	mOwner->change_honor(1);
}

time_t
CCouncilActionBreakAlly::get_period()
{
	return (CAction::mPeriodCouncilBreakAlly * CGame::mSecondPerTurn);
}

void
CCouncilActionBreakAlly::feedback()
{
	SLOG("expire action_council_break_ally %s", mOwner->get_nick());
}

time_t
CCouncilActionDeclareTotalWar::get_period()
{
	return (CAction::mPeriodCouncilDeclareTotalWar * CGame::mSecondPerTurn);
}

time_t
CCouncilActionDeclareWar::get_period()
{
	return (CAction::mPeriodCouncilDeclareWar * CGame::mSecondPerTurn);
}

time_t
CCouncilActionImproveRelation::get_period()
{
	return (CAction::mPeriodCouncilImproveRelation * CGame::mSecondPerTurn);
}

time_t
CCouncilActionMergingOffer::get_period()
{
	return (CAction::mPeriodCouncilMergingOffer * CGame::mSecondPerTurn);
}

void
CCouncilActionMergingOffer::feedback()
{
	SLOG("expire action_council_merging_offer %s", mOwner->get_nick());
}
