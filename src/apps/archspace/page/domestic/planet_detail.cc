#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>

bool
CPagePlanetDetail::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	QUERY("PLANET_ID", PlanetIDString);
	int
		PlanetID = as_atoi(PlanetIDString);
	CPlanet *
		Planet = aPlayer->get_planet(PlanetID);
	CHECK(!Planet,
			(char *)format(GETTEXT("You don't have a planet with ID %1$s."),
							dec2unit(PlanetID)));


	ITEM("MENU_TITLE", GETTEXT("Planet Detail"));

	ITEM("PLANET_ORDER_NAME", Planet->get_order_name());
	ITEM("PLANET_NAME", Planet->get_name());

	ITEM("STRING_POPULATION", GETTEXT("Population"));
	ITEM("STRING_POPULATION_MAX", GETTEXT("Population Max"));
	ITEM("POPULATION_M",
			(char *)format(GETTEXT("%1$s k"), dec2unit(Planet->get_population())));
	ITEM("POPULATION_M_MAX",
			(char *)format(GETTEXT("%1$s k"), dec2unit(Planet->get_max_population())));

	ITEM("STRING_PP_PRODUCTION_PER_TURN",
			GETTEXT("Production per Turn"));
	ITEM("PP_PRODUCT_PER_TURN",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(Planet->get_production_per_turn())));

	ITEM("STRING_COMMERCE_WITH",
			GETTEXT("Commerce With"));

	static CString
		CommercialPlanet;
	CommercialPlanet = Planet->all_commercial_planet_name();
	if (CommercialPlanet.length())
	{
		ITEM("COMMERCE_WITH", (char *)CommercialPlanet);
	} else
	{
		ITEM("COMMERCE_WITH", GETTEXT("None"));
	}

	ITEM("STRING_UPKEEP", GETTEXT("Upkeep"));
	ITEM("UPKEEP",
			(char *)format(GETTEXT("%1$s PP"), dec2unit(Planet->get_upkeep_per_turn())));
	ITEM("FACTORY_UPKEEP",
			(char *)format(GETTEXT("Factory %1$s PP"),
							dec2unit(Planet->get_upkeep_per_turn(BUILDING_FACTORY) / 10)));
	ITEM("RESEARCH_LAB_UPKEEP",
			(char *)format(GETTEXT("Research Lab. %1$s PP"),
							dec2unit(Planet->get_upkeep_per_turn(BUILDING_RESEARCH_LAB) / 10)));
	ITEM("MILITARY_BASE_UPKEEP",
			(char *)format(GETTEXT("Military Base %1$s PP"),
							dec2unit(Planet->get_upkeep_per_turn(BUILDING_MILITARY_BASE) / 10)));

	ITEM("STRING_ENVIRONMENT", GETTEXT("Environment"));
	ITEM("STRING_SIZE", GETTEXT("Size"));
	ITEM("STRING_TEMPERATURE", GETTEXT("Temperature"));
	ITEM("STRING_WASTE_RATE", GETTEXT("Waste Rate"));
	ITEM("STRING_ATMOSPHERE", GETTEXT("Atmosphere"));
	ITEM("STRING_RESOURCE", GETTEXT("Resource"));
	ITEM("STRING_GRAVITY", GETTEXT("Gravity"));
	ITEM("STRING_ATTRIBUTE", GETTEXT("Attribute"));
	ITEM("STRING_COMMERCE_MODIFIER", GETTEXT("Commerce Modifier"));

	ITEM("STRING_BUILDINGS", GETTEXT("Buildings"));
	ITEM("SET_RATIO_MESSAGE",
			GETTEXT("Set the facility(factory, research lab., military base) building ratio of this planet."));
	ITEM("STRING_FACTORY", GETTEXT("Factory"));
	ITEM("STRING_RESEARCH_LAB", GETTEXT("Research Lab."));
	ITEM("STRING_MILITARY_BASE", GETTEXT("Military Base"));

	ITEM("STRING_INVESTMENT", GETTEXT("Investment"));
	ITEM("STRING_INVESTED_PP_LEFT", GETTEXT("Invested PP Left"));
	ITEM("STRING_MAXIMUM_PP_USAGE_PER_TURN",
		GETTEXT("Maximum PP Usage per Turn"));
	ITEM("STRING_AMOUNT_TO_INVEST", GETTEXT("Amount to Invest"));

	ITEM("ENVIRONMENT",
			CControlModel::get_environment_description(
					Planet->calc_environment_control()));
	ITEM("SIZE", Planet->get_size_description());
	ITEM("TEMPERATURE", format(GETTEXT("%1$s K"), dec2unit(Planet->get_temperature())));
	ITEM("WASTE_RATE",
			(char *)format(GETTEXT("%1$s %%"), dec2unit(Planet->get_waste_rate())));
