#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageResearchResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());
	int ID = 0;
	CTech *
		Tech = NULL;
	QUERY("ID", IDString);
	if(IDString != NULL)
	{
		ID = as_atoi(IDString);
		Tech = TECH_TABLE->get_by_id(ID);
		CHECK(!Tech,
			(char *)format(GETTEXT("There is no such a technology with ID %1$s."),
					dec2unit(ID)));
	}
	QUERY("SelectedTech", TechName);
	if(TechName != NULL)
	{
		Tech = TECH_TABLE->get_by_name(TechName);

		CHECK(!Tech,
		(char *)format(GETTEXT("There is no such a technology with name %1$s."),
				TechName));
	}
	CHECK(!Tech,
			(char *)format(GETTEXT("No Tech Was Selected.")));


	//removed this to allow for tech queue'ing
	//if (!Tech->evaluate(aPlayer))
	//{
	//	ITEM("ERROR_MESSAGE",
	//			(char *)format(GETTEXT("You can't research %1$s tech right now."),
	//							Tech->get_name_with_level()));

	//	return output("domestic/research_error.html");
	//}

	static CString
		ImageURL;
	ImageURL.clear();
	ImageURL.format("%s/image/as_game/", (char *)CGame::mImageServerURL);

	QUERY("SELECT_CHECK", Check);
	if (!Check || strcmp(Check, "ON"))
	{
		aPlayer->set_target_tech(NONE);

		ImageURL += "result/domestic/research_tech_select_none.gif";
		ITEM("RESULT_IMAGE_URL", (char *)ImageURL);

		ITEM("RESULT_MESSAGE", GETTEXT("You unchecked select goal box."));

		return output("domestic/research_target_result.html");
	}
	if (aPlayer->has_tech(ID))
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You can't research %1$s tech for you alreeady have it"),
								Tech->get_name_with_level()));

		return output("domestic/research_error.html");
	}

	switch (Tech->get_type())
	{
		case CTech::TYPE_INFORMATION :
			ImageURL += "result/domestic/research_tech_select_info.gif";
			break;

		case CTech::TYPE_LIFE :
			ImageURL += "result/domestic/research_tech_select_life.gif";
			break;

		case CTech::TYPE_MATTER_ENERGY :
			ImageURL += "result/domestic/research_tech_select_matter.gif";
			break;

		case CTech::TYPE_SOCIAL :
			ImageURL += "result/domestic/research_tech_select_social.gif";
			break;

		default :
			break;
	}
	static CString
		Message;
	Message.clear();

	aPlayer->set_target_tech(Tech->get_id());

	Message.format(GETTEXT("Current goal technology is %1$s."),
					Tech->get_name_with_level());
	Message += "<BR>\n";

	int
		NeededRP = aPlayer->get_research_cost(Tech) - aPlayer->get_research();
	if (NeededRP > 0)
	{
		Message.format(GETTEXT("You will need %1$s more RP to complete the research."),
						dec2unit(NeededRP));
	} else
	{
		Message += GETTEXT("You don't need RP any more to complete the research.");
	}

	ITEM("RESULT_IMAGE_URL", (char *)ImageURL);

	ITEM("RESULT_MESSAGE", (char *)Message);

//	system_log("end page handler %s", get_name());

	return output("domestic/research_target_result.html");
}
