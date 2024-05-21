#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../player.h"
#include "../../preference.h"
#include <cstdio>

bool
CPageSiegePlanetFleetDeployment::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

   	// update preference

    	CPreference *aPreference = aPlayer->get_preference();

    	QUERY("CHANGE_PREF", ChangePrefString);
    	if (ChangePrefString)
    	{
	       int ChangePref = as_atoi(ChangePrefString);
       		aPreference->handle(ChangePref);
    	}

	if (aPlayer->has_siege_blockade_restriction() == true)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Due to Imperial regulations, you may not siege for another %1$d seconds."), aPlayer->get_siege_blockade_restriction_time()));

		return output("war/war_error.html");
	}

	QUERY("TARGET_PLAYER_ID", TargetPlayerIDString);
	CHECK(TargetPlayerIDString == NULL,
			GETTEXT("You didn't enter a target player's ID."));

	int
		TargetPlayerID = as_atoi(TargetPlayerIDString);
	CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetPlayerID);

	CHECK(!TargetPlayer,
			GETTEXT("The targetted player doesn't exist."));

	CHECK(TargetPlayer->get_game_id() == EMPIRE_GAME_ID,
			GETTEXT("You can't attack the Empire in this menu."));

	CHECK(TargetPlayer->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't attack yourself."));

	CHECK(TargetPlayer->is_dead(),
	             (char *)format(GETTEXT("%1$s is dead."),
				 				TargetPlayer->get_nick()));

	if (TargetPlayer->has_siege_blockade_protection() == true)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Recently that player has had a planetary siege battle in his/her domain. You decided not to move your armada there until you get a clear information."));

		return output("war/war_error.html");
	}

	CString Attack;
	Attack = aPlayer->check_attackable(TargetPlayer);
	CHECK(Attack.length(), Attack);

	CCouncil *TargetPlayerCouncil = TargetPlayer->get_council();
	CRelation* Relation = aPlayer->get_relation(TargetPlayer);

	if (Relation != NULL && Relation->get_relation() == CRelation::RELATION_BOUNTY)
	{
	}
	else if (aPlayer->get_council() == TargetPlayerCouncil)
	{
		CHECK(!Relation,
			(char*)format(GETTEXT("You have no relation with %1$s."),
								TargetPlayer->get_nick()));

			CHECK((Relation->get_relation() != CRelation::RELATION_WAR && Relation->get_relation() != CRelation::RELATION_TOTAL_WAR),
			(char*)format(GETTEXT("You are not at war with %1$s."),
						TargetPlayer->get_nick()));
		if (TargetPlayer->is_protected())
		{
			ITEM("ERROR_MESSAGE",
			(char *)format(GETTEXT("The player %1$s has lost a large amount of their planets recently and Imperial Law prevents you from attacking them."),
			TargetPlayer->get_nick()));

			return output("war/war_error.html");
		}
	}
	else
	{
		Relation = TargetPlayerCouncil->get_relation(aPlayer->get_council());

		CHECK(!Relation,
			(char *)format(GETTEXT("Your council has no relationship with %1$s's council."),
							TargetPlayer->get_nick()));

		CHECK(Relation->get_relation() != CRelation::RELATION_WAR &&
				Relation->get_relation() != CRelation::RELATION_TOTAL_WAR,
			(char *)format(GETTEXT("Your council is not at war or total war with %1$s's council."),
								TargetPlayer->get_nick()));
		if (Relation->get_relation() == CRelation::RELATION_WAR)
		{
			if (TargetPlayer->is_protected())
			{
				ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("The player %1$s has lost a large amount of their planets recently and Imperial Law prevents you from attacking them."),
				TargetPlayer->get_nick()));
				return output("war/war_error.html");
			}
			else if((CGame::get_game_time() - Relation->get_time()) < 600)
			{
				ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You have not been at war with the player %1$s long enough to attack. Please wait another %2$s seconds"),
				TargetPlayer->get_nick(), dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
				return output("war/war_error.html");
			}
		}
	}

	static CString
		CapitalFleetImageURL,
		NonCapitalFleetImageURL;
	CapitalFleetImageURL.clear();
	NonCapitalFleetImageURL.clear();

	CapitalFleetImageURL.format("%s/image/as_game/fleet/ship_cap.gif",
								(char *)CGame::mImageServerURL),
	NonCapitalFleetImageURL.format("%s/image/as_game/fleet/ship_set.gif",
									(char *)CGame::mImageServerURL);

	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	QUERY("CAPITAL", CapitalString)
	CHECK(!CapitalString,
			GETTEXT("You did not select a capital fleet.<BR>"
					"Please try again."));
	int
		CapitalID = as_atoi(CapitalString);
	CFleet *
		CapitalFleet = FleetList->get_by_id(CapitalID);
	CHECK(!CapitalFleet,
			GETTEXT("You don't have such a fleet that you selected as a capital fleet.<BR>"
					"Please try again."));

	CHECK(CapitalFleet->get_status() != CFleet::FLEET_STAND_BY,
			format(GETTEXT("%1$s fleet is not on stand-by."),
			CapitalFleet->get_nick()));

	CIntegerList FleetIDList;

	FleetIDList.push((TSomething)CapitalID);

	char Query[100];
	CFleet* Fleet;
	int FleetID;
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		sprintf(Query, "FLEET%d", i);

		QUERY(Query, FleetNumberString);

		if (!FleetNumberString) continue;
		if (strcasecmp(FleetNumberString, "ON")) continue;

