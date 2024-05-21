#include <libintl.h>
#include "../../pages.h"

bool
CPageClassSpecification::handler(CPlayer *aPlayer)
{
	QUERY("DESIGN_ID", DesignIDString);
	if (!DesignIDString)
	{
		message_page("Wrong ship design number.");
		return true;
	}

	int
		DesignID = as_atoi(DesignIDString);
	CShipDesignList *
		DesignList = aPlayer->get_ship_design_list();
	CShipDesign *
		Design  = DesignList->get_by_id(DesignID);
	if (!Design)
	{
		message_page(GETTEXT("You don't have such a ship design."));
		return true;
	}

	ITEM("SPECIFICATION", Design->print_html(aPlayer));

	return output("fleet/class_specification.html");
}
