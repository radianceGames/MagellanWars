#include "../triggers.h"
#include "../archspace.h"

bool
CTriggerBlackMarketItem::handler()
{
   	if (!CGame::mUpdateTurn) return true;
	if (EMPIRE->is_dead() == true) return true;

	int
		chance;
	// tech
	chance = number(24);
	if( chance < 8 )
	{
		BLACK_MARKET->create_new_tech();
	}
	// fleet
	chance = number(24);
	if ( chance < 8 )
	{
		BLACK_MARKET->create_new_fleet();
	}
	// admiral
	BLACK_MARKET->create_new_admiral();
	// project
	chance = number(7 * 24);
	if ( chance < 12 )
	{
		BLACK_MARKET->create_new_project();
	}
	// planet
	chance = number(3 * 24);
	if ( chance < 5 )
	{
		BLACK_MARKET->create_new_planet();
	}
	return true;
};

bool
CTriggerBlackMarketExpire::handler()
{
   	if (!CGame::mUpdateTurn) return true;
	if (EMPIRE->is_dead() == true) return true;

	CBidList *
		BidList = BLACK_MARKET->get_bid_list();

	for (int i=BidList->length()-1 ; i>=0 ; i--)
	{
		CBid *
			Bid = (CBid *)BidList->get(i);
		Bid->set_expire_time(Bid->get_expire_time() - 1);

		if (Bid->get_expire_time() == 0)
		{
			BLACK_MARKET->expire(Bid);
		}
		else
		{
			Bid->type(QUERY_UPDATE);
			STORE_CENTER->store(*Bid);
		}
	}

	return true;
};
