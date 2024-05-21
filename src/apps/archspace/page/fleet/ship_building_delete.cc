#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageShipBuildingDelete::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("INDEX", IndexString);
	int
		Index = as_atoi(IndexString);
	CShipBuildQ *
		BuildQ = aPlayer->get_build_q();
	if (Index<0 || Index>=BuildQ->length())
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("That ship building schedule doesn't exist."));
	}
	else
	{
		CShipToBuild *
			Ship = (CShipToBuild *)BuildQ->get(Index);
		Ship->type(QUERY_DELETE);
		*STORE_CENTER << *Ship;

		ITEM("RESULT_MESSAGE",
				format(GETTEXT("You have cancelled the ship building schedule for %1$s."),
						Ship->get_name()));
		BuildQ->remove_ship_to_build(Ship);
	}

	ITEM("STRING_PP_INCOME",
			GETTEXT("PP income assigned to build ships(per turn)"));
	ITEM("STRING_PP_IN_POOL",
			GETTEXT("PP in Ship Production Pool"));

	ITEM("PP_INCOME", dec2unit(aPlayer->calc_real_ship_production()));
	ITEM("PP_IN_POOL", dec2unit(aPlayer->get_ship_production()));

	ITEM("STRING_INVESTMENT", GETTEXT("Investment"));
	ITEM("STRING_INVESTED_PP_LEFT", GETTEXT("Invested PP Left"));
	ITEM("STRING_MAXIMUM_PP_USAGE", GETTEXT("Maximum PP Usage per Turn"));
	ITEM("STRING_AMOUNT_TO_INVEST", GETTEXT("Amount to Invest"));

	ITEM("INVESTED_PP_LEFT", dec2unit(aPlayer->get_invested_ship_production()));
	ITEM("MAXIMUM_PP_USAGE", dec2unit(aPlayer->calc_ship_production()));

	ITEM("STRING_SHIP_BUILDING_QUEUE", GETTEXT("Ship Building Queue"));
	ITEM("BUILDING_QUEUE_INFO",
			aPlayer->get_build_q()->building_queue_info_html());

	ITEM("STRING_CLASS", GETTEXT("Class"));
	ITEM("CLASS_SELECT",
			aPlayer->get_ship_design_list()->building_class_select_html(aPlayer));

	ITEM("STRING_NUMBER_OF_SHIPS_TO_BUILD",
			GETTEXT("Number of Ship(s) to Build"));

//	system_log( "end page handler %s", get_name() );

	return output("fleet/ship_building.html");
}

/* end of file ship_building_delete.cc */
