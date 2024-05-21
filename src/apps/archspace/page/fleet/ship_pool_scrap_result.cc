#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageShipPoolScrapResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("DESIGN_ID", DesignIDString);
	QUERY("SCRAP_NUMBER", ScrapNumberString);
	int
		DesignID = as_atoi(DesignIDString);
	int
		ScrapNumber = as_atoi(ScrapNumberString);

	CDock *
		Pool = aPlayer->get_dock();
	if (!Pool)
	{
		message_page("Pool is NULL.");
		return true;
	}
	CDockedShip *
		Ship = (CDockedShip *)Pool->get_by_id(DesignID);
	if (!Ship)
	{
		message_page("You don't have any ships of that kind of ship design.");
		return true;
	}

	if (ScrapNumber<1 || ScrapNumber>Ship->get_number())
	{
		if (ScrapNumber < 1)
		{
			message_page(GETTEXT("The number of ships must be greater than 0."));
		}
		else
		{
			message_page(GETTEXT("The number of ships can't be greater than the current number of ships."));
		}
		return true;
	}

	static CString
		ScrapResult;
	ScrapResult.clear();

	CShipSize *
		Body = SHIP_SIZE_TABLE->get_by_id(Ship->get_body());
	CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id(Ship->get_armor());

	int
		Income;
	if (Armor->get_armor_type() == AT_BIO &&
		aPlayer->has_ability(ABILITY_GREAT_SPAWNING_POOL))
	{
		Income = ((Body->get_cost() * 80 / 100) / 10) * ScrapNumber;
	}
	else
	{
		Income = (Body->get_cost() / 10) * ScrapNumber;
	}

	aPlayer->change_reserved_production(Income);
	refresh_product_point_item();

	ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("You have successfully scrapped %1$d %2$s ship(s)."),
					ScrapNumber, Ship->get_name()));
	Pool->change_ship(Ship, -ScrapNumber);

	ITEM("STRING_SHIPS_IN_THE_POOL", GETTEXT("Ships in the Pool"));

	ITEM("POOL_INFO", aPlayer->get_dock()->dock_info_html());

	ITEM("STRING_REPAIRING_BAY", GETTEXT("Repairing Bay"));

	ITEM("REPAIRING_BAY_INFORMATION",
			aPlayer->get_repair_bay()->repairing_bay_info_html());

	ITEM("STRING_CLASS_SPECIFICATION", GETTEXT("Class Specification"));

	ITEM("CLASS_INFORMATION", aPlayer->get_ship_design_list()->print_html(aPlayer));

//	system_log( "end page handler %s", get_name() );

	return output("fleet/ship_pool.html");
}

/* end of file ship_pool.cc */
