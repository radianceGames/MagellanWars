#include <libintl.h>
#include "../../pages.h"
#include "../../planet.h"

bool
CPagePlanetInvestPoolEnableResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	int index = 0;
	CPlanetList *planetlist = aPlayer->get_planet_list();
	aPlayer->set_planet_invest_pool_usage(0);
	double effic = 1;
	if (aPlayer->has_ability(ABILITY_EFFICIENT_INVESTMENT)) {
		effic = .5;
	}
	while (index < planetlist->length() ) {
		CPlanet *planet = (CPlanet *)planetlist->get(index);
		
		CString QueryVar;
		QueryVar.clear();
		QueryVar.format("PLANET_ID%d", planet->get_id());

		QUERY((char *)QueryVar, MessageIndexString);
		if (MessageIndexString)
		{
			planet->set_planet_invest_pool(TRUE);
			int amount = (int)((double)planet->get_invest_max_production() * effic);
			aPlayer->add_planet_invest_pool_usage(amount);	
		} else {
			planet->set_planet_invest_pool(FALSE);
		}
		index++;
	}

	ITEM("RESULT", GETTEXT("You have enabled planet invest pool for the selected planets"));
	return output("domestic/planet_invest_pool_enable_result.html");
}
