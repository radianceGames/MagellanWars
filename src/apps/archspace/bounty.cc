#include "common.h"
#include "util.h"
#include "relation.h"
#include "player.h"
#include "archspace.h"
#include "game.h"
#include "bounty.h"
#include <libintl.h>
#include "empire.h"

TZone gBountyZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CBounty),
	0,
	0,
	NULL,
	"Zone CBounty"
};

CBounty::CBounty(MYSQL_ROW aRow)
{
	set_source_player(atoi(aRow[FIELD_SOURCE_PLAYER]));
	mID = atoi(aRow[FIELD_ID]);
	if ((!mSourcePlayer)||(!mSourcePlayer->insert_bounty(this)))
	{
		mSourcePlayer = NULL;
		mTargetPlayer = NULL;
		mEmpirePoints = 0;
		mExpireTime = 0;
		SLOG("unable to add bounty!! bounty.cc - sql");
		return;
	}
	set_relation(atoi(aRow[FIELD_ID]));
	set_target_player(atoi(aRow[FIELD_TARGET_PLAYER]));
	mEmpirePoints = atoi(aRow[FIELD_EMPIRE_POINTS]);
	mExpireTime = (time_t)atoi(aRow[FIELD_EXPIRE_TIME]);
}

CBounty::CBounty(CPlayer *aSourcePlayer, CPlayer *aTargetPlayer)
{
	CPlayerRelation *aRelation = aSourcePlayer->get_relation(aTargetPlayer);
	if (aRelation == NULL)
	{
		aRelation = new CPlayerRelation(aSourcePlayer, aTargetPlayer, CRelation::RELATION_BOUNTY);
		aSourcePlayer->add_relation(aRelation);
		aTargetPlayer->add_relation(aRelation);


		PLAYER_RELATION_TABLE->add_relation(aRelation);
		aRelation->type(QUERY_INSERT);

		SLOG("created player relation for bounty");
	}
	else
	{
		aRelation->set_relation(CRelation::RELATION_BOUNTY);
		aRelation->type(QUERY_UPDATE);
		SLOG("recycled player relation for bounty");
	}

	aSourcePlayer->time_news((char*)format(GETTEXT("You have accepted the bounty on %1$s."),
				aTargetPlayer->get_nick()));


	aTargetPlayer->time_news((char*)format(GETTEXT("%1$s has accepted a bounty on you."),
				aSourcePlayer->get_nick()));

	STORE_CENTER->store(*aRelation);

	mID = aRelation->get_id();

	mSourcePlayer = aSourcePlayer;
	mTargetPlayer = aTargetPlayer;
	mEmpirePoints = 0;
	mExpireTime = time(0) + BOUNTY_DURATION;
}

CBounty::~CBounty()
{

}

