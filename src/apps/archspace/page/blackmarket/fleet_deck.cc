#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageBlackMarketFleetDeck::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());

	ITEM("FLEET_DECK_MESSAGE",
			GETTEXT("This Deck is filled with Fleets of many different Classes and Levels. The current owners will tell you their Class, Level, and how many ships are in each fleet. These fleets do not come with commanders and you cannot bid unless you have an extra. Good Shopping."));

	CBidList *
		BidList = BLACK_MARKET->get_bid_list();
	if (BidList->count_by_type(CBid::ITEM_FLEET) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no fleets for sale right now."));

		return output("black_market/fleet_deck_no_item.html");
	}

	CBlackMarketHTML
		html;
	ITEM("FLEET_LIST", html.get_fleet_list_html(aPlayer) );

	ITEM("BID_MESSAGE",
			GETTEXT("Your bid must be at least 5% higher than the current bid."));

	//system_log("end page handler %s", get_name());

	return output("black_market/fleet_deck.html");
}

