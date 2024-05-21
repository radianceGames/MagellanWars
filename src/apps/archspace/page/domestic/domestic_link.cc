#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../banner.h"

bool
CPageDomesticLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country, "DOMESTIC");
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country, "DOMESTIC");

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("DOMESTIC_MAIN", GETTEXT("[Domestic Main]"));
	ITEM("CONCENTRATION_MODE", GETTEXT("[Concentration Mode]"));
	ITEM("WHITEBOOK", GETTEXT("[Whitebook]"));
	ITEM("PROJECT", GETTEXT("[Project]"));
	ITEM("RESEARCH", GETTEXT("[Research]"));
	ITEM("PLANET_MANAGEMENT", GETTEXT("[Planet Management]"));

	return true;
}
