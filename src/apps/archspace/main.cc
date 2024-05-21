#include "archspace.h"
#include "pages.h"
#include "triggers.h"
#include <cerrno>
#include <cstdlib>
#include <clocale>
#include <libintl.h>
#include <sys/types.h>
#include <sys/wait.h>

TZone gArchspaceTriggerStationZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CArchspaceTriggerStation),
	0,
	0,
	NULL,
	"Zone CArchspaceTriggerStation"
};

bool
CArchspacePageStation::registration()
{
	// system
	insert_sorted(new CPageNotFound());

	// player
	insert_sorted(new CPageCreate());
	insert_sorted(new CPageCreate2());
	insert_sorted(new CPageLogin());
	insert_sorted(new CPageLogout());
	insert_sorted(new CPageDeathMenu());
	insert_sorted(new CPageDeathMain());
	insert_sorted(new CPageRestart());
	insert_sorted(new CPageQuit());
	insert_sorted(new CPageRetire());
	insert_sorted(new CPageRetireResult());

	// bot
	insert_sorted(new CBotPagePlayerPower());
	insert_sorted(new CBotPageDeletePlayer());
	insert_sorted(new CBotPageEndScore());
	// game
	insert_sorted(new CPageMenu());
	insert_sorted(new CPageMain());
	insert_sorted(new CPageShowEvent());
	insert_sorted(new CPageAnswerEvent());
	insert_sorted(new CPagePreference());

	// domestic
	insert_sorted(new CPageDomestic());
	insert_sorted(new CPageConcentrationMode());
	insert_sorted(new CPageConcentrationModeResult());

	insert_sorted(new CPagePlanetManagement());
	insert_sorted(new CPagePlanetDetail());
	insert_sorted(new CPagePlanetDestroyConfirm());
	insert_sorted(new CPagePlanetDestroyCancel());
	insert_sorted(new CPagePlanetManagementBuildingResult());
	insert_sorted(new CPagePlanetManagementInvestResult());

	insert_sorted(new CPagePlanetInvestPool());
	insert_sorted(new CPagePlanetInvestPoolResult());
	insert_sorted(new CPagePlanetInvestPoolEnable());
	insert_sorted(new CPagePlanetInvestPoolEnableResult());

	insert_sorted(new CPageResearch());
	insert_sorted(new CPageResearchInvestResult());
	insert_sorted(new CPageResearchTechDetail());
	insert_sorted(new CPageResearchResult());
	insert_sorted(new CPageTechTree());
	insert_sorted(new CPageResearchTechAchievement());

	insert_sorted(new CPageProject());
	insert_sorted(new CPageProjectConfirm());
	insert_sorted(new CPageProjectResult());

	insert_sorted(new CPageWhitebook());
	insert_sorted(new CPageWhitebookControlModel());
	insert_sorted(new CPageWhitebookControlModelDetail());
	insert_sorted(new CPageWhitebookControlModelTech());
	insert_sorted(new CPageWhitebookControlModelProject());
	insert_sorted(new CPageWhitebookControlModelPlanet());

	// diplomacy
	insert_sorted(new CPageDiplomacy());
	insert_sorted(new CPagePlayerDiplomacyStatusList());
	insert_sorted(new CPagePlayerDiplomacyManagement());
	insert_sorted(new CPageReadDiplomaticMessage());
	insert_sorted(new CPageReadDiplomaticMessageUnanswered());
	insert_sorted(new CPageReadDiplomaticMessageDelete());
	insert_sorted(new CPagePlayerDiplomaticMessageAccuse());
	insert_sorted(new CPagePlayerReadDiplomaticMessageResult());
	insert_sorted(new CPagePlayerSendDiplomaticMessage());
	insert_sorted(new CPagePlayerSendDiplomaticMessageWrite());
	insert_sorted(new CPagePlayerSendDiplomaticMessageResult());
	insert_sorted(new CPageInspectOtherPlayer());
	insert_sorted(new CPageInspectOtherPlayerResult());
	insert_sorted(new CPageSpy());
	insert_sorted(new CPageSpyChangeSecurityLevel());
	insert_sorted(new CPageSpyChangeSecurityLevelResult());
	insert_sorted(new CPageSpyConfirm());
	insert_sorted(new CPageSpyResult());

	// council
	insert_sorted(new CPageCouncil());
	insert_sorted(new CPageCouncilVote());
	insert_sorted(new CPageCouncilVoteResult());
	insert_sorted(new CPageAdmissionRequest());
	insert_sorted(new CPageAdmissionRequestResult());
	insert_sorted(new CPageDeleteAdmissionRequestResult());
	insert_sorted(new CPageDonation());
	insert_sorted(new CPageDonationResult());
	insert_sorted(new CPageSpeakerMenu());
	insert_sorted(new CPageIndependenceDeclaration());
	insert_sorted(new CPageIndependenceDeclarationResult());
	insert_sorted(new CPageSetSecondarySpeaker());
	insert_sorted(new CPageTeamSpirit());
	insert_sorted(new CPageDistribute());
	insert_sorted(new CPageCouncilProject());
	insert_sorted(new CPageFinancialAid());
	insert_sorted(new CPageChangeCouncilName());
	insert_sorted(new CPageChangeCouncilSlogan());
	insert_sorted(new CPagePlayerAdmission());
	insert_sorted(new CPagePlayerAdmissionRead());
	insert_sorted(new CPagePlayerAdmissionReply());
	insert_sorted(new CPagePlayerAdmissionAccuse());
	insert_sorted(new CPageNewplayerAssignmentOption());
	insert_sorted(new CPageCouncilDiplomacy());
	insert_sorted(new CPageDiplomaticMessageRead());
	insert_sorted(new CPageDiplomaticMessageReply());
	insert_sorted(new CPageCouncilDiplomaticMessageAccuse());
	insert_sorted(new CPageInspectOtherCouncil());
	insert_sorted(new CPageInspectOtherCouncilResult());
	insert_sorted(new CPageCouncilDiplomacyStatusList());
	insert_sorted(new CPageDiplomaticMessage());
	insert_sorted(new CPageCouncilSendDiplomaticMessage());
	insert_sorted(new CPageCouncilSendDiplomaticMessageWrite());
	insert_sorted(new CPageCouncilSendDiplomaticMessageResult());

	insert_sorted(new CPagePlayerDismiss());
	insert_sorted(new CPagePlayerDismissConfirm());
	insert_sorted(new CPagePlayerDismissResult());

	insert_sorted(new CPageInfo());
	insert_sorted(new CPagePlayerSearch());
	insert_sorted(new CPagePlayerSearchResult());
	insert_sorted(new CPageCouncilSearch());
	insert_sorted(new CPageCouncilSearchResult());
	insert_sorted(new CPageClusters());
	insert_sorted(new CPageClustersCouncil());
	insert_sorted(new CPageRankPlayerOverall());
	insert_sorted(new CPageRankPlayerFleet());
	insert_sorted(new CPageRankPlayerPlanet());
	insert_sorted(new CPageRankPlayerTech());
	insert_sorted(new CPageRankCouncilOverall());
	insert_sorted(new CPageRankCouncilFleet());
	insert_sorted(new CPageRankCouncilPlanet());
	insert_sorted(new CPageRankCouncilDiplomatic());

	insert_sorted( new CPageShipDesign() ); /* telecard 2000/10/09 */
	insert_sorted( new CPageShipDesign2() ); /* telecard 2000/10/10 */
	insert_sorted( new CPageShipDesignConfirm() ); /* telecard 2000/10/11 */
	insert_sorted( new CPageShipDesignRegister() ); /* telecard 2000/10/11 */
	insert_sorted( new CPageShipDesignNotregister() );
	insert_sorted( new CPageShipDesignDeleteConfirm() );
	insert_sorted( new CPageShipDesignDeleteResult() );
	insert_sorted( new CPageShipBuilding() );
	insert_sorted( new CPageShipBuildingInvest() );
	insert_sorted( new CPageShipBuildingInsert() );
	insert_sorted( new CPageShipBuildingDelete() );
	insert_sorted( new CPageShipPool() );
	insert_sorted( new CPageShipPoolScrapConfirm() );
	insert_sorted( new CPageShipPoolScrapResult() );

	insert_sorted(new CPageFleet());
	insert_sorted(new CPageFormNewFleet());
	insert_sorted(new CPageFormNewFleetConfirm());
	insert_sorted(new CPageFormNewFleetResult());

	insert_sorted(new CPageReassignment());
	insert_sorted(new CPageReassignment2());
	insert_sorted(new CPageExpedition());
	insert_sorted(new CPageExpeditionConfirm());
	insert_sorted(new CPageExpeditionResult());
	insert_sorted(new CPageMission());
	insert_sorted(new CPageMission2());

	insert_sorted(new CPageFleetCommander());
	insert_sorted(new CPageFleetCommanderViewOptions());
	insert_sorted(new CPageFleetCommanderInformation());
	insert_sorted(new CPageFleetCommanderDismissConfirm());
	insert_sorted(new CPageFleetCommanderDismissResult());

	insert_sorted(new CPageMissionTrainResult());
	insert_sorted(new CPageMissionStationResult());
	insert_sorted(new CPageMissionPatrolResult());
	insert_sorted(new CPageMissionDispatchResult());
	insert_sorted(new CPageRecall());
	insert_sorted(new CPageRecallConfirm());
	insert_sorted(new CPageRecallResult());
	insert_sorted(new CPageDisband());
	insert_sorted(new CPageDisbandConfirm());
	insert_sorted(new CPageDisbandResult());
	insert_sorted(new CPageReassignmentCommanderChangeConfirm());
	insert_sorted(new CPageReassignmentCommanderChangeResult());
	insert_sorted(new CPageReassignmentRemoveAddShipConfirm());
	insert_sorted(new CPageReassignmentRemoveAddShipResult());
	insert_sorted(new CPageReassignmentShipReassignmentConfirm());
	insert_sorted(new CPageReassignmentShipReassignmentResult());

	insert_sorted(new CPageWar());
	insert_sorted(new CPageSiegePlanet());
	insert_sorted(new CPageSiegePlanetFleet());
	insert_sorted(new CPageSiegePlanetFleetDeployment());
	insert_sorted(new CPageSiegePlanetPlanet());
	insert_sorted(new CPageSiegePlanetResult());
	insert_sorted(new CPageBlockade());
	insert_sorted(new CPageBlockadeFleet());
	insert_sorted(new CPageBlockadeFleetDeployment());
	insert_sorted(new CPageBlockadePlanet());
	insert_sorted(new CPageBlockadeResult());
	insert_sorted(new CPageRaid());
	insert_sorted(new CPageRaidFleet());
	insert_sorted(new CPageRaidPlanet());
	insert_sorted(new CPageRaidResult());
	insert_sorted(new CPagePrivateer());
	insert_sorted(new CPagePrivateerFleet());
	insert_sorted(new CPagePrivateerResult());
	insert_sorted(new CPageDefensePlan()); /* telecard 2000/10/26 */
	insert_sorted(new CPageDefensePlanGenericNew());
	insert_sorted(new CPageDefensePlanGenericDeployment());
	insert_sorted(new CPageDefensePlanGenericResult());
	insert_sorted(new CPageDefensePlanGenericDelete());
	insert_sorted(new CPageDefensePlanSpecialNew());
	insert_sorted(new CPageDefensePlanSpecialNewDeployment());
	insert_sorted(new CPageDefensePlanSpecialNewResult());
	insert_sorted(new CPageDefensePlanSpecialOld());
	insert_sorted(new CPageDefensePlanSpecialChangeDeployment());
	insert_sorted(new CPageDefensePlanSpecialChangeResult());
	insert_sorted(new CPageDefensePlanSpecialDeleteResult());

	insert_sorted(new CPageBattleReport());
	insert_sorted(new CPageBattleReport2());
	insert_sorted(new CPageBattleReportDetail());	//telecard 2001/01/17
	insert_sorted(new CPageBattleReportSearch());	//telecard 2001/01/19

	insert_sorted(new CPageClassSpecification());

	/* black market */
	insert_sorted( new CPageBlackMarket() );	// telecard 2001/02/22
	insert_sorted( new CPageBlackMarketTechDeck() );	// telecard 2001/02/27
	insert_sorted( new CPageBlackMarketTechDeckBid() );
	insert_sorted( new CPageBlackMarketTechDeckBidResult() );
	insert_sorted( new CPageBlackMarketFleetDeck() );	// telecard 2001/03/03
	insert_sorted( new CPageBlackMarketFleetDeckAdmiral() );	// telecard 2001/03/05
	insert_sorted( new CPageBlackMarketFleetDeckAdmiralBid() );	// telecard 2001/03/13
	insert_sorted( new CPageBlackMarketFleetDeckAdmiralBidResult() );	// telecard 2001/03/13
	insert_sorted( new CPageBlackMarketOfficersLounge() );
	insert_sorted( new CPageBlackMarketOfficersLoungeBid() );
	insert_sorted( new CPageBlackMarketOfficersLoungeBidResult() );	// telecard 2001/03/13
	insert_sorted( new CPageBlackMarketRareGoods() );
	insert_sorted( new CPageBlackMarketRareGoodsBid() );
	insert_sorted( new CPageBlackMarketRareGoodsBidResult() );
	insert_sorted( new CPageBlackMarketLeasersOffice() );
	insert_sorted( new CPageBlackMarketLeasersOfficeBid() );
	insert_sorted( new CPageBlackMarketLeasersOfficeBidResult() );	// telecard 2001/03/08

	insert_sorted( new CPageEmpireDiplomacy() );
	insert_sorted( new CPageEmpireShowAction() );
	insert_sorted( new CPageEmpireAnswerAction() );
//	insert_sorted( new CPageEmpireBounty() );
//	insert_sorted( new CPageEmpireBountyConfirm() );
//	insert_sorted( new CPageEmpireBountyResult() );
	insert_sorted( new CPageEmpireBribe() );
	insert_sorted( new CPageEmpireBribeResult() );
	insert_sorted( new CPageEmpireRequestBoon() );
	insert_sorted( new CPageEmpireRequestBoonResult() );
	insert_sorted( new CPageEmpireRequestMediation() );
	insert_sorted( new CPageEmpireRequestMediationResult() );
	insert_sorted(new CPageEmpireInvadeEmpire());
	insert_sorted(new CPageEmpireInvadeMagistrateConfirm());
	insert_sorted(new CPageEmpireInvadeMagistrateFleet());
	insert_sorted(new CPageEmpireInvadeMagistrateFleetDeployment());
	insert_sorted(new CPageEmpireInvadeMagistrateResult());
	insert_sorted(new CPageEmpireInvadeEmpirePlanetConfirm());
	insert_sorted(new CPageEmpireInvadeEmpirePlanetFleet());
	insert_sorted(new CPageEmpireInvadeEmpirePlanetFleetDeployment());
	insert_sorted(new CPageEmpireInvadeEmpirePlanetResult());
	insert_sorted(new CPageEmpireInvadeFortressFleet());
	insert_sorted(new CPageEmpireInvadeFortressFleetDeployment());
	insert_sorted(new CPageEmpireInvadeFortressResult());
	insert_sorted(new CPageEmpireInvadeEmpireCapitalPlanetConfirm());
	insert_sorted(new CPageEmpireInvadeEmpireCapitalPlanetFleet());
	insert_sorted(new CPageEmpireInvadeEmpireCapitalPlanetFleetDeployment());
	insert_sorted(new CPageEmpireInvadeEmpireCapitalPlanetResult());

	insert_sorted(new CPageAdmin());
	insert_sorted(new CPageInspect());
	insert_sorted(new CPagePlayerConfirm());
	insert_sorted(new CPagePlayer());
	insert_sorted(new CPagePlayerResearchStatus());
	insert_sorted(new CPagePlayerSeeDomestic());
	insert_sorted(new CPagePlayerMessage());
	insert_sorted(new CPagePlayerMessageRead());
	insert_sorted(new CPagePlayerMessageDeleteConfirm());
	insert_sorted(new CPagePlayerMessageDeleteResult());
	insert_sorted(new CPageCouncilMessage());
	insert_sorted(new CPageCouncilMessageRead());
	insert_sorted(new CPageCouncilMessageDeleteConfirm());
	insert_sorted(new CPageCouncilMessageDeleteResult());
	insert_sorted(new CPagePlayerSendMessage());
	insert_sorted(new CPagePlayerSendMessageResult());
	insert_sorted(new CPagePlayerPunishment());
	insert_sorted(new CPagePlayerPunishmentResult());
	insert_sorted(new CPagePlayerPunishmentDeleteCharacterConfirm());
	insert_sorted(new CPagePlayerPunishmentDeleteCharacterResult());
	insert_sorted(new CPageAdminCouncil());
	insert_sorted(new CPageCouncilControl());
	insert_sorted(new CPageCouncilControlResult());
	insert_sorted(new CPageBlackMarketControl());
	insert_sorted(new CPageBlackMarketControlResult());
	insert_sorted(new CPageConfig());
	insert_sorted(new CPageRebootConfirm());
	insert_sorted(new CPageRebootResult());
	insert_sorted(new CPageResetConfirm());
	insert_sorted(new CPageResetResult());
	insert_sorted(new CPageBroadcastConfirm());
	insert_sorted(new CPageBroadcastResult());

	return true;
}

