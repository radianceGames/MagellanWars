#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../define.h"

bool
CPageBlackMarketLeasersOfficeBid::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());

	CBidList *
		BidList = BLACK_MARKET->get_bid_list();
	if (BidList->count_by_type(CBid::ITEM_PLANET) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no planets for sale right now."));

		return output("black_market/leasers_office_no_item.html");
	}

	QUERY( "PLANET_ITEM_ID", planetItemIDstring );
	int
		planetItemID = as_atoi( planetItemIDstring );
	CBidList*
		bidList = BLACK_MARKET->get_bid_list();
	CBid *
		bid = bidList->get_by_id( planetItemID );
	if (bid == NULL)
	{
		ITEM( "ERROR_MESSAGE",
				GETTEXT("The item you tried to bid for doesn't exist."));
		return output("black_market/leasers_office_error.html");
	}
	else if ( bid->get_type() != CBid::ITEM_PLANET )
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The item you tried to bid for is not a planet item."));
		return output("black_market/leasers_office_error.html");
	}

	QUERY("BID_PRICE", newPriceString);
	int
		newPrice = as_atoi(newPriceString);
	if (newPrice <= 0)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You have to enter a larger number than 0."),
								dec2unit(bid->get_price())));
		return output("black_market/leasers_office_error.html");
	}
	if (newPrice > MAX_BID_PRICE)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You tried to bid more than the limit of bid price."));
		return output("black_market/leasers_office_error.html");
	}
	if ((long long int)aPlayer->get_production() <
		(long long int)newPrice * ((long long int) BLACK_MARKET_PLANET_COUNT_OFFSET + (long long int)aPlayer->get_planet_list()->length()))
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("We are sorry, you don't have total of (%1$s * %2$d) %3$d PP."),
							dec2unit(newPrice), (BLACK_MARKET_PLANET_COUNT_OFFSET + aPlayer->get_planet_list()->length()), (newPrice*(BLACK_MARKET_PLANET_COUNT_OFFSET + aPlayer->get_planet_list()->length()))));
		return output("black_market/leasers_office_error.html");
	}
	if (bid->get_price() == MAX_BID_PRICE)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("We are sorry, the current price is already the limit."));
		return output("black_market/leasers_office_error.html");
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
		return output("black_market/leasers_office_error.html");
	}

	CPlanet *
		planet = BLACK_MARKET->get_planet_list()->get_by_id( bid->get_item() );

	ITEM("PLANET_NAME", planet->get_name());

	ITEM("STRING_POPULATION", GETTEXT("Population"));
	ITEM("STRING_POPULATION_MAX", GETTEXT("Population Max"));
	ITEM("POPULATION_M", (char *)format("%s k", dec2unit(planet->get_population())));
	ITEM("POPULATION_M_MAX", (char *)format("%s k", dec2unit(planet->get_max_population())));

	ITEM("STRING_PP_PRODUCTION_PER_TURN", GETTEXT("Production per Turn"));
	ITEM("PP_PRODUCT_PER_TURN", (char *)format("%s PP", dec2unit(planet->get_pp_per_turn())));

	ITEM("STRING_COMMERCE_WITH", GETTEXT("Commerce With"));

	static CString
		CommercialPlanet;
	CommercialPlanet = planet->all_commercial_planet_name();
	if (CommercialPlanet.length())
	{
		ITEM("COMMERCE_WITH", (char *)CommercialPlanet);
	}
	else
	{
		ITEM("COMMERCE_WITH", GETTEXT("None"));
	}

	ITEM("STRING_UPKEEP", GETTEXT("Upkeep"));
	ITEM("UPKEEP", (char *)format("%s PP", dec2unit(planet->get_upkeep().total())));
	ITEM("FACTORY_UPKEEP", (char *)format("Factory %s PP", dec2unit(planet->get_upkeep().get(RESOURCE_PRODUCTION))));
	ITEM("RESEARCH_LAB_UPKEEP", (char *)format("Research Lab. %s PP", dec2unit(planet->get_upkeep().get(RESOURCE_RESEARCH))));
	ITEM("MILITARY_BASE_UPKEEP", (char *)format("Military Base %s PP",dec2unit(planet->get_upkeep().get(RESOURCE_MILITARY))));

	ITEM("STRING_ENVIRONMENT", GETTEXT("Environment"));
	ITEM("STRING_SIZE", GETTEXT("Size"));
	ITEM("STRING_TEMPERATURE", GETTEXT("Temperature"));
	ITEM("STRING_WASTE_RATE", GETTEXT("Waste Rate"));
	ITEM("STRING_ATMOSPHERE", GETTEXT("Atmosphere"));
	ITEM("STRING_RESOURCE", GETTEXT("Resource"));
	ITEM("STRING_GRAVITY", GETTEXT("Gravity"));
	ITEM("STRING_ATTRIBUTE", GETTEXT("Attribute"));

	ITEM("STRING_BUILDINGS", GETTEXT("Buildings"));
	ITEM("SET_RATIO_MESSAGE", GETTEXT("Set the facility(factory, research lab., military base) building ratio of this planet."));
	ITEM("STRING_FACTORY", GETTEXT("Factory"));
	ITEM("STRING_RESEARCH_LAB", GETTEXT("Research Lab."));
	ITEM("STRING_MILITARY_BASE", GETTEXT("Military Base"));

	ITEM("STRING_INVESTMENT", GETTEXT("Investment"));
	ITEM("STRING_INVESTED_PP_LEFT", GETTEXT("Invested PP Left"));
	ITEM("STRING_MAXIMUM_PP_USAGE_PER_TURN", GETTEXT("Maximum PP Usage per Turn"));
	ITEM("STRING_AMOUNT_TO_INVEST", GETTEXT("Amount to Invest"));

	ITEM("ENVIRONMENT", CControlModel::get_environment_description(planet->calc_environment_control(aPlayer)));
	ITEM("SIZE", planet->get_size_description());
	ITEM("TEMPERATURE", format("%d K", planet->get_temperature()));
	ITEM("WASTE_RATE", (char *)format("%d %%", planet->get_waste_rate()));
	ITEM("ATMOSPHERE", planet->get_atmosphere_html());

	ITEM("RESOURCE", planet->get_resource_description());
	ITEM("GRAVITY", format("%2.2f G", planet->get_gravity()));
	ITEM("ATTRIBUTE", planet->get_attribute_html());

	ITEM("FACTORY_NUMBER",
			(char *)format("%s %s",
							GETTEXT("Factory"),
							dec2unit((planet->get_building()).get(RESOURCE_PRODUCTION))));
	ITEM("RESEARCH_LAB_NUMBER",
			(char *)format("%s %s",
							GETTEXT("Research Lab."),
							dec2unit((planet->get_building()).get(RESOURCE_RESEARCH))));
	ITEM("MILITARY_BASE_NUMBER",
			(char *)format("%s %s",
							GETTEXT("Military Base"),
							dec2unit((planet->get_building()).get(RESOURCE_MILITARY))));

	ITEM( "PLANET_ITEM_ID", bid->get_id() );

	ITEM("STRING_CURRENT_BID", GETTEXT("Current Bid"));
	ITEM( "CURRENT_BID", bid->get_price() );
	CString
		bidString,
		NewPriceString,
		NumberOfPlanetString,
		TotalPriceString;
	bidString.clear();
	NewPriceString.clear();
	NumberOfPlanetString.clear();
	TotalPriceString.clear();

	CPlanetList *
		PlanetList = aPlayer->get_planet_list();

	NewPriceString = dec2unit(newPrice);
	NumberOfPlanetString = dec2unit(BLACK_MARKET_PLANET_COUNT_OFFSET + PlanetList->length());
	TotalPriceString = dec2unit(newPrice * (BLACK_MARKET_PLANET_COUNT_OFFSET + PlanetList->length()));

	bidString.format(GETTEXT("%1$s * %2$s Planets = %3$s"),
						(char *)NewPriceString,
						(char *)NumberOfPlanetString,
						(char *)TotalPriceString);

	ITEM("STRING_YOUR_BID", GETTEXT("Your Bid"));
	ITEM( "YOUR_BID", bidString );
	ITEM( "BID_PRICE", newPrice );
	//system_log("end page handler %s", get_name());

	return output("black_market/leasers_office_bid.html");
}

