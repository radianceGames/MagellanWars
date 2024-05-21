#include "project.h"
#include "define.h"
#include <libintl.h>
#include "player.h"

CProjectTable::CProjectTable()
{
}

CProjectTable::~CProjectTable()
{
	remove_all();
}

bool
CProjectTable::free_item(TSomething aItem)
{
	CProject
		*Project = (CProject*)aItem;

	if (!Project) return false;

	delete Project;

	return true;
}

int
CProjectTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CProject
		*Project1 = (CProject *)aItem1,
		*Project2 = (CProject *)aItem2;

	if (Project1->get_id() > Project2->get_id()) return 1;
	if (Project1->get_id() < Project2->get_id()) return -1;
	return 0;
}

int
CProjectTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CProject
		*Project = (CProject *)aItem;

	if (Project->get_id() > (int)aKey) return 1;
	if (Project->get_id() < (int)aKey) return -1;
	return 0;
}

int
CProjectTable::add_project(CProject *aProject)
{
	if (!aProject) return -1;

	if (find_sorted_key((TConstSomething)aProject->get_id()) >= 0) return -1;
	insert_sorted(aProject);

	return aProject->get_id();
}

bool
CProjectTable::remove_project(int aProjectID)
{
	int
		Index;

	Index = find_sorted_key((void*)aProjectID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CProject *
CProjectTable::get_by_id(int aProjectID)
{
	if (!length()) return NULL;

	int
		Index;

	Index = find_sorted_key((void*)aProjectID);

	if (Index < 0) return NULL;

	return (CProject*)get(Index);
}

char *
CProjectTable::selected_project_information_html(CCommandSet &aProjectSet)
{
	static CString
		Info;
	Info.clear();

	bool
		AnyProject = false;

	for (int i=0 ; i<length() ; i++)
	{
		if (!aProjectSet.has(i)) continue;

		if (AnyProject)
		{
			Info += "</td>\n";
			Info += "</tr>\n";

			Info += "<tr>\n";
			Info += "<td class=\"maintext\" align=\"center\" WIDTH=\"574\">&nbsp;</td>\n";
			Info += "</tr>\n";

			Info += "<tr>\n";
			Info += "<td class=\"maintext\" align=\"center\" WIDTH=\"574\">\n";
		}

		CProject *
			Project = (CProject *)get(i);

		Info += "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";

		Info += "<TR>\n";
		Info.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s (#%d)</TD>\n",
					Project->get_name(), Project->get_id());
		Info += "</TR>\n";

		Info += "<TR>\n";
		Info += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">";
		Info += Project->get_description();
		Info += "</TD>\n";
		Info += "</TR>\n";

		Info += "</TABLE>\n";

		AnyProject = true;
	}

	if (AnyProject)
	{
		return (char *)Info;
	} else
	{
		return NULL;
	}
}

char *
CProjectTable::selected_project_effect_html(CPlayer *aPlayer, CCommandSet &aProjectSet)
{
	static CString
		Info;
	Info.clear();

	Info = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Info += "<TR>\n";

	Info += "<TD CLASS=\"tabletxt\" WIDTH=\"149\" BGCOLOR=\"#171717\">";
	Info.format("&nbsp;%s", GETTEXT("Control Model"));
	Info += "</TD>\n";

	Info += "<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"395\">&nbsp;</TD>\n";

	Info += "</TR>\n";

	CControlModel
		Effect;
	Effect.clear();

	for (int i=0 ; i<length() ; i++)
	{
		if (!aProjectSet.has(i)) continue;

		CProject *
			Project = (CProject *)get(i);

		Effect += Project->get_effect();
	}

	Info += "<TR ALIGN=\"CENTER\">\n";
	Info += "<TD CLASS=\"tabletxt\" COLSPAN=\"4\">\n";
	Info += "<TABLE BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" WIDTH=\"350\">\n";

	CControlModel *
		ControlModel = aPlayer->get_control_model();
	int
		Column = 0;

	for (int i=CControlModel::CM_ENVIRONMENT ; i<CControlModel::CM_MAX ; i++)
	{
		if (Column == 1)
		{
			Info += "<TD CLASS=\"maintext\" WIDTH=\"83\" VALIGN=\"top\">&nbsp;</TD>\n";
		} else if (Column == 2)
		{
			Info += "</TR>\n";
			Info += "<TR>\n";
			Column = 0;
		}

		Info.format("<TD CLASS=\"maintext\" COLSPAN=\"2\" VALIGN=\"top\">%s %d (%s)</TD>\n",
					CControlModel::get_cm_description(i),
					ControlModel->get_value(i),
					integer_with_sign(Effect.get_value(i)));

		Column++;
	}

	if (Column == 1)
	{
		Info += "<TD CLASS=\"maintext\" WIDTH=\"83\" VALIGN=\"top\">&nbsp;</TD>\n";
		Info += "<TD CLASS=\"maintext\" COLSPAN=\"2\" VALIGN=\"top\">&nbsp;</TD>\n";
	}

	Info += "</TR>\n";
	Info += "</TABLE>\n";
	Info += "</TD>\n";
	Info += "</TR>\n";

/*
	Info += "<TR>\n";

	Info += "<TD CLASS=\"tabletxt\" WIDTH=\"149\" BGCOLOR=\"#171717\">";
	Info.format("&nbsp;%s", GETTEXT("Other Effect"));
	Info += "</TD>\n";

	Info += "<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"395\">";
	Info += "&nbsp;";
	Info += "</TD>\n";

	Info += "</TR>\n";

	Info += "<TR>\n";

	Info += "<TD CLASS=\"tabletxt\" COLSPAN=\"4\" ALIGN=\"CENTER\">&nbsp;</TD>\n";

	Info += "</TR>\n";
*/

	Info += "</TABLE>\n";

	return (char *)Info;
}