//		SLOG("Battle Fleet %s", Query);

		sprintf(Query, "FLEET%d_ID", i);

		QUERY(Query, FleetIDString);
		CHECK(!FleetIDString,
				GETTEXT("1 or more fleets' ID data were not found."));
		FleetID = as_atoi(FleetIDString);
		CHECK(!FleetID,
				GETTEXT("1 or more fleets' ID were not valid."));

		Fleet = (CFleet *)FleetList->get_by_id(FleetID);
		CHECK(!Fleet,
				(char *)format(
						GETTEXT("You don't have the fleet with ID %1$s."),
						dec2unit(FleetID)));

		CHECK(Fleet->get_status() != CFleet::FLEET_STAND_BY,
			format(GETTEXT("%1$s fleet is not on stand-by."),
					Fleet->get_nick()));

		if (Fleet->get_id() == CapitalID) continue;

		FleetIDList.push((TSomething)(Fleet->get_id()));
	}

//	SLOG("FleetIDList : %d", FleetIDList.length());

	CHECK(!FleetIDList.length(),
				GETTEXT("No fleets have been selected.<BR>Please try again."));

	CHECK(FleetIDList.length() > 20,
			GETTEXT("The number of fleets you selected is more than 20.<BR>Please try again."));
   CString FleetIndexToID;
   FleetIndexToID.clear();
   // Java specific defense plan code
    if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
    {
    	// Create Java Applet Image URL Var for 'IMAGEDIR'
    	CString FleetImageURL;
    	FleetImageURL.clear();
    	FleetImageURL.format("%s/image/as_game/fleet",(char *)CGame::mImageServerURL);
    	ITEM("IMAGEDIR", (char *)FleetImageURL);

	 	// Set-up Applet Fleet ID vars
	 	FleetIndexToID.format("<PARAM NAME=capFleet_ID VALUE=%d>",
							FleetIDList.get(0));
		FleetIndexToID += "\n";

		for (int i=1 ; i<FleetIDList.length() ; i++)
		{
		  FleetIndexToID.format("<PARAM NAME=Fleet%d_ID VALUE=%d>",
								i, FleetIDList.get(i));
		  FleetIndexToID += "\n";
		}
		ITEM("FLEET_INDEX_TO_ID", (char *)FleetIndexToID);
		ITEM("TARGET_PLAYER_ID", TargetPlayerID);
    }
    else // Javascript specific defense plan code
    {
    	ITEM("TARGET_PLAYER_ID", TargetPlayerID);
		ITEM("STRING_ORDER", GETTEXT("Order"));
		ITEM("STRING_NORMAL", GETTEXT("Normal"));
		ITEM("STRING_FORMATION", GETTEXT("Formation"));
		ITEM("STRING_PENETRATE", GETTEXT("Penetrate"));
		ITEM("STRING_FLANK", GETTEXT("Flank"));
		ITEM("STRING_RESERVE", GETTEXT("Reserve"));
		ITEM("STRING_ASSAULT", GETTEXT("Assault"));
		ITEM("STRING_FREE", GETTEXT("Free"));
		ITEM("STRING_STAND_GROUND", GETTEXT("Stand Ground"));

	 	// Set-up JavaScript Fleet ID vars
	 	FleetIndexToID.format("<INPUT TYPE=hidden NAME=capFleet_id VALUE=%d>",
							FleetIDList.get(0));
		FleetIndexToID += "\n";

		for (int i=1 ; i<FleetIDList.length() ; i++)
		{
		  FleetIndexToID.format("<INPUT TYPE=hidden NAME=fleet%d_id VALUE=%d>",
								i+1, FleetIDList.get(i));
		  FleetIndexToID += "\n";
		}
		ITEM("FLEET_INDEX_TO_ID", (char *)FleetIndexToID);


    }

