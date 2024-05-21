#include <libintl.h>
#include "../../pages.h"

bool
CPageShipBuilding::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("RESULT_MESSAGE", " ");

	ITEM("STRING_PP_INCOME",
			GETTEXT("PP income assigned to build ships(per turn)"));
	ITEM("STRING_PP_IN_POOL",
			GETTEXT("PP in Ship Production Pool"));

	ITEM("PP_INCOME",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(aPlayer->calc_real_ship_production())));
	ITEM("PP_IN_POOL",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(aPlayer->get_ship_production())));

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

	CShipDesignList *
		DesignList = aPlayer->get_ship_design_list();
	ITEM("CLASS_SELECT",
			DesignList->building_class_select_html(aPlayer));

	ITEM("STRING_NUMBER_OF_SHIPS_TO_BUILD",
			GETTEXT("Number of Ship(s) to Build"));

//	system_log( "end page handler %s", get_name() );

	return output("fleet/ship_building.html");
}

