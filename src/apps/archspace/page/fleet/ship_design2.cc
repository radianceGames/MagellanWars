#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../define.h"

bool
CPageShipDesign2::handler( CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("STRING_NAME", GETTEXT("Name"));
	ITEM("STRING_SIZE", GETTEXT("Size"));
	ITEM("STRING_COMPUTER", GETTEXT("Computer"));
	ITEM("STRING_ENGINE", GETTEXT("Engine"));
	ITEM("STRING_SHIELD",GETTEXT("Shield"));

	QUERY("CLASS_NAME", ClassName);
	if (is_valid_name((const char *)ClassName) == false)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't enter a valid name for class."));
		return output("fleet/ship_design_error.html");
	}
	aPlayer->set_component_list();
	ITEM("NAME",
			(char *)format("%s<INPUT TYPE=hidden NAME=CLASS_NAME VALUE=\"%s\">",
					ClassName, ClassName));

	QUERY("SHIP_SIZE", ShipSizeString);
	int
		ShipSize = as_atoi(ShipSizeString);
	if (ShipSize < 1 || ShipSize > MAX_SHIP_CLASS)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You selected wrong ship size. Please try again."));
		return output("fleet/ship_design_error.html");
	}

	CShipSize * 
		Body = (CShipSize *)(SHIP_SIZE_TABLE->get_by_id(4000 + ShipSize));
	ITEM("SIZE",
			(char *)format("%s<INPUT TYPE=hidden NAME=SHIP_SIZE VALUE=\"%d\">",
					Body->get_name(), ShipSize));

	CComponentList *
		ComponentList = aPlayer->get_component_list();

	CComputer *
		Computer = (CComputer *)ComponentList->get_best_component(CComponent::CC_COMPUTER);
	ITEM("COMPUTER",
			format("%s (%s %d)<INPUT TYPE=hidden NAME=COMPUTER VALUE=\"%d\">",
					Computer->get_name(), GETTEXT("Level"), Computer->get_level(), Computer->get_id()));

	CEngine *
		Engine = (CEngine *)ComponentList->get_best_component(CComponent::CC_ENGINE);
	ITEM("ENGINE",
			format("%s (%s %d)<INPUT TYPE=hidden NAME=ENGINE VALUE=\"%d\">",
					Engine->get_name(), GETTEXT("Level"), Engine->get_level(), Engine->get_id()));

	CShield *
		Shield = (CShield *)ComponentList->get_best_component(CComponent::CC_SHIELD);
	ITEM("SHIELD",
			(char *)format("%s (%s %d)<INPUT TYPE=hidden NAME=SHIELD VALUE=\"%d\">",
					Shield->get_name(), GETTEXT("Level"), Shield->get_level(), Shield->get_id()));

	ITEM("STRING_ARMOR", GETTEXT("Armor"));

	ITEM("ARMOR",
			ComponentList->armor_list_html());

	ITEM("SPACE_SLOT",
			(char *)format("%s / %s : %d",
							GETTEXT("Space"),
							GETTEXT("Slot"),
							Body->get_slot()));

	ITEM("WEAPON_LIST",
			Body->input_weapon_list_html(aPlayer));

	ITEM("DEVICE_LIST",
			Body->input_device_list_html(aPlayer));

	ITEM("STRING_PREVIOUS_DESIGN", GETTEXT("Previous Design"));
	ITEM("CLASS_INFORMATION", aPlayer->get_ship_design_list()->print_html(aPlayer));

//	system_log( "end page handler %s", get_name() );

	return output( "fleet/ship_design2.html" );
}