CString &
CBounty::query()
{
	static CString
		Query;

	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query = "INSERT INTO bounty "
					"(id, source_player, target_player, empire_points, expire_time) VALUES ";
			Query.format("( %d, %d, %d, %d, %d )",
					mID,
					mSourcePlayer->get_game_id(),
					mTargetPlayer->get_game_id(),
					mEmpirePoints, mExpireTime);
			break;

		case QUERY_UPDATE:

			Query.format("UPDATE bounty SET empire_points = %d WHERE id = %d",
							mEmpirePoints, mID);
			break;

		case QUERY_DELETE:
			Query.format("DELETE FROM bounty WHERE id = %d",
									mID);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

void
CBounty::change_empire_points(int aPoints)
{
	if (aPoints < 0)
	{
		if ((long int)aPoints + (long int)mEmpirePoints < -MAX_BOUNTY_EMPIRE_CHANGE)
		{
			mEmpirePoints = -MAX_BOUNTY_EMPIRE_CHANGE;
		}
		else
		{
			mEmpirePoints += aPoints;
		}
	}
	else
	{
		if ((long int)aPoints + (long int)mEmpirePoints > MAX_BOUNTY_EMPIRE_CHANGE)
		{
			mEmpirePoints = MAX_BOUNTY_EMPIRE_CHANGE;
		}
		else
		{
			mEmpirePoints += aPoints;
		}
	}
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

bool
CBounty::is_valid()
{
	if (mSourcePlayer == NULL) return false;
	if (mTargetPlayer == NULL) return false;
	if (PLAYER_RELATION_TABLE->find_by_id(mID) == NULL)
	{
		//SLOG("CBounty::is_valid() PLAYER_RELATION_TABLE->find_by_id(mID) == NULL");
		return false;
	}
	return true;
}

void
CBounty::set_relation(int aID)
{
	mID = aID;
}

void
CBounty::set_source_player(int aID)
{
	mSourcePlayer = PLAYER_TABLE->get_by_game_id(aID);
}

void
CBounty::set_target_player(int aID)
{
	mTargetPlayer = PLAYER_TABLE->get_by_game_id(aID);
}

void
CBounty::set_empire_points(int aPoints)
{
	mEmpirePoints = aPoints;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

bool
CBounty::is_expired()
{
	return (time(0) > mExpireTime);
}

void
CBounty::expire()
{
	if(mSourcePlayer)
	{
		if (mEmpirePoints < 5) mEmpirePoints = -10;
		if (mEmpirePoints >= MAX_BOUNTY_EMPIRE_CHANGE) {
			mEmpirePoints = MAX_BOUNTY_EMPIRE_CHANGE;
			CEmpireAction *
				Action = mSourcePlayer->grant_boon(CEmpireAction::EA_ACTION_REQUEST_BOON);
			mSourcePlayer->time_news( (char*)format("The empire was pleased with your perfomance on hunting the bounty and has sent you %1$s.",Action->get_target_amount_str(mSourcePlayer)));
		}
		mSourcePlayer->change_empire_relation(mEmpirePoints);
		mSourcePlayer->remove_relation(mID);
		if (mTargetPlayer)
		{
			mTargetPlayer->remove_relation(mID);
		}
		else
		{
			return;
		}
	}

	CPlayerRelation *aRelation = (CPlayerRelation*)PLAYER_RELATION_TABLE->find_by_id(mID);

	if (aRelation == NULL)
	{
		//SLOG("aRelation == NULL in CBounty::expire()");
	}
	else
	{
		aRelation->type(QUERY_DELETE);
		STORE_CENTER->store(*aRelation);
	}

	if (!PLAYER_RELATION_TABLE->remove_relation(mID))
	{
		//SLOG("!PLAYER_RELATION_TABLE->remove_relation(mID) in CBounty::expire()");//is this a bad thing?
	}
}

bool
CBountyList::accept_bounty(COfferedBounty* aBounty, CPlayer *aPlayer)
{
	if (size() >= MAX_PLAYER_BOUNTIES) {
		SLOG("CBountyList::accept_bounty : player tried to accept more bounties then allowed, current bounties %d, max bounties %d",mBounties.size(), MAX_PLAYER_BOUNTIES );
		return false;
	}
	CBounty *newBounty = new CBounty(aPlayer, aBounty->get_player());
	mBounties.push_back(newBounty);

	BOUNTY_TABLE->add(newBounty);
	SLOG("Player accepted bounty - CBountyList");
	return true;
}

bool
CBountyList::insert_bounty(CBounty* aBounty)
{
	if (mBounties.size() >= MAX_PLAYER_BOUNTIES) return false;
	mBounties.push_back(aBounty);
	return true;
}

void
CBountyList::expire_bounties()
{
	list<CBounty*>::iterator current;

	for (current = mBounties.begin(); current != mBounties.end(); current++)
	{
		if((*current) == NULL && (int)(*current) != 0xffffffff)
		{
			mBounties.erase(current);
			continue;
		}
		if ((*current)->is_expired() && (int)(*current) != 0xffffffff)
		{
			SLOG("properly expired bounty");
			(*current)->expire();
			mBounties.erase(current);
		}
	}
}

void
CBountyList::remove_bounty(int aID)
{
	std::list<CBounty*>::iterator current;
	CBounty *aBounty;
	for (current = mBounties.begin(); current != mBounties.end(); current++)
	{
		aBounty = (*current);
		if (aBounty==NULL || (int)aBounty==0xffffffff) return;

		if(aBounty->get_id() == aID)
		{
			aBounty->expire();
			mBounties.erase(current);
			return;
		}
	}
}

void
CBountyList::remove_bounties()
{
	std::list<CBounty*>::iterator current;
	for (current = mBounties.begin(); current != mBounties.end(); current++)
	{
		if ((*current) != NULL && (int)(*current) != 0xffffffff)
			(*current)->expire();
	}
	mBounties.clear();
}

void
CBountyList::remove_bounties_on_player(int aID)
{
	std::list<CBounty*>::iterator current;
	for (current = mBounties.begin(); current != mBounties.end(); current++)
	{
		if ((*current)->get_target_player()->get_game_id() == aID)
		{
			BOUNTY_TABLE->remove((*current)->get_id());
		}
	}
}

CBounty*
CBountyList::get_bounty_by_player_id(int aID)
{
	std::list<CBounty*>::iterator current;
	for (current = mBounties.begin(); current != mBounties.end(); current++)
	{
		if ((*current)->get_target_player()->get_game_id() == aID) return *current;
	}
	return NULL;
}
