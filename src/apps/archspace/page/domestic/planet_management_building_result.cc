#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"

bool
CPagePlanetManagementBuildingResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	QUERY("PLANET_ID", PlanetIDString);
	int
		PlanetID = as_atoi(PlanetIDString);
	CPlanet *
		Planet = aPlayer->get_planet(PlanetID);
	CHECK(!Planet,
			(char *)format(GETTEXT("You don't have a planet with ID %1$s."),
							dec2unit(PlanetID)));

	QUERY("factory", FactoryString);
	int
		Factory = as_atoi(FactoryString);
	CHECK((Factory % 10), GETTEXT("The factory ratio is illegal."));
	CHECK((Factory < 0 || Factory > 100 ), GETTEXT("The factory ratio is out of range."));
	
	QUERY("laboratory", ResearchLabString);
	int
		ResearchLab = as_atoi(ResearchLabString);
	CHECK((ResearchLab % 10), GETTEXT("The research lab ratio is illegal."));
	CHECK((ResearchLab < 0 || ResearchLab > 100 ), GETTEXT("The reasearch lab ratio is out of range."));

	QUERY("military", MilitaryBaseString);
	int
		MilitaryBase = as_atoi(MilitaryBaseString);
	CHECK((MilitaryBase % 10), GETTEXT("The military base ratio is illegal."));
	CHECK((MilitaryBase < 0 || MilitaryBase > 100 ), GETTEXT("The military base ratio is out of range."));

	CHECK((Factory + ResearchLab + MilitaryBase) != 100,
					  		GETTEXT("The sum of ratio is not 100 %."));

	ITEM("TITLE", GETTEXT("Buildings"));
	ITEM("MESSAGE",
			GETTEXT("The building ratio has changed into :"));
	ITEM("STRING_FACTORY", GETTEXT("Factory"));
	ITEM("STRING_RESEARCH_LAB", GETTEXT("Research Lab."));
	ITEM("STRING_MILITARY_BASE", GETTEXT("Military Base"));

	ITEM("PLANET_NAME", Planet->get_name());
	ITEM("FACTORY_RATIO", Factory);
	ITEM("RESEARCH_LAB_RATIO", ResearchLab);
	ITEM("MILITARY_BASE_RATIO", MilitaryBase);
	ITEM("PLANET_ID", Planet->get_id());

	Planet->get_distribute_ratio().set_all(Factory, MilitaryBase, ResearchLab);
	Planet->type(QUERY_UPDATE);
	STORE_CENTER->store(*Planet);

//	system_log("end page handler %s", get_name());

	return output("domestic/planet_management_building_result.html");
}
