#include <libintl.h>
#include "../../pages.h"

bool
CPageEmpireRequestMediation::handler(CPlayer *aPlayer)
{
	ITEM( "DESCRIPTION", (char*)(*mRequestMediationDescription) );
	ITEM( "EMPIRE_RELATION", aPlayer->get_degree_name_of_empire_relation() );
	ITEM( "COURT_RANK", aPlayer->get_court_rank_name() );

	CCouncil
		*Council = aPlayer->get_council();

	CString
		Select,
		Message;

	Message = "<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	Message.format( "<TR><TH WIDTH=200 CLASS=tabletxt BGCOLOR=#171717><FONT COLOR=666666>%s</FONT></TH>\n", GETTEXT("Player") );
	Message.format( "<TH WIDTH=100 CLASS=tabletxt BGCOLOR=#171717><FONT COLOR=666666>%s</FONT></TH>\n", GETTEXT("Relation") );
	Message.format( "<TH WIDTH=100 CLASS=tabletxt BGCOLOR=#171717><FONT COLOR=666666>%s</FONT></TH>\n", GETTEXT("Power") );
	Message.format( "<TH WIDTH=100 CLASS=tabletxt BGCOLOR=#171717><FONT COLOR=666666>%s</FONT></TH></TR>\n", GETTEXT("Honor") );

	Select = "<SELECT NAME=FOE>\n";

	for( int i = 0; i < Council->get_members()->length(); i++ ){
		CPlayer
			*Foe = (CPlayer*)(Council->get_members()->get(i));
		if( Foe->get_game_id() == aPlayer->get_game_id() ) continue;
		if( Foe->is_dead() ) continue;

		CPlayerRelation
			*Relation = aPlayer->get_relation(Foe);
		if( Relation == NULL ) continue;
		if( Relation->get_relation() != CRelation::RELATION_WAR &&
			Relation->get_relation() != CRelation::RELATION_TOTAL_WAR )
			continue;

		Message.format( "<TR><TD CLASS=tabletxt>&nbsp;%s</TD>\n", Foe->get_nick() );
		Message.format( "<TD CLASS=tabletxt>&nbsp;%s</TD>\n", Relation->get_relation_description() );
		Message.format( "<TD CLASS=tabletxt ALIGN=center VALIGN=middle>%s</TD>\n", dec2unit(Foe->get_power()) );
		Message.format( "<TD CLASS=tabletxt ALIGN=center>%s</TD></TR>\n", Foe->get_honor_description() );

		Select.format( "<OPTION VALUE=p%d>Player - %s</OPTION>\n", Foe->get_game_id(), Foe->get_nick() );
	}

	Message += "</TABLE>\n";

	if( Council->get_speaker_id() == aPlayer->get_game_id() ){
		Message += "&nbsp;<BR>\n";
		Message += "<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
		Message.format( "<TR><TH WIDTH=200 CLASS=tabletxt BGCOLOR=#171717><FONT COLOR=666666>%s</FONT></TH>\n", GETTEXT("Council") );
		Message.format( "<TH WIDTH=100 CLASS=tabletxt BGCOLOR=#171717><FONT COLOR=666666>%s</FONT></TH>\n", GETTEXT("Relation") );
		Message.format( "<TH WIDTH=100 CLASS=tabletxt BGCOLOR=#171717><FONT COLOR=666666>%s</FONT></TH>\n", GETTEXT("Power") );
		Message.format( "<TH WIDTH=100 CLASS=tabletxt BGCOLOR=#171717><FONT COLOR=666666>%s</FONT></TH></TR>\n", GETTEXT("Honor") );

		for( int i = 0; i < Council->get_relation_list()->length(); i++ ){
			CCouncilRelation
				*Relation = (CCouncilRelation*)(Council->get_relation_list()->get(i));
			if( Relation->get_relation() != CRelation::RELATION_WAR &&
				Relation->get_relation() != CRelation::RELATION_TOTAL_WAR )
				continue;
			
			CCouncil
				*Foe = (Relation->get_council1() == Council)?Relation->get_council2():Relation->get_council1();

			Message.format( "<TR><TD CLASS=tabletxt>&nbsp;%s</TD>\n", Foe->get_nick() );
			Message.format( "<TD CLASS=tabletxt>&nbsp;%s</TD>\n", Relation->get_relation_description() );
			Message.format( "<TD CLASS=tabletxt ALIGN=center VALIGN=middle>%s</TD>\n", dec2unit(Foe->get_power()) );
			Message.format( "<TD CLASS=tabletxt ALIGN=center>%s</TD></TR>\n", Foe->get_honor_description() );

			Select.format( "<OPTION VALUE=c%d>Council - %s</OPTION>\n", Foe->get_id(), Foe->get_nick() );
		}

		Message += "</TABLE>\n";
	}

	Select += "</SELECT>\n";

	ITEM( "ENEMY_LIST", (char*)Message );
	ITEM( "ENEMY_SELECT", (char*)Select );

	return output("empire/request_mediation.html");
}
