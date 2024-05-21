#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageShipBuildingInsert::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	while (1)
	{
		QUERY("CLASS", ClassString);
		if (!ClassString)
		{
			ITEM("RESULT_MESSAGE",
					GETTEXT("You didn't select your own ship design."));
			break;
		}

		int
			Class = as_atoi(ClassString);
		CShipDesignList *
			DesignList = aPlayer->get_ship_design_list();
		CShipDesign *
			Design = (CShipDesign *)DesignList->get_by_id(Class);
		if (Design == NULL)
		{
			ITEM("RESULT_MESSAGE",
					GETTEXT("You haven't designed the class."));
			break;
		}
		if (Design->is_black_market_design())
		{
		    ITEM("RESULT_MESSAGE",
		           GETTEXT("You cannot build black market ship designs!"));
		    break;		
		}
		
		if (Design->is_empire_design())
		{
		    ITEM("RESULT_MESSAGE",
		           GETTEXT("You cannot build Empire ship designs!"));
		    break;		
		}
		
		QUERY("BUILD_NUMBER", NumberString);
		int
			Number = as_atoi(NumberString);
		if (Number < 1)
		{
			ITEM("RESULT_MESSAGE",
					GETTEXT("The number of ships must be greater than 0."));
			break;
		}

		CShipBuildQ *
			BuildQ = aPlayer->get_build_q();
		if (BuildQ->length() >= 10)
		{
			ITEM("RESULT_MESSAGE",
					GETTEXT("You can schedule 10 building processes at most."));
			break;
		}

		CShipToBuild *
			NewShip = new CShipToBuild(aPlayer->get_game_id(), Design, Number, time(0));
		time_t
			Now = NewShip->get_order();

		bool
			AnyDuplicate = false;
		for (int i=0 ; i<BuildQ->length() ; i++)
		{
			CShipToBuild *
				ShipToBuild = (CShipToBuild *)BuildQ->get(i);

			if (ShipToBuild->get_order() == Now)
			{
				ITEM("RESULT_MESSAGE",
						GETTEXT("You have probably clicked 2 or more times at the same time, or reloaded this page.<BR>Please try again."));
				AnyDuplicate = true;
				break;
			}
		}

		if (!AnyDuplicate)
		{
			NewShip->type(QUERY_INSERT);
			*STORE_CENTER << *NewShip;
			BuildQ->add_ship_to_build(NewShip);

			ITEM("RESULT_MESSAGE",
					(char *)format(GETTEXT("You have scheduled %1$d %2$s to be built."),
									Number, Design->get_name()));
		}

		break;
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