bool
CArchspaceTriggerStation::registration()
{
//	push(new CTriggerGetKey());
	push(new CTriggerDebug());

//	removed by Nara at 2001. 6. 19. for server optimizing
//	push(new CTriggerDatabasePush());

//	push(new CTriggerUpdateTurn());
	push(new CTriggerAction());
//	push(new CTriggerOptimize());
	push(new CTriggerRelationTimeout());
	push(new CTriggerRank());
	// start telecard 2001/02/15
	push( new CTriggerBlackMarketItem() );
	push( new CTriggerBlackMarketExpire() );
	// end telecard 2001/02/15
//	push( new CTriggerPersonalEnding() );	//telecard 2001/03/31

	CTriggerRunCronTab *
		CronTabHandler = new CTriggerRunCronTab();

	CCronTabDummy *
		Dummy = new CCronTabDummy();
//	CCronTabDatabasePush *DatabasePush = new CCronTabDatabasePush();
//	CCronTabNewPlayer *
//		NewPlayer = new CCronTabNewPlayer();
	CCronTabCouncil *
		Council = new CCronTabCouncil();

	CCronTabEvent *
		Event = new CCronTabEvent();
	CCronTabCommerce *
	    Commerce = new CCronTabCommerce();

	CCronTabEmpireRelation *
		EmpireRelation = new CCronTabEmpireRelation();
	CCronTabEmpireDemand *
		EmpireDemand = new CCronTabEmpireDemand();

	CCronTabEmpireFleetRegeneration *
		EmpireFleetRegeneration = new CCronTabEmpireFleetRegeneration();
	CCronTabEmpireCounterattackMagistrate *
		EmpireCounterattackMagistrate = new CCronTabEmpireCounterattackMagistrate();
	CCronTabEmpireCounterattackEmpirePlanet *
		EmpireCounterattackEmpirePlanet = new CCronTabEmpireCounterattackEmpirePlanet();
    CCronTabEmpireDatabaseSave *
        EmpireDatabaseSave = new CCronTabEmpireDatabaseSave();

	Dummy->load_last_activated_time();
//	NewPlayer->load_last_activated_time();
//	DatabasePush->load_last_activated_time();
	Council->load_last_activated_time();

	Event->load_last_activated_time();
	Commerce->load_last_activated_time();

	EmpireRelation->load_last_activated_time();
	EmpireDemand->load_last_activated_time();

	EmpireFleetRegeneration->load_last_activated_time();
	EmpireCounterattackMagistrate->load_last_activated_time();
	EmpireCounterattackEmpirePlanet->load_last_activated_time();
    EmpireDatabaseSave->load_last_activated_time();

	CronTabHandler->push(Dummy);
//	CronTabHandler->push(NewPlayer);
//	CronTabHandler->push(DatabasePush);
	CronTabHandler->push(Council);

	CronTabHandler->push(Event);

	CronTabHandler->push(Commerce);

	CronTabHandler->push(EmpireRelation);
	CronTabHandler->push(EmpireDemand);

	CronTabHandler->push(EmpireFleetRegeneration);
	CronTabHandler->push(EmpireCounterattackMagistrate);
	CronTabHandler->push(EmpireCounterattackEmpirePlanet);
    CronTabHandler->push(EmpireDatabaseSave);

	push(CronTabHandler);

	return true;
}

