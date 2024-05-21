#include <libintl.h>
#include "../../pages.h"
#include "../../planet.h"

bool
CPagePlanetInvestPoolEnable::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	QUERY("PLANET_ID", PlanetIDString);
	QUERY("Enable", Check);
	
	if(!PlanetIDString) {
		ITEM("RESULT", GETTEXT("Invalid ID string"));
		return output("domestic/planet_invest_pool_enable.html");
	}
	int PlanetID = as_atoi(PlanetIDString);
	CPlanet *planet = aPlayer->get_planet(PlanetID);
	if(!planet) {
		ITEM("RESULT", GETTEXT("This planet does not belong to you"));
		return output("domestic/planet_invest_pool_enable.html");
	}
	if (Check) {
		if (!planet->get_planet_invest_pool()){
			planet->set_planet_invest_pool(TRUE);
			int amount = planet->get_invest_max_production();
			if (aPlayer->has_ability(ABILITY_EFFICIENT_INVESTMENT)) {
				amount /= 2;
			}
			aPlayer->add_planet_invest_pool_usage(amount);	
		}
		ITEM("RESULT", GETTEXT("You have enabled planet invest pool for this planet"));
	} else {
		if (planet->get_planet_invest_pool()){
			planet->set_planet_invest_pool(FALSE);
			int amount = planet->get_invest_max_production();
			if (aPlayer->has_ability(ABILITY_EFFICIENT_INVESTMENT)) {
				amount /= 2;
			}
			aPlayer->add_planet_invest_pool_usage(-amount);
		}
		ITEM("RESULT", GETTEXT("You have disabled planet invest pool for this planet"));
	}
	//	system_log("end page handler %s", get_name());
	ITEM("PLANET_ID", PlanetIDString);
	return output("domestic/planet_invest_pool_enable.html");
}
