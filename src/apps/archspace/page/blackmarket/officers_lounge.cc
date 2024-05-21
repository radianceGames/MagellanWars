#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageBlackMarketOfficersLounge::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());

	ITEM("OFFICER_S_LOUNGE_MESSAGE",
			GETTEXT("This spacious room is filled with people of all races. They are commanders of many different calibers. You may try to hire commanders of any races. Good shopping."));

	CBidList *
		BidList = BLACK_MARKET->get_bid_list();
	if (BidList->count_by_type(CBid::ITEM_ADMIRAL) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no admirals for sale right now."));

		return output("black_market/officers_lounge_no_item.html");
	}

	CBlackMarketHTML
		html;
	ITEM("ADMIRAL_LIST", html.get_admiral_list_html(aPlayer) );

    if ((aPlayer->get_admiral_list()->length() + aPlayer->get_admiral_pool()->length()) >= CPlayer::mMaxAdmirals + CPlayer::mExtraBlackMarketAdmirals)
    {
	     ITEM("BID_MESSAGE",
              GETTEXT("You currently have too many commanders to purchase any more."));
    }
    else
    {
	     ITEM("BID_MESSAGE",
              GETTEXT("Your bid must be at least 5% higher than the current bid."));
    }
	//system_log("end page handler %s", get_name());

	return output("black_market/officers_lounge.html");
}

