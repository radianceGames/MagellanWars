#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../banner.h"

bool
CPageCouncilLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country, "COUNCIL");
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country, "COUNCIL");

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("COUNCIL_MAIN", GETTEXT("[Council Main]"));
	ITEM("COUNCIL_VOTE", GETTEXT("[Council Vote]"));
	ITEM("ADMISSION_REQUEST", GETTEXT("[Admission Request]"));
	ITEM("INDEPENDENCE_DECLARATION", GETTEXT("[Independence Declaration]"));
	ITEM("DONATION", GETTEXT("[Donation]"));
	ITEM("SPEAKER_MENU", GETTEXT("[Speaker Menu]"));
	ITEM("PLAYER_ADMISSION", GETTEXT("[Player Admission]"));
	ITEM("COUNCIL_DIPLOMACY", GETTEXT("[Council Diplomacy]"));

	return true;
}
