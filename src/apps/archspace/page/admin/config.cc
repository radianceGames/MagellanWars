#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include <stdlib.h>

bool
CPageConfig::handler(CPlayer *aPlayer)
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

    char *ConfigLocation = CONFIG->get_string("Server", "ConfigFile", NULL);
    if (ConfigLocation == NULL)
    {
		ITEM("ERROR_MESSAGE",
				GETTEXT("[Server] ConfigFile = /Config/File is not specified"));
		return output("admin/admin_error.html");
    }    
    char *ConfigTempLocation = CONFIG->get_string("Server", "ConfigTempFile", NULL);
    if (ConfigTempLocation == NULL)
    {
		ITEM("ERROR_MESSAGE",
				GETTEXT("[Server] ConfigTempFile = /Config/Temp/File is not specified"));
		return output("admin/admin_error.html");
    }    


	CString
		SEDTest;
	SEDTest.format("%s s/dummy/dummy/g /dev/null", (char *)CAdminTool::mSEDPath);

	if (system((char *)SEDTest) != 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("SED invoking test failed. Please ask Archspace Development Team."));
		return output("admin/admin_error.html");
	}

	int
		OldSecondPerTurn = CGame::mSecondPerTurn;

	QUERY("SECS_PER_TURN", SecsPerTurnString);
	if (SecsPerTurnString != NULL)
	{
		int
			SecsPerTurn = as_atoi(SecsPerTurnString);
		if (SecsPerTurn > 0)
		{
			int
				OldSecsPerTurn = CGame::mSecondPerTurn;
			CGame::mSecondPerTurn = SecsPerTurn;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed SECS_PER_TURN from %d to %d", OldSecsPerTurn, SecsPerTurn));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"SecondPerTurn[[:blank:]]*=[[:blank:]]*%d\"/\"SecondPerTurn = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldSecsPerTurn, CGame::mSecondPerTurn, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("MAX_PLAYER", MaxPlayerString);
	if (MaxPlayerString != NULL)
	{
		int
			MaxPlayer = as_atoi(MaxPlayerString);
		if (MaxPlayer > 0)
		{
			int
				OldMaxPlayer = CGame::mMaxUser;
			CGame::mMaxUser = MaxPlayer;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed MAX_PLAYER from %d to %d", OldMaxPlayer, MaxPlayer));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"MaxUser[[:blank:]]*=[[:blank:]]*%d\"/\"MaxUser = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldMaxPlayer, CGame::mMaxUser, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("TURNS_FOR_TRAIN", TurnsForTrainString);
	if (TurnsForTrainString != NULL)
	{
		int
			TurnsForTrain = as_atoi(TurnsForTrainString);
		if (TurnsForTrain > 0)
		{
			int
				OldTurnsForTrain = CMission::mTrainMissionTime / OldSecondPerTurn;
			CMission::mTrainMissionTime = TurnsForTrain * CGame::mSecondPerTurn;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed TURNS_FOR_TRAIN from %d to %d", OldTurnsForTrain, TurnsForTrain));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"TrainMissionTime[[:blank:]]*=[[:blank:]]*%d\"/\"TrainMissionTime = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldTurnsForTrain, CMission::mTrainMissionTime / CGame::mSecondPerTurn, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("TURNS_FOR_PATROL", TurnsForPatrolString);
	if (TurnsForPatrolString != NULL)
	{
		int
			TurnsForPatrol = as_atoi(TurnsForPatrolString);
		if (TurnsForPatrol > 0)
		{
			int
				OldTurnsForPatrol = CMission::mPatrolMissionTime / OldSecondPerTurn;
			CMission::mPatrolMissionTime = TurnsForPatrol * CGame::mSecondPerTurn;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed TURNS_FOR_PATROL from %d to %d", OldTurnsForPatrol, TurnsForPatrol));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"PatrolMissionTime[[:blank:]]*=[[:blank:]]*%d\"/\"PatrolMissionTime = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldTurnsForPatrol, CMission::mPatrolMissionTime / CGame::mSecondPerTurn, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("TURNS_FOR_DISPATCH", TurnsForDispatchString);
	if (TurnsForDispatchString != NULL)
	{
		int
			TurnsForDispatch = as_atoi(TurnsForDispatchString);
		if (TurnsForDispatch > 0)
		{
			int
				OldTurnsForDispatch = CMission::mDispatchToAllyMissionTime / OldSecondPerTurn;
			CMission::mDispatchToAllyMissionTime = TurnsForDispatch * CGame::mSecondPerTurn;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed TURNS_FOR_DISPATCH from %d to %d", OldTurnsForDispatch, TurnsForDispatch));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"DispatchToAllyMissionTime[[:blank:]]*=[[:blank:]]*%d\"/\"DispatchToAllyMissionTime = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldTurnsForDispatch, CMission::mDispatchToAllyMissionTime / CGame::mSecondPerTurn, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("TURNS_FOR_EXPEDITION", TurnsForExpeditionString);
	if (TurnsForExpeditionString != NULL)
	{
		int
			TurnsForExpedition = as_atoi(TurnsForExpeditionString);
		if (TurnsForExpedition > 0)
		{
			int
				OldTurnsForExpedition = CMission::mExpeditionMissionTime / OldSecondPerTurn;
			CMission::mExpeditionMissionTime = TurnsForExpedition * CGame::mSecondPerTurn;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed TURNS_FOR_EXPEDITION from %d to %d", OldTurnsForExpedition, TurnsForExpedition));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"ExpeditionMissionTime[[:blank:]]*=[[:blank:]]*%d\"/\"ExpeditionMissionTime = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldTurnsForExpedition, CMission::mExpeditionMissionTime / CGame::mSecondPerTurn, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("TURNS_FOR_RETURNING", TurnsForReturningString);
	if (TurnsForReturningString != NULL)
	{
		int
			TurnsForReturning = as_atoi(TurnsForReturningString);
		if (TurnsForReturning > 0)
		{
			int
				OldTurnsForReturning = CMission::mReturningWithPlanetMissionTime / OldSecondPerTurn;
			CMission::mReturningWithPlanetMissionTime = TurnsForReturning * CGame::mSecondPerTurn;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed TURNS_FOR_RETURNING from %d to %d", OldTurnsForReturning, TurnsForReturning));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"ReturningWithPlanetMissionTime[[:blank:]]*=[[:blank:]]*%d\"/\"ReturningWithPlanetMissionTime = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldTurnsForReturning, CMission::mReturningWithPlanetMissionTime / CGame::mSecondPerTurn, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("BLACK_MARKET_ITEM_REGEN", BlackMarketItemRegenString);
	if (BlackMarketItemRegenString != NULL)
	{
		int
			BlackMarketItemRegen = as_atoi(BlackMarketItemRegenString);
		if (BlackMarketItemRegen > 0)
		{
			int
				OldBlackMarketItemRegen = CBlackMarket::mBlackMarketItemRegen;
			CBlackMarket::mBlackMarketItemRegen = BlackMarketItemRegen;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed BLACK_MARKET_ITEM_REGEN from %d to %d", OldBlackMarketItemRegen, BlackMarketItemRegen));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"BlackMarketItemRegen[[:blank:]]*=[[:blank:]]*%d\"/\"BlackMarketItemRegen = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldBlackMarketItemRegen, CBlackMarket::mBlackMarketItemRegen, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("BID_EXPIRE_TIME", BidExpireTimeString);
	if (BidExpireTimeString != NULL)
	{
		int
			BidExpireTime = as_atoi(BidExpireTimeString);
		if (BidExpireTime > 0)
		{
			int
				OldBidExpireTime = CBlackMarket::mBidExpireTime;
			CBlackMarket::mBidExpireTime = BidExpireTime;
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed BID_EXPIRE_TIME from %d to %d", OldBidExpireTime, BidExpireTime));
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"BidExpireTime[[:blank:]]*=[[:blank:]]*%d\"/\"BidExpireTime = %d\"/ %s > %s", (char *)CAdminTool::mSEDPath, OldBidExpireTime, CBlackMarket::mBidExpireTime, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	QUERY("START_GAME", StartGameString);
	if (StartGameString != NULL)
	{
	    CString CurrentString = "no";
	    bool StartGame = (!strcasecmp(StartGameString, "yes"));
		if (StartGame != CGame::mUpdateTurn)
		{
		    if (!StartGame)
		    {
                StartGameString = "no";  
                CurrentString = "yes";
            }    
			ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Config - Changed START_GAME from %d to %d", CGame::mUpdateTurn, StartGame));
		    CGame::mUpdateTurn = StartGame;
			CString
				SED;
			SED	.clear();
			SED.format("%s s/\"StartGame[[:blank:]]*=[[:blank:]]*%s\"/\"StartGame = %s\"/ %s > %s", (char *)CAdminTool::mSEDPath, (char *)CurrentString, StartGameString, ConfigLocation, ConfigTempLocation);
			system((char *)SED);
			system((char*)format("cp --reply=yes %s %s", ConfigTempLocation, ConfigLocation));
			system((char*)format("rm %s", ConfigTempLocation));
		}
	}

	ITEM("STRING_GAME_CONFIGURATION", GETTEXT("Game Configuration"));

	ITEM("STRING_SECS_PER_TURN", GETTEXT("Secs Per Turn"));
	ITEM("SECS_PER_TURN", CGame::mSecondPerTurn);

	ITEM("STRING_MAX__NUMBER_OF_PLAYERS", GETTEXT("Max. Number Of Players"));
	ITEM("MAX_PLAYER", CGame::mMaxUser);

	ITEM("STRING_TURNS_FOR_TRAIN", GETTEXT("Turns For Train"));
	ITEM("TURNS_FOR_TRAIN", CMission::mTrainMissionTime/CGame::mSecondPerTurn);

	ITEM("STRING_TURNS_FOR_PATROL", GETTEXT("Turns For Patrol"));
	ITEM("TURNS_FOR_PATROL", CMission::mPatrolMissionTime/CGame::mSecondPerTurn);

	ITEM("STRING_TURNS_FOR_DISPATCH", GETTEXT("Turns For Dispatch"));
	ITEM("TURNS_FOR_DISPATCH", CMission::mDispatchToAllyMissionTime/CGame::mSecondPerTurn);

	ITEM("STRING_TURNS_FOR_EXPEDITION", GETTEXT("Turns For Expedition"));
	ITEM("TURNS_FOR_EXPEDITION", CMission::mExpeditionMissionTime/CGame::mSecondPerTurn);

	ITEM("STRING_TURNS_FOR_RETURNING", GETTEXT("Turns For Returning"));
	ITEM("TURNS_FOR_RETURNING",
			CMission::mReturningWithPlanetMissionTime/CGame::mSecondPerTurn);

	ITEM("STRING_TIME_FOR_BLACK_MARKET_ITEM_REGEN__IN_SECS_",
			GETTEXT("Time For Black Market Item Regen.(in secs)"));
	ITEM("BLACK_MARKET_ITEM_REGEN", CBlackMarket::mBlackMarketItemRegen);

	ITEM("STRING_BLACK_MARKET_BID_EXPIRE_TIME_IN_MINS_",
			GETTEXT("Black Market Bid Expire Time"));
	ITEM("BID_EXPIRE_TIME", CBlackMarket::mBidExpireTime);

	if (CGame::mUpdateTurn)
	{
	ITEM("STRING_START_GAME", GETTEXT("Start Game?"));
	ITEM("START_GAME", "yes");
	}
  	else
    {    
	ITEM("STRING_START_GAME", GETTEXT("Start Game?"));
	ITEM("START_GAME", "no");
    }    
      // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));


//	system_log("end page handler %s", get_name());

	return output("admin/config.html");
}