pth_mutex_t CArchspaceStoreCenter::mSQLMutex = PTH_MUTEX_INIT;
pth_t CArchspaceStoreCenter::mSQLThread;

void
CArchspaceStoreCenter::lock_sql()
{
         pth_mutex_acquire(&mSQLMutex, FALSE, NULL);
}

void
CArchspaceStoreCenter::unlock_sql()
{
         pth_mutex_release(&mSQLMutex);
         pth_nap((pth_time_t){0, 5});
}

bool
CArchspaceStoreCenter::store(CStore &aStore)
{
	bool Ret;

    lock_sql();
	Ret = CStoreCenter::store(aStore);
    unlock_sql();

	return Ret;
}

bool CArchspaceStoreCenter::query (const char *aTable, const char *aQuery)
{
    lock_sql();
    bool Ret = CStoreCenter::query(aTable,aQuery);
    unlock_sql();
    return Ret;
}

void
*CArchspaceStoreCenter::write(void *aArg)
{
	CArchspaceStoreCenter *StoreCenter = (CArchspaceStoreCenter*)aArg;
	int Ret = 1;
	SLOG("prio min: %d max: %d std: %d", (int)PTH_PRIO_MIN, (int)PTH_PRIO_MAX, (int)PTH_PRIO_STD);
	SLOG("Priority: %d", (int)pth_ctrl(PTH_CTRL_GETPRIO, mSQLThread));
	while (CArchspace::get_server_loop())
	{
	    lock_sql();
	    if (!StoreCenter->is_locked())
	    {
	      if (StoreCenter->length() > 50000)
          {
              // detected bottlekneck, flushing!
                SLOG("Query Bottleneck Flushing Start: %d", StoreCenter->length());
              	while(Ret == 0 && StoreCenter->length() > 10000)
	            {
	              Ret = STORE_CENTER->CStoreCenter::write();
                }
                SLOG("Query Bottleneck Flushing End: %d", StoreCenter->length());
          }
          else
          {
            Ret = StoreCenter->CStoreCenter::write();
          }
        }
 	    else
             Ret = 0;
     	unlock_sql();

        if (Ret == 1 || Ret == -1) pth_nap(pth_time(1, 0));
        else
        {
//            pth_nap(pth_time(0, 15));
            pth_yield(NULL);
        }
	}
	return NULL;
}

