#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilSearch::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("BY_WHAT", ByWhatString);
	if (!ByWhatString)
	{
		message_page("You didn't specify the way how to search a council.");
		return true;
	}

	CCouncil *
		Council = NULL;

	if (!strcmp(ByWhatString, "COUNCIL_NAME"))
	{
		QUERY("COUNCIL_NAME", CouncilNameString);

		if (!CouncilNameString)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You didn't enter a council name."));
			return output("info/council_search_error.html");
		}

		static CString
			NameList;
		NameList.clear();

		bool
			AnyCouncil = false;
		for (int i=0 ; i<COUNCIL_TABLE->length() ; i++)
		{
			CCouncil *
				Council = (CCouncil *)COUNCIL_TABLE->get(i);
			if (strstr(Council->get_name(), CouncilNameString))
			{
				NameList += "<BR>\n";
				NameList.format("<A HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A>\n",
								Council->get_id(), Council->get_nick());

				AnyCouncil = true;
			}
		}

		if (AnyCouncil)
		{
			ITEM("SELECT_COUNCIL_MESSAGE",
					GETTEXT("Select the council you want from the following list."));

			ITEM("COUNCIL_LIST", (char *)NameList);

			return output("info/council_search_name_list.html");
		} else
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("That council doesn't exist."));
			return output("info/council_search_error.html");
		}
	}

	if (!strcmp(ByWhatString, "COUNCIL_ID"))
	{
		QUERY("COUNCIL_ID", CouncilIDString);
		int
			CouncilID = as_atoi(CouncilIDString);
		Council = COUNCIL_TABLE->get_by_id(CouncilID);
	}

	if (!Council)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("That council doesn't exist."));
		return output("info/council_search_error.html");
	}

	ITEM("STRING_COUNCIL_NAME", GETTEXT("Council Name"));
	ITEM("COUNCIL_NAME", Council->get_nick());

	ITEM("STRING_COUNCIL_SPEAKER", GETTEXT("Council Speaker"));

	CPlayer *
		Speaker = Council->get_speaker();
	if (Speaker != NULL)
	{
		ITEM("COUNCIL_SPEAKER", Speaker->get_nick());
	}
	else
	{
		ITEM("COUNCIL_SPEAKER", GETTEXT("None"));
	}

	ITEM("STRING_COUNCIL_SLOGAN", GETTEXT("Council Slogan"));
	ITEM("COUNCIL_SLOGAN", Council->get_slogan());

	ITEM("STRING_COUNCIL_HONOR", GETTEXT("Council Honor"));
	ITEM("COUNCIL_HONOR", Council->get_honor());

	ITEM("STRING_COUNCIL_MEMBERS", GETTEXT("Council Members"));
	ITEM("COUNCIL_MEMBERS",
			(char *)format("%d/%d", Council->get_number_of_members(), Council->max_member()));

	ITEM("MEMBERS_INFORMATION", Council->info_members_information_html());

//	system_log("end page handler %s", get_name());

	return output("info/council_search_result.html");
}

