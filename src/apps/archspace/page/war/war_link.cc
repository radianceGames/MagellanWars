#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../banner.h"

void
CPageWarLink::message_page(const char* aMessage)
{
	ITEM("ERROR_MESSAGE", aMessage);
	output("war/war_error.html");
}

bool
CPageWarLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country, "WAR");
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country, "WAR");

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("WAR_MAIN", GETTEXT("[War Main]"));
	ITEM("SIEGE_PLANET", GETTEXT("[Siege Planet]"));
	ITEM("BLOCKADE", GETTEXT("[Blockade]"));
	ITEM("RAID", GETTEXT("[Raid]"));
	ITEM("PRIVATEER", GETTEXT("[Privateer]"));
	ITEM("DEFENSE_PLAN", GETTEXT("[Defense Plan]"));
	ITEM("BATTLE_REPORT", GETTEXT("[Battle Report]"));

	return true;
}