//	SLOG("FLLET_INDEX_TO_ID %d[%s]",
//				FleetIndexToID.length(), (char*)FleetIndexToID);

	// Java fleet params
	static CString 	JFleetInfo,
			JFleetList;
	JFleetInfo.clear();
	JFleetList.clear();

	CString
		JSFleetInfo,
		JSFleetList,
		Result;

	JSFleetInfo = "<DIV ID=capFleet CLASS=pointer_h>\n";
	JFleetInfo.format("<PARAM NAME=capFleet_O VALUE=\"%s\">",CDefenseFleet::get_command_string_normal(CDefenseFleet::COMMAND_NORMAL));
	JSFleetInfo.format("<IMG NAME=\"img_capFleet\" SRC=\"%s\" "
						"ALT=\"- %s -&#13;%s %d&#13;%s %s\" "
						"TITLE=\"- %s -&#13;%s %d&#13;%s %s\">\n",
			(char *)CapitalFleetImageURL,
			GETTEXT("Fleet Info."),
			GETTEXT("No."),
			CapitalID,
			GETTEXT("Order"),
			CDefenseFleet::get_command_string_normal(
										CDefenseFleet::COMMAND_NORMAL),
			GETTEXT("Fleet Info."),
			GETTEXT("No."),
			CapitalID,
			GETTEXT("Order"),
			CDefenseFleet::get_command_string_normal(
									CDefenseFleet::COMMAND_NORMAL));

	JSFleetInfo += "</DIV>\n";
	JFleetInfo  += "\n";
	JSFleetList = "<STYLE TYPE=\"text/css\">\n";
	JSFleetList +=
			"#capFleet{position:absolute; left:303; top:316; z-index:0}\n";

	Result = "<INPUT TYPE=hidden NAME=capFleet_O VALUE=\"NORMAL\">\n";

	bool AnyFleet[11][31];
	memset(AnyFleet, 0, sizeof(AnyFleet));

	AnyFleet[5][15] = true;

	int
		LocationX = 609,
		LocationY = 226;

	for (int i=1; i<FleetIDList.length(); i++)
	{
		int
			FleetID = (int)FleetIDList.get(i);

		int
			Column = (LocationX - 9)/20,
			Row = (LocationY - 226)/20;

		while (AnyFleet[Row][Column] == true)
		{
			if (Row == 10)
			{
				LocationX -= 20;
				LocationY = 226;
			} else LocationY += 20;

			Column = (LocationX - 9)/20;
			Row = (LocationY - 226)/20;
		}

		JSFleetList.format("#fleet%d{position:absolute; "
							"left:%d; top:%d; z-index:%d}",
							i+1, LocationX - 5, LocationY - 8, i);

		Result.format("<INPUT TYPE=hidden NAME=fleet%d_X VALUE=\"%d\">\n",
						i+1, LocationX);
		Result.format("<INPUT TYPE=hidden NAME=fleet%d_Y VALUE=\"%d\">\n",
						i+1, LocationY);
		Result.format("<INPUT TYPE=hidden NAME=fleet%d_O VALUE=\"NORMAL\">\n",
						 i+1);

//		SLOG("ROW:%d, COLUMN:%d LocationX:%d, LocationY:%d", Row, Column,
//				LocationX, LocationY);
		AnyFleet[Row][Column] = true;

		JSFleetInfo.format("<DIV ID=fleet%d CLASS=pointer_h>", i+1);
		JFleetList.format("<PARAM NAME=Fleet%d_X VALUE=%d>\n<PARAM NAME=Fleet%d_Y VALUE=%d>\n",i,LocationX,i,LocationY);
		JFleetInfo.format("<PARAM NAME=Fleet%d_O VALUE=\"%s\">\n",i,CDefenseFleet::get_command_string_normal(CDefenseFleet::COMMAND_NORMAL));
		JSFleetInfo.format("<IMG NAME=\"img_fleet%d\" SRC=\"%s\" "
					"ALT=\"- %s -&#13;%s %d&#13;%s %s\" "
					"TITLE=\"- %s -&#13;%s %d&#13;%s %s\">",
				i+1,
				(char *)NonCapitalFleetImageURL,
				GETTEXT("Fleet Info."),
				GETTEXT("No."),
				FleetID,
				GETTEXT("Order"),
				CDefenseFleet::get_command_string_normal(
								CDefenseFleet::COMMAND_NORMAL),
				GETTEXT("Fleet Info."),
				GETTEXT("No."),
				FleetID,
				GETTEXT("Order"),
				CDefenseFleet::get_command_string_normal(
								CDefenseFleet::COMMAND_NORMAL));
		JSFleetInfo += "</DIV>\n";
	}

	JSFleetList += "</STYLE>\n";

	ITEM("J_FLEET_LIST",(char *)JFleetList);
	ITEM("J_FLEET_INFO",(char *)JFleetInfo);

	ITEM("JS_FLEET_INFO", JSFleetInfo);
//	SLOG("JS_FLEET_INFO: %d[%s]", JSFleetInfo.length(), (char*)JSFleetInfo);

	ITEM("JS_FLEET_LIST", JSFleetList);
//	SLOG("JS_FLEET_LIST: %d[%s]", JSFleetList.length(), (char*)JSFleetList);
	ITEM("RESULT", Result);
//	SLOG("RESULT: %d[%s]", Result.length(), (char*)Result);


	ITEM("FLEET_NUMBER", FleetIDList.length());
    if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
    	return output("war/siege_planet_fleet_deployment.html");
	return output("war/siege_planet_fleet_deployment_js.html");


//	system_log( "end page handler %s", get_name() );
}

