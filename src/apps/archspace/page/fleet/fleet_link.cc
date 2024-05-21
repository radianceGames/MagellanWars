#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../banner.h"

bool
CPageFleetLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country, "FLEET");
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country, "FLEET");

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("FLEET_MAIN", GETTEXT("[Fleet Main]"));
	ITEM("SHIP_DESIGN", GETTEXT("[Ship Design]"));
	ITEM("SHIP_BUILDING", GETTEXT("[Ship Building]"));
	ITEM("FLEET_COMMANDER", GETTEXT("[Fleet Commander]"));
	ITEM("SHIP_POOL", GETTEXT("[Ship Pool]"));
	ITEM("FORM_NEW_FLEET", GETTEXT("[Form a New Fleet]"));
	ITEM("REASSIGNMENT", GETTEXT("[Reassignment]"));
	ITEM("EXPEDITION", GETTEXT("[Expedition]"));
	ITEM("MISSION", GETTEXT("[Mission]"));
	ITEM("RECALL", GETTEXT("[Recall]"));
	ITEM("DISBAND", GETTEXT("[Disband]"));

	return true;
}
