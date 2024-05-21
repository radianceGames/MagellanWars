#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../define.h"

bool
CPageEmpireDiplomacy::handler(CPlayer *aPlayer)
{
	ITEM( "EMPIRE_RELATION", aPlayer->get_degree_name_of_empire_relation() );
	ITEM( "COURT_RANK", aPlayer->get_court_rank_name() );

	CString
		Message;

	Message = "<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	Message += "<TR>\n";
	Message.format("<TH CLASS=tabletxt WIDTH=200 BGCOLOR=#171717>&nbsp;%1$s</TH>\n",
					GETTEXT("Action"));
	Message.format("<TH CLASS=tabletxt WIDTH=100 BGCOLOR=#171717>&nbsp;%1$s</TH>\n",
					GETTEXT("Date"));
	Message.format("<TH CLASS=tabletxt WIDTH=100 BGCOLOR=#171717>&nbsp;%1$s</TH>\n",
					GETTEXT("Response"));
	Message.format("<TH CLASS=tabletxt WIDTH=150 BGCOLOR=#171717>&nbsp;%1$s</TH>\n",
					GETTEXT("Target/Amount"));
	Message += "</TR>\n";

	for (int i=0 ; i<aPlayer->get_empire_action_list()->length() ; i++)
	{
		CEmpireAction *
			Action = (CEmpireAction *)aPlayer->get_empire_action_list()->get(i);

		Message += "<TR>\n";
		if (Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING)
		{
			Message.format(
				"<TD CLASS=white>&nbsp;<A HREF=\"show_action.as?ACTION_ID=%d\">%s</A></TD>\n"
				"<TD CLASS=white ALIGN=center>%s</TD>\n"
				"<TD CLASS=white ALIGN=center>%s</TD>\n"
				"<TD CLASS=white ALIGN=center>%s</TD></TR>\n",
					Action->get_id(),
					Action->get_action_name(),
					Action->get_time_str(),
					Action->get_answer_name(),
					Action->get_target_amount_str( aPlayer ) );
		}
		else
		{
			Message.format(
				"<TD CLASS=tabletxt>&nbsp;<A HREF=\"show_action.as?ACTION_ID=%d\">%s</A></TD>\n"
				"<TD CLASS=tabletxt ALIGN=center>%s</TD>\n"
				"<TD CLASS=tabletxt ALIGN=center>%s</TD>\n"
				"<TD CLASS=tabletxt ALIGN=center>%s</TD></TR>\n",
					Action->get_id(),
					Action->get_action_name(),
					Action->get_time_str(),
					Action->get_answer_name(),
					Action->get_target_amount_str( aPlayer ) );
		}
	}

	Message += "</TABLE>\n";

	ITEM("STRING_THE_NUMBER_OF_LEFT_PLANETS_IN_THE_EMPIRE_CLUSTER",
			GETTEXT("The number of planets left in the Empire cluster"));
	ITEM("THE_NUMBER_OF_LEFT_PLANETS_IN_THE_EMPIRE_CLUSTER",
			dec2unit(EMPIRE->get_number_of_empire_planets()));
	ITEM("STRING_THE_NUMBER_OF_TAKEN_PLANETS_BY_YOU",
			GETTEXT("The number of planets taken by you"));
	ITEM("THE_NUMBER_OF_TAKEN_PLANETS_BY_YOU",
			dec2unit(EMPIRE_PLANET_INFO_LIST->get_number_of_planets_by_owner(aPlayer, CEmpire::LAYER_EMPIRE_PLANET)));
	ITEM( "RESULT_MESSAGE", (char*)Message );

	return output("empire/empire_diplomacy.html");
}
