#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../player.h"
#include "../../preference.h"
#include "../../define.h"
#include <cstdio>

bool
CPageEmpireInvadeEmpireCapitalPlanetFleetDeployment::handler(CPlayer *aPlayer)
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

#ifdef PROTECTION_BEGINNER
	if (aPlayer->get_protected_mode() == CPlayer::PROTECTED_BEGINNER)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You cannot attack the empire for it is protecting you")));
		return output("empire/invade_empire_error.html");
	}
#endif

	static CString
		CapitalFleetImageURL,
		NonCapitalFleetImageURL;
	CapitalFleetImageURL.clear();
	NonCapitalFleetImageURL.clear();

	CapitalFleetImageURL.format("%s/image/as_game/fleet/ship_cap.gif",
								(char *)CGame::mImageServerURL),
	NonCapitalFleetImageURL.format("%s/image/as_game/fleet/ship_set.gif",
									(char *)CGame::mImageServerURL);

	int
		HomeClusterID = aPlayer->get_home_cluster_id();

	CMagistrateList *
		MagistrateList = EMPIRE->get_magistrate_list();
	CMagistrate *
		Magistrate = MagistrateList->get_by_cluster_id(HomeClusterID);

	if (Magistrate == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no magistrate in the cluster(#%1$s)."),
							dec2unit(HomeClusterID)));
		return output("empire/invade_empire_error.html");
	}
	if (Magistrate->is_dead() == false)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("The magistrate in the cluster(#%1$s) is still alive. So you can't attack The Empire's planets yet."),
							dec2unit(HomeClusterID)));
		return output("empire/invade_empire_error.html");
	}

	if (EMPIRE->get_number_of_empire_planets() > 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Not all the Empire' planets have taken by players yet. So you can't attack The Empire's fortress yet."));
		return output("empire/invade_empire_error.html");
	}

	CFortressList *
		FortressList = EMPIRE->get_fortress_list();
	CCouncil *
		Council = aPlayer->get_council();

	if (FortressList->get_number_of_fortresses_taken_by_council(Council, 4) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("To attack the Empire capital planet, your council must have at least 1 fortress in the 4th layer."));
		return output("empire/invade_empire_error.html");
	}

	if (CEmpire::mEmpireInvasionLimit != 0)
	{
		CPlayerActionList *
			PlayerActionList = aPlayer->get_action_list();

		int
			InvasionHistory = 0;
		time_t
			EffectiveStartTime = CGame::get_game_time() - CEmpire::mEmpireInvasionLimitDuration;
		for (int i=0 ; i<PlayerActionList->length() ; i++)
		{
			CPlayerAction *
				PlayerAction = (CPlayerAction *)PlayerActionList->get(0);
			if (PlayerAction->get_type() != CAction::ACTION_PLAYER_EMPIRE_INVASION_HISTORY) continue;
			if (PlayerAction->get_start_time() > EffectiveStartTime) InvasionHistory++;
		}

		if (InvasionHistory >= CEmpire::mEmpireInvasionLimit)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You are trying to attack the empire too often. Take a little rest."));
			return output("empire/invade_empire_error.html");
		}
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	if (FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You don't have any stand-by fleets."));
		return output("empire/invade_empire_error.html");
	}

	QUERY("CAPITAL", CapitalString);
	int
		CapitalID = as_atoi(CapitalString);
	CFleet *
		CapitalFleet = FleetList->get_by_id(CapitalID);
	if (CapitalFleet == NULL)
	{
		ITEM("ERROR_MESSAGE",
			GETTEXT("You don't have such a fleet that you selected as a capital fleet.<BR>"
					"Please try again."));
		return output("empire/invade_empire_error.html");
	}

	if (CapitalFleet->get_status() != CFleet::FLEET_STAND_BY)
	{
		ITEM("ERROR_MESSAGE",
			(char *)format(GETTEXT("%1$s fleet is not on stand-by."),
							CapitalFleet->get_nick()));
		return output("empire/invade_empire_error.html");
	}

	CIntegerList FleetIDList;

	FleetIDList.push((TSomething)CapitalID);

	char
		Query[100];
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		sprintf(Query, "FLEET%d", i);

		QUERY(Query, FleetNumberString);

		if (FleetNumberString == NULL) continue;
		if (strcasecmp(FleetNumberString, "ON") != 0) continue;

		sprintf(Query, "FLEET%d_ID", i);

		QUERY(Query, FleetIDString);
		int
			FleetID = as_atoi(FleetIDString);
		if (FleetID < 1)
		{
			ITEM("ERROR_MESSAGE",
				GETTEXT("1 or more fleets' ID were not valid."));
			return output("empire/invade_empire_error.html");
		}

		CFleet *
			Fleet = (CFleet *)FleetList->get_by_id(FleetID);
		if (Fleet == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You don't have the fleet with ID %1$s."),
									dec2unit(FleetID)));
			return output("empire/invade_empire_error.html");
		}

		if (Fleet->get_status() != CFleet::FLEET_STAND_BY)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("%1$s fleet is not on stand-by."),
									Fleet->get_nick()));
			return output("empire/invade_empire_error.html");
		}

#define SHIP_SIZE_CRUISER	5
		if (Fleet->get_size() < SHIP_SIZE_CRUISER)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("The size of %1$s is too small to voyage out to the Empire's cluster."), Fleet->get_nick()));

			return output("empire/invade_empire_error.html");
		}
#undef SHIP_SIZE_CRUISER

		if (Fleet->get_id() == CapitalID) continue;

		FleetIDList.push((TSomething)(Fleet->get_id()));
	}

//	SLOG("FleetIDList : %d", FleetIDList.length());

	if (FleetIDList.length() == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("No fleets have been selected.<BR>"
						"Please try again."));
		return output("empire/invade_empire_error.html");
	}
	if (FleetIDList.length() > 20)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The number of fleets you selected is more than 20.<BR>"
						"Please try again."));
		return output("empire/invade_empire_error.html");
	}

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
    }
    else // Javascript specific defense plan code
    {
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
		JFleetList.format("<PARAM NAME=Fleet%d_X VALUE=%d>\n<PARAM NAME=Fleet%d_Y VALUE=%d>\n",i,LocationX,i,LocationY);
		JFleetInfo.format("<PARAM NAME=Fleet%d_O VALUE=\"%s\">\n",i,CDefenseFleet::get_command_string_normal(CDefenseFleet::COMMAND_NORMAL));


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

	ITEM("JS_FLEET_LIST", JSFleetList);
	ITEM("RESULT", (char *)Result);

	ITEM("FLEET_NUMBER", FleetIDList.length());

//	system_log( "end page handler %s", get_name() );
    if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
    	return output("empire/invade_empire_capital_planet_fleet_deployment.html");
   	return output("empire/invade_empire_capital_planet_fleet_deployment_js.html");    	
}

