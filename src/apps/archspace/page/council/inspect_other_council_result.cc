#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"

bool
CPageInspectOtherCouncilResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	QUERY("COUNCIL_ID", CouncilIDString);
	CHECK(CouncilIDString == NULL, GETTEXT("You have not entered a council ID."));
	int
		CouncilID = as_atoi(CouncilIDString);
	CCouncil *
		PlayerCouncil = aPlayer->get_council();
	CCouncil *
		Council = PlayerCouncil->relation_council(CouncilID);

	CHECK(!Council,
			GETTEXT("We don't share any clusters with this council."));

	ITEM("MENU_TITLE", GETTEXT("Council Diplomacy - Inspect Other Council"));

	ITEM("STRING_COUNCIL_NAME", GETTEXT("Council Name"));
	ITEM("STRING_COUNCIL_SPEAKER", GETTEXT("Council Speaker"));
	ITEM("STRING_COUNCIL_SLOGAN", GETTEXT("Council Slogan"));
	ITEM("STRING_COUNCIL_HONOR", GETTEXT("Council Honor"));
	ITEM("STRING_COUNCIL_RANKING", GETTEXT("Council Ranking"));
	ITEM("STRING_CLUSTER", GETTEXT("Cluster"));
	ITEM("STRING_RELATION", GETTEXT("Relation"));
	ITEM("STRING_TOP___MEMBERS", GETTEXT("Top Members"));
	ITEM("STRING_NAME", GETTEXT("Name"));
	ITEM("STRING_RACE", GETTEXT("Race"));
	ITEM("STRING_POWER", GETTEXT("Power"));
	ITEM("STRING_RANKING_IN_THE_COUNCIL", GETTEXT("Ranking in the Council"));


	ITEM("COUNCIL_NICK", Council->get_nick());

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

	ITEM("COUNCIL_SLOGAN", Council->get_slogan());
	ITEM("COUNCIL_HONOR", Council->get_honor_description());

	CRankTable *
		RankTable = COUNCIL_TABLE->get_overall_rank_table();
	int
		Rank = RankTable->get_rank_by_id(Council->get_id());
	ITEM("COUNCIL_RANKING", dec2unit(Rank));

	ITEM("CLUSTER", Council->get_cluster_names());
	CCouncilRelation *
		Relation = PlayerCouncil->get_relation(Council);
	if (Relation == NULL)
	{
		ITEM("RELATION", GETTEXT("None"));
	}
	else
	{
		if (Relation->get_relation() == CRelation::RELATION_SUBORDINARY)
		{
			if (Relation->get_council1() == PlayerCouncil)
			{
				ITEM("RELATION", GETTEXT("Subordinary"));
			}
			else
			{
				ITEM("RELATION", GETTEXT("Supremacy"));
			}
		}
		else
		{
			ITEM("RELATION", Relation->get_relation_description());
		}
	}

	ITEM("TOP___LIST", Council->top_three_list_html());

//	system_log("end page handler %s", get_name());

	return output("council/inspect_other_council_result.html");
}
