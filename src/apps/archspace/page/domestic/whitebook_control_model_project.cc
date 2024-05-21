#include <libintl.h>
#include "../../pages.h"

bool
CPageWhitebookControlModelProject::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ITEM("STRING_MODIFIER_S__BY_PROJECT",
			GETTEXT("Modifier(s) by Project"));

	CControlModel
		ProjectCM;

	CPurchasedProjectList *
		ProjectList = aPlayer->get_purchased_project_list();

	for (int i=0 ; i<ProjectList->length() ; i++)
	{
		CPurchasedProject *
			Project = (CPurchasedProject *)ProjectList->get(i);

		ProjectCM += Project->get_effect();
	}

	ITEM("STRING_ENVIRONMENT", GETTEXT("Environment"));
	ITEM("ENVIRONMENT_CM", ProjectCM.get_real_environment());

	ITEM("STRING_GROWTH", GETTEXT("Growth"));
	ITEM("GROWTH_CM", ProjectCM.get_real_growth());

	ITEM("STRING_PRODUCTION", GETTEXT("Production"));
	ITEM("PRODUCTION_CM", ProjectCM.get_real_production());

	ITEM("STRING_MILITARY", GETTEXT("Military"));
	ITEM("MILITARY_CM", ProjectCM.get_real_military());

	ITEM("STRING_COMMERCE", GETTEXT("Commerce"));
	ITEM("COMMERCE_CM", ProjectCM.get_real_commerce());

	ITEM("STRING_DIPLOMACY", GETTEXT("Diplomacy"));
	ITEM("DIPLOMACY_CM", ProjectCM.get_real_diplomacy());

	ITEM("STRING_EFFICIENCY", GETTEXT("Efficiency"));
	ITEM("EFFICIENCY_CM", ProjectCM.get_real_efficiency());

	ITEM("STRING_GENIUS", GETTEXT("Genius"));
	ITEM("GENIUS_CM", ProjectCM.get_real_genius());

	ITEM("STRING_FACILITY_COST", GETTEXT("Facility Cost"));
	ITEM("FACILITY_COST_CM", ProjectCM.get_real_facility_cost());

	ITEM("STRING_SPY", GETTEXT("Spy"));
	ITEM("SPY_CM", ProjectCM.get_real_spy());

	ITEM("STRING_RESEARCH", GETTEXT("Research"));
	ITEM("RESEARCH_CM", ProjectCM.get_real_research());

//	system_log("end page handler %s", get_name());

	return output("domestic/whitebook_control_model_project.html");
}
