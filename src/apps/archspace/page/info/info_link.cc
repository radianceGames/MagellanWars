#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../banner.h"

bool
CPageInfoLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country, "INFO");
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country, "INFO");

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("INFO_MAIN", GETTEXT("[Info Main]"));
	ITEM("CLUSTERS", GETTEXT("[Clusters]"));
	ITEM("RANKING", GETTEXT("[Ranking]"));

	return true;
}
