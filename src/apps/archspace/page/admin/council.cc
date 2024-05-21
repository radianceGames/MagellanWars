#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../player.h"

bool
CPageAdminCouncil::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());
    SLOG("page/admin/council.cc : CPageAdminCouncil::hander : Player #%d tried to access council.as", aPlayer->get_game_id());
   // Default player access check
    CPortalUser *thisAdmin = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
    CString error_msg;
    error_msg.format("%s '%s' %s.", 
                     GETTEXT("Access Level "), 
                     user_level_to_string(thisAdmin->get_user_level()), 
                     GETTEXT("does not grant access"));
    if (!ADMIN_TOOL->has_access(aPlayer))
    {
       ITEM("ERROR_MESSAGE", (char *)error_msg);
       return output("admin/admin_error.html"); 
    }

    SLOG("page/admin/council.cc : CPageAdminCouncil::hander : Player #%d has admin access", aPlayer->get_game_id());

	QUERY("COUNCIL_ID", CouncilIDString);
	
	SLOG("page/admin/council.cc : CPageAdminCouncil::hander : Player #%d's query was successful", aPlayer->get_game_id());

	int
		CouncilID = as_atoi(CouncilIDString);
	CCouncil *
		Council = COUNCIL_TABLE->get_by_id(CouncilID);

	if (Council == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no such a council with ID #%1$s."),
							dec2unit(CouncilID)));
		return output("admin/admin_error.html");
	}

	QUERY("BY_WHAT", ByWhatString);

	if (ByWhatString)
	{
		if (!strcmp(ByWhatString, "NEW_SLOGAN"))
		{
			QUERY("NEW_SLOGAN", NewSloganString);

			if (NewSloganString == NULL)
			{
			}
			else
			{
			   	ADMIN_TOOL->add_admin_action_log(thisAdmin, 
                     (char *)format("Changed Council #%d slogan from %s to %s",
                     Council->get_id(), Council->get_slogan(), NewSloganString));
				Council->set_slogan(NewSloganString);
				Council->type(QUERY_UPDATE);
				STORE_CENTER->store(*Council);
			}
		}
		else if (!strcmp(ByWhatString, "NEW_PRODUCTION_POINT"))
		{
			QUERY("NEW_PRODUCTION_POINT", NewProductionPointString);

			int
				NewProductionPoint = as_atoi(NewProductionPointString);
		   	ADMIN_TOOL->add_admin_action_log(thisAdmin, 
                  (char *)format("Changed Council #%d PP from %d to %d",
                  Council->get_id(), Council->get_production(), NewProductionPoint));

			Council->set_production(NewProductionPoint);

			Council->type(QUERY_UPDATE);
			STORE_CENTER->store(*Council);
		}
		else if (!strcmp(ByWhatString, "NEW_AUTO_ASSIGN"))
		{
			bool
				AutoAssign = Council->get_auto_assign();
			if (AutoAssign == true)
			{
				Council->set_auto_assign(false);
			   	ADMIN_TOOL->add_admin_action_log(thisAdmin, 
                     (char *)format("Changed Council #%d autoassign from TRUE to FALSE",
                     Council->get_id()));
			}
			else
			{
				Council->set_auto_assign(true);
			   	ADMIN_TOOL->add_admin_action_log(thisAdmin, 
                     (char *)format("Changed Council #%d autoassign from FALSE to TRUE",
                     Council->get_id()));
			}

			Council->type(QUERY_UPDATE);
			STORE_CENTER->store(*Council);
		}
	}

	static CString
		CouncilInfo;
	CouncilInfo.clear();

	CouncilInfo = "<TR>\n";
	CouncilInfo += "<TD ALIGN=\"center\">\n";

	CouncilInfo += "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	CouncilInfo += "<TR>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Name"));
	CouncilInfo += "</TD>\n";

	CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
						Council->get_nick());

	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo += "<FORM METHOD=post ACTION=council.as>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Slogan"));
	CouncilInfo += "</TD>\n";

	CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s<BR>\n",
						Council->get_slogan());

	CouncilInfo += "<INPUT TYPE=input NAME=NEW_SLOGAN>\n";
	CouncilInfo.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n", Council->get_id());
	CouncilInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=NEW_SLOGAN>\n";
	CouncilInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
						(char *)CGame::mImageServerURL);
	CouncilInfo += "</TD>\n";

	CouncilInfo += "</FORM>\n";
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Speaker"));
	CouncilInfo += "</TD>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\">\n";
	CouncilInfo += "<FORM METHOD=post ACTION=\"player_confirm.as\">\n";
	CouncilInfo += Council->get_speaker()->get_nick();
	CouncilInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=PLAYER_ID>\n";
	CouncilInfo.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",
						Council->get_speaker()->get_game_id());
	CouncilInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
						(char *)CGame::mImageServerURL);
	CouncilInfo += "</FORM>\n";
	CouncilInfo += "</TD>\n";

	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Cluster"));
	CouncilInfo += "</TD>\n";

	int
		HomeClusterID = Council->get_home_cluster_id();
	CCluster *
		HomeCluster = UNIVERSE->get_by_id(HomeClusterID);
	if (HomeCluster == NULL)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
							HomeCluster->get_nick());
	}

	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Honor"));
	CouncilInfo += "</TD>\n";

	CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s(%s)</TD>\n",
						Council->get_honor_description(), dec2unit(Council->get_honor()));

	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo += "<FORM METHOD=post ACTION=council.as>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Council Production"));
	CouncilInfo += "</TD>\n";

	CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s %s<BR>\n",
						dec2unit(Council->get_production()), GETTEXT("PP"));

	CouncilInfo += "<INPUT TYPE=input NAME=NEW_PRODUCTION_POINT>\n";
	CouncilInfo.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n", Council->get_id());
	CouncilInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=NEW_PRODUCTION_POINT>\n";
	CouncilInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
						(char *)CGame::mImageServerURL);
	CouncilInfo += "</TD>\n";

	CouncilInfo += "</FORM>\n";
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Member(s)"));
	CouncilInfo += "</TD>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\">\n";
	CPlayerList *
		MemberList = Council->get_members();
	for (int i=0 ; i<MemberList->length() ; i++)
	{
		CPlayer *
			Member = (CPlayer *)MemberList->get(i);

		if (i > 0) CouncilInfo += "<BR>\n";
		CouncilInfo += "<FORM METHOD=post ACTION=\"player_confirm.as\">\n";
		CouncilInfo += Member->get_nick();
		CouncilInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=PLAYER_ID>\n";
		CouncilInfo.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",
							Member->get_game_id());
		CouncilInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
							(char *)CGame::mImageServerURL);
		CouncilInfo += "</FORM>\n";
	}
	CouncilInfo += "</TD>\n";

	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Rank"));
	CouncilInfo += "</TD>\n";

	CRankTable *
		RankTable = COUNCIL_TABLE->get_overall_rank_table();
	int
		Rank = RankTable->get_rank_by_id(Council->get_id());
	CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
						dec2unit(Rank));

	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Achieved Project(s)"));
	CouncilInfo += "</TD>\n";

	CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
						Council->achieved_project_html());

	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("The player(s) who requested admission"));
	CouncilInfo += "</TD>\n";

	CAdmissionList *
		AdmissionList = Council->get_admission_list();
	if (AdmissionList->length() == 0)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo += "<TD CLASS=\"tabletxt\">\n";

		for (int i=0 ; i<AdmissionList->length() ; i++)
		{
			CAdmission *
				Admission = (CAdmission *)AdmissionList->get(i);

			if (i > 0) CouncilInfo += "<BR>\n";
			CouncilInfo += "<FORM METHOD=post ACTION=\"player_confirm.as\">\n";
			CouncilInfo += Admission->get_player()->get_nick();
			CouncilInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=PLAYER_ID>\n";
			CouncilInfo.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",
								Admission->get_player()->get_game_id());
			CouncilInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
								(char *)CGame::mImageServerURL);
			CouncilInfo += "</FORM>\n";
		}

		CouncilInfo += "</TD>\n";
	}

	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo += "<FORM METHOD=post ACTION=\"council.as\">\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("New Player Admission"));
	CouncilInfo += "</TD>\n";

	CouncilInfo += "<TD CLASS=\"tabletxt\">\n";
	if (Council->get_auto_assign() == true)
	{
		CouncilInfo.format("&nbsp;%s", GETTEXT("Yes"));
	}
	else
	{
		CouncilInfo.format("&nbsp;%s", GETTEXT("No"));
	}
	CouncilInfo.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n", Council->get_id());
	CouncilInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=NEW_AUTO_ASSIGN>\n";
	CouncilInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
						(char *)CGame::mImageServerURL);
	CouncilInfo += "</TD>\n";

	CouncilInfo += "</FORM>\n";
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
	CouncilInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
						GETTEXT("Council Relationship"));
	CouncilInfo += "</TD>\n";

	static CString
		TotalWarCouncil,
		WarCouncil,
		TruceCouncil,
		PeaceCouncil,
		AllyCouncil,
		SupremacyCouncil,
		SubordinaryCouncil;
	TotalWarCouncil.clear();
	WarCouncil.clear();
	TruceCouncil.clear();
	PeaceCouncil.clear();
	AllyCouncil.clear();
	SupremacyCouncil.clear();
	SubordinaryCouncil.clear();

	CCouncilRelationList *
		RelationList = Council->get_relation_list();
	for (int i=0 ; i<RelationList->length() ; i++)
	{
		CCouncilRelation *
			Relation = (CCouncilRelation *)RelationList->get(i);
		switch (Relation->get_relation())
		{
			case CRelation::RELATION_TOTAL_WAR :
			{
				CCouncil *
					TargetCouncil;
				if (Relation->get_council1()->get_id() == Council->get_id())
				{
					TargetCouncil = Relation->get_council2();
				}
				else
				{
					TargetCouncil = Relation->get_council1();
				}
				if (TotalWarCouncil.length() > 0) TotalWarCouncil += "<BR>\n";
				TotalWarCouncil += TargetCouncil->get_nick();
				TotalWarCouncil += "<FORM METHOD=post ACTION=council.as>\n";
				TotalWarCouncil.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n",
										TargetCouncil->get_id());
				TotalWarCouncil.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
										(char *)CGame::mImageServerURL);
				TotalWarCouncil += "</FORM>\n";
			}
				break;

			case CRelation::RELATION_WAR :
			{
				CCouncil *
					TargetCouncil;
				if (Relation->get_council1()->get_id() == Council->get_id())
				{
					TargetCouncil = Relation->get_council2();
				}
				else
				{
					TargetCouncil = Relation->get_council1();
				}
				if (WarCouncil.length() > 0) WarCouncil += "<BR>\n";
				WarCouncil += TargetCouncil->get_nick();
				WarCouncil += "<FORM METHOD=post ACTION=council.as>\n";
				WarCouncil.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n",
									TargetCouncil->get_id());
				WarCouncil.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
									(char *)CGame::mImageServerURL);
				WarCouncil += "</FORM>\n";
			}
				break;

			case CRelation::RELATION_TRUCE :
			{
				CCouncil *
					TargetCouncil;
				if (Relation->get_council1()->get_id() == Council->get_id())
				{
					TargetCouncil = Relation->get_council2();
				}
				else
				{
					TargetCouncil = Relation->get_council1();
				}
				if (TruceCouncil.length() > 0) TruceCouncil += "<BR>\n";
				TruceCouncil += TargetCouncil->get_nick();
				TruceCouncil += "<FORM METHOD=post ACTION=council.as>\n";
				TruceCouncil.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n",
										TargetCouncil->get_id());
				TruceCouncil.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
									(char *)CGame::mImageServerURL);
				TruceCouncil += "</FORM>\n";
			}
				break;

			case CRelation::RELATION_PEACE :
			{
				CCouncil *
					TargetCouncil;
				if (Relation->get_council1()->get_id() == Council->get_id())
				{
					TargetCouncil = Relation->get_council2();
				}
				else
				{
					TargetCouncil = Relation->get_council1();
				}
				if (PeaceCouncil.length() > 0) PeaceCouncil += "<BR>\n";
				PeaceCouncil += TargetCouncil->get_nick();
				PeaceCouncil += "<FORM METHOD=post ACTION=council.as>\n";
				PeaceCouncil.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n",
										TargetCouncil->get_id());
				PeaceCouncil.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
									(char *)CGame::mImageServerURL);
				PeaceCouncil += "</FORM>\n";
			}
				break;

			case CRelation::RELATION_ALLY :
			{
				CCouncil *
					TargetCouncil;
				if (Relation->get_council1()->get_id() == Council->get_id())
				{
					TargetCouncil = Relation->get_council2();
				}
				else
				{
					TargetCouncil = Relation->get_council1();
				}
				if (AllyCouncil.length() > 0) AllyCouncil += "<BR>\n";
				AllyCouncil += TargetCouncil->get_nick();
				AllyCouncil += "<FORM METHOD=post ACTION=council.as>\n";
				AllyCouncil.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n",
										TargetCouncil->get_id());
				AllyCouncil.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
									(char *)CGame::mImageServerURL);
				AllyCouncil += "</FORM>\n";
			}
				break;

			case CRelation::RELATION_SUBORDINARY :
			{
				CCouncil *
					TargetCouncil;
				if (Relation->get_council1()->get_id() == Council->get_id())
				{
					TargetCouncil = Relation->get_council2();

					if (SubordinaryCouncil.length() > 0) SubordinaryCouncil += "<BR>\n";
					SubordinaryCouncil += TargetCouncil->get_nick();
					SubordinaryCouncil += "<FORM METHOD=post ACTION=council.as>\n";
					SubordinaryCouncil.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n",
												TargetCouncil->get_id());
					SubordinaryCouncil.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
												(char *)CGame::mImageServerURL);
					SubordinaryCouncil += "</FORM>\n";
				}
				else
				{
					TargetCouncil = Relation->get_council1();

					if (SupremacyCouncil.length() > 0) SupremacyCouncil += "<BR>\n";
					SupremacyCouncil += TargetCouncil->get_nick();
					SupremacyCouncil += "<FORM METHOD=post ACTION=council.as>\n";
					SupremacyCouncil.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n",
												TargetCouncil->get_id());
					SupremacyCouncil.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
											(char *)CGame::mImageServerURL);
					SupremacyCouncil += "</FORM>\n";
				}
			}
				break;

			default :
			{
				SLOG("ERROR : Wrong Relation->get_relation() in CAdminCouncil::handler(), Relation->get_id() = %d", Relation->get_id());
				continue;
			}
		}
	}

	CouncilInfo += "<TD CLASS=\"tabletxt\">\n";

	CouncilInfo += "<TABLE CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=\"#2A2A2A\">\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("Total War"));
	if (TotalWarCouncil.length() == 0)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", (char *)TotalWarCouncil);
	}
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("War"));
	if (WarCouncil.length() == 0)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", (char *)WarCouncil);
	}
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("Truce"));
	if (TruceCouncil.length() == 0)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", (char *)TruceCouncil);
	}
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("Peace"));
	if (PeaceCouncil.length() == 0)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", (char *)PeaceCouncil);
	}
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("Ally"));
	if (AllyCouncil.length() == 0)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", (char *)AllyCouncil);
	}
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("Supremacy"));
	if (SupremacyCouncil.length() == 0)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", (char *)SupremacyCouncil);
	}
	CouncilInfo += "</TR>\n";

	CouncilInfo += "<TR>\n";
	CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("Subordinary"));
	if (SubordinaryCouncil.length() == 0)
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", GETTEXT("None"));
	}
	else
	{
		CouncilInfo.format("<TD CLASS=\"tabletxt\">%s</TD>\n", (char *)SubordinaryCouncil);
	}
	CouncilInfo += "</TR>\n";

	CouncilInfo += "</TABLE>\n";

	CouncilInfo += "</TD>\n";

	CouncilInfo += "</TR>\n";

	CouncilInfo += "</TABLE>\n";

	CouncilInfo += "</TD>\n";
	CouncilInfo += "</TR>\n";

	ITEM("COUNCIL_INFORMATION_MESSAGE",
			(char *)format(GETTEXT("The information about the council(#%1$s)."),
							dec2unit(Council->get_id())));

	ITEM("COUNCIL_INFO", (char *)CouncilInfo);

	ITEM("SEE_MESSAGE_MESSAGE", GETTEXT("See Messages"));

/*
	ITEM("SEND_MESSAGE_MESSAGE",
			GETTEXT("To send a message to this player, please click <A HREF=\"player_send_message.as\">this</A>."));

	ITEM("SEND_MAIL_MESSAGE",
			(char *)format(GETTEXT("To send a mail to this player, please click <A HREF=\"mailto:%1$s\">this</A>."),
					mPortal.get_email()));
*/
     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));



//	system_log("end page handler %s", get_name());

	return output("admin/council.html");
}
