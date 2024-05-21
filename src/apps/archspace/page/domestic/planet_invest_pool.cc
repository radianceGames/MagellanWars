#include <libintl.h>
#include "../../pages.h"

bool
CPagePlanetInvestPool::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	int Invest = aPlayer->get_planet_invest_pool();
	ITEM("INVESTED_PP_LEFT", (!Invest) ? 
					GETTEXT("None"):dec2unit(Invest));

	int Usage = aPlayer->get_planet_invest_pool_usage();
	ITEM("MAX_USAGE_PER_TURN", (!Usage) ? 
					GETTEXT("None"):dec2unit(Usage));
					
	ITEM("PLANETS", aPlayer->get_planets_invest_pool_html());

	//	system_log("end page handler %s", get_name());

	return output("domestic/planet_invest_pool.html");
}
