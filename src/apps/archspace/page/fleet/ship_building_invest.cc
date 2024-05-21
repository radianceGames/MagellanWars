#include <libintl.h>
#include "../../pages.h"

bool
CPageShipBuildingInvest::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	QUERY("INVEST", InvestString);
	int
		Invest = as_atoi(InvestString);
	if (Invest < 1)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("The amount of PP has to be greater than 0."));
	}
	else if (aPlayer->get_production() < Invest)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("You don't have enough PP."));
	}
	else
	{
		aPlayer->change_reserved_production(-Invest);
		aPlayer->change_invested_ship_production(Invest);

		refresh_product_point_item();

		Message.format(GETTEXT("You have successfully invested %1$s PP in ship building."),
						dec2unit(Invest));
		Message += "<BR>\n";
		Message += GETTEXT("Now you can make more Military Points in one turn,"
							" and the ship building process has been"
							" accelerated, too.");
		Message += "<BR><BR>\n";
		ITEM("RESULT_MESSAGE", (char *)Message);
	}

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

	ITEM("INVESTED_PP_LEFT",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(aPlayer->get_invested_ship_production())));
	ITEM("MAXIMUM_PP_USAGE",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(aPlayer->calc_ship_production())));

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

