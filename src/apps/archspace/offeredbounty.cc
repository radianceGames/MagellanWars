#include "bounty.h"
#include "archspace.h"

COfferedBounty::COfferedBounty(CPlayer *aTargetPlayer)
{
	mPlayer = aTargetPlayer;
	mHunters = 0;
}

void
COfferedBounty::add_hunter(CPlayer *aPlayer)
{
	//mHunters++;
	//BOUNTY_TABLE->new_bounty(this,aPlayer);
	//if (mHunters >= MAX_PURSUED_BOUNTIES) OFFERED_BOUNTY_TABLE->remove_bounty_on_player(mPlayer->get_game_id());
}

int
COfferedBounty::get_id()
{
	return mPlayer->get_game_id();
}
