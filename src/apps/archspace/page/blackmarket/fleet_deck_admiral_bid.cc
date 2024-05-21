#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../race.h"

bool
CPageBlackMarketFleetDeckAdmiralBid::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());
	CBidList *
		BidList = BLACK_MARKET->get_bid_list();
	if (BidList->count_by_type(CBid::ITEM_FLEET) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no fleets for sale right now."));

		return output("black_market/fleet_deck_no_item.html");
	}

	QUERY( "FLEET_ITEM_ID", fleetItemIDstring );
	int
		fleetItemID = as_atoi( fleetItemIDstring );
	CBidList*
		bidList = BLACK_MARKET->get_bid_list();
	CBid *
		bid = bidList->get_by_id( fleetItemID );
	if (bid == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The fleet you tried to bid for doesn't exist. "));
		return output("black_market/fleet_deck_error.html");
	}
	else if ( bid->get_type() != CBid::ITEM_FLEET )
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The item you tried to bid for is not a fleet item."));
		return output("black_market/fleet_deck_error.html");
	}
	CFleet
		*fleet = BLACK_MARKET->get_fleet_list()->get_by_id( bid->get_item() );
	if( fleet == NULL )
	{
		ITEM( "ERROR_MESSAGE", GETTEXT( "The fleet you bid doesn't exist." ) );
		return output("black_market/fleet_deck_error.html");
	}
	CShipDesign
		*shipDesign = BLACK_MARKET->get_ship_design_list()->get_by_id( fleet->get_design_id() );
	if( shipDesign == NULL )
	{
		ITEM( "ERROR_MESSAGE", GETTEXT( "The ship design of fleet you bid doesn't exist." ) );
		return output("black_market/fleet_deck_error.html");
	}

	QUERY("BID_PRICE", newPriceString);
	int
		newPrice = as_atoi(newPriceString);
	if (newPrice <= 0)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You have to enter a larger number than 0."),
								dec2unit(bid->get_price())));
		return output("black_market/fleet_deck_error.html");
	}
	if (newPrice > MAX_BID_PRICE)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You tried to bid more than the limit of bid price."));
		return output("black_market/fleet_deck_error.html");
	}
	if (aPlayer->get_production() < newPrice)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("We are sorry, you don't have %1$s PP."),
							dec2unit(newPrice)));
		return output("black_market/fleet_deck_error.html");
	}
	if (bid->get_price() == MAX_BID_PRICE)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("We are sorry, the current price is already the limit."));
		return output("black_market/fleet_deck_error.html");
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
		return output("black_market/fleet_deck_error.html");
	}

	QUERY( "ADMIRAL_ID", admiralIDstring );
	int
		admiralID = as_atoi( admiralIDstring );
	CAdmiral *
		admiral = (CAdmiral*)aPlayer->get_admiral_pool()->get_by_id(admiralID);
	if (admiral == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The admiral you selected doesn't exist."));
		return output("black_market/fleet_deck_error.html");
	}
	else if (admiral->get_fleet_commanding() < fleet->get_max_ship())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The admiral you selected can't command this fleet."));
		return output("black_market/fleet_deck_error.html");
	}

	ITEM("STRING_FLEET_NAME", GETTEXT("Fleet Name"));
	ITEM( "FLEET_NAME", fleet->get_name() );

	ITEM("STRING_NUMBER_OF_SHIPS", GETTEXT("Number of Ships"));
	ITEM( "SHIPS", fleet->get_current_ship() );
	ITEM( "SHIP_DETAIL", shipDesign->print_html(aPlayer, 0) );

	CString
		html;
	html.clear();
	html.format( "<TABLE BORDER=1 WIDTH=550 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=\"#2A2A2A\">\n" );
	html.format( "<TR BGCOLOR=\"#171717\">\n" );
	html.format( "<TH class=tabletxt COLOR=\"#666666\">Name</TH>\n" );
	html.format( "<TH class=tabletxt COLOR=\"#666666\">Race</TH>\n" );
	html.format( "<TH class=tabletxt COLOR=\"#666666\">Level</TH>\n" );
	html.format( "<TH class=tabletxt COLOR=\"#666666\">Exp</TH>\n" );
	html.format( "<TH class=tabletxt COLOR=\"#666666\">Fleet Commanding</TH>\n" );
	html.format( "<TH class=tabletxt COLOR=\"#666666\">Efficiency</TH>\n" );
	html.format( "</TR>\n" );
	html.format( "<TR>\n" );
	html.format( "<TD class=maintext ALIGN=center>%s</TD>\n", admiral->get_name() );
	html.format( "<TD class=maintext ALIGN=center>%s</TD>\n", RACE_TABLE->get_name_by_id( admiral->get_race() ) );
	html.format( "<TD class=maintext ALIGN=center>%d</TD>\n", admiral->get_level() );
	html.format( "<TD class=maintext ALIGN=center>%d</TD>\n", admiral->get_exp() );
	html.format( "<TD class=maintext ALIGN=center>%d</TD>\n", admiral->get_fleet_commanding() );
	html.format( "<TD class=maintext ALIGN=center>%d</TD>\n", admiral->get_real_efficiency() );
	html.format( "</TR>\n" );
	html.format( "</TABLE>\n" );

	ITEM( "ADMIRAL_DETAIL", html );
	ITEM( "ADMIRAL_ID", admiral->get_id() );
	ITEM( "FLEET_ITEM_ID", bid->get_id() );

	ITEM("STRING_CURRENT_BID", GETTEXT("Current Bid"));
	ITEM( "CURRENT_BID", bid->get_price() );

	ITEM("STRING_YOUR_BID", GETTEXT("Your Bid"));
	ITEM( "YOUR_BID", newPrice);
	ITEM( "BID_PRICE", newPrice);
	//system_log("end page handler %s", get_name());

	return output("black_market/fleet_deck_admiral_bid.html");
}

