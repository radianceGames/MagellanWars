#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../banner.h"

bool
CPageDiplomacyLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country, "DIPLOMACY");
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country, "DIPLOMACY");

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("DIPLOMACY_MAIN", GETTEXT("[Diplomacy Main]"));
	ITEM("DIPLOMACY_STATUS_LIST", GETTEXT("[Diplomacy Status List]"));
	ITEM("READ_MESSAGE", GETTEXT("[Read Message]"));
	ITEM("SEND_MESSAGE", GETTEXT("[Send Message]"));
	ITEM("INSPECT_OTHER_PLAYER", GETTEXT("[Inspect Other Player]"));
	ITEM("SPECIAL_OPERATION", GETTEXT("[Special Operation]"));

	return true;
}