/*	ITEM("COMMERCE_MODIFIER",
			(char *)format(GETTEXT("%1$s %%"), dec2unit(Planet->get_diplomacy_modifier_to_commerce())));*/
	ITEM("ATMOSPHERE", Planet->get_atmosphere_html());

	ITEM("RESOURCE", Planet->get_resource_description());
	ITEM("GRAVITY", (char *)format("%2.2f G", Planet->get_gravity()));
	ITEM("ATTRIBUTE", Planet->get_attribute_html());

	ITEM("FACTORY_NUMBER",
			(char *)format("%s %s",
							GETTEXT("Factory"),
							dec2unit((Planet->get_building()).get(RESOURCE_PRODUCTION))));
	ITEM("RESEARCH_LAB_NUMBER",
			(char *)format("%s %s",
							GETTEXT("Research Lab."),
							dec2unit((Planet->get_building()).get(RESOURCE_RESEARCH))));
	ITEM("MILITARY_Base_NUMBER",
			(char *)format("%s %s",
							GETTEXT("Military Base"),
							dec2unit((Planet->get_building()).get(RESOURCE_MILITARY))));

	ITEM("FACTORY_RATIO",
			Planet->get_distribute_ratio().get(BUILDING_FACTORY));
	ITEM("RESEARCH_LAB_RATIO",
			Planet->get_distribute_ratio().get(BUILDING_RESEARCH_LAB));
	ITEM("MILITARY_BASE_RATIO",
			Planet->get_distribute_ratio().get(BUILDING_MILITARY_BASE));

	ITEM("PLANET_ID", Planet->get_id());

	if (Planet->get_planet_invest_pool()) {
		ITEM("POOL_STATUS", "CHECKED");
	} else {
		ITEM("POOL_STATUS", "UNCHECKED");
	}
	int Temp = Planet->get_investment();
	ITEM("INVESTED_PP", (Temp) ? dec2unit(Temp):GETTEXT("None"));
	Temp = Planet->get_invest_max_production();
	if (aPlayer->has_ability(ABILITY_EFFICIENT_INVESTMENT)) {
		Temp /= 2;
	}
	ITEM("USAGE_PER_TURN", (Temp) ? dec2unit(Temp):GETTEXT("None"));

	int PlanetDestroyTurns = Planet->get_destroy_turns();

	if( PlanetDestroyTurns > 1 ) {
		ITEM("STRING_DESTROY_OPTION", GETTEXT("cancel"));
		ITEM("STRING_DESTROY_IMAGE", GETTEXT("cancel"));
		ITEM("STRING_DESTROY_PLANET",
				(char *)format("%s %d",
							GETTEXT("Turns Till Planet's Destruction: "),
							PlanetDestroyTurns - 1));
	} else {
		ITEM("STRING_DESTROY_OPTION", GETTEXT("confirm"));
		ITEM("STRING_DESTROY_IMAGE", GETTEXT("ok"));
		ITEM("STRING_DESTROY_PLANET", GETTEXT("Destroy Planet"));
	}


#define PLANET_PER_PAGE		20

	int
		StartPlanet = 1;

	while (1)
	{
		if (StartPlanet + PLANET_PER_PAGE > Planet->get_order()+1)
		{
			ITEM("START_PLANET", StartPlanet);
			break;
		}

		StartPlanet += PLANET_PER_PAGE;
	}

//	system_log("end page handler %s", get_name());

	return output("domestic/planet_detail.html");
}
