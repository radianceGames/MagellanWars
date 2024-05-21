#include <libintl.h>
#include "../../pages.h"

bool
CPagePlanetInvestPoolResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	QUERY("INVEST", InvestString);
	
	if(!InvestString) {
		ITEM("RESULT", GETTEXT("You did not enter in a value to invest"));
		return output("domestic/planet_invest_pool_result.html");
	}
	int Invest = as_atoi(InvestString);
	int PP = aPlayer->get_production();
	if(Invest < 1)
	{
		ITEM("RESULT", GETTEXT("You can not invest less than 1 PP."));
		return output("domestic/planet_invest_pool_result.html");
	}
	if (Invest > PP) {
		ITEM("RESULT", GETTEXT("You do not have enough money to invest that much"));
		return output("domestic/planet_invest_pool_result.html");
	}

	aPlayer->set_production(PP - Invest - aPlayer->add_planet_invest_pool(Invest));
	ITEM("RESULT", GETTEXT("You have successfully invested"));
	//	system_log("end page handler %s", get_name());

	return output("domestic/planet_invest_pool_result.html");
}
