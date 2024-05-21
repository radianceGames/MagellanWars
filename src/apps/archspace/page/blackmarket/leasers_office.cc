#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../define.h"

bool
CPageBlackMarketLeasersOffice::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());
/*
	ITEM("ERROR_MESSAGE",
			GETTEXT("We are fixing the problem in the Leaser's Office menu, so you can't access this menu at the moment."));

	return output("black_market/leasers_office_no_item.html");
*/
	ITEM("LEASER_S_OFFICE_MESSAGE",
			(char *)format(GETTEXT("This office is the domain of the servants of the Empire. Here is where the Empire auctions off extra planets. You may only bid on planets of your own race. In order to make sure no single person can surpass the Empire in power, the amount you bid is the amount that each of your current planets plus %d will pay. This means, if you bid 1 million PP and have ten planets, you will pay %d million PP. Take Care and Good Shopping."), BLACK_MARKET_PLANET_COUNT_OFFSET, BLACK_MARKET_PLANET_COUNT_OFFSET + aPlayer->get_planet_list()->length()));

	CBidList *
		BidList = BLACK_MARKET->get_bid_list();
	if (BidList->count_by_type(CBid::ITEM_PLANET) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no planets for sale right now."));

		return output("black_market/leasers_office_no_item.html");
	}

	CBlackMarketHTML
		html;
	ITEM("PLANET_LIST", html.get_planet_list_html(aPlayer) );

	ITEM("BID_MESSAGE",
			GETTEXT("Your bid must be at least 5% higher than the current bid."));

	//system_log("end page handler %s", get_name());

	return output("black_market/leasers_office.html");
}

