#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerResearchStatus::handler(CPlayer *aPlayer)
{
    //	system_log("start page handler %s", get_name());
   // Default player access check
    CPortalUser *thisAdmin = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
    CString error_msg;
    error_msg.format("%s '%s' %s.", 
                     GETTEXT("Access Level "), 
                     user_level_to_string(thisAdmin->get_user_level()), 
                     GETTEXT("does not grant access"));
    if (!ADMIN_TOOL->has_access(aPlayer, UL_ADMIN))
    {
       ITEM("ERROR_MESSAGE", (char *)error_msg);
       return output("admin/admin_error.html"); 
    }  

    // Load target player 
	QUERY("PLAYER_ID", PlayerIDString);
	int
		PlayerID = as_atoi(PlayerIDString);
	CPlayer *
		Player = PLAYER_TABLE->get_by_game_id(PlayerID);
    
	if (Player == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no player whose game ID is %1$s."),
								dec2unit(PlayerID)));
		return output("admin/admin_error.html");
	}
	if (Player->get_game_id() == EMPIRE_GAME_ID)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You can't inspect the Empire."));
		return output("admin/admin_error.html");
	}
    ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("View player %s research status", Player->get_nick())); 
    QUERY("TECH_ACTION", TechActionString);
    if (TechActionString)
    {
       QUERY("TECH_ID", TechIDString);
       
       if (TechIDString)
       {
          int TechID = as_atoi(TechIDString);
          if (!strcmp(TechActionString,"ADD_TECH"))
          {
             if (Player->get_available_tech_list()->get_by_id(TechID) != NULL)
             {
                ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Added tech #%d to %s", TechID, Player->get_nick()));
				Player->discover_tech(TechID);                 
             }
          }
          else if (!strcmp(TechActionString,"DELETE_TECH"))
          {
             CKnownTechList *TargetTechList = Player->get_tech_list();
             CKnownTech *TempTech = TargetTechList->get_by_id(TechID);
             if (TempTech != NULL)
             {
                ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Removed tech #%d from %s", TechID, Player->get_nick()));
                TargetTechList->remove_known_tech(TechID);
             }
          }
       }
    }
    ITEM("PORTAL_ID", Player->get_portal_id());
	ITEM("STRING_TARGET_TECH", GETTEXT("Target Tech"));
	int
		TargetTechID = Player->get_target_tech();
	if (TargetTechID == 0)
	{
		ITEM("TARGET_TECH", GETTEXT("Free Research"));
	}
	else
	{
		CTech *
			TargetTech = TECH_TABLE->get_by_id(TargetTechID);
		if (TargetTech != NULL)
		{
			ITEM("TARGET_TECH", TargetTech->get_name());
		}
		else
		{
			ITEM("TARGET_TECH",
				(char *)format(GETTEXT("The player's target tech(#%1$s) doesn't exist."),
								dec2unit(TargetTechID)));
		}
	}

	ITEM("RESEARCHED_TECH_MESSAGE",
			GETTEXT("The player has researched the following tech(s) :"));

	ITEM("STRING_SOCIAL_SCIENCE", GETTEXT("Social Science"));
	ITEM("STRING_INFORMATION_SCIENCE", GETTEXT("Information Science"));
	ITEM("STRING_MATTER_ENERGY_SCIENCE", GETTEXT("Matter-Energy Science"));
	ITEM("STRING_LIFE_SCIENCE", GETTEXT("Life Science"));
    
    CKnownTechList *ResearchedTechList = Player->get_tech_list();
	CKnownTech	*KnownTech;

	CString RS_SS_HTML;
 	CString RS_IS_HTML;
  	CString RS_MS_HTML;
   	CString RS_LS_HTML;
	RS_SS_HTML.clear();
 	RS_IS_HTML.clear();
  	RS_MS_HTML.clear();
   	RS_LS_HTML.clear();

	for(int i=0; i < ResearchedTechList->length(); i++)
	{
		KnownTech = (CKnownTech*)ResearchedTechList->get(i);
		if (KnownTech->get_type() == CTech::TYPE_SOCIAL)
        {
           RS_SS_HTML.format("<TR><TD CLASS='tabletxt' >");
           RS_SS_HTML.format("&nbsp;<A HREF=\"/encyclopedia/game/tech/%d.html\">%s</A>",
					KnownTech->get_id(), KnownTech->get_name());
           RS_SS_HTML.format("</TD><TD CLASS='tabletxt'>");
           RS_SS_HTML.format("<FORM METHOD=GET ACTION=player_research_status.as>\n");
           RS_SS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ACTION VALUE=DELETE_TECH>\n"); 
           RS_SS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ID VALUE=%d>\n",KnownTech->get_id());
           RS_SS_HTML.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",PlayerID);
           RS_SS_HTML.format("<INPUT TYPE=image SRC='%s/image/as_game/bu_delete.gif'>\n",(char *)CGame::mImageServerURL);           
           RS_SS_HTML.format("</FORM>\n");
           RS_SS_HTML.format("</TD></TR>");
        }
        else if (KnownTech->get_type() == CTech::TYPE_INFORMATION)
        {
           RS_IS_HTML.format("<TR><TD CLASS='tabletxt' >");
           RS_IS_HTML.format("&nbsp;<A HREF=\"/encyclopedia/game/tech/%d.html\">%s</A>",
					KnownTech->get_id(), KnownTech->get_name());
           RS_IS_HTML.format("</TD><TD CLASS='tabletxt'>");
           RS_IS_HTML.format("<FORM METHOD=GET ACTION=player_research_status.as>\n");
           RS_IS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ACTION VALUE=DELETE_TECH>\n"); 
           RS_IS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ID VALUE=%d>\n",KnownTech->get_id());
           RS_IS_HTML.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",PlayerID);
           RS_IS_HTML.format("<INPUT TYPE=image SRC='%s/image/as_game/bu_delete.gif'>\n",(char *)CGame::mImageServerURL);           
           RS_IS_HTML.format("</FORM>\n");
           RS_IS_HTML.format("</TD></TR>");           
        }
        else if (KnownTech->get_type() == CTech::TYPE_MATTER_ENERGY)
        {
           RS_MS_HTML.format("<TR><TD CLASS='tabletxt' >");        
           RS_MS_HTML.format("&nbsp;<A HREF=\"/encyclopedia/game/tech/%d.html\">%s</A>",
					KnownTech->get_id(), KnownTech->get_name());
           RS_MS_HTML.format("</TD><TD CLASS='tabletxt'>");
           RS_MS_HTML.format("<FORM METHOD=GET ACTION=player_research_status.as>\n");
           RS_MS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ACTION VALUE=DELETE_TECH>\n"); 
           RS_MS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ID VALUE=%d>\n",KnownTech->get_id());
           RS_MS_HTML.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",PlayerID);
           RS_MS_HTML.format("<INPUT TYPE=image SRC='%s/image/as_game/bu_delete.gif'>\n",(char *)CGame::mImageServerURL);           
           RS_MS_HTML.format("</FORM>\n");
           RS_MS_HTML.format("</TD></TR>");           
        }
        else if (KnownTech->get_type() == CTech::TYPE_LIFE)
        {
           RS_LS_HTML.format("<TR><TD CLASS='tabletxt' >");        
           RS_LS_HTML.format("&nbsp;<A HREF=\"/encyclopedia/game/tech/%d.html\">%s</A>",
					KnownTech->get_id(), KnownTech->get_name());
           RS_LS_HTML.format("</TD><TD CLASS='tabletxt'>");
           RS_LS_HTML.format("<FORM METHOD=GET ACTION=player_research_status.as>\n");
           RS_LS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ACTION VALUE=DELETE_TECH>\n"); 
           RS_LS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ID VALUE=%d>\n",KnownTech->get_id());
           RS_LS_HTML.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",PlayerID);
           RS_LS_HTML.format("<INPUT TYPE=image SRC='%s/image/as_game/bu_delete.gif'>\n",(char *)CGame::mImageServerURL);           
           RS_LS_HTML.format("</FORM>\n");
           RS_LS_HTML.format("</TD></TR>");           
        }
	}
    if (RS_SS_HTML.length() == 0)
       RS_SS_HTML.format("No Results");
    if (RS_IS_HTML.length() == 0)
       RS_IS_HTML.format("No Results");
    if (RS_MS_HTML.length() == 0)
       RS_MS_HTML.format("No Results");
    if (RS_LS_HTML.length() == 0)
       RS_LS_HTML.format("No Results");
	

	ITEM("RESEARCHED_SOCIAL_SCIENCE_LIST", (char *)RS_SS_HTML);
	ITEM("RESEARCHED_INFORMATION_SCIENCE_LIST", (char *)RS_IS_HTML);
	ITEM("RESEARCHED_MATTER_ENERGY_SCIENCE_LIST", (char *)RS_MS_HTML);
	ITEM("RESEARCHED_LIFE_SCIENCE_LIST", (char *)RS_LS_HTML);

    CTechList *AvailableTechList = Player->get_available_tech_list();
	CTech	*Tech;
	CString AV_SS_HTML;
 	CString AV_IS_HTML;
  	CString AV_MS_HTML;
   	CString AV_LS_HTML;
	AV_SS_HTML.clear();
 	AV_IS_HTML.clear();
  	AV_MS_HTML.clear();
   	AV_LS_HTML.clear();

	for(int i=0; i < AvailableTechList->length(); i++)
	{
		Tech = (CTech*)AvailableTechList->get(i);
		if (Tech->get_type() == CTech::TYPE_SOCIAL)
        {
           AV_SS_HTML.format("<TR><TD CLASS='tabletxt' >");
           AV_SS_HTML.format("&nbsp;<A HREF=\"/encyclopedia/game/tech/%d.html\">%s</A>",
					Tech->get_id(), Tech->get_name());
           AV_SS_HTML.format("</TD><TD CLASS='tabletxt'>");
           AV_SS_HTML.format("<FORM METHOD=GET ACTION=player_research_status.as>\n");
           AV_SS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ACTION VALUE=ADD_TECH>\n"); 
           AV_SS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ID VALUE=%d>\n",Tech->get_id());
           AV_SS_HTML.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",PlayerID);
           AV_SS_HTML.format("<INPUT TYPE=image SRC='%s/image/as_game/bu_change.gif'>\n",(char *)CGame::mImageServerURL);           
           AV_SS_HTML.format("</FORM>\n");
           AV_SS_HTML.format("</TD></TR>");
        }
        else if (Tech->get_type() == CTech::TYPE_INFORMATION)
        {
           AV_IS_HTML.format("<TR><TD CLASS='tabletxt' >");
           AV_IS_HTML.format("&nbsp;<A HREF=\"/encyclopedia/game/tech/%d.html\">%s</A>",
					Tech->get_id(), Tech->get_name());
           AV_IS_HTML.format("</TD><TD CLASS='tabletxt'>");
           AV_IS_HTML.format("<FORM METHOD=GET ACTION=player_research_status.as>\n");
           AV_IS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ACTION VALUE=ADD_TECH>\n"); 
           AV_IS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ID VALUE=%d>\n",Tech->get_id());
           AV_IS_HTML.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",PlayerID);
           AV_IS_HTML.format("<INPUT TYPE=image SRC='%s/image/as_game/bu_change.gif'>\n",(char *)CGame::mImageServerURL);           
           AV_IS_HTML.format("</FORM>\n");
           AV_IS_HTML.format("</TD></TR>");           
        }
        else if (Tech->get_type() == CTech::TYPE_MATTER_ENERGY)
        {
           AV_MS_HTML.format("<TR><TD CLASS='tabletxt' >");        
           AV_MS_HTML.format("&nbsp;<A HREF=\"/encyclopedia/game/tech/%d.html\">%s</A>",
					Tech->get_id(), Tech->get_name());
           AV_MS_HTML.format("</TD><TD CLASS='tabletxt'>");
           AV_MS_HTML.format("<FORM METHOD=GET ACTION=player_research_status.as>\n");
           AV_MS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ACTION VALUE=ADD_TECH>\n"); 
           AV_MS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ID VALUE=%d>\n",Tech->get_id());
           AV_MS_HTML.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",PlayerID);
           AV_MS_HTML.format("<INPUT TYPE=image SRC='%s/image/as_game/bu_change.gif'>\n",(char *)CGame::mImageServerURL);           
           AV_MS_HTML.format("</FORM>\n");
           AV_MS_HTML.format("</TD></TR>");           
        }
        else if (Tech->get_type() == CTech::TYPE_LIFE)
        {
           AV_LS_HTML.format("<TR><TD CLASS='tabletxt' >");        
           AV_LS_HTML.format("&nbsp;<A HREF=\"/encyclopedia/game/tech/%d.html\">%s</A>",
					Tech->get_id(), Tech->get_name());
           AV_LS_HTML.format("</TD><TD CLASS='tabletxt'>");
           AV_LS_HTML.format("<FORM METHOD=GET ACTION=player_research_status.as>\n");
           AV_LS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ACTION VALUE=ADD_TECH>\n"); 
           AV_LS_HTML.format("<INPUT TYPE=hidden NAME=TECH_ID VALUE=%d>\n",Tech->get_id());
           AV_LS_HTML.format("<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>\n",PlayerID);
           AV_LS_HTML.format("<INPUT TYPE=image SRC='%s/image/as_game/bu_change.gif'>\n",(char *)CGame::mImageServerURL);           
           AV_LS_HTML.format("</FORM>\n");
           AV_LS_HTML.format("</TD></TR>");           
        }
	}
    if (AV_SS_HTML.length() == 0)
       AV_SS_HTML.format("No Results");
    if (AV_IS_HTML.length() == 0)
       AV_IS_HTML.format("No Results");
    if (AV_MS_HTML.length() == 0)
       AV_MS_HTML.format("No Results");
    if (AV_LS_HTML.length() == 0)
       AV_LS_HTML.format("No Results");
	

	ITEM("NOT_RESEARCHED_TECH_MESSAGE",
			GETTEXT("The player can research the following tech(s) :"));

	ITEM("NOT_RESEARCHED_SOCIAL_SCIENCE_LIST", (char *)AV_SS_HTML);
	ITEM("NOT_RESEARCHED_INFORMATION_SCIENCE_LIST", (char *)AV_IS_HTML);
	ITEM("NOT_RESEARCHED_MATTER_ENERGY_SCIENCE_LIST", (char *)AV_MS_HTML);
	ITEM("NOT_RESEARCHED_LIFE_SCIENCE_LIST", (char *)AV_LS_HTML);

     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));

//	system_log("end page handler %s", get_name());

	return output("admin/player_research_status.html");
}
