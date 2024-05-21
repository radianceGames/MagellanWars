#include <libintl.h>
#include "../../pages.h"
#include "../../race.h"

bool
CPageWhitebookControlModelDetail::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	QUERY("SECTION", SectionString);
	if (!SectionString)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't select any control model."));
		return output("domestic/whitebook_error.html");
	}

	int
		Section;
	if (!strcmp(SectionString, "ENVIRONMENT"))
	{
		Section = CControlModel::CM_ENVIRONMENT;
	} else if (!strcmp(SectionString, "GROWTH"))
	{
		Section = CControlModel::CM_GROWTH;
	} else if (!strcmp(SectionString, "RESEARCH"))
	{
		Section = CControlModel::CM_RESEARCH;
	} else if (!strcmp(SectionString, "PRODUCTION"))
	{
		Section = CControlModel::CM_PRODUCTION;
	} else if (!strcmp(SectionString, "MILITARY"))
	{
		Section = CControlModel::CM_MILITARY;
	} else if (!strcmp(SectionString, "SPY"))
	{
		Section = CControlModel::CM_SPY;
	} else if (!strcmp(SectionString, "COMMERCE"))
	{
		Section = CControlModel::CM_COMMERCE;
	} else if (!strcmp(SectionString, "EFFICIENCY"))
	{
		Section = CControlModel::CM_EFFICIENCY;
	} else if (!strcmp(SectionString, "GENIUS"))
	{
		Section = CControlModel::CM_GENIUS;
	} else if (!strcmp(SectionString, "DIPLOMACY"))
	{
		Section = CControlModel::CM_DIPLOMACY;
	} else if (!strcmp(SectionString, "FACILITY_COST"))
	{
		Section = CControlModel::CM_FACILITY_COST;
	} else
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You selected a wrong ontrol model."));
		return output("domestic/whitebook_error.html");
	}

	ITEM("TITLE",
			(char *)format("%s %s",
							CControlModel::get_cm_description(Section),
							GETTEXT("Control Model")));

	ITEM("STRING_RACIAL_MODIFIER", GETTEXT("Racial Modifier"));

	CRace *
		Race = aPlayer->get_race_data();
	CControlModel &
		RaceCM = (CControlModel &)Race->get_control_model();
	int
		RaceValue = RaceCM.get_value(Section);

	ITEM("RACIAL_MODIFIER", integer_with_sign(RaceValue));

	ITEM("STRING_MODIFIER_S__BY_TECH",
			GETTEXT("Modifier(s) by Tech"));

	CControlModel
		TechCM;
	CKnownTechList *
		TechList = aPlayer->get_tech_list();

	for (int i=0 ; i<TechList->length() ; i++)
	{
		CKnownTech *
			Tech = (CKnownTech *)TechList->get(i);

		TechCM += Tech->get_effect();
	}
	int
		TechValue = TechCM.get_value(Section);

	ITEM("MODIFIER_BY_TECH", integer_with_sign(TechValue));

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
	int
		ProjectValue = ProjectCM.get_value(Section);

	ITEM("MODIFIER_BY_PROJECT", integer_with_sign(ProjectValue));

	ITEM("STRING_TEMPORARY_MODIFIER", GETTEXT("Temporary Modifier"));

	CControlModel *
		PlayerCM = aPlayer->get_control_model();
	int
		PlayerValue = PlayerCM->get_value(Section);
	int
		TemporaryCM = PlayerValue - (RaceValue + TechValue + ProjectValue);

	ITEM("TEMPORARY_MODIFIER", integer_with_sign(TemporaryCM));

//	system_log("end page handler %s", get_name());

	return output("domestic/whitebook_control_model_detail.html");
}
