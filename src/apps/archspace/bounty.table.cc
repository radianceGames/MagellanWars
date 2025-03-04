#include "bounty.h"
#include "archspace.h"

CBountyTable::~CBountyTable()
{

}

bool
CBountyTable::load(CMySQL &aMySQL)
{
	// load bounty
	//aMySQL.query( "LOCK TABLE bounty READ" );
	aMySQL.query( "SELECT id, source_player, target_player, empire_points, expire_time FROM bounty" );

	aMySQL.use_result();

	while(aMySQL.fetch_row())
	{
		new_bounty(aMySQL.row());
	}

	aMySQL.free_result();
	//aMySQL.query( "UNLOCK TABLES" );
	SLOG("load bounty");

	return true;
}

void
CBountyTable::new_bounty(COfferedBounty *aOfferedBounty, CPlayer *aTargetPlayer)
{
	if (aOfferedBounty == NULL || aTargetPlayer == NULL)
	{
		SLOG("CBountyTable::new_bounty() was sent null junk");
		return;
	}
	aTargetPlayer->accept_bounty(aOfferedBounty);
}

void
CBountyTable::new_bounty(MYSQL_ROW aRow)
{
	CBounty *newBounty = new CBounty(aRow);
	if ((newBounty->is_valid())&&(!newBounty->is_expired()))
	{
		//mBounties.reserve(mBounties.size() + 32);
		mBounties.push_back(newBounty);
	}
	else
	{
		SLOG("CBountyTable::new_bounty(MYSQL_ROW aRow) : bounty is not valid or it expired");
		newBounty->expire();
		newBounty->type(QUERY_DELETE);
		STORE_CENTER->store(*newBounty);
	}
}

CBounty*
CBountyTable::get_by_id(int aID)
{
	std::list<CBounty*>::iterator aIter;

	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
		//aBounty = *aIter;
		if ((*aIter)==NULL) aIter = mBounties.end();

		if((*aIter)->get_id() == aID)
		{
			break;
		}

	}

	if (aIter == mBounties.end())
	{
		SLOG("CBountyTable::get_by_id a bounty was not found that was requested for");
		return NULL;
	}
	return *aIter;
}

CBounty*
CBountyTable::get_by_target_id(int aID)
{
	std::list<CBounty*>::iterator aIter;
	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
		if ((*aIter)->get_target_player()->get_game_id()==aID) return *aIter;
	}
	return NULL;
}

void
CBountyTable::remove(int aID)
{
	std::list<CBounty*>::iterator aIter;
	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
		//aBounty = *aIter;
		if ((*aIter)==NULL)
		{
			continue;
		}

		if((*aIter)->get_id() == aID)
		{
			break;
		}

	}
	if (aIter == mBounties.end() || (*aIter)==NULL)
	{
		SLOG("CBountyTable::remove(int aID) a bounty was not found that was requested for");
		return;
	}
	CBounty *aBounty = (*aIter);
	aBounty->type(QUERY_DELETE);
	STORE_CENTER->store(*aBounty);
	aBounty->get_source_player()->remove_bounty(aBounty->get_id());
	mBounties.erase(aIter);
}

void
CBountyTable::remove(CBounty *aBounty)
{
	if (!aBounty) return;
	remove(aBounty->get_id());
}

void
CBountyTable::remove_by_target_id(int aID)
{
	//blob
	std::list<CBounty*>::iterator aIter;
	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
/*		if ((*aIter)==NULL)
		{
			continue;
		}
*/
		CPlayer* tempy = (*aIter)->get_target_player();
		if(tempy)
		{
			if (tempy->get_game_id()==aID)
			{
				remove(*aIter);

			}

		}
		else
		{
			remove(*aIter);

		}
	}
	if (aIter == mBounties.end() || (*aIter)==NULL)
	{
			SLOG("CBountyTable::remove(int aID) a bounty was not found that was requested for");
			return;
	}
}

void
CBountyTable::add(CBounty* aBounty)
{
	//mBounties.reserve(mBounties.size() + 32);
	mBounties.push_back(aBounty);
	aBounty->type(QUERY_INSERT);
	STORE_CENTER->store(*aBounty);
}
