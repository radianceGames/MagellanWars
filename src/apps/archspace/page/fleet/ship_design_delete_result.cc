#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageShipDesignDeleteResult::handler(CPlayer *aPlayer)
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

	switch (Design->can_delete(aPlayer))
	{
		case CShipDesign::USED_IN_FLEET :
		{
			ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Ships of %1$s class exist in your fleet. You"
										" cannot delete this class.<BR>Please try it"
										" later."),
								Design->get_name()));
			return output("fleet/ship_design_error.html");
		}

		case CShipDesign::USED_IN_DOCK :
		{
			ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Ships of %1$s class exist in your dock. You"
										" cannot delete this class.<BR>Please try it"
										" later."),
								Design->get_name()));
			return output("fleet/ship_design_error.html");
		}

		case CShipDesign::USED_IN_REPAIR_BAY :
		{
			ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Ships of %1$s class exist in your repairing bay."
										" You cannot delete this class.<BR>Please try"
										" it later."),
								Design->get_name()));
			return output("fleet/ship_design_error.html");
		}

		case CShipDesign::USED_IN_BUILD_Q :
		{
			ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Ships of %1$s class are in construction. You"
										" cannot delete this class.<BR>Please try it"
										" later."),
								Design->get_name()));
			return output("fleet/ship_design_error.html");
		}
	}

	Design->type(QUERY_DELETE);
	*STORE_CENTER << *Design;

	ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("You have successfully deleted"
									" the old ship design : %1$s."),
							Design->get_name()));

	DesignList->remove_ship_design(DesignID);

	ITEM("STRING_NEW_CLASS_NAME", GETTEXT("New Class Name"));
	ITEM("STRING_SIZE", GETTEXT("Size"));

	ITEM("AVAILABLE_SIZE_LIST", SHIP_SIZE_TABLE->available_size_list_html(aPlayer));

	ITEM("STRING_SIZE_LIST", GETTEXT("Size List"));

	ITEM("STRING_SIZE", GETTEXT("Size"));
	ITEM("STRING_NAME", GETTEXT("Name"));
	ITEM("STRING_SPACE", GETTEXT("Space"));
	ITEM("STRING_WEAPON", GETTEXT("Weapon"));
	ITEM("STRING_SPACE_SLOT", GETTEXT("Space/Slot"));
	ITEM("STRING_DEVICE_SLOT", GETTEXT("Device Slot"));
	ITEM("STRING_BUILDING_COST", GETTEXT("Building Cost"));

	ITEM("SIZE_INFORMATION", SHIP_SIZE_TABLE->size_information_html(aPlayer));

	ITEM("STRING_PREVIOUS_DESIGN", GETTEXT("Previous Design"));

	ITEM("CLASS_INFORMATION", aPlayer->get_ship_design_list()->print_html(aPlayer));

//	system_log( "end page handler %s", get_name() );

	return output("fleet/ship_design.html");
}

