#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncil::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	CString Temp;

	ITEM("STRING_COUNCIL_NAME", GETTEXT("Council Name"));
	ITEM("STRING_COUNCIL_SLOGAN", GETTEXT("Council Slogan"));
	ITEM("STRING_COUNCIL_SPEAKER", GETTEXT("Council Speaker"));
	ITEM("STRING_CLUSTER", GETTEXT("Cluster"));
	ITEM("STRING_COUNCIL_HONOR", GETTEXT("Council Honor"));
	ITEM("STRING_COUNCIL_MEMBERS", GETTEXT("Council Members"));
	ITEM("STRING_COUNCIL_SAFE", GETTEXT("Council Safe"));
	ITEM("STRING_COUNCIL_RANKING",
			GETTEXT("Council Ranking"));

	CCouncil *Council = aPlayer->get_council();
	assert(Council);

	ITEM("COUNCIL_NICK", Council->get_nick());
	ITEM("COUNCIL_SLOGAN", Council->get_slogan());

	CPlayer *
		Speaker = Council->get_speaker();
	if (Speaker != NULL)
	{
		ITEM("SPEAKER_NAME", Speaker->get_nick());	
	}
	else
	{
		ITEM("SPEAKER_NAME", GETTEXT("None"));
	}

	ITEM("CLUSTER_NICK", Council->get_cluster_names());
	ITEM("COUNCIL_HONOR", (char*)format("%s(%d)", 
			Council->get_honor_description(), Council->get_honor()));
	ITEM("COUNCIL_MEMBERS", 
			(char*)format("%d/%d", Council->get_number_of_members(), 
							Council->max_member()));
	Temp.format("%s %s", dec2unit(Council->get_production()), GETTEXT("PP"));
	ITEM("COUNCIL_SAFE", (char*)Temp);

	CRankTable *
		RankTable = COUNCIL_TABLE->get_overall_rank_table();
	int
		Rank = RankTable->get_rank_by_id(Council->get_id());
	ITEM("COUNCIL_RANKING", dec2unit(Rank));

	ITEM("ACHIEVED_PROJECT_LIST", Council->achieved_project_html());

	ITEM("STRING_ACHIEVED_PROJECT_S_", GETTEXT("Achieved Project(s)"));
	ITEM("STRING_COUNCIL_VOTE", GETTEXT("Council Vote"));
	ITEM("STRING_ADMISSION_REQUEST", GETTEXT("Admission Request"));
	ITEM("STRING_INDEPENDENCE_DECLARATION", 
			GETTEXT("Independence Declaration"));
	ITEM("STRING_DONATION", GETTEXT("Donation"));
	ITEM("STRING_SPEAKER_MENU", GETTEXT("Speaker Menu"));
	ITEM("EXPLAIN_COUNCIL_VOTE",
			GETTEXT("You can vote for a new council speaker.")); 
	ITEM("EXPLAIN_ADMISSION_REQUEST",
			GETTEXT("You may request admission to another council."));
	ITEM("EXPLAIN_INDEPENDENCE_DECLARATION",
			GETTEXT("You can create a new council."));
	ITEM("EXPLAIN_DONATION",
			GETTEXT("You can donate some PP to the council.")); 
	ITEM("EXPLAIN_SPEAKER_MENU",
			GETTEXT("If you are the speaker of this council,"
					" you can use this menu to manage council functions."));

//	system_log("end page handler %s", get_name());

	return output("council/council.html");
}
