#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../banner.h"

bool
CPageBlackMarketLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country, "BLACK_MARKET");
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country, "BLACK_MARKET");

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("THE_TECH_DECK", GETTEXT("[The Tech Deck]"));
	ITEM("THE_FLEET_DECK", GETTEXT("[The Fleet Deck]"));
	ITEM("OFFICER_S_LOUNGE", GETTEXT("[Officer's Lounge]"));
	ITEM("RARE_GOODS_OFFICE", GETTEXT("[Rare Goods Officer]"));
	ITEM("LEASER_S_OFFICE", GETTEXT("[Leaser's Office]"));

	return true;
}
