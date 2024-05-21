#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../race.h"

bool
CPageWhitebookControlModel::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ITEM("STRING_CONTROL_MODEL", GETTEXT("Control Model"));

	ITEM("STRING_GROSS_CONTROL_MODEL", GETTEXT("Gross Control Model"));

	CControlModel *
		ControlModel = aPlayer->get_control_model();

	ITEM("STRING_ENVIRONMENT", GETTEXT("Environment"));
	ITEM("ENVIRONMENT_CM", ControlModel->get_real_environment());

	ITEM("STRING_GROWTH", GETTEXT("Growth"));
	ITEM("GROWTH_CM", ControlModel->get_real_growth());

	ITEM("STRING_PRODUCTION", GETTEXT("Production"));
	ITEM("PRODUCTION_CM", ControlModel->get_real_production());

	ITEM("STRING_MILITARY", GETTEXT("Military"));
	ITEM("MILITARY_CM", ControlModel->get_real_military());

	ITEM("STRING_COMMERCE", GETTEXT("Commerce"));
	ITEM("COMMERCE_CM", ControlModel->get_real_commerce());

	ITEM("STRING_DIPLOMACY", GETTEXT("Diplomacy"));
	ITEM("DIPLOMACY_CM", ControlModel->get_real_diplomacy());

	ITEM("STRING_EFFICIENCY", GETTEXT("Efficiency"));
	ITEM("EFFICIENCY_CM", ControlModel->get_real_efficiency());

	ITEM("STRING_GENIUS", GETTEXT("Genius"));
	ITEM("GENIUS_CM", ControlModel->get_real_genius());

	ITEM("STRING_FACILITY_COST", GETTEXT("Facility Cost"));
	ITEM("FACILITY_COST_CM", ControlModel->get_real_facility_cost());

	ITEM("STRING_SPY", GETTEXT("Spy"));
	ITEM("SPY_CM", ControlModel->get_real_spy());

	ITEM("STRING_RESEARCH", GETTEXT("Research"));
	ITEM("RESEARCH_CM", ControlModel->get_real_research());

	ITEM("STRING_RACIAL_MODIFIER", GETTEXT("Racial Modifier"));

	CRace *
		Race = RACE_TABLE->get_by_id(aPlayer->get_race());

	ITEM("RACE_MODIFIER", Race->race_control_model_information_html());

	ITEM("STRING_MODIFIER_S__BY_TECH",
			GETTEXT("Modifier(s) by Tech"));
	ITEM("STRING_MODIFIER_S__BY_PROJECT",
			GETTEXT("Modifier(s) by Project"));

	ITEM("STRING_MODIFIER_S__ON_PLANET",
			GETTEXT("Modifier(s) on Planet"));

	CPlanetList *
		PlanetList = aPlayer->get_planet_list();
	ITEM("PLANET_TABLE", PlanetList->whitebook_planet_list_html());

//	system_log("end page handler %s", get_name());

	return output("domestic/whitebook_control_model.html");
}
