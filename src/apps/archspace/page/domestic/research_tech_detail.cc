#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"
#include "../../game.h"

bool
CPageResearchTechDetail::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	static CString
		Effect;
	Effect.clear();

	QUERY("ID", IDString);
	int 
		ID = as_atoi(IDString);
	CTech *
		Tech = TECH_TABLE->get_by_id(ID);
	CHECK(!Tech,
			(char *)format(GETTEXT("There is no such a technology with ID %1$s."),
							dec2unit(ID)));

	CTechList *
		AvailableTechList = aPlayer->get_available_tech_list();

	if (!AvailableTechList->get_by_id(ID))
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You can't research %1$s tech right now."),
								Tech->get_name_with_level()));

		return output("domestic/research_error.html");
	}

	ITEM("STRING_CHECK",
			GETTEXT("Check this box if you want this technology to be researched now,"
					" and otherwise uncheck this box."));

	ITEM("TECH_NAME",
			(char *)format(
					GETTEXT("You have chosen the technology, %1$s."),
					Tech->get_name_with_level()));
	ITEM("DESCRIPTION", Tech->get_description());

	ITEM("STRING_EFFECT", GETTEXT("Effect"));

	CControlModel
		CMEffect = (CControlModel &)Tech->get_effect();

	for (int i=0 ; i<CControlModel::CM_MAX ; i++)
	{
		if (CMEffect.get_value(i) == 0) continue;

		Effect.format("%s %s %s",
						CMEffect.get_cm_description(i),
						GETTEXT("CM"),
						integer_with_sign(CMEffect.get_value(i)));
		Effect += "<BR>\n";
	}

	if (Tech->get_project())
	{
		CProject *
			Project = PROJECT_TABLE->get_by_id(Tech->get_project());
		if (Project)
		{
			Effect.format(GETTEXT("New Project : %1$s"),
							Project->get_name());
			Effect += "<BR>\n";
		}
	}

	CString
		ComponentName;
	ComponentName.clear();

	CIntegerList &
		ComponentList = (CIntegerList &)Tech->get_component_list();
	if (ComponentList.length() > 0)
	{
		bool
			Comma = false;

		for (int i=0 ; i<ComponentList.length() ; i++)
		{
			CComponent *
				Component = COMPONENT_TABLE->get_by_id((int)ComponentList.get(i));
			if (!Component) continue;
			if (Comma) ComponentName += ", ";
			ComponentName += Component->get_name();
			Comma = true;
		}
		Effect.format(GETTEXT("New Component : %1$s"),
				(char *)ComponentName);
	}


	if (Effect.length() > 0)
	{
		ITEM("EFFECT", (char *)Effect);
	} else
	{
		ITEM("EFFECT", GETTEXT("None"));
	}

	ITEM("COST",
			(char *)format(GETTEXT("Cost : %1$d RP"),
							aPlayer->get_research_cost(Tech)));

	ITEM("TECH_ID", Tech->get_id());

	ITEM("CHECK", (aPlayer->get_target_tech() == Tech->get_id()) ?
								"CHECKED":"");	

//	system_log("end page handler %s", get_name());

	return output("domestic/research_tech_detail.html");
}
