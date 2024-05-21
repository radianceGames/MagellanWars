#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../player.h"
#include "../../planet.h"
#include "../../admin.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool
CPagePlayer::handler(CPlayer *aPlayer)
{
    // system_log("start page handler %s", get_name());

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

    // Get the player portal ID -- requirement
	CPlayer *Player = NULL;
    CPortalUser *PortalUser = NULL;
    int PortalID;

	QUERY("PORTAL_ID", PortalIDString);
	if (PortalIDString)
    {
        PortalID = as_atoi(PortalIDString);
        PortalUser = PORTAL_TABLE->get_by_portal_id(PortalID);
		Player  = PLAYER_TABLE->get_by_portal_id(PortalID);
    }
    if (!PortalIDString || PortalUser == NULL)
    {
     ITEM("ERROR_MESSAGE", GETTEXT("No Portal ID specified or invalid portal ID"));
     return output("admin/admin_error.html");
    }

    // Check for action
	QUERY("BY_WHAT", ByWhatString);
	if (ByWhatString)
	{
       if (!strcmp(ByWhatString, "PORTAL_NAME"))
       {
          QUERY("NEW_PORTAL_NAME", NewPortalName);
          if (strcmp(NewPortalName, PortalUser->get_name()))
          {
             ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Changed PortalUser #%d NAME from %s to %s",
                          PortalUser->get_portal_id(),
                          (char *)PortalUser->get_name(),
                          (char *)NewPortalName));
             PortalUser->set_name((char*)NewPortalName);
             PortalUser->type(QUERY_UPDATE);
             STORE_CENTER->store(*PortalUser);
          }
       }
       else if (!strcmp(ByWhatString, "PASSWORD"))
       {
          QUERY("NEW_PASSWORD", NewPassword);
          if (((int)PortalUser->get_user_level() >= (int)thisAdmin->get_user_level())
              && (PortalUser->get_portal_id() != thisAdmin->get_portal_id()))
          {
            ITEM("ERROR_MESSAGE", GETTEXT("You cannot change a user's password "
                                           "that has a higher or equal users level"
                                           " to yours."));
            return output("admin/admin_error.html");
          }

          if (strcmp(NewPassword, PortalUser->get_password()))
          {
             ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Changed PortalUser #%d PASSWORD from %s to %s",                           PortalUser->get_portal_id(),
                          (char *)PortalUser->get_password(),
                          (char *)NewPassword));
             PortalUser->set_password((char*)NewPassword);
             PortalUser->type(QUERY_UPDATE);
             STORE_CENTER->store(*PortalUser);
          }
       }
       else if (!strcmp(ByWhatString, "EMAIL"))
       {
          QUERY("NEW_EMAIL", NewEmail);
          if (strcmp(NewEmail, PortalUser->get_email()))
          {
             ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Changed PortalUser #%d EMAIL from %s to %s",
                          PortalUser->get_portal_id(),
                          (char *)PortalUser->get_email(),
                          (char *)NewEmail));

             PortalUser->set_email((char*)NewEmail);
             PortalUser->type(QUERY_UPDATE);
             STORE_CENTER->store(*PortalUser);
          }
       }
       else if (!strcmp(ByWhatString, "USER_LEVEL"))
       {
          QUERY("NEW_USER_LEVEL", NewUserLevelString);
          EUserLevel NewUserLevel = (EUserLevel)as_atoi(NewUserLevelString);
          if ((int)NewUserLevel != (int)PortalUser->get_user_level())
          {
            if ((int)PortalUser->get_user_level() > (int)thisAdmin->get_user_level())
            {
             ITEM("ERROR_MESSAGE", GETTEXT("You cannot change an admins "
                                           "user level that is higher than "
                                           "yours!"));
             return output("admin/admin_error.html");
            }
            if (PortalUser->get_portal_id() == thisAdmin->get_portal_id())
            {
              if ((int)thisAdmin->get_user_level() < (int)NewUserLevel)
               {
                 ITEM("ERROR_MESSAGE", GETTEXT("You cannot set your own "
                                               "user level higher than it "
                                               "already is!"));
                 return output("admin/admin_error.html");
                }
             }

             ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Changed PortalUser #%d USER_LEVEL from %d to %d",
                          PortalUser->get_portal_id(),
                          (int)PortalUser->get_user_level(),
                          (int)NewUserLevel));
             PortalUser->set_user_level(NewUserLevel);
             PortalUser->type(QUERY_UPDATE);
             STORE_CENTER->store(*PortalUser);
          }
       }
       else if (!strcmp(ByWhatString, "DEV_LEVEL"))
       {
          QUERY("NEW_DEV_LEVEL", NewDevLevelString);
          EDevLevel NewDevLevel = (EDevLevel)as_atoi(NewDevLevelString);
          if ((int)NewDevLevel != (int)PortalUser->get_dev_level())
          {
              if (PortalUser->get_user_level() != UL_DEVELOPER)
              {
                ITEM("ERROR_MESSAGE", GETTEXT("You cannot give non-developers"
                                      " developer privledges!"));
                return output("admin/admin_error.html");
              }
              if (thisAdmin->get_user_level() != UL_DEVELOPER &&
                  thisAdmin->get_user_level() != UL_ADMIN)
              {
                ITEM("ERROR_MESSAGE", GETTEXT("You cannot give out "
                                      " developer privledges!"));
                return output("admin/admin_error.html");
              }

             ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Changed PortalUser #%d DEV_LEVEL from %d to %d",
                          PortalUser->get_portal_id(),
                          (int)PortalUser->get_dev_level(),
                          (int)NewDevLevel));

             PortalUser->set_dev_level(NewDevLevel);
             PortalUser->type(QUERY_UPDATE);
             STORE_CENTER->store(*PortalUser);
          }
       }
	   else if (Player)
       {
	      if (!strcmp(ByWhatString, "IS_BANNED"))
	      {
	         QUERY("NEW_IS_BANNED", NewIsBanned);
             CIPList * IPBanList = ADMIN_TOOL->get_banned_ip_list();

             if (NewIsBanned)
             {
               if (!strcmp(NewIsBanned, "YES"))
               {
                  if (IPBanList->has(inet_addr(Player->get_last_login_ip())) == false)
                   {
                     ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Banned Player: %s IP: %s",
                          Player->get_nick(), Player->get_last_login_ip()));
                     IPBanList->add_ip(Player->get_last_login_ip());
                     IPBanList->save();
                   }
               }
               else if (!strcmp(NewIsBanned, "NO"))
               {
                   if (IPBanList->has(inet_addr(Player->get_last_login_ip())) == true)
                   {
                     ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Removed Ban Player: %s IP: %s",
                          Player->get_nick(), Player->get_last_login_ip()));
                      IPBanList->remove_ip(inet_addr(Player->get_last_login_ip()));
                      IPBanList->save();
                   }
                }
               }
            }
            else if (!strcmp(ByWhatString, "PRODUCTION_POINT"))
            {
                 QUERY("NEW_PRODUCTION_POINT", NewProductionPointString);
                 int NewProductionPoint = as_atoi(NewProductionPointString);
                 ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Set Player: %s PP to %d",
                          Player->get_nick(), NewProductionPoint));
                 Player->set_production(NewProductionPoint);
            }
            else if (!strcmp(ByWhatString, "HONOR"))
            {
                 QUERY("NEW_HONOR", NewHonorString);
                 int NewHonor = as_atoi(NewHonorString);
                 ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Set Player: %s HONOR to %d",
                          Player->get_nick(), NewHonor));
                 Player->set_honor(NewHonor);
            }
            else if (!strcmp(ByWhatString, "EMPIRE_RELATION"))
            {
                 QUERY("NEW_EMPIRE_RELATION", NewEmpireRelationString);
                 int NewEmpireRelation = as_atoi(NewEmpireRelationString);
                 ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Set Player: %s Empire Relation to %d",
                          Player->get_nick(), NewEmpireRelation));
                 Player->set_empire_relation(NewEmpireRelation);
            }
            else if (!strcmp(ByWhatString, "EMPIRE_COURT_RANK"))
            {
                 QUERY("NEW_EMPIRE_COURT_RANK", NewEmpireCourtRankString);
                 int NewEmpireCourtRank = as_atoi(NewEmpireCourtRankString);
                 ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Set Player: %s Court Rank to %d",
                          Player->get_nick(), NewEmpireCourtRank));
                 Player->set_court_rank(NewEmpireCourtRank);
            }
            else if (!strcmp(ByWhatString, "SHIP_POOL_PRODUCTION"))
            {
                 QUERY("NEW_SHIP_POOL_PRODUCTION", NewShipPoolProductionString);
          		 int NewShipPoolProduction = as_atoi(NewShipPoolProductionString);
                 ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Set Player: %s Ship Pool PP to %d",
                          Player->get_nick(), NewShipPoolProduction));
                 Player->set_ship_production(NewShipPoolProduction);
             }
             else if (!strcmp(ByWhatString, "GIVE_ALL_TECHS"))
             {
                  CTechList *AvailableTechList = Player->get_available_tech_list();
                  while (AvailableTechList->length() > 0)
					{
						CTech *Tech = (CTech *)AvailableTechList->get(0);
						Player->discover_tech(Tech->get_id());
					}
                 ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Set Player: %s ALL TECHS",
                          Player->get_nick()));
              }
              else if (!strcmp(ByWhatString, "EXPEDITION"))
              {
                   QUERY("EXPEDITION_RESOURCE", ExpeditionResourceString);
                   if (!ExpeditionResourceString)
                    {
                      ITEM("ERROR_MESSAGE", GETTEXT("No expedition resource set"));
                      return output("admin/admin_error.html");
                    }
                   QUERY("EXPEDITION_SIZE", ExpeditionSizeString);
                   if (!ExpeditionSizeString)
                    {
                      ITEM("ERROR_MESSAGE", GETTEXT("No expedition size set"));
                      return output("admin/admin_error.html");
                    }
                   QUERY("EXPEDITION_ATTR1", ExpeditionAtt1String);
                   if (!ExpeditionAtt1String)
                    {
                      ITEM("ERROR_MESSAGE", GETTEXT("No attribute #1 set"));
                      return output("admin/admin_error.html");
                    }
                   QUERY("EXPEDITION_ATTR2", ExpeditionAtt2String);
                   if (!ExpeditionAtt2String)
                    {
                      ITEM("ERROR_MESSAGE", GETTEXT("No attribute #2 set"));
                      return output("admin/admin_error.html");
                    }
                   QUERY("EXPEDITION_ATTR3", ExpeditionAtt3String);
                   if (!ExpeditionAtt3String)
                    {
                      ITEM("ERROR_MESSAGE", GETTEXT("No attribute #3 set"));
                      return output("admin/admin_error.html");
                    }

                   int Resource = as_atoi(ExpeditionResourceString);
                   int Size = as_atoi(ExpeditionSizeString);
                   int ClusterID = Player->find_new_planet(true);
                   if (ClusterID > 0)
					{
						CPlanet *Planet = new CPlanet();
						CCluster *Cluster = UNIVERSE->get_by_id(ClusterID);

						Planet->initialize();
						Planet->set_cluster(Cluster);
						Planet->set_name(Cluster->get_new_planet_name());
                        Planet->set_resource(Resource);
                        Planet->set_size(Size);
                        if (as_atoi(ExpeditionAtt1String) >= 0)
                        {
                           CPlanet::EPlanetAttribute att1 =
                             (CPlanet::EPlanetAttribute)as_atoi(ExpeditionAtt1String);
                           if (!Planet->has_attribute(att1))
                               Planet->set_attribute(att1);
                        }
                        if (as_atoi(ExpeditionAtt2String) >= 0)
                        {
                           CPlanet::EPlanetAttribute att2 =
                             (CPlanet::EPlanetAttribute)as_atoi(ExpeditionAtt2String);
                           if (!Planet->has_attribute(att2))
                               Planet->set_attribute(att2);
                        }
                        if (as_atoi(ExpeditionAtt3String) >= 0)
                        {
                           CPlanet::EPlanetAttribute att3 =
                             (CPlanet::EPlanetAttribute)as_atoi(ExpeditionAtt3String);
                           if (!Planet->has_attribute(att3))
                               Planet->set_attribute(att3);
                        }


						PLANET_TABLE->add_planet(Planet);
						Player->add_planet(Planet);
						Planet->start_terraforming();

						Planet->type(QUERY_INSERT);
						STORE_CENTER->store(*Planet);

						Player->new_planet_news(Planet);
						ADMIN_TOOL->add_admin_action_log(thisAdmin,
                          (char *)format("Set Player: %s ADDED PLANET",
                          Player->get_nick()));
					}
               }
			   /*else if (!strcmp(ByWhatString, "ADMIRAL"))
				{
					CAdmiralList *
						AdmiralPool = Player->get_admiral_pool();
					CAdmiral *
						Admiral = new CAdmiral(Player);
					if ((AdmiralPool->length() + Player->get_admiral_list()->length()) < CPlayer::mMaxAdmirals)
					{
						AdmiralPool->add_admiral(Admiral);

						Admiral->type(QUERY_INSERT);
						STORE_CENTER->store(*Admiral);

						Player->new_admiral_news(Admiral);
						ADMIN_TOOL->add_admin_action_log(thisAdmin,
							  (char *)format("Set Player: %s ADDED ADMIRAL",
							  Player->get_nick()));
					}
					else
					{
						ITEM("ERROR_MESSAGE", GETTEXT("Player already has the maximum allowed number of commanders."));
                    	return output("admin/admin_error.html");
					}
				}*/
				else if (!strcmp(ByWhatString, "FILL_ADMIRAL_POOL"))
				{
					CAdmiralList *
						AdmiralPool = Player->get_admiral_pool();
					if ((AdmiralPool->length() + Player->get_admiral_list()->length()) < CPlayer::mMaxAdmirals)
					{
						int nAdmirals = 0;
						for (nAdmirals = (AdmiralPool->length() + Player->get_admiral_list()->length()); nAdmirals < CPlayer::mMaxAdmirals; nAdmirals++)
						{
							CAdmiral *
									Admiral = new CAdmiral(Player);
							AdmiralPool->add_admiral(Admiral);
							Admiral->type(QUERY_INSERT);
							STORE_CENTER->store(*Admiral);
							Player->new_admiral_news(Admiral);
						}
						ADMIN_TOOL->add_admin_action_log(thisAdmin,
							 (char *)format("Set Player: %s ADDED %d ADMIRALS",
							 Player->get_nick(), nAdmirals - AdmiralPool->length()));
					}
					else
					{
						ITEM("ERROR_MESSAGE", GETTEXT("Player already has the maximum allowed number of commanders."));
						return output("admin/admin_error.html");
					}
				}
				else if (!strcmp(ByWhatString, "LEVEL_ALL_ADMIRALS"))
				{
					if (Player->get_admiral_list()->length() + Player->get_admiral_pool()->length() > 0)
					{
						for (int i = 0; i < Player->get_admiral_list()->length(); i++)
						{
							CAdmiral *
								Admiral = (CAdmiral*) Player->get_admiral_list()->get(i);
							Admiral->give_level(19);
						}
						for (int i = 0; i < Player->get_admiral_pool()->length(); i++)
						{
							CAdmiral *
								Admiral = (CAdmiral*) Player->get_admiral_pool()->get(i);
							Admiral->give_level(19);
						}
					}
					else
					{
						ITEM("ERROR_MESSAGE", GETTEXT("Player has no admirals."));
						return output("admin/admin_error.html");
					}
				}
				else if (!strcmp(ByWhatString, "PROJECT"))
				{
                    QUERY("NEW_PROJECT_ID", NewProjectIDString);

                    int NewProjectID = as_atoi(NewProjectIDString);
                   	CProject *Project = (CProject *)PROJECT_TABLE->get_by_id(NewProjectID);
                   	if (!Project)
                   	{
                      ITEM("ERROR_MESSAGE", GETTEXT("No such project"));
                      return output("admin/admin_error.html");
                    }
                    if ((Player->has_project(Project->get_id()))||(Project->get_type() == TYPE_COUNCIL)||(Project->get_type() == TYPE_BM))
                   	{
                      ITEM("ERROR_MESSAGE", GETTEXT("Player cannot be given this project"));
                      return output("admin/admin_error.html");
                    }

					Player->buy_project(NewProjectID, false);
					ADMIN_TOOL->add_admin_action_log(thisAdmin,
                        (char *)format("Set Player: %s GAVE PROJECT #%d",
                        Player->get_nick(), NewProjectID));
				}
			}
	}

    // Show appropriate Player info
	static CString
		PlayerInfo;
	PlayerInfo.clear();

	if (!(Player))
	{
		PlayerInfo = "<TR>\n";
		PlayerInfo += "<TD ALIGN=\"center\">\n";

		PlayerInfo += "<TABLE WIDTH=\"550\"\n";
		PlayerInfo += "<TR>\n";
		PlayerInfo.format("<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>\n", GETTEXT("This account has no character."));
		PlayerInfo += "</TR>\n";
		PlayerInfo += "</TABLE>\n";

		PlayerInfo += "</TD>\n";
		PlayerInfo += "</TR>\n";
	}
	else
	{
		PlayerInfo = "<TR>\n";
		PlayerInfo += "<TD ALIGN=\"center\">\n";

		PlayerInfo += "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Player Name"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
							Player->get_nick());

		PlayerInfo += "</TR>\n";
  		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Race"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
							Player->get_race_name());

		PlayerInfo += "</TR>\n";
  		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Last login"));
		PlayerInfo += "</TD>\n";
        const time_t *time = new time_t(Player->get_last_login());
		PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
							ctime(time));
        delete time;
		PlayerInfo += "</TR>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Production Point"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">\n");
		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo.format("&nbsp;<INPUT TYPE=input NAME=NEW_PRODUCTION_POINT VALUE=%s>\n",
                            dec2unit(Player->get_production()));
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=PRODUCTION_POINT>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Empire Relation"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">\n");
		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo.format("&nbsp;<INPUT TYPE=input NAME=NEW_EMPIRE_RELATION VALUE=%s>\n",
                        dec2unit(Player->get_empire_relation()));
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=EMPIRE_RELATION>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Empire Court Rank"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">\n");
		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
        PlayerInfo.format("&nbsp;<SELECT NAME=NEW_EMPIRE_COURT_RANK>\n");
        PlayerInfo.format("<OPTION SELECTED VALUE=%d>%s</OPTION>\n", Player->get_court_rank(),
                         Player->get_court_rank_name());
        for (int temp_court_rank=0;
            Player->get_court_rank_name(temp_court_rank) != NULL;
            temp_court_rank++)
        {
            if (temp_court_rank != Player->get_court_rank())
               PlayerInfo.format("<OPTION VALUE=%d>%s</OPTION>\n", temp_court_rank,
                    Player->get_court_rank_name(temp_court_rank));
        }
        PlayerInfo.format("</SELECT>\n");

		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=EMPIRE_COURT_RANK>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";


        PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Honor"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">\n");
		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo.format("&nbsp;<INPUT TYPE=input NAME=NEW_HONOR VALUE=%s>\n",
                            dec2unit(Player->get_honor()));
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=HONOR>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Ship Pool Production"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">\n");
		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo.format("&nbsp;<INPUT TYPE=input NAME=NEW_SHIP_POOL_PRODUCTION VALUE=%s>\n",
                            dec2unit(Player->get_ship_production()));
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=SHIP_POOL_PRODUCTION>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Give Project"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">\n");
		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo += "&nbsp;<INPUT TYPE=input NAME=NEW_PROJECT_ID>\n";
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=PROJECT>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("The number of Techs"));
		PlayerInfo += "</TD>\n";

		CKnownTechList *
			KnownTechList = Player->get_tech_list();
		PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s<BR>\n",
							dec2unit(KnownTechList->length()));
		PlayerInfo.format("<A HREF=\"player_research_status.as?PLAYER_ID=%d\">%s</A><BR>\n",
							Player->get_game_id(),
							GETTEXT("To see this player's research status"));

		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=GIVE_ALL_TECHS>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("# of Planet(s)"));
		PlayerInfo += "</TD>\n";

		CPlanetList *
			PlanetList = Player->get_planet_list();
		PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s<BR>",
							dec2unit(PlanetList->length()));
		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);

        PlayerInfo += "<SELECT NAME=EXPEDITION_RESOURCE>\n";
        for (int i=0; get_resource_description(i) != NULL; i++)
            PlayerInfo.format("<OPTION VALUE=\"%d\">%s</OPTION>\n", i,
                                       get_resource_description(i));
        PlayerInfo += "</SELECT>";

        PlayerInfo += "<SELECT NAME=EXPEDITION_SIZE>\n";
        for (int i=0; get_size_description(i) != NULL; i++)
            PlayerInfo.format("<OPTION VALUE=\"%d\">%s</OPTION>\n", i,
                                       get_size_description(i));
        PlayerInfo += "</SELECT>";

        PlayerInfo += "<SELECT NAME=EXPEDITION_ATTR1>\n";
        PlayerInfo += "<OPTION VALUE=\"-1\">NONE</OPTION>\n";
        for (int i=0; get_attribute_description((CPlanet::EPlanetAttribute)i) != NULL; i++)
            PlayerInfo.format("<OPTION VALUE=\"%d\">%s</OPTION>\n", i,
                                       get_attribute_description((CPlanet::EPlanetAttribute)i));
        PlayerInfo += "</SELECT>";

        PlayerInfo += "<SELECT NAME=EXPEDITION_ATTR2>\n";
        PlayerInfo += "<OPTION VALUE=\"-1\">NONE</OPTION>\n";
        for (int i=0; get_attribute_description((CPlanet::EPlanetAttribute)i) != NULL; i++)
            PlayerInfo.format("<OPTION VALUE=\"%d\">%s</OPTION>\n", i,
                                       get_attribute_description((CPlanet::EPlanetAttribute)i));
        PlayerInfo += "</SELECT>";

        PlayerInfo += "<SELECT NAME=EXPEDITION_ATTR3>\n";
        PlayerInfo += "<OPTION VALUE=\"-1\">NONE</OPTION>\n";
        for (int i=0; get_attribute_description((CPlanet::EPlanetAttribute)i) != NULL; i++)
            PlayerInfo.format("<OPTION VALUE=\"%d\">%s</OPTION>\n", i,
                                       get_attribute_description((CPlanet::EPlanetAttribute)i));
        PlayerInfo += "</SELECT>";

		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=EXPEDITION>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_expedition.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		/*PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("# of Admiral(s) in the Pool"));
		PlayerInfo += "</TD>\n";

		CAdmiralList *
			AdmiralList = Player->get_admiral_pool();
		PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s<BR>",
							dec2unit(AdmiralList->length()));

		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=ADMIRAL>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";*/

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Fill Admiral Pool"));
		PlayerInfo += "</TD>\n";

		PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%d<BR>\n", Player->get_admiral_pool()->length() + Player->get_admiral_list()->length());

		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=FILL_ADMIRAL_POOL>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
				PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
									GETTEXT("Level All Admirals"));
		PlayerInfo += "</TD>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\">\n";

		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=LEVEL_ALL_ADMIRALS>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);

		PlayerInfo += "</TD>\n";

		PlayerInfo += "</FORM>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("Council"));
		PlayerInfo += "</TD>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\">\n";
		CCouncil *
			Council = Player->get_council();
		PlayerInfo.format("%s\n", Council->get_nick());
		PlayerInfo += "<FORM METHOD='POST' ACTION='council.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo.format("<INPUT TYPE=hidden NAME=COUNCIL_ID VALUE=%d>\n",
							Council->get_id());
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_view.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</FORM>\n";
		PlayerInfo += "</TD>\n";
		PlayerInfo += "</TR>\n";

		PlayerInfo += "<TR>\n";

		PlayerInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">\n";
		PlayerInfo.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>\n",
							GETTEXT("IP Banned"));
		PlayerInfo += "</TD>\n";

		CIPList *
			PlayerIPBanList = ADMIN_TOOL->get_banned_ip_list();

		if (PlayerIPBanList->has(inet_addr(Player->get_last_login_ip())))
		{
			PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s(%s : %s)<BR>\n",
								GETTEXT("Banned"),
								GETTEXT("Last Login IP"),
								Player->get_last_login_ip());
		}
		else
		{
			PlayerInfo.format("<TD CLASS=\"tabletxt\">&nbsp;%s (%s : %s)<BR>\n",
								GETTEXT("Not Banned"),
								GETTEXT("Last Login IP"),
								Player->get_last_login_ip());
		}
		PlayerInfo += "<FORM METHOD='POST' ACTION='player.as'>\n";
		PlayerInfo.format("<INPUT TYPE=hidden NAME=PORTAL_ID VALUE=%s>\n",PortalIDString);
		PlayerInfo += "<SELECT NAME=NEW_IS_BANNED>\n";
		PlayerInfo += "<OPTION VALUE=\"YES\">Yes</OPTION>\n";
		PlayerInfo += "<OPTION VALUE=\"NO\">No</OPTION>\n";
		PlayerInfo += "</SELECT>\n";
		PlayerInfo += "<INPUT TYPE=hidden NAME=BY_WHAT VALUE=IS_BANNED>\n";
		PlayerInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\">\n",
							(char *)CGame::mImageServerURL);
		PlayerInfo += "</TD>\n";

		PlayerInfo += "</TR>\n";
		PlayerInfo += "</FORM>\n";

		PlayerInfo += "</TABLE>\n";

		PlayerInfo += "</TD>\n";
		PlayerInfo += "</TR>\n";

		PlayerInfo += "<TR>\n";
		PlayerInfo += "<TD CLASS=\"maintxt\" ALIGN=\"center\">\n";
		PlayerInfo.format("<A HREF=\"player_see_domestic.as?PORTAL_ID=%d\">", Player->get_portal_id());
		PlayerInfo.format(GETTEXT("See %1$s's domestic status"), Player->get_nick());
		PlayerInfo += "</A>\n";
		PlayerInfo += "</TD>\n";
		PlayerInfo += "</TR>\n";

		PlayerInfo += "<TR>\n";
		PlayerInfo += "<TD CLASS=\"maintxt\" ALIGN=\"center\">\n";
		PlayerInfo.format("<A HREF=\"player_message.as?PORTAL_ID=%d\">", Player->get_portal_id());
		PlayerInfo.format(GETTEXT("See %1$s's messages"), Player->get_nick());
		PlayerInfo += "</A>\n";
		PlayerInfo += "</TD>\n";
		PlayerInfo += "</TR>\n";

		PlayerInfo += "<TR>\n";
		PlayerInfo += "<TD CLASS=\"maintxt\" ALIGN=\"center\">\n";
		PlayerInfo.format("<A HREF=\"player_punishment.as?PORTAL_ID=%d\">", Player->get_portal_id());
		PlayerInfo.format(GETTEXT("Punish %1$s"), Player->get_nick());
		PlayerInfo += "</A>\n";
		PlayerInfo += "</TD>\n";
		PlayerInfo += "</TR>\n";
		
		PlayerInfo += "<TR>\n";
		PlayerInfo += "<TD CLASS=\"maintxt\" ALIGN=\"center\">\n";
		PlayerInfo.format("<A HREF=\"player_send_message.as?PORTAL_ID=%d\">", Player->get_portal_id());
		PlayerInfo += "Send a message to this player";
		PlayerInfo += "</A>\n";
		PlayerInfo += "</TD>\n";
		PlayerInfo += "</TR>\n";
	}

    ITEM("PLAYER_INFORMATION_MESSAGE",GETTEXT("Player Game Information"));
	ITEM("PLAYER_INFO", (char *)PlayerInfo);

	ITEM("SEE_MESSAGES_MESSAGE", GETTEXT("See Messages"));
	//ITEM("SEND_MESSAGE_MESSAGE",
        //GETTEXT("To send a message to this player, please click <A HREF=\"player_send_message.as\">this</A>.")
        //);
	ITEM("SEND_MAIL_MESSAGE", GETTEXT("The ability to send an email is currently <b>disabled</b>"));

    // Set Portal Info Strings
    ITEM("PORTAL_INFORMATION_MESSAGE",GETTEXT("Portal User Information"));
    ITEM("STRING_PORTAL_ID", GETTEXT("Portal ID:"));
    ITEM("STRING_PORTAL_NAME", GETTEXT("Portal Name:"));
    ITEM("STRING_PASSWORD", GETTEXT("Password:"));
    ITEM("STRING_E_MAIL", GETTEXT("E-Mail:"));
    ITEM("STRING_FIRST_NAME", GETTEXT("First Name:"));
    ITEM("STRING_LAST_NAME", GETTEXT("Last Name:"));
    ITEM("STRING_ICQ", GETTEXT("ICQ:"));
    ITEM("STRING_GENDER", GETTEXT("Gender:"));
    ITEM("STRING_AGE", GETTEXT("Age:"));
    ITEM("STRING_COUNTRY", GETTEXT("Country:"));
    ITEM("STRING_JOB", GETTEXT("Job:"));
    ITEM("STRING_HOW_KNOW_US", GETTEXT("Referred By:"));
    ITEM("STRING_CREATED_TIME", GETTEXT("Time Created:"));
    ITEM("STRING_LAST_LOGIN", GETTEXT("Last Login:"));
    ITEM("STRING_USER_LEVEL", GETTEXT("User Level:"));
    ITEM("STRING_DEV_LEVEL", GETTEXT("Developer Level:"));

    // Load Portal Info Values
    // TODO: add/remove HTML variables for unused fields
    // or add the fields to the portal DB + class + registration page
    ITEM("PORTAL_ID", PortalID);
    ITEM("PORTAL_NAME", PortalUser->get_name());
    if (!ADMIN_TOOL->has_access(aPlayer, UL_ADMIN) ||
        ((int)PortalUser->get_user_level() >= (int)thisAdmin->get_user_level())
        && (PortalUser->get_portal_id() != thisAdmin->get_portal_id()))
    {
        ITEM("PASSWORD", "Restricted");
    }
    else
    {
        ITEM("PASSWORD", PortalUser->get_password());
    }
    ITEM("E_MAIL", PortalUser->get_email());
    ITEM("FIRST_NAME", "N/A");
    ITEM("LAST_NAME", "N/A");
    ITEM("ICQ", "N/A");
    ITEM("GENDER","N/A");
    ITEM("AGE", PortalUser->get_age());
    ITEM("COUNTRY", PortalUser->get_country());
    ITEM("HOW_KNOW_US", "N/A");
    ITEM("CREATED_TIME", PortalUser->get_first_login());
    // these are auto-displayed with the selected, with the current user level SELECTED
    //ITEM("USER_LEVEL", user_level_to_string(PortalUser->get_user_level()));
    //ITEM("DEV_LEVEL", dev_level_to_string(PortalUser->get_dev_level()));
    ITEM("JOB", "N/A");

    CString UserLevelSelect;
    UserLevelSelect.format("<SELECT NAME=NEW_USER_LEVEL>");
    UserLevelSelect.format("<OPTION SELECTED VALUE=%d>%s</OPTION>", (int)PortalUser->get_user_level(),
                         user_level_to_string(PortalUser->get_user_level()));
    for (int UserLevel=MIN_USER_LEVEL; UserLevel <= MAX_USER_LEVEL; UserLevel++)
    {
        if (UserLevel != (int)PortalUser->get_user_level())
           UserLevelSelect.format("<OPTION VALUE=%d>%s</OPTION>", UserLevel,
                    user_level_to_string(EUserLevel(UserLevel)));
    }
    UserLevelSelect.format("</SELECT>");

    if (UserLevelSelect.length() == 0)
    {
       ITEM("USER_LEVEL_SELECT", " ");
    }
    else
    {
       ITEM("USER_LEVEL_SELECT", (char *)UserLevelSelect);
    }


    CString DevLevelSelect;
    if (PortalUser->get_user_level() != UL_DEVELOPER)
       DevLevelSelect.format("<SELECT NAME=NEW_DEV_LEVEL DISABLED>");
    else
       DevLevelSelect.format("<SELECT NAME=NEW_DEV_LEVEL>");
    DevLevelSelect.format("<OPTION SELECTED VALUE=%d>%s</OPTION>", (int)PortalUser->get_dev_level(),
                         dev_level_to_string(PortalUser->get_dev_level()));
    for (int DevLevel=MIN_DEV_LEVEL; DevLevel <= MAX_DEV_LEVEL; DevLevel++)
    {
        if (DevLevel != (int)PortalUser->get_dev_level())
           DevLevelSelect.format("<OPTION VALUE=%d>%s</OPTION>", DevLevel,
                    dev_level_to_string(EDevLevel(DevLevel)));
    }
    DevLevelSelect.format("</SELECT>");

    if (DevLevelSelect.length() == 0)
    {
       ITEM("DEV_LEVEL_SELECT", " ");
    }
    else
    {
       ITEM("DEV_LEVEL_SELECT", (char *)DevLevelSelect);
    }



    // Log Shiz
/*    ITEM("STRING_SEE_LOG_MESSAGE", GETTEXT("View Log"));
    ITEM("STRING_ACTION_LOG",GETTEXT("View Action Log"));
    ITEM("STRING_NEW_ACCOUNT_LOG",GETTEXT("View New Account Log"));
    ITEM("STRING_NEW_PLAYER_LOG",GETTEXT("View New Player Log"));
    ITEM("STRING_DEAD_PLAYER_LOG",GETTEXT("View Dead Player Log"));
    ITEM("STRING_YEAR",GETTEXT("Year"));
    ITEM("STRING_MONTH",GETTEXT("Month"));
    ITEM("STRING_DAY",GETTEXT("Day"));*/

     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));



    //system_log("end page handler %s", get_name());
	return output("admin/player.html");
}
