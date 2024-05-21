#include "bounty.h"
#include "archspace.h"
#include "player.h"

COfferedBountyTable::COfferedBountyTable()
{
	refresh_bounties();
}

void
COfferedBountyTable::new_bounty(CPlayer *aTargetPlayer)
{
	std::list<COfferedBounty*>::iterator aIter;

	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
		//aBounty = *aIter;
		if ((*aIter)==NULL) continue;

		if((*aIter)->get_id() == aTargetPlayer->get_game_id())
		{
			break;
		}

	}
	if (aIter != mBounties.end()) return;
	COfferedBounty *aBounty = new COfferedBounty(aTargetPlayer);
	//mBounties.reserve(mBounties.size() + 32);
	mBounties.push_back(aBounty);
}

void
COfferedBountyTable::expire_bounties()
{
	std::list<COfferedBounty*>::iterator aIter;
	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
		if ((*aIter)->get_player()->get_degree_of_empire_relation() < CPlayer::DM_LIGHT_BOUNTY_OFFERED) {
			mBounties.erase(aIter);
			delete *aIter;
		}
	}
}

void
COfferedBountyTable::refresh_bounties()
{
	//if (!CGame::mUpdateTurn) return;
	expire_bounties();
	SLOG("Searching for bounties");
	if (EMPIRE->is_dead() == false)
	{
		for (int i=PLAYER_TABLE->length() -1; i>=0 ; i--)
		{
			CPlayer *
				Player = (CPlayer *)PLAYER_TABLE->get(i);
			if (Player->get_game_id() == EMPIRE_GAME_ID) continue;
			if (Player->is_dead()) continue;
			if (Player->is_protected()) continue;
			if (Player->get_protected_mode() != CPlayer::PROTECTED_NONE) continue;
			if (Player->get_degree_of_empire_relation() < CPlayer::DM_LIGHT_BOUNTY_OFFERED) continue;
			int EmpireRelation = abs(Player->get_empire_relation());
			if (number(EmpireRelation) < 50) continue;
			//if (get_by_id(Player->get_game_id())) continue; //already have a bounty on this person
			new_bounty(Player);
		}
	}
}

void
COfferedBountyTable::remove_bounty_on_player(int aID)
{
	std::list<COfferedBounty*>::iterator aIter;
	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
		//aBounty = *aIter;
		if ((*aIter)==NULL) continue;

		if((*aIter)->get_id() == aID)
		{
			break;
		}

	}
	if ((aIter == mBounties.end())||((*aIter)==NULL)) return;
	mBounties.erase(aIter);
	delete *aIter;
}

COfferedBounty*
COfferedBountyTable::get_by_id(int aID)
{
	std::list<COfferedBounty*>::iterator aIter;
	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
		//aBounty = *aIter;
		if ((*aIter)==NULL) continue;

		if((*aIter)->get_id() == aID)
		{
			break;
		}

	}
	if (aIter == mBounties.end()) return NULL;
	return *aIter;
}

std::list<COfferedBounty*>*
COfferedBountyTable::get_available_bounties(CPlayer *aPlayer)
{
	std::list<COfferedBounty*> *aResult = new std::list<COfferedBounty*>;
	int hunterPowerMin, hunterPowerMax, currentPower;
	hunterPowerMin = (int)(aPlayer->get_power()/2);
	hunterPowerMax = (int)(aPlayer->get_power()*2);
	std::list<COfferedBounty*>::iterator aIter;
	for (aIter = mBounties.begin(); aIter != mBounties.end(); aIter++)
	{
		currentPower = (*aIter)->get_player()->get_power();
		if ((currentPower > hunterPowerMin) && (currentPower < hunterPowerMax) && (aPlayer->get_bounty_by_player_id((*aIter)->get_player()->get_game_id()) == NULL) && ((*aIter)->get_player()->get_game_id() != aPlayer->get_game_id())) {
			aResult->push_back(*aIter);
		}
	}
	SLOG("Available bounties:%d",mBounties.size());
	return aResult;
}
