#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilSearchResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("COUNCIL_ID", CouncilIDString);
	int
		CouncilID = as_atoi(CouncilIDString);
	CCouncil *
		Council = COUNCIL_TABLE->get_by_id(CouncilID);
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

