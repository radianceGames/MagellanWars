#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageShipPoolScrapConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("DESIGN_ID", DesignIDString);
	QUERY("SCRAP_NUMBER", ScrapNumberString);
	int
		DesignID = as_atoi(DesignIDString);
	int
		ScrapNumber = as_atoi(ScrapNumberString);

	CDockedShip *
		Ship = aPlayer->get_dock()->get_by_id(DesignID);
	if (!Ship)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("You don't have any ships of that kind of ship design."));

		ITEM("STRING_SHIPS_IN_THE_POOL", GETTEXT("Ships in the Pool"));

		ITEM("POOL_INFO", aPlayer->get_dock()->dock_info_html());

		ITEM("STRING_REPAIRING_BAY", GETTEXT("Repairing Bay"));

		ITEM("REPAIRING_BAY_INFORMATION",
				aPlayer->get_repair_bay()->repairing_bay_info_html());

		ITEM("STRING_CLASS_SPECIFICATION", GETTEXT("Class Specification"));

		ITEM("CLASS_INFORMATION", aPlayer->get_ship_design_list()->print_html(aPlayer));

		return output("fleet/ship_pool.html");
	}

	if (ScrapNumber<1 || ScrapNumber>Ship->get_number())
	{
		if (ScrapNumber < 1)
		{
			ITEM("RESULT_MESSAGE",
					GETTEXT("The number of ships must be greater than 0."));
		}
		else
		{
			ITEM("RESULT_MESSAGE",
					GETTEXT("The number of ships can't be greater than the current number of ships."));
		}

		ITEM("STRING_SHIPS_IN_THE_POOL", GETTEXT("Ships in the Pool"));

		ITEM("POOL_INFO", aPlayer->get_dock()->dock_info_html());

		ITEM("STRING_REPAIRING_BAY", GETTEXT("Repairing Bay"));

		ITEM("REPAIRING_BAY_INFORMATION",
				aPlayer->get_repair_bay()->repairing_bay_info_html());

		ITEM("STRING_CLASS_SPECIFICATION", GETTEXT("Class Specification"));

		ITEM("CLASS_INFORMATION", aPlayer->get_ship_design_list()->print_html(aPlayer));

		return output("fleet/ship_pool.html");
	}

	CShipSize *
		Body = SHIP_SIZE_TABLE->get_by_id(Ship->get_body());
	CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id(Ship->get_armor());

	static CString
		Message;
	Message.clear();

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

	CString
		ClassInfoURL;
	ClassInfoURL.clear();
	ClassInfoURL.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
						DesignID,
						Ship->get_name());

	CString
		IncomeStringWithComma,
		ScrapNumberStringWithComma;
	IncomeStringWithComma = dec2unit(Income);
	ScrapNumberStringWithComma = dec2unit(ScrapNumber);

	Message.format(GETTEXT("You will earn %1$s PP by scrapping %2$s ship(s) of %3$s."),
							(char *)IncomeStringWithComma,
							(char *)ScrapNumberStringWithComma,
							(char *)ClassInfoURL);

	Message += "<BR>\n";
	Message += GETTEXT("Are you sure you want to scrap the ship(s)?");

	ITEM("CONFIRM_MESSAGE", (char *)Message);

	ITEM("DESIGN_ID", DesignIDString);
	ITEM("SCRAP_NUMBER", ScrapNumberString);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/ship_pool_scrap_confirm.html");
}

