#include <libintl.h>
#include "../../pages.h"

bool
CPagePlanetManagement::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

#define PLANET_PER_PAGE		20

	CPlanetList *
		PlanetList = aPlayer->get_planet_list();

	QUERY("START_PLANET", StartPlanetString);
	int
		StartPlanetOrder = as_atoi(StartPlanetString);
	if (StartPlanetOrder < 1) StartPlanetOrder = 1;
	if (PlanetList->length() < StartPlanetOrder) StartPlanetOrder = 1;

	ITEM("PLANET_MANAGEMENT_LIST",
			aPlayer->planet_management_html(StartPlanetOrder - 1, PLANET_PER_PAGE));

	static CString
		PageList;
	PageList.clear();

	PageList.format("%s : ", GETTEXT("Planet Order"));

	int
		StartOrder = 1;
	while (1)
	{
		if (StartOrder != 1) PageList += "&nbsp;&nbsp;";

		if (StartOrder + PLANET_PER_PAGE > PlanetList->length())
		{
			PageList.format("<A HREF=\"planet_management.as?START_PLANET=%d\">%d - %d</A>",
							StartOrder, StartOrder, PlanetList->length());
			break;
		}
		else
		{
			PageList.format("<A HREF=\"planet_management.as?START_PLANET=%d\">%d - %d</A>",
							StartOrder, StartOrder, StartOrder + PLANET_PER_PAGE - 1);

			StartOrder += PLANET_PER_PAGE;
		}
	}

	ITEM("PLANET_PAGE_LIST", (char *)PageList);

//	system_log("end page handler %s", get_name());

#undef PLANET_PER_PAGE

	return output("domestic/planet_management.html");
}
