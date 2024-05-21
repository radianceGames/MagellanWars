#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageShipDesignDeleteConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	QUERY("DESIGN_ID", DesignIDString);

	int
		DesignID = as_atoi(DesignIDString);
	CShipDesignList *
		DesignList = aPlayer->get_ship_design_list();
	CShipDesign *
		Design = (CShipDesign *)DesignList->get_by_id(DesignID);
	if (!Design)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have ship design of ID number %1$d."),
								DesignID));
		return output("fleet/ship_design_error.html");
	}

	Message += GETTEXT("Now you are deleting the following ship design :");
	Message += "<BR><BR>\n";

	CShipSize *
		ShipSize = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(Design->get_body());
	Message.format("%s (%s %s)", Design->get_name(), ShipSize->get_name(), GETTEXT("Class"));
	Message += "<BR>\n";

	CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id(Design->get_armor());
	CComputer *
		Computer = (CComputer *)COMPONENT_TABLE->get_by_id(Design->get_computer());
	CEngine *
		Engine = (CEngine *)COMPONENT_TABLE->get_by_id(Design->get_engine());
	CShield *
		Shield = (CShield *)COMPONENT_TABLE->get_by_id(Design->get_shield());

	Message.format(GETTEXT("Equipped with %1$s Armor, %2$s Computer,"
							" %3$s Engine, %4$s Shield."),
					Armor->get_name(), Computer->get_name(),
					Engine->get_name(), Shield->get_name());
	Message += "<BR>\n";

	bool
		Comma = false;
	Message += GETTEXT("Weapon");
	Message += " - ";
	for (int i=0 ; i<ShipSize->get_weapon() ; i++)
	{
		CWeapon *
			Weapon = (CWeapon *)COMPONENT_TABLE->get_by_id(Design->get_weapon(i));
		if (Comma) Message += ", ";
		Message.format("%d %s", Design->get_weapon_number(i), Weapon->get_name());
		Comma = true;
	}
	Message += "<BR>\n";

	Comma = false;
	Message += GETTEXT("Device");
	Message += " - ";
	for (int i=0 ; i<ShipSize->get_device() ; i++)
	{
		CDevice *
			Device = (CDevice *)COMPONENT_TABLE->get_by_id(Design->get_device(i));
		if (!Device) continue;
		if (Comma) Message += ", ";
		Message += Device->get_name();
		Comma = true;
	}
	if (!Comma) Message += GETTEXT("None");

	Message += "<BR><BR>\n";

	Message += GETTEXT("Are you sure you want to delete this design?");

	ITEM("CONFIRM_MESSAGE", (char *)Message);

	ITEM("DESIGN_ID", DesignID);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/ship_design_delete_confirm.html");
}

