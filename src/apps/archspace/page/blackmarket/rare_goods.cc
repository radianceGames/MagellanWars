#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageBlackMarketRareGoods::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());

	ITEM("RARE_GOODS_MESSAGE",
			GETTEXT("This store carries many wonders of the Ancient Empire. After looking hard enough, you are sometimes able to find the plans for a secret project.<BR>Good Shopping."));

	CBidList *
		BidList = BLACK_MARKET->get_bid_list();
	if (BidList->count_by_type(CBid::ITEM_PROJECT) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no projects for sale right now."));

		return output("black_market/rare_goods_no_item.html");
	}

	CBlackMarketHTML
		html;
	ITEM("PROJECT_LIST", html.get_project_list_html( aPlayer ) );

	ITEM("BID_MESSAGE",
			GETTEXT("Your bid must be at least 5% higher than the current bid."));

	//system_log("end page handler %s", get_name());

	return output("black_market/rare_goods.html");
}

