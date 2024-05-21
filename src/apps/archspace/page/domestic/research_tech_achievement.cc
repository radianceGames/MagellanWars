#include <libintl.h>
#include "../../pages.h"

bool
CPageResearchTechAchievement::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ITEM("TITLE", GETTEXT("You have researched the following techs :"));
	ITEM("STRING_SOCIAL_SCIENCE", GETTEXT("Social Science"));
	ITEM("STRING_INFORMATION_SCIENCE", GETTEXT("Information Science"));
	ITEM("STRING_MATTER_ENERGY_SCIENCE", GETTEXT("Matter-Energy Science"));
	ITEM("STRING_LIFE_SCIENCE", GETTEXT("Life Science"));
	ITEM("STRING_UPGRADE", GETTEXT("Upgrades"));
	ITEM("STRING_SCHEMATICS", GETTEXT("Schematics"));
	ITEM("STRING_ADV_MATTER_ENERGY_SCIENCE", GETTEXT("Advanced Matter-Energy Science"));

	ITEM("SOCIAL_SCIENCE_LIST", 
			aPlayer->known_science_list_html(CTech::TYPE_SOCIAL));
	ITEM("INFORMATION_SCIENCE_LIST", 
			aPlayer->known_science_list_html(CTech::TYPE_INFORMATION));
	ITEM("MATTER_ENERGY_SCIENCE_LIST", 
			aPlayer->known_science_list_html(CTech::TYPE_MATTER_ENERGY));
	ITEM("LIFE_SCIENCE_LIST", 
			aPlayer->known_science_list_html(CTech::TYPE_LIFE));	
//	system_log("end page handler %s", get_name());

	return output("domestic/research_tech_achievement.html");
}
