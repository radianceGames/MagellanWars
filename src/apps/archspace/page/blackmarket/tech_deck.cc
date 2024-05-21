#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageBlackMarketTechDeck::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());

	ITEM("TECH_DECK_MESSAGE",
			GETTEXT("This is the place to come when you are tired of researching. We have just about any technologies you can imagine here. If you know enough to understand what you are buying, we will sell it to you. Of course, the Empire in its wisdom has limited the amount that we can teach in a day. So if you have already learned a tech from us today, then you cannot buy anything here. Good shopping."));

	CBidList *
		BidList = BLACK_MARKET->get_bid_list();
	if (BidList->count_by_type(CBid::ITEM_TECH) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no techs for sale right now."));

		return output("black_market/tech_deck_no_item.html");
	}

	CBlackMarketHTML
		html;
	ITEM("TECH_LIST", html.get_tech_list_html(aPlayer));

	ITEM("BID_MESSAGE",
			GETTEXT("Your bid must be at least 5% higher than the current bid."));

	//system_log("end page handler %s", get_name());

	return output("black_market/tech_deck.html");
}

