#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageBlackMarketTechDeckBid::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());
	CBidList *
		BidList = BLACK_MARKET->get_bid_list();
	if (BidList->count_by_type(CBid::ITEM_TECH) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no techs for sale right now."));

		return output("black_market/tech_deck_no_item.html");
	}


	QUERY( "TECH_ITEM_ID", techItemIDstring );
	int
		techItemID = as_atoi( techItemIDstring );
	CBidList*
		bidList = BLACK_MARKET->get_bid_list();
	CBid
		*bid = bidList->get_by_id( techItemID );
	if( bid == NULL )
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The item you tried to bid for doesn't exist."));
		return output("black_market/tech_deck_error.html");
	}
	else if ( bid->get_type() != CBid::ITEM_TECH )
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The item you tried to bid for is not a tech item."));
		return output("black_market/tech_deck_error.html");
	}

	QUERY("BID_PRICE", newPriceString);
	int
		newPrice = as_atoi(newPriceString);
	if (newPrice <= 0)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You have to enter a larger number than 0."),
								dec2unit(bid->get_price())));
		return output("black_market/tech_deck_error.html");
	}
	if (newPrice > MAX_BID_PRICE)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You tried to bid more than the limit of bid price."));
		return output("black_market/tech_deck_error.html");
	}
	if (aPlayer->get_production() < newPrice)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("We are sorry, you don't have %1$s PP."),
							dec2unit(newPrice)));
		return output("black_market/tech_deck_error.html");
	}
	if (bid->get_price() == MAX_BID_PRICE)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("We are sorry, the current price is already the limit."));
		return output("black_market/tech_deck_error.html");
	}

	int
		MinAvailablePrice;
	if ((long long int)bid->get_price() * 105/100 > (long long int)MAX_BID_PRICE)
	{
		MinAvailablePrice = MAX_BID_PRICE;
	}
	else
	{
		MinAvailablePrice = bid->get_price() * 105/100;
	}

	if (newPrice < MinAvailablePrice)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("We are sorry, you must bid at least 5%% higher price than the current bid's price %1$s."),
								dec2unit(bid->get_price())));
		return output("black_market/tech_deck_error.html");
	}

	CTech
		*tech = TECH_TABLE->get_by_id( bid->get_item() );
	CKnownTechList
		*techList = aPlayer->get_tech_list();
	if( techList->get_by_id( bid->get_item() ) != NULL )
	{
		ITEM( "ERROR_MESSAGE", "We are sorry, you appear to have this Tech Already." );
		return output("black_market/tech_deck_error.html");
	}
	if(!tech->evaluate(aPlayer))
	{
		ITEM( "ERROR_MESSAGE", "We are sorry, you do not seem to understand enough to purchase this Tech" );
		return output("black_makret/tech_deck_error.html");
	}
	ITEM( "TECH_DETAIL", tech->html() );
	ITEM( "TECH_ITEM_ID", bid->get_id() );

	ITEM("STRING_CURRENT_BID", GETTEXT("Current Bid"));
	ITEM( "CURRENT_BID", bid->get_price() );

	ITEM("STRING_YOUR_BID", GETTEXT("Your Bid"));
	ITEM( "YOUR_BID", newPrice);
	ITEM( "BID_PRICE", newPrice);
	//system_log("end page handler %s", get_name());

	return output("black_market/tech_deck_bid.html");
}