void
CArchspaceStoreCenter::spawn_database_sql_thread()
{
  mSQLThread = pth_spawn(PTH_ATTR_DEFAULT, CArchspaceStoreCenter::write, STORE_CENTER);
  if (mSQLThread == NULL)
      SLOG("Could not spawn database sql thread");
  SLOG("Spawn database sql thread");
}

void
CArchspaceStoreCenter::kill_database_sql_thread()
{
         pth_cancel(mSQLThread);
}
int
main(int argc, char* argv[])
{
	// for deamon
	int
		Child;
/*
	if ((Child = fork()) < 0) return -1;
	else if (Child != 0) exit(0);

	setsid();
	umask(0);
*/

	pth_init();

/*	if ((Child = fork()) < 0) return -1;
	if (Child == 0)
	{
		CArchspaceDatabase
			Database;
		if (!Database.initialize(argc, argv))
		{
			SLOG("Cannot start database process");
			return -1;
		}
		Database.run();

		SLOG("terminate database");
		return 0;
	}
	else
	{*/
		char
			DomainDir[255];
		strcpy(DomainDir, getenv("HOME"));
		strcat(DomainDir, "/src/apps/archspace/msg");
		setlocale(LC_ALL, "");

		bindtextdomain("archspace", DomainDir);
		textdomain("archspace");

		CArchspace
			Archspace;
		if (!Archspace.initialize(argc, argv))
		{
			SLOG("Cannot start archspace server!");
		}
		else
		{
			Archspace.run();
		}

//		kill(Child, SIGTERM); //Child isn't used because of the above comments
		int
			Status;
		wait(&Status);
		SLOG("wait status %d", Status);
//	}
	pth_kill();

	exit(EXIT_SUCCESS);
}

