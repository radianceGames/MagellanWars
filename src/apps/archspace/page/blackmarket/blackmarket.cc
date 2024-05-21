#include <libintl.h>
#include "../../pages.h"

bool
CPageBlackMarket::handler(CPlayer *aPlayer)
{
	//system_log( "start page handler %s", get_name());

	ITEM("STRING_THE_TECH_DECK", GETTEXT("The Tech Deck"));
	ITEM("EXPLAIN_THE_TECH_DECK",
			GETTEXT("Here you can find many various technologies, so you don't have to research them yourself."));

	ITEM("STRING_THE_FLEET_DECK", GETTEXT("The Fleet Deck"));
	ITEM("EXPLAIN_THE_FLEET_DECK",
			GETTEXT("Here you can buy ships of many different types."));

	ITEM("STRING_OFFICER_S_LOUNGE", GETTEXT("Officer's Lounge"));
	ITEM("EXPLAIN_OFFICER_S_LOUNGE",
			GETTEXT("Here is where commanders both famous and imfamous can be found."));

	ITEM("STRING_RARE_GOODS_OFFICE", GETTEXT("Rare Goods Office"));
	ITEM("EXPLAIN_RARE_GOODS_OFFICE",
			GETTEXT("There are some achievements of the Empire that cannot be found anywhere but here."));

	ITEM("STRING_LEASER_S_OFFICE", GETTEXT("Leaser's Office"));
	ITEM("EXPLAIN_LEASER_S_OFFICE",
			GETTEXT("The Empire will sometimes put up a planet for rent here."));

	//system_log("end page handler %s", get_name());

	return output("black_market/black_market.html");
}

