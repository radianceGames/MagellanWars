#include "bounty.h"
#include "common.h"
#include "util.h"
#include "player.h"
#include <cstdlib>
#include "archspace.h"
#include "game.h"
#include "council.h"
#include <libintl.h>
#include "race.h"
#include <cmath>
#include <cstdio>
#include "battle.h"
#include "admin.h"
#include "admiral.h"
#include "fleet.h"

int CPlayer::mPeriodCreateAdmiral = 24;
int CPlayer::mAdmiralMilitaryBonus = 4;
int CPlayer::mMaxPeriodCreateAdmiral = 1;
int CPlayer::mExtraBlackMarketAdmirals = 5;
int CPlayer::mMaxAdmirals = 250;
int CPlayer::mRetireTimeLimit = 60*0*24;
int CPlayer::mAdmissionLimitTime = 60*60*12;
int CPlayer::mHonorIncreaseTurns = 288;
int CPlayer::mProtectionPlanetLossTime = 60*60*12;
int CPlayer::mProtectionPlanetLossAmount = 70;

TZone gPlayerZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPlayer),
	0,
	0,
	NULL,
	"Zone CPlayer"
};

// constructor for load player
CPlayer::CPlayer(MYSQL_ROW aRow)
{
	try
	{
		mNewsCenter.set_owner(this);
		mRelationList.set_owner(this);

		mGameID = atoi(aRow[STORE_GAME_ID]);
		mPortalID = atoi(aRow[STORE_PORTAL_ID]);

		mName = aRow[STORE_NAME];
		mHomeClusterID = atoi(aRow[STORE_HOME_CLUSTER_ID]);
		mLastLogin = atoi(aRow[STORE_LAST_LOGIN]);
		mLastLoginIP = aRow[STORE_LAST_LOGIN_IP];

		mMode = (EConcentrationMode)atoi(aRow[STORE_MODE]);
		set_race(atoi(aRow[STORE_RACE]));

		mHonor = atoi(aRow[STORE_HONOR]);

		mResearchInvest = atoi(aRow[STORE_RESEARCH_INVEST]);

		mTick = atoi(aRow[STORE_TICK]);
		mTurn = atoi(aRow[STORE_TURN]);

		mProduction = atoi(aRow[STORE_PRODUCTION]);
		mShipProduction = atoi(aRow[STORE_SHIP_PRODUCTION]);
		mInvestedShipProduction = as_atoi(aRow[STORE_INVESTED_SHIP_PRODUCTION]);
		mResearch = atoi(aRow[STORE_RESEARCH]);
		mAbility.set_string(aRow[STORE_ABILITY]);

		mTargetTech = atoi(aRow[STORE_TARGET_TECH]);

		mAdmiralTimer = atoi(aRow[STORE_ADMIRAL_TIMER]);

		mHonorTimer = atoi(aRow[STORE_HONOR_TIMER]);

		mLastTurnResource.set(RESOURCE_PRODUCTION,
			atoi(aRow[STORE_LAST_TURN_PRODUCTION]));
		mLastTurnResource.set(RESOURCE_RESEARCH,
			atoi(aRow[STORE_LAST_TURN_RESEARCH]));
		mLastTurnResource.set(RESOURCE_MILITARY,
			atoi(aRow[STORE_LAST_TURN_MILITARY]));
		mAdmission = NULL;
		mCouncilID = atoi(aRow[STORE_COUNCIL_ID]);
		if (is_dead() == false)
		{
			CCouncil *
			Council = get_council();
			if (Council == NULL)
			{
				set_dead((char *)format(GETTEXT("Player %1$s is in a council which doesn't exist."), (char *)mName));
				SLOG("WARNING: Player %s(#%d) has unknown council id(#%d), so this player is set dead_flag", (char*)mName, mGameID, mCouncilID);
				return;
			}
			else
			{
				Council->add_member(this);
			}
		}
		else
		{
			mCouncilID = 0;
			mStoreFlag += STORE_COUNCIL_ID;
		}
		mCouncilVote = atoi(aRow[STORE_COUNCIL_VOTE]);
		mCouncilProduction = atoi(aRow[STORE_COUNCIL_PRODUCTION]);
		mCouncilDonation = atoi(aRow[STORE_COUNCIL_DONATION]);

		mSecurityLevel = atoi(aRow[STORE_SECURITY_LEVEL]);
		mAlertness = atoi(aRow[STORE_SECURITY_LEVEL]);
		mEmpireRelation = atoi(aRow[STORE_EMPIRE_RELATION]);

		mProtectedMode = (EProtectedMode)atoi(aRow[STORE_PROTECTED_MODE]);
		if (mProtectedMode == PROTECTED_PLANET_LOSS)
		{
			mProtectedTerminateTime = atoi(aRow[STORE_PROTECTED_TERMINATE_TIME]) + CGameStatus::get_down_time();
			mStoreFlag += STORE_PROTECTED_TERMINATE_TIME;
			type(QUERY_UPDATE);
			STORE_CENTER->store(*this);
		}
		else
		{
			mProtectedTerminateTime = atoi(aRow[STORE_PROTECTED_TERMINATE_TIME]);
		}

		mNewsCenter.set_turn(atoi(aRow[STORE_NEWS_TURN]));
		mNewsCenter.set_production(atoi(aRow[STORE_NEWS_PRODUCTION]));
		mNewsCenter.set_research(atoi(aRow[STORE_NEWS_RESEARCH]));
		mNewsCenter.set_population(atoi(aRow[STORE_NEWS_POPULATION]));

		mPlanetInvestPool = atoi(aRow[STORE_PLANET_INVEST_POOL]);
		mPlanetInvestPoolUsage = 0;
		mAdmissionTimeLimit = as_atoi(aRow[STORE_ADMISSION_TIME_LIMIT]);
		mPreference = NULL;

		mAdmiralList.parent = this;
		mAdmiralPool.parent = this;

		load_news();

		mControlModel.clear();

		if (mTick > CGame::mGameStartTime)
			CGame::mGameStartTime = mTick;

		type(QUERY_NONE);

		mPrev = NULL;
		mNext = NULL;

		refresh_nick();

		mTechList.set_owner(mGameID);
		mFirstLostPlanet = NULL;
		mLastLostPlanet = NULL;
		mLostPlanets = 0;

	/* For Empire */
		(void)mEmpireAdmiralInfoList;
		(void)mEmpireFleetInfoList;

		(void)mMagistrateList;
		(void)mFortressList;
		(void)mEmpireCapitalPlanet;

		(void)mBlackMarket;
	}
	catch(...)
	{
		SLOG("WARNING: Something was stupid here in player Loading");

	}
}

// contructor for new player
CPlayer::CPlayer(int aPortalID, int aGameID,
	const char *aName, int aRace, CCouncil* aCouncil)
{
	mNewsCenter.set_owner(this);
	mRelationList.set_owner(this);

	mPortalID = aPortalID;
	mGameID = aGameID;
	mName = aName;
	mLastLogin = time(0);
	set_race(aRace);
	mMode = MODE_BALANCED;
	mProtectedMode = PROTECTED_BEGINNER;
	mProtectedTerminateTime = PROTECTION_MODE_TURNS;
	mResearch = 0;
	mProduction = 0;
	mTargetTech = 0;
	mShipProduction = 0;
	mInvestedShipProduction = 0;
	mHonor = 50;
	mResearchInvest = 0;
	mTick = CGame::get_game_time() + CGame::mSecondPerTurn;
	mTurn = 0;
	mAdmiralTimer = 0;
	mHonorTimer = 0;

	mNick.clear();
	mNick.format("%s (#%d)", (char *)mName, mGameID);

	mCouncilID = aCouncil->get_id();
	mCouncilProduction = mCouncilDonation = mCouncilVote = 0;
	mPower = 0;
	mPreference = new CPreference(aGameID);

	mPlanetInvestPoolUsage = 0;
	mPlanetInvestPool = 0;
	if (aCouncil->add_member(this))
	{
		CPlayerList *
		MemberList = aCouncil->get_members();
		for (int i=0 ; i<MemberList->length() ; i++)
		{
			static CString
				News;
			News.clear();
			CPlayer *
			Member = (CPlayer *)MemberList->get(i);

			if (Member->get_game_id() == get_game_id())
			{
				News.format(GETTEXT("You have joined council %1$s."), aCouncil->get_nick());
				Member->time_news((char *)News);
			}
			else
			{
				News.format(GETTEXT("New player %1$s has joined our council."), get_nick());
				Member->time_news((char *)News);
			}
		}
	}

	mSecurityLevel = 1;
	mAlertness = 0;
	mEmpireRelation = mRace->get_empire_relation();

	mControlModel.clear();

	mNewsCenter.set_turn(0);
	mNewsCenter.set_production(0);
	mNewsCenter.set_research(0);
	mNewsCenter.set_population(0);
	mNewsCenter.set_ability(NULL);

	mAdmission = NULL;

	mAdmiralTimer = 0;
	mHonorTimer = 0;

	mLastTurnResource.clear();
	mNewResource.clear();
	mAdmiralList.parent = this;
	mAdmiralPool.parent = this;

	type(QUERY_INSERT);

	mPrev = NULL;
	mNext = NULL;
	mAdmissionTimeLimit = -1;
	mTechList.set_owner(mGameID);

	mFirstLostPlanet = NULL;
	mLastLostPlanet = NULL;
	mLostPlanets = 0;


	/* For Empire */
	(void)mEmpireAdmiralInfoList;
	(void)mEmpireFleetInfoList;

	(void)mMagistrateList;
	(void)mFortressList;
	(void)mEmpireCapitalPlanet;

	(void)mBlackMarket;
}

CPlayer::CPlayer(int aGameID)
{
	mGameID = aGameID;
	mPortalID = 0;
	mTick = 0;
	mTurn = 0;
	mLastLogin = 0;
	mLastLoginIP = "0.0.0.0";
	mRace = NULL;
	mTargetTech = 0;
	mResearch = 0;
	mShipProduction = 0;
	mInvestedShipProduction = 0;
	mProduction = 0;
	mPopulationIncreased = 0;
	mHonor = 0;
	mResearchInvest = 0;
	mAdmiralTimer = 0;
	mHonorTimer = 0;
	mName = GETTEXT("The Empire");
	mNick.clear();
	mNick.format("%s (#%s)", (char *)mName, dec2unit(mGameID));
	(void)mAbility;
	(void)mClusterList;
	(void)mPlanetList;
	(void)mProjectCM;
	(void)mTechCM;
	(void)mControlModel;
	(void)mLastTurnResource;
	(void)mNewResource;
	(void)mTechList;
	(void)mAvailableTechList;
	(void)mPurchasedProjectList;
	(void)mProjectList;
	(void)mAdmiralList;
	(void)mAdmiralPool;
	(void)mComponentList;
	(void)mShipDesignList;
	(void)mShipBuildQ;
	(void)mDock;
	(void)mRepairBay;
	(void)mActionList;
	(void)mRelationList;
	(void)mDefensePlanList;
	(void)mMessageBox;
	mCouncilID = 0;
	mHomeClusterID = 0;
	mAdmission = NULL;
	mCouncilProduction = 0;
	mCouncilDonation = 0;
	mCouncilVoteGain = 0;
	mCouncilVote = 0;
	mSecurityLevel = 0;
	mAlertness = 0;
	mEmpireRelation = 0;
	(void)mEmpireActionList;
	mPower = 0;
	(void)mNewsCenter;
	(void)mFleetList;
	(void)mAllyFleetList;
	mMode = (EConcentrationMode)0;
	mProtectedMode = (EProtectedMode)0;
	mProtectedTerminateTime = 0;
	mPrev = NULL;
	mNext = NULL;
	mPreference = NULL;
	mPlanetInvestPoolUsage = 0;
	mPlanetInvestPool = 0;
	(void)mEffectList;
	(void)mEventList;
	mAdmissionTimeLimit = -1;
	mAdmiralList.parent = this;
	mAdmiralPool.parent = this;

	mFirstLostPlanet = NULL;
	mLastLostPlanet = NULL;
	mLostPlanets = 0;


	/* For Empire */
	(void)mEmpireAdmiralInfoList;
	(void)mEmpireFleetInfoList;

	(void)mMagistrateList;
	(void)mFortressList;
	(void)mEmpireCapitalPlanet;

	(void)mBlackMarket;
}

CPlayer::~CPlayer()
{
	mRace = NULL;
	mCouncilID = 0;
	mAdmission = NULL;
	mPreference = NULL;
}


CString&
CPlayer::query()
{
	static CString
		Query;

	Query.clear();

	switch(CStore::type())
	{
		case QUERY_INSERT :
			/*Query.format("INSERT INTO player"
				" (game_id, portal_id, name, home_cluster_id, last_login,"
				" mode, race, honor, research_invest, production, council_id, tick,"
				" ability, protected_mode, protected_terminate_time, empire_relation)"
				" VALUES (%d, %d, '%s', %d, %d,"
				" %d, %d, %d, %d, %d, %d, %d,"
				" '%s', %d, %d, %d)",         */
            Query.format("INSERT INTO player"
                         " set game_id=%d, portal_id=%d, name='%s', home_cluster_id=%d, last_login=%d,"
                         " mode=%d, race=%d, honor=%d, research_invest=%d, production=%d, council_id=%d, tick=%d,"
                         " ability='%s', protected_mode=%d, protected_terminate_time=%d, empire_relation=%d",
				mGameID,
				mPortalID,
				(char*)add_slashes((char *)mName),
				mHomeClusterID,
				mLastLogin,
//						(char *)mLastLoginIP,
				mMode,
				get_race(),
				mHonor,
				mResearchInvest,
				mProduction,
				get_council_id(),
				get_tick(),
				mAbility.get_string(ABILITY_MAX),
				mProtectedMode,
				mProtectedTerminateTime,
				mEmpireRelation);
			break;

		case QUERY_UPDATE :
			Query.format("UPDATE player SET turn = %d, admiral_timer = %d, honor_timer = %d",
				mTurn, mAdmiralTimer, mHonorTimer);

			#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_LAST_LOGIN, ", last_login = %d", mLastLogin);

			if (mLastLoginIP.length() > 0)
			{
				STORE(STORE_LAST_LOGIN_IP, ", last_login_ip = '%s'",
					(char *)add_slashes((char *)mLastLoginIP));
			}

			STORE(STORE_MODE, ", mode = %d", mMode);

			STORE(STORE_PRODUCTION, ", production = %d", mProduction);

			STORE(STORE_SHIP_PRODUCTION, ", ship_production = %d",
				mShipProduction);

			STORE(STORE_INVESTED_SHIP_PRODUCTION,
				", invested_ship_production = %d",
				mInvestedShipProduction);

			STORE(STORE_HONOR, ", honor = %d", mHonor);

			STORE(STORE_RESEARCH_INVEST, ", research_invest = %d",
				mResearchInvest);

			STORE(STORE_TICK, ", tick = %d", mTick);

			STORE(STORE_RESEARCH, ", research = %d", mResearch);

			STORE(STORE_ABILITY, ", ability = '%s'",
				mAbility.get_string(ABILITY_MAX));

			STORE(STORE_TARGET_TECH, ", research_tech = %d", mTargetTech);

			STORE(STORE_COUNCIL_ID, ", council_id = %d", mCouncilID);

			STORE(STORE_COUNCIL_VOTE, ", council_vote = %d",
				mCouncilVote);

			STORE(STORE_COUNCIL_PRODUCTION,
				", council_production = %d", mCouncilProduction );

			STORE(STORE_COUNCIL_DONATION,
				", council_donation = %d", mCouncilDonation );

			STORE(STORE_SECURITY_LEVEL,
				", security_level = %d", mSecurityLevel );
			STORE(STORE_ALERTNESS,
				", alertness = %d", mAlertness );
			STORE(STORE_EMPIRE_RELATION,
				", empire_relation = %d", mEmpireRelation );

			STORE(STORE_PROTECTED_MODE,
				", protected_mode = %d", (int)mProtectedMode );

			STORE(STORE_PROTECTED_TERMINATE_TIME,
				", protected_terminate_time = %d",
				mProtectedTerminateTime);

			STORE(STORE_PLANET_INVEST_POOL,
				", planet_invest_pool = %d",
				mPlanetInvestPool);

			STORE(STORE_HOME_CLUSTER_ID,
				", home_cluster_id = %d",
				mHomeClusterID);

			Query.format(", last_turn_production = %d",
				mLastTurnResource.get(RESOURCE_PRODUCTION));

			Query.format(", last_turn_research = %d",
				mLastTurnResource.get(RESOURCE_RESEARCH));

			Query.format(", last_turn_military = %d",
				mLastTurnResource.get(RESOURCE_MILITARY));

			Query += mNewsCenter.get_query();

			Query.format(" WHERE game_id = %d", mGameID);

			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM player WHERE game_id = %d",
				mGameID );
			break;
	}

	mStoreFlag.clear();

	return Query;
}

bool
	CPlayer::refresh_nick()
{
	mNick.clear();
	if (get_court_rank() > CR_NONE)
	{
		CCluster *
		Cluster = UNIVERSE->get_by_id(mHomeClusterID);
		if (Cluster == NULL)
		{
			mNick.format( "%s the %s (#%d)", (char *)mName, get_court_rank_name(), mGameID );
		}
		else
		{
			mNick.format( "%s the %s of %s (#%d)", (char *)mName, get_court_rank_name(), Cluster->get_name(), mGameID );
		}
	}
	else
	{
		mNick.format("%s (#%d)", (char *)mName, mGameID);
	}

	return true;
}

int
	CPlayer::get_race()
{
	return mRace->get_id();
}

char *
	CPlayer::get_race_name()
{
	return mRace->get_name();
}

// Todo: make it remove all planets possibly? give them to the empire?
void
	CPlayer::set_dead(char *aReasonString)
{
	if (is_dead() == true) return;

	mStoreFlag += STORE_ABILITY;
	mAbility += ABILITY_DEAD;

	if (mAdmission != NULL)
	{
		mAdmission->type(QUERY_DELETE);
		STORE_CENTER->store(*mAdmission);

		CCouncil *
		Council = mAdmission->get_council();
		Council->remove_admission(this);

		mAdmission = NULL;
	}

	if (mCouncilID > 0)
	{
		CCouncil *
		Council = get_council();
		if (Council != NULL)
		{
			Council->remove_admission(this);
			Council->remove_member(get_game_id());
			if (Council->get_number_of_members() == 0)
			{
				COUNCIL_TABLE->remove_council(Council->get_id());
			}
		}

		mCouncilID = 0;
	}

	for (int i=mShipBuildQ.length()-1 ; i>=0 ; i--)
	{
		CShipToBuild *
		Ship = (CShipToBuild *)mShipBuildQ.get(i);

		Ship->type(QUERY_DELETE);
		STORE_CENTER->store(*Ship);

		mShipBuildQ.remove_ship_to_build(Ship);
	}

	for (int i=mDock.length()-1 ; i>=0 ; i--)
	{
		CDockedShip *
		Ship = (CDockedShip *)mDock.get(i);

		Ship->type(QUERY_DELETE);
		STORE_CENTER->store(*Ship);

		mDock.remove_docked_ship(Ship);
	}

	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)mFleetList.get(i);

		Fleet->end_mission();

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	for (int i=mAllyFleetList.length()-1 ; i>=0 ; i--)
	{
		CFleet *
		Fleet = (CFleet *)mAllyFleetList.get(i);
		Fleet->end_mission();

		CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(Fleet->get_owner());
		Owner->time_news(
			(char *)format(GETTEXT("The fleet %1$s is returning from your ally %2$s cause your ally is dead."),
				Fleet->get_nick(), Owner->get_nick()));

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	for (int i=0 ; i<EMPIRE_PLANET_INFO_LIST->length() ; i++)
	{
		CEmpirePlanetInfo *
		EmpirePlanetInfo = (CEmpirePlanetInfo *)EMPIRE_PLANET_INFO_LIST->get(i);
		if (EmpirePlanetInfo->get_owner_id() == mGameID)
		{
			EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);

			EmpirePlanetInfo->type(QUERY_UPDATE);
			STORE_CENTER->store(*EmpirePlanetInfo);

			CPlanet *
			Planet = PLANET_TABLE->get_by_id(EmpirePlanetInfo->get_planet_id());
			if (Planet != NULL)
			{
				Planet->set_owner(EMPIRE);

				Planet->type(QUERY_UPDATE);
				STORE_CENTER->store(*Planet);
			}
		}
	}

	for (int i=0 ; i<FORTRESS_LIST->length() ; i++)
	{
		CFortress *
		Fortress = (CFortress *)FORTRESS_LIST->get(i);
		if (Fortress->get_owner_id() == mGameID)
		{
			Fortress->set_owner_id(EMPIRE_GAME_ID);

			CFleetList *
			FleetList = Fortress->get_fleet_list();
			for (int j=0 ; j<FleetList->length() ; j++)
			{
				CFleet *
				Fleet = (CFleet *)FleetList->get(j);
				Fleet->set_current_ship(0);

				Fleet->type(QUERY_UPDATE);
				STORE_CENTER->store(*Fleet);
			}

			Fortress->type(QUERY_UPDATE);
			STORE_CENTER->store(*Fortress);
		}
	}

	PLAYER_TABLE->remove_player_rank(mGameID);

	if (BOUNTY_TABLE)
	{


		//BOUNTY_TABLE->remove_by_target_id(get_game_id());
		//OFFERED_BOUNTY_TABLE->remove_bounty_on_player(get_game_id());
		//mBountyList.remove_bounties();
		remove_relation_all();
	}

	if (aReasonString)
	{
		ADMIN_TOOL->add_dead_player_log(aReasonString);
	}
}
// end telecard 2001/01/29

bool
	CPlayer::set_commerce_between(CPlayer *aPlayer1, CPlayer *aPlayer2)
{
	aPlayer1->set_commerce_with(aPlayer2);
	aPlayer2->set_commerce_with(aPlayer1);

	return true;
}

bool
	CPlayer::clear_commerce_between(CPlayer *aPlayer1, CPlayer *aPlayer2)
{
	aPlayer1->clear_commerce_with(aPlayer2);
	aPlayer2->clear_commerce_with(aPlayer1);

	return true;
}

const char *
	CPlayer::get_mode_name()
{
	return get_mode_name(mMode);
}

const char*
	CPlayer::get_mode_name(int aMode)
{
	switch(aMode)
	{
		case MODE_BALANCED:
			return GETTEXT("Balanced");
		case MODE_INDUSTRY:
			return GETTEXT("Industry");
		case MODE_MILITARY:
			return GETTEXT("Military");
		case MODE_RESEARCH:
			return GETTEXT("Research");
	}
	return NULL;
}

CCouncil *
CPlayer::get_council()
{
	CCouncil *
	Council = COUNCIL_TABLE->get_by_id(mCouncilID);
	return Council;
}

int
	CPlayer::calc_population()
{
	return mPlanetList.total_population();
}

char *
	CPlayer::calc_population_with_unit()
{
	static CString
		PopulationString;
	PopulationString.clear();

	int
		Population = calc_population();

	if (Population < 1000)
	{
		PopulationString.format("%s k", dec2unit(Population));
		return (char *)PopulationString;
	} else
	{
		PopulationString.format("%s M", dec2unit(Population/1000));
		return (char *)PopulationString;
	}
}

int
	CPlayer::get_research_lab()
{
	int
		ResearchLab = 0;

	for(int i=0; i<mPlanetList.length(); i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);
		if (MAX_RESOURCE - ResearchLab < Planet->get_building().get(BUILDING_RESEARCH_LAB))
		{
			return MAX_RESOURCE;
		}
		ResearchLab += Planet->get_building().get(BUILDING_RESEARCH_LAB);
	}

	return ResearchLab;
}

bool
	CPlayer::set_production(int aProduction)
{
	safe_set_type<int,MAX_PLAYER_PP>(aProduction, mProduction);
	mStoreFlag += STORE_PRODUCTION;
	return true;
}

void
	CPlayer::set_protected_mode(CPlayer::EProtectedMode aMode)
{
	mStoreFlag += STORE_PROTECTED_MODE;
	mProtectedMode = aMode;
	switch(aMode)
	{
		case PROTECTED_PLANET_LOSS:
			mProtectedTerminateTime = CGame::get_game_time() + CPlayer::mProtectionPlanetLossTime;
			break;

#if defined(PROTECTION_24HOURS)
		case PROTECTED_24HOURS:
			mProtectedTerminateTime = CGame::get_game_time()
				+TURNS_PER_24HOURS*CGame::mSecondPerTurn;
			break;
#endif
		case PROTECTED_NONE:
			mProtectedTerminateTime = 0;
			break;
#if defined(PROTECTION_BEGINNER)
		case PROTECTED_BEGINNER:
			mProtectedTerminateTime = PROTECTION_MODE_TURNS;
			break;
#endif
	}
}

bool
	CPlayer::add_planet(CPlanet *aPlanet)
{
	aPlanet->set_owner(this);
	aPlanet->set_order(mPlanetList.length());

	CCluster *
	Cluster = aPlanet->get_cluster();

	if (add_cluster(Cluster))
	{
		Cluster->add_player(this);
		CCouncil *
		Council = get_council();
		Council->add_cluster(Cluster);
	}

	mPlanetList.add_planet(aPlanet);
	aPlanet->set_destroy_timer(0);

	return true;
}

bool
	CPlayer::add_planet_when_loading(CPlanet *aPlanet)
{
	aPlanet->set_owner(this);

	CCluster* Cluster = aPlanet->get_cluster();

//	SLOG("add planet, cluster name %s", Cluster->get_name());
	if (add_cluster(Cluster))
	{
		Cluster->add_player(this);
//		SLOG("add planet, council name %s", mCouncil->get_name());
		if (is_dead() == false)
		{
			CCouncil *
			Council = get_council();
			if (Council != NULL)
			{
				Council->add_cluster(Cluster);
			}
		}
	}

	return mPlanetList.add_planet(aPlanet);
}

bool
	CPlayer::drop_planet(CPlanet *aPlanet)
{
	CCluster *
	Cluster = aPlanet->get_cluster();
	if (!mPlanetList.remove_planet(aPlanet->get_id()))
	{
		SLOG("Could not drop planet(%d) on player(%d)",
			aPlanet->get_id(), get_game_id());
		return false;
	}

	if (remove_cluster(Cluster->get_id()))
	{
		Cluster->remove_player(get_game_id());

		CCouncil *
		Council = get_council();
		if (Council != NULL)
		{
			Council->remove_cluster(Cluster->get_id());
		}
	}

	//remove commerce
	aPlanet->clear_commerce_all();

	CFleet
		*Fleet;
	while( (Fleet = get_fleet_list()->get_fleet_station_on_planet(aPlanet->get_id())) ){
		Fleet->end_mission();
	}

	aPlanet->set_owner_id(0);
	aPlanet->set_order(-1);

	aPlanet->type(QUERY_UPDATE);
	STORE_CENTER->store(*aPlanet);

	return true;
}

bool
	CPlayer::has_cluster(int aClusterID)
{
	int
		Index = mClusterList.find_sorted_key((TSomething)aClusterID);

	if (Index < 0) return false;

	return true;
}

bool
	CPlayer::add_cluster(CCluster* aCluster)
{
	if (mClusterList.add_cluster(aCluster) < 0)
		return false;
	return true;
}

bool
	CPlayer::remove_cluster(int aClusterID)
{
	for (int i=0; i<mPlanetList.length(); i++)
	{
		CPlanet* Planet = (CPlanet*)mPlanetList.get(i);

		if (Planet->get_cluster_id() == aClusterID) return false;
	}

	mClusterList.remove_cluster(aClusterID);
	return true;
}

void
	CPlayer::rearrange_cluster()
{
	bool
		Dirty = false;

	// regathering clusters
	mClusterList.remove_all();
	for (int i=0 ; i<mPlanetList.length() ; i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);
		mClusterList.add_cluster(Planet->get_cluster());
	}

	// update Cluster
	for (int i=0 ; i<UNIVERSE->length() ; i++)
	{
		CCluster *
		Cluster = (CCluster *)UNIVERSE->get(i);
		if (Cluster->find_player(mGameID) != NULL)
		{
			if (mClusterList.get_by_id(Cluster->get_id()) == NULL)
			{
				Cluster->remove_player(mGameID);
				Dirty = true;
			}
		}
	}

	if (Dirty == true)
	{
		CCouncil *
		Council = get_council();
		if (Council != NULL) Council->rearrange_cluster();
	}
}

void
	CPlayer::add_cluster_list(CClusterList &aClusterList)
{
	for (int i=0 ; i<mClusterList.length() ; i++)
	{
		aClusterList.add_cluster((CCluster *)mClusterList.get(i));
	}
}

void
	CPlayer::build_control_model()
{
	mControlModel.clear();

	// add control by race
	mControlModel += mRace->get_control_model();

	// add control by tech
	mControlModel += mTechCM;

	// add control by project
	mControlModel += mProjectCM;

	// add control by council project
	if (get_council() != NULL)
	{
		mControlModel += get_council()->get_control_model();
	}

	// add control by invest
	// research invest
	if (get_research_invest() > 0)
	{
		int
			MaxPerTurn = get_research_lab();

		if (get_research_invest() >= MaxPerTurn)
			mControlModel.change_research(3);
		else
		{
			int Ratio = (int)(get_research_invest()*100/MaxPerTurn);
			if (Ratio > 66) mControlModel.change_research(2);
			else if (Ratio > 33) mControlModel.change_research(1);
		}
	}

	bool bPlayerHasPAMSC = false, bPlayerHasPAAR = false, bPlayerHasPAMS = false, bPlayerHasPACA = false, bPlayerHasPAATL = false;

	// add control modifiers for +global CM planet attributes.
	for(int i=0; i<mPlanetList.length(); i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);

		if (Planet->has_attribute(CPlanet::PA_MAJOR_SPACE_CROSSROUTE))
		{
			bPlayerHasPAMSC = true;
		}
		if (Planet->has_attribute(CPlanet::PA_ANCIENT_RUINS))
		{
			bPlayerHasPAAR = true;
		}
		if (Planet->has_attribute(CPlanet::PA_MILITARY_STRONGHOLD))
		{
			bPlayerHasPAMS = true;
		}
		if (Planet->has_attribute(CPlanet::PA_COGNITION_AMPLIFIER))
		{
			bPlayerHasPACA = true;
		}
		if (Planet->has_attribute(CPlanet::PA_LOST_TRABOTULIN_LIBRARY))
		{
			bPlayerHasPAATL = true;
		}
	}

	if (bPlayerHasPAMSC)
		mControlModel.change_commerce(2);
	if (bPlayerHasPAAR)
		mControlModel.change_genius(2);
	if (bPlayerHasPAMS)
		mControlModel.change_military(1);
	/*if (bPlayerHasPACA)
		mControlModel.change_production(2); */
	if (bPlayerHasPAATL)
	{
		mControlModel.change_facility_cost(1);
		mControlModel.change_research(2);
		mControlModel.change_military(1);
	}
	
	// Cumulative Global Bonuses
//	get_control_model()->change_efficiency(get_planet_list()->count_planet_with_attribute(CPlanet::PA_STABLE_WORMHOLE));
//	get_control_model()->change_diplomacy(get_planet_list()->count_planet_with_attribute(CPlanet::PA_DIPLOMATIC_OUTPOST));
//	get_control_model()->change_production(get_planet_list()->count_planet_with_attribute(CPlanet::PA_COGNITION_AMPLIFIER));
//    get_control_model()->change_research((int) (get_planet_list()->count_planet_with_attribute(CPlanet::PA_ARTIFACT) / 4));
//    get_control_model()->change_research(get_planet_list()->count_planet_with_attribute(CPlanet::PA_MASSIVE_ARTIFACT));

	// add control by concentration mode
	switch(mMode)
	{
		case MODE_INDUSTRY :
			mControlModel.change_production(3);
			mControlModel.change_military(-2);
			mControlModel.change_research(-1);
			break;

		case MODE_MILITARY :
			mControlModel.change_production(-2);
			mControlModel.change_military(5);
			mControlModel.change_research(-2);
			break;

		case MODE_RESEARCH :
			mControlModel.change_production(-2);
			mControlModel.change_military(-2);
			mControlModel.change_research(3);
			break;

		case MODE_BALANCED :
		default :
			break;
	}

	// add council relation
	if (get_council() != NULL)
	{
		if (get_council()->check_total_war())
			mControlModel.change_commerce(-2);
		else if (get_council()->check_war())
			mControlModel.change_commerce(-1);
	}

	for (int i=0; i < mFleetList.length(); i++)
	{
		CFleet *
		Fleet = (CFleet*)mFleetList.get(i);

		if (!Fleet->under_mission()) continue;

		if (Fleet->get_mission().get_mission() != CMission::MISSION_EXPEDITION)
			continue;

		CAdmiral *
		Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());
		if (!Admiral) continue;

		if( Admiral->get_racial_ability() != CAdmiral::RA_COMMERCE_KING )
			continue;

		if( Admiral->get_level() <= 10 )
			mControlModel.change_commerce(1);
		else if( Admiral->get_level() <= 19 )
			mControlModel.change_commerce(2);
		else if( Admiral->get_level() == 20 )
			mControlModel.change_commerce(3);

		break;
	}

	for( int i = 0; i < mEffectList.length(); i++ ){
		CPlayerEffect
			*Effect = (CPlayerEffect*)mEffectList.get(i);
		if( Effect->get_type() != CPlayerEffect::PA_CHANGE_CONTROL_MODEL ) continue;
		switch( Effect->get_target() ){
			case CControlModel::CM_ENVIRONMENT :
				mControlModel.change_environment(Effect->get_argument1());
				break;
			case CControlModel::CM_GROWTH :
				mControlModel.change_growth(Effect->get_argument1());
				break;
			case CControlModel::CM_RESEARCH :
				mControlModel.change_research(Effect->get_argument1());
				break;
			case CControlModel::CM_PRODUCTION :
				mControlModel.change_production(Effect->get_argument1());
				break;
			case CControlModel::CM_MILITARY :
				mControlModel.change_military(Effect->get_argument1());
				break;
			case CControlModel::CM_SPY :
				mControlModel.change_spy(Effect->get_argument1());
				break;
			case CControlModel::CM_COMMERCE :
				mControlModel.change_commerce(Effect->get_argument1());
				break;
			case CControlModel::CM_EFFICIENCY :
				mControlModel.change_efficiency(Effect->get_argument1());
				break;
			case CControlModel::CM_GENIUS :
				mControlModel.change_genius(Effect->get_argument1());
				break;
			case CControlModel::CM_DIPLOMACY :
				mControlModel.change_diplomacy(Effect->get_argument1());
				break;
			case CControlModel::CM_FACILITY_COST :
				mControlModel.change_facility_cost(Effect->get_argument1());
				break;
			default :
				SLOG( "Effect Wrong Control Model %d - %d/%d", Effect->get_target(), Effect->get_source_type(), Effect->get_source() );
				break;
		}
	}
}

/**
 * Change the CPlayer's mProduction (+/-)
 * @param aProduction integer to add to mProduction
 */
bool
	CPlayer::change_reserved_production(int aProduction)
{
	mStoreFlag += STORE_PRODUCTION;
	safe_fast_change_type<int,MAX_PLAYER_PP>(aProduction,mProduction);
	return true;
}

/**
 * Set the CPlayer's mShipProduction
 * @param aProduction integer that mShipProduction will become
 */
bool
	CPlayer::set_ship_production(int aProduction)
{
	mStoreFlag += STORE_SHIP_PRODUCTION;
	safe_set_type<int,MAX_SHIP_PRODUCTION>(aProduction, mShipProduction);
	return true;
}

/**
 * Change the CPlayer's mShipProduction (+/-)
 * @param aProduction integer to add to mShipProduction
 */
bool
	CPlayer::change_ship_production(int aProduction)
{

	mStoreFlag += STORE_SHIP_PRODUCTION;
	safe_fast_change_type<int,MAX_SHIP_PRODUCTION>(aProduction,mShipProduction);
	return true;
}

/**
 * Change the CPlayer's mInvestedShipProduction (+/-)
 * @param aProduction integer to add to mInvestedShipProduction
 */
bool
	CPlayer::change_invested_ship_production(int aProduction)
{

	mStoreFlag += STORE_INVESTED_SHIP_PRODUCTION;
	safe_fast_change_type<int,MAX_INVESTED_SHIP_PP>(aProduction,mInvestedShipProduction);
	return true;
}

bool
	CPlayer::change_research(int aResearch)
{
	mStoreFlag += STORE_RESEARCH;
	safe_fast_change_type<int,MAX_RESEARCH>(aResearch,mResearch);
	return true;
}

// Returns the RP cost required by the Player
// to research tech aTech
int CPlayer::get_research_cost(CTech *aTech) {
    
    int ResearchCost = 0;
    // If the base cost is more than 2b, set it to 2b
    if ((int) pow((double)2.0, (double)(aTech->get_level())) > MAX_RESEARCH/20000) {
        ResearchCost = MAX_RESEARCH;
    } else {
        ResearchCost = 20000 * (int)pow((double)2.0, (double)(aTech->get_level()));
    }

	// tech level is never less than 0 -- this should never be less than 0
	if (ResearchCost < 0)
	{
		SLOG("ResearchCost is negative in CPlayer::get_research_cost - changing value");
		ResearchCost = MAX_RESEARCH;
	}
	
	// Level 18s and above cost 2b on whatever mode
	if (aTech->get_level() >= 18) {
        return MAX_RESEARCH;
    }

	// player RP cost discount -- max 40 (pays 60% of total cost)
	int Discount = 0;
	if (!mTargetTech) Discount = 20 + mControlModel.get_research()*4;
	if (Discount < 0) Discount = 0;
	if (Discount > 40) Discount = 40;

	// real cost with discount
	int RealResearchCost = (int)(ResearchCost * ((100 - Discount)/100.0));
	//SLOG("RESEARCH COST: %d TOTAL COST: %d", ResearchCost, RealResearchCost);

	RealResearchCost = (int)(RealResearchCost/GAME->mTechRate);


	// don't allow a value over MAX_RESEARCH or under 0 (overflow protection)
	if (RealResearchCost > MAX_RESEARCH || RealResearchCost < 0)
	{
		SLOG("Bad research cost - Tech Rate: %d", GAME->mTechRate);
		return (int)MAX_RESEARCH;
	}
	return (int)RealResearchCost;
}


void
	CPlayer::update_turn()
{
	// build control model
	// invest effect
	// do planet update turn and correct building upkeep and resources
	// pay building upkeep
	// pay fleet upkeep
	// process research
	// process fleet building
	// process reserved production
	// check protected mode
//    SLOG("CPlayer:: update_turn() start");
	if (mGameID == EMPIRE_GAME_ID)
	{
		mEmpireAdmiralInfoList.update_DB();
		mEmpireFleetInfoList.update_DB();
		mEmpirePlanetInfoList.update_DB();

		mEmpirePlanetList.update_DB();

		mMagistrateList.update_DB();
		mFortressList.update_DB();
		mEmpireCapitalPlanet.update_DB();

		/* Last Game Time */
		SLOG("down_time_update: %d", CGameStatus::get_down_time());
		CGameStatus::mLastGameTime = time(0); //now
		CGameStatus * gstat = new CGameStatus();
		gstat->type(QUERY_UPDATE);
		STORE_CENTER->store(*gstat);
		delete gstat;

		return;
	}
	else
	{
		if(mNewsCenter.isViewed())
		{
			mNewsCenter.clear_news();
		}
	}
	
	//refresh_nick();

    // todo: this was crashing with just if(mBounty1)...pointers are NULL if unassigned
//	mBountyList.expire_bounties();

	CPlayerEffect *
	SkipTurnEffect = mEffectList.get_by_type(CPlayerEffect::PA_SKIP_TURN);
	if (SkipTurnEffect != NULL)
	{
		if (SkipTurnEffect->get_life() < CGame::get_game_time())
		{
			SkipTurnEffect->type(QUERY_DELETE);
			STORE_CENTER->store(*SkipTurnEffect);

			mEffectList.remove_player_effect(SkipTurnEffect);
		}
	}

	if (has_effect(CPlayerEffect::PA_SKIP_TURN))
	{
		SLOG("%s is under the effect of SKIP TURN", (char *)mNick);
		return;
	}

	expire_event();
	upload_project_effect();

	for (int i=0 ; i<mAdmiralPool.length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)mAdmiralPool.get(i);
		Admiral->clear_level_by_effect();
	}
	for (int i=0 ; i<mAdmiralList.length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)mAdmiralList.get(i);
		Admiral->clear_level_by_effect();
	}

	CString
		News;
	for (int i=0 ; i<mEffectList.length() ; i++)
	{
		CPlayerEffect *
		Effect = (CPlayerEffect *)mEffectList.get(i);
		if (Effect->get_type() == CPlayerEffect::PA_IMPERIAL_RETRIBUTION)
		{
			SLOG("CPlayer::update_turn() -- PA_IMPERIAL_RETRIBUTION (skipping)");
			continue;
		}
		else
		{
			News += apply_effect(Effect);
		}
	}
	if (News.length() > 0) time_news((char *)News);

	const char *
		MissionNews = mission_handler();
	if (MissionNews != NULL) time_news(MissionNews);

	if (get_research_invest() > 0)
	{
		long long int
			Invest;
		if (mAbility.has(ABILITY_EFFICIENT_INVESTMENT))
		{
			Invest = (long long int)get_research_lab() * 2;
		}
		else
		{
			Invest = (long long int)get_research_lab();
		}

		int
			RP = 0;

		if ((long long int)mResearchInvest < Invest)
		{
			Invest = (long long int)mResearchInvest;
		}
		change_research_invest(-(int)Invest);
		RP = (int)Invest / 20;

		change_research(RP);
	}

	mNewResource.clear();

	long long int
		Upkeep = 0;
	int
		oldPopulation = calc_population();

	set_planet_invest_pool_usage(0);
	int planet_invest_pool_change = mPlanetInvestPool;
	for (int i=0 ; i<mPlanetList.length() ; i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);
		if (Planet->update_destroy_timer() == true)
		{
			drop_planet(Planet);
			Planet->type(QUERY_DELETE);
			STORE_CENTER->store(*Planet);
			delete(Planet);
			i--;
		}
		else
		{
			if (Planet->is_paralyzed() == true) continue;

			Planet->update_turn();

			Upkeep += (long long int)Planet->get_upkeep_per_turn() / 10;
			mNewResource += Planet->get_new_resource();
		}
	}

	if (planet_invest_pool_change != mPlanetInvestPool) {
		mStoreFlag += STORE_PLANET_INVEST_POOL;
	}
	int
		currentPopulation = calc_population();
	build_control_model();

	/*int
		MPFromFortress = 0;
	for (int i=0 ; i<FORTRESS_LIST->length() ; i++)
	{
		CFortress *
		Fortress = (CFortress *)FORTRESS_LIST->get(i);
		if (Fortress->get_owner_id() == mGameID) MPFromFortress += 100000;
	}
	mNewResource.change(RESOURCE_MILITARY, MPFromFortress);*/

	mPopulationIncreased = currentPopulation - oldPopulation;	//telecard 2001/04/02

	get_extra_income_by_effect();

	mLastTurnResource = mNewResource;

	pay_extra_cost_by_effect();

	change_research(mNewResource.get(RESOURCE_RESEARCH));

	process_imperial_retribution();

	long long int
		Balance = (long long int)mNewResource.get(RESOURCE_PRODUCTION);
	int
		SecurityUpkeep = update_security(mNewResource.get(RESOURCE_PRODUCTION));
	Balance -= (long long int)SecurityUpkeep;

	Balance -= Upkeep;

	Balance -= (long long int)calc_ship_production();
	change_ship_production(calc_real_ship_production());

	build_ship();

	int
		FleetUpkeep = pay_fleet_upkeep(Balance);
	Balance -= (long long int)FleetUpkeep;

	repair_damaged_ship();

	if (Balance > 100)
	{
		change_council_production((int)(Balance * 5/100));
		change_reserved_production(-(int)(Balance * 5/100));
	}

/*	if  ((get_race() ==1) && (has_project(9001)))

	{
		int HumanEndingBonus = (int)((double)mProduction * 0.05);
		 if (HumanEndingBonus >= 2000000) HumanEndingBonus = 2000000;
		change_reserved_production(HumanEndingBonus);
	}*/

	change_reserved_production((int)Balance);

	update_tech();

	spend_turn();

	update_admiral_pool();

	mHonorTimer++;
	if (mHonorTimer >= CPlayer::mHonorIncreaseTurns) {
		change_honor(1);
		mHonorTimer = 0;
	}

	// update admiral
	for (int i=0 ; i<mAdmiralList.length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)mAdmiralList.get(i);

		Admiral->type(QUERY_UPDATE);
		STORE_CENTER->store(*Admiral);
	}

	for (int i=0 ; i<mAdmiralPool.length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)mAdmiralPool.get(i);

		Admiral->type(QUERY_UPDATE);
		STORE_CENTER->store(*Admiral);
	}

	// update fleet
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)mFleetList.get(i);

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	for (int i=0 ; i<mPlanetList.length() ; i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);

		Planet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Planet);
	}

	mEffectList.update_turn();
	mEmpireActionList.flush_db();
	if (is_protected() && time(0) - mLastLostPlanet->time <= CGame::mSecondPerTurn)
	{
		time_news(GETTEXT("Imperial Fleets are now protecting you for 12 hours due to recent planet losses."));
	}
	else if (is_protected() && time(0) - mLastLostPlanet->time >= CPlayer::mProtectionPlanetLossTime - CGame::mSecondPerTurn)
	{
		time_news(GETTEXT("The Imperial Fleets that were defending your territory have left."));
	}
	if (mProtectedMode == PROTECTED_PLANET_LOSS)
	{
		if (mProtectedTerminateTime <= CGame::get_game_time())
		{
			set_protected_mode(PROTECTED_NONE);
		}
	}

#ifdef PROTECTION_BEGINNER
	if (mProtectedMode == PROTECTED_BEGINNER)
	{
		if(mProtectedTerminateTime < mTurn)
		{
			set_protected_mode(PROTECTED_NONE);
			time_news(GETTEXT("The Imperial Fleets that were defending your territory have left."));
		}
		else if(mPlanetList.length() >= PROTECTION_MODE_PLANET_LIMIT)
		{
			set_protected_mode(PROTECTED_NONE);
			time_news(GETTEXT("The Imperial Fleets that were defending your territory have left."));
		}
	}

#endif
#if defined(PROTECTION_BEGINNER) && defined(PROTECTION_24HOURS)
	else if (mProtectedMode == PROTECTED_24HOURS)
#endif
#if defined(PROTECTION_24HOURS) && !defined(PROTECTION_BEGINNER)
		if (mProtectedMode == PROTECTED_24HOURS)
#endif
#ifdef PROTECTION_24HOURS
		{
			if (mProtectedTerminateTime > CGame::get_game_time())
			{
				set_protected_mode(PROTECTED_NONE);
			}

		}
#endif
}

void
	CPlayer::get_extra_income_by_effect()
{
	if (has_effect(CPlayerEffect::PA_PRODUCE_MP_PER_TURN))
	{
		int
			OldMP = mNewResource.get(RESOURCE_MILITARY);
		int
			NewMP = calc_PA(OldMP, CPlayerEffect::PA_PRODUCE_MP_PER_TURN);
		//SLOG("Boosting MP of player by %d", NewMP-OldMP);
		mNewResource.set(RESOURCE_MILITARY, NewMP);
	}
	if (has_effect(CPlayerEffect::PA_PRODUCE_RP_PER_TURN))
	{
		int
			OldRP = mNewResource.get(RESOURCE_RESEARCH);
			//nasty hack to make GU work correctly
		int
//			NewRP = OldRP + ((float)OldRP * 0.75f); //calc_PA(OldRP, CPlayerEffect::PA_PRODUCE_RP_PER_TURN);
			NewRP = calc_PA(OldRP, CPlayerEffect::PA_PRODUCE_RP_PER_TURN);
		mNewResource.set(RESOURCE_RESEARCH, NewRP);
		//SLOG("OldRP = %d, NewRP=%d, CurrentRP = %d", OldRP, NewRP,mNewResource.get(RESOURCE_RESEARCH));
		if (has_effect(CPlayerEffect::PA_CONSUME_PP_BY_RP_FROM_EFFECT_PER_TURN))
		{
         // Extra variables in case we want to change how it works
			int OldPP = mNewResource.get(RESOURCE_PRODUCTION);
			int RPDiff = NewRP - OldRP;
			int PPChange = calc_PA(RPDiff, CPlayerEffect::PA_CONSUME_PP_BY_RP_FROM_EFFECT_PER_TURN);
			int NewPP = OldPP - PPChange;

			mNewResource.set(RESOURCE_PRODUCTION, NewPP);
			SLOG("OldPP = %d, RPDiff = %d, PPChange = %d, NewPP = %d, CurrentPP = %d",
				OldPP, RPDiff, PPChange, NewPP, mNewResource.get(RESOURCE_PRODUCTION));
		}
	}
#define SPACE_MINING_MODULE		5525
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)mFleetList.get(i);

		for (int i=0 ; i<DEVICE_MAX_NUMBER ; i++)
		{
			if (Fleet->get_device(i) == SPACE_MINING_MODULE && Fleet->get_status() != CFleet::FLEET_DEACTIVATED)
			{
				int
					NumberOfTech =
					mTechList.count_by_category(CTech::TYPE_MATTER_ENERGY);
				int
					PPDiff = (int)(Fleet->get_size() * Fleet->get_size() * (100 + NumberOfTech) * Fleet->get_current_ship() / 50);

				mNewResource.change(RESOURCE_PRODUCTION, PPDiff);

				break;
			}
		}
	}
#undef SPACE_MINING_MODULE
}

void
	CPlayer::pay_extra_cost_by_effect()
{
	if (has_effect(CPlayerEffect::PA_CONSUME_PP_PER_TURN))
	{
		int
			OldPP = mNewResource.get(RESOURCE_PRODUCTION);
		int
			NewPP = calc_minus_PA(OldPP, CPlayerEffect::PA_CONSUME_PP_PER_TURN);
//			NewPP = calc_PA(1, CPlayerEffect::PA_CONSUME_PP_PER_TURN);
//		if(NewPP > 10)
//			NewPP = OldPP - 25000;
		mNewResource.set(RESOURCE_PRODUCTION, NewPP);
		SLOG("OldPP = %d, NewPP = %d, CurrentPP = %d", OldPP, NewPP, mNewResource.get(RESOURCE_PRODUCTION));
	}
}

int
	CPlayer::update_security(int aIncome)
{
	mAlertness -= 5;
	if (mAlertness < 0) mAlertness = 0;

	int
		Upkeep = CSpy::get_security_upkeep(mSecurityLevel, aIncome);

	return Upkeep;
}

void
	CPlayer::process_imperial_retribution()
{
	if (has_effect(CPlayerEffect::PA_IMPERIAL_RETRIBUTION))
	{
		change_reserved_production(-mProduction/2);
		change_research(-mResearch/2);

		int
			OldMP = mNewResource.get(RESOURCE_MILITARY);
		mNewResource.change(RESOURCE_MILITARY, -OldMP/2);

		change_ship_production(-mShipProduction/2);

		change_invested_ship_production(-mInvestedShipProduction/2);
		change_research_invest(-get_research_invest()/2);

		for (int i=0 ; i<mPlanetList.length() ; i++)
		{
			CPlanet *
			Planet = (CPlanet *)mPlanetList.get(i);
			int
				PlanetInvestment = Planet->get_investment();
			Planet->change_investment(-PlanetInvestment/2);
		}
	}
}

void
	CPlayer::repair_damaged_ship()
{
	for (int i=mRepairBay.length()-1 ; i>=0 ; i--)
	{
		if (mProduction <= 0 && mNewResource.get(RESOURCE_MILITARY) == 0) break;

		CDamagedShip *
		Ship = (CDamagedShip *)mRepairBay.get(i);

		CShipSize *
				Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(Ship->get_body());

		int
			Upkeep = Body->get_upkeep();

		if (mNewResource.get(RESOURCE_MILITARY) > Upkeep)
		{
			Ship->repair(20);
			mNewResource.change(RESOURCE_MILITARY, -Upkeep);
		}
		else
		{
			Upkeep -= mNewResource.get(RESOURCE_MILITARY);
			mNewResource.set(RESOURCE_MILITARY,0);
			Upkeep *= 2;
			change_reserved_production( -Upkeep );
			Ship->repair(10);
		}

		if (Ship->is_fully_repaired() == true)
		{
			mDock.change_ship((CShipDesign *)Ship, 1);

			Ship->type(QUERY_DELETE);
			STORE_CENTER->store(*Ship);
			mRepairBay.remove_damaged_ship(Ship);
		}
		else
		{
			Ship->type(QUERY_UPDATE);
			STORE_CENTER->store(*Ship);
		}
	}
}

bool
	CPlayer::simulate_repair_damaged_ship(int *aReservedPP, int *aNewMilitaryPoint, int *aLosingProductionPoint, int *aLosingMilitaryPoint)
{
	for (int i=0 ; i<mRepairBay.length() ; i++)
	{
		if ((*aReservedPP - *aLosingProductionPoint) <= 0 && *aNewMilitaryPoint == 0) break;

		CDamagedShip *
		Ship = (CDamagedShip *)mRepairBay.get(i);

		CShipSize *
				Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(Ship->get_body());

		int
			Upkeep = Body->get_upkeep();

		if (*aNewMilitaryPoint > Upkeep)
		{
			//*aNewMilitaryPoint -= Upkeep;
			*aLosingMilitaryPoint += Upkeep;
		}
		else
		{
			Upkeep -= *aNewMilitaryPoint;
			*aLosingMilitaryPoint += *aNewMilitaryPoint;
			//*aNewMilitaryPoint = 0;
			Upkeep *= 2;
			*aReservedPP -= Upkeep;

			*aLosingProductionPoint += Upkeep;
		}
	}

	return true;
}

int
	CPlayer::pay_fleet_upkeep(int aNetIncome)
{
	long int
		Upkeep = 0;
	CString
		News;

	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)mFleetList.get(i);
		CMission &
		Mission = Fleet->get_mission();

		if (Fleet->get_status() == CFleet::FLEET_DEACTIVATED)
		{
			if (MAX_PLAYER_MP - Upkeep < Fleet->calc_upkeep()/10)
			{
				Upkeep = MAX_PLAYER_MP;
				break;
			}
			else
			{
				Upkeep += Fleet->calc_upkeep()/10;	// pay 10%
			}
		}
		else if (Mission.get_mission() == CMission::MISSION_TRAIN)
		{
			if (MAX_PLAYER_MP - Upkeep < Fleet->calc_upkeep())
			{
				Upkeep = MAX_PLAYER_MP;
				break;
			}
			else
			{
				Upkeep += Fleet->calc_upkeep() * 3;
			}
		}
/*		else if (Mission.get_mission() == CMission::MISSION_DISPATCH_TO_ALLY)
		{
			if (MAX_PLAYER_MP - Upkeep < Fleet->calc_upkeep())
			{
				Upkeep = MAX_PLAYER_MP;
				break;
			}
			else
			{
				Upkeep += (long int) ( (double) Fleet->calc_upkeep() * (double) ((double) 1.5 * (double) Fleet->get_size()));
			}
		}     */
		else
		{
			if (MAX_PLAYER_MP - Upkeep < Fleet->calc_upkeep())
			{
				Upkeep = MAX_PLAYER_MP;
				break;
			}
			else
			{
				Upkeep += Fleet->calc_upkeep();
			}
		}
	}
	// calc anti fluff costs
	long int ShipTotal = 0;
	float AntiFluffMod = 1.0f;
	for (int i=0 ; i<mDock.length() ; i++)
	{
		CDockedShip *
		Ship = (CDockedShip *)mDock.get(i);

		if (Ship->get_size() < 10)
		{
			ShipTotal += (long int)Ship->get_number();
		}
	}

	if(ShipTotal > 5000)
	{
		AntiFluffMod = 1.0f + ((float)ShipTotal * .001);
	}
	if (ShipTotal > 20000)
	{
		AntiFluffMod *= 3.0f;
	}
	// calc docked ship upkeep
	for (int i=0 ; i<mDock.length() ; i++)
	{
		CDockedShip *
		Ship = (CDockedShip *)mDock.get(i);
		CShipSize *
				Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(Ship->get_body());
		long int
			ShipUpkeep = (long int)(Ship->get_number()*Body->get_upkeep()/2);

		Upkeep += (long long int)ShipUpkeep;

		if(Upkeep > 50000000 || Upkeep < 0)
		{
			Upkeep = 50000000;
			break;
		}

	}
	Upkeep *= (long long int)AntiFluffMod;

	if(Upkeep > 100000000 || Upkeep < 0)
	{
		Upkeep = 100000000;
	}


	if (Upkeep > mNewResource.get(RESOURCE_MILITARY))
	{
		Upkeep -= mNewResource.get(RESOURCE_MILITARY);
		mNewResource.set(RESOURCE_MILITARY,0);
		Upkeep *= 3;
		if ((Upkeep >= MAX_PLAYER_PP / 2) || (Upkeep < 0))
		{
			//destroy everything.
			for (int i = mDock.length() - 1; i >= 0; i--)
			{
				CDockedShip *
				Ship = (CDockedShip *)mDock.get(i);

				Ship->type(QUERY_DELETE);
				STORE_CENTER->store(*Ship);

				mDock.remove_docked_ship(Ship);
			}

			/*for (int i = mFleetList.length() - 1; i >= 0; i--)
			{
				CFleet *
					Fleet = (CFleet *)mFleetList.get(i);
				CAdmiral *
					Admiral = (CAdmiral *)mAdmiralList.get_by_id(Fleet->get_admiral_id());
				mFleetList.remove_fleet(Fleet->get_id());
				Admiral->set_fleet_number(0);
				mAdmiralList.remove_without_free_admiral(Admiral->get_id());
				mAdmiralPool.add_admiral(Admiral);
			}*/
			News = GETTEXT("Your ship pool has suffered from a massive explosion and all your ships and fleets were destroyed.<BR>\n");
			time_news((char *)News);
			return aNetIncome;
		}

		if ((long long int)aNetIncome + (long long int)mProduction < Upkeep)
		{
	//	find a stand-by fleet and deactive it
	//	if every stand-by fleet is deactivated, scrap 50% of docked ship
			CFleetList
				CandidateFleetList;
			for (int i=0 ; i<mFleetList.length() ; i++)
			{
				CFleet *
				Fleet = (CFleet *)mFleetList.get(i);
				if (Fleet->get_status() == CFleet::FLEET_DEACTIVATED) continue;

				CDefensePlan *
				DefensePlan = NULL;
				for (int j=0 ; j<mDefensePlanList.length() ; j++)
				{
					CDefensePlan *
					TempDefensePlan = (CDefensePlan *)mDefensePlanList.get(j);
					int
						CapitalFleetID = TempDefensePlan->get_capital();
					if (Fleet->get_id() == CapitalFleetID)
					{
						DefensePlan = TempDefensePlan;
						break;
					}
				}

				if (DefensePlan == NULL)
				{
					CandidateFleetList.add_fleet(Fleet);
				}
			}

			if (CandidateFleetList.length() > 0)
			{
				int i;
				for (i = 0; i < CandidateFleetList.length(); i++) {
                    CFleet *Fleet = (CFleet *)CandidateFleetList.get(i);
                    Fleet->set_status(CFleet::FLEET_DEACTIVATED);
                    News = GETTEXT("You are in financial difficulty to supply the whole upkeep of your fleets.");
				    News += "<BR>\n";
				    News.format(GETTEXT("Fleet %1$s has been deactivated."), Fleet->get_name());
				    time_news((char *)News);

                }
            }
              /*  int
					Random = number(CandidateFleetList.length()) - 1;
				CFleet *
				Fleet = (CFleet *)CandidateFleetList.get(Random);

				Fleet->set_status(CFleet::FLEET_DEACTIVATED);
				News = GETTEXT("You are in financial difficulty to supply the whole upkeep of your fleets.");
				News += "<BR>\n";
				News.format(GETTEXT("Fleet %1$s has been deactivated."), Fleet->get_name());
				time_news((char *)News);      */
			else
			{	// scrap 50% of docked ship
				News = GETTEXT("You are in financial difficulty to supply whe whole upkeep of your fleets.");
				News += "<BR>\n";
				News = GETTEXT("Scrapping ships for PP is started.");
				News += "<BR>\n";

				int
					ScrapProduction = 0;
				CString
					ShipName;
				ShipName.clear();

				if (mDock.length() > 0)
				{
					for (int i=0 ; i<mDock.length() ; i++)
					{
						CDockedShip *
						Ship = (CDockedShip *)mDock.get(i);
						int
							DestNumber = Ship->get_number()/2;
						if (DestNumber <= 0)
							DestNumber = 1;
						ScrapProduction += Ship->get_build_cost() * DestNumber/10;
						if (i != 0) ShipName += ", ";
						ShipName.format("%s %s",
							dec2unit(DestNumber),
							Ship->get_name());
						mDock.change_ship( (CShipDesign*)Ship, -DestNumber );
					}

					News.format(GETTEXT("You scrapped %1$s and earned %2$s PP."),
						(char *)ShipName,
						dec2unit(ScrapProduction));
					News += "<BR>\n";
					change_reserved_production( ScrapProduction );
					time_news((char *)News);

					ADMIN_TOOL->add_bankrupt_report_log(
						(char *)format("Player %s has become a bankrupt, so he had to scrapped %s and earned %d PP.",
							(char *)mNick,
							(char *)ShipName,
							ScrapProduction));
				}
				else
				{
					int
						Random = number(mFleetList.length()) - 1;
					CFleet *
					Fleet = (CFleet *)mFleetList.get(Random);
					int
						AdmiralID = Fleet->get_admiral_id();
					CAdmiral *
					Admiral = mAdmiralList.get_by_id(AdmiralID);

					CString
						FleetName;
					FleetName = Fleet->get_nick();

					Fleet->type(QUERY_DELETE);
					STORE_CENTER->store(*Fleet);

					mDock.change_ship((CShipDesign *)Fleet, Fleet->get_current_ship());
					mFleetList.remove_fleet(Fleet->get_id());

					Admiral->set_fleet_number(0);
					mAdmiralList.remove_without_free_admiral(Admiral->get_id());
					mAdmiralPool.add_admiral(Admiral);

					Admiral->type(QUERY_UPDATE);
					STORE_CENTER->store(*Admiral);

					ADMIN_TOOL->add_bankrupt_report_log(
						(char *)format("Player %s has become a bankrupt, so he had to disband fleet %s.",
							(char *)mNick, (char *)FleetName));
				}
			}

			CandidateFleetList.remove_without_free_all();

			for (int i=0 ; i<mFleetList.length() ; i++)
			{
				CFleet *
				Fleet = (CFleet *)mFleetList.get(i);
				if (Fleet->get_status() != CFleet::FLEET_DEACTIVATED) continue;
				if (Fleet->get_mission().get_mission() == CMission::MISSION_NONE) continue;

				Fleet->delay_mission(CGame::mSecondPerTurn);
			}

			if (mProduction < Upkeep)
			{
				return mProduction;
			}
			else
			{
				return Upkeep;
			}
		}
		else
		{
			// if there is deactive fleet, standy-by it
			for (int i=0 ; i<mFleetList.length() ; i++)
			{
				CFleet *
				Fleet = (CFleet *)mFleetList.get(i);
				if (Fleet->get_status() == CFleet::FLEET_DEACTIVATED)
				{
					if (Fleet->get_mission().get_mission() == CMission::MISSION_NONE)
					{
						Fleet->set_status(CFleet::FLEET_STAND_BY);
						News.clear();
						News.format(GETTEXT("Your deactivated fleet %1$s is activated."),
							Fleet->get_name());
						News += "<BR>\n";
						News += GETTEXT("It is on stand-by, waiting for your orders.");
						time_news((char*)News);
					}
					else
					{
						Fleet->set_status(CFleet::FLEET_UNDER_MISSION);
						News.clear();
						News.format(GETTEXT("Your deactivated fleet %1$s is activated."),
							Fleet->get_name());
						News += "<BR>\n";
						News += GETTEXT("The fleet continues its mission.");
						time_news((char*)News);
					}
				}
			}
			return Upkeep;
		}
	}
	else
	{
	// if there is deactive fleet, standy-by it
		for (int i=0 ; i<mFleetList.length() ; i++)
		{
			CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
			if (Fleet->get_status() == CFleet::FLEET_DEACTIVATED)
			{
				if (Fleet->get_mission().get_mission() == CMission::MISSION_NONE)
				{
					Fleet->set_status(CFleet::FLEET_STAND_BY);
					News.clear();
					News.format(GETTEXT("Your deactivated fleet %1$s is activated."),
						Fleet->get_name());
					News += "<BR>\n";
					News += GETTEXT("It is on stand-by, waiting for your orders.");
					time_news((char*)News);
				}
				else
				{
					Fleet->set_status(CFleet::FLEET_UNDER_MISSION);
					News.clear();
					News.format(GETTEXT("Your deactivated fleet %1$s is activated."),
						Fleet->get_name());
					News += "<BR>\n";
					News += GETTEXT("The fleet continues its mission.");
					time_news((char*)News);
				}
			}
		}
		mNewResource.change(RESOURCE_MILITARY, -Upkeep);
		return 0;
	}
}

void
	CPlayer::build_ship()
{
	if (mShipBuildQ.length() == 0)
	{
		change_invested_ship_production(mShipProduction);
		set_ship_production(0);
		mStoreFlag += STORE_SHIP_PRODUCTION;
		mStoreFlag += STORE_INVESTED_SHIP_PRODUCTION;
		return;
	}

	while (mShipBuildQ.length() > 0)
	{
		CShipToBuild *
		ShipToBuild = (CShipToBuild *)get_build_q()->get(0);
		CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id(ShipToBuild->get_armor());

		int
			Count = 0;
		int
			CostPerShip;
		if (Armor->get_armor_type() == AT_BIO &&
			mAbility.has(ABILITY_GREAT_SPAWNING_POOL))
		{
			if (mAbility.has(ABILITY_HIVE_SHIP_YARD))
			{
				CostPerShip = ShipToBuild->get_build_cost() * 70 / 100;
			}
			else
			{
				CostPerShip = ShipToBuild->get_build_cost() * 80 / 100;
			}
		}
		else
		{
			if (mAbility.has(ABILITY_HIVE_SHIP_YARD))
			{
				CostPerShip = ShipToBuild->get_build_cost() * 90 / 100;
			}
			else
			{
				CostPerShip = ShipToBuild->get_build_cost();
			}
		}

		while (mShipProduction > CostPerShip && ShipToBuild->get_number() > 0)
		{
			Count++;
			change_ship_production(-CostPerShip);
			ShipToBuild->change_number(-1);
		}

		if (Count == 0)
		{
			break;
		}
		else
		{
			mDock.change_ship((CShipDesign *)ShipToBuild, Count);

			if (Count == 1)
			{
				time_news((char *)format(GETTEXT("1 %1$s ship is built and put in the ship pool."),
						ShipToBuild->get_name()));
			} else
			{
				time_news((char *)format(GETTEXT("%1$s %2$s ships are built and put in the ship pool."),
						dec2unit(Count), ShipToBuild->get_name()));
			}

			if (ShipToBuild->get_number() > 0)
			{
				ShipToBuild->type(QUERY_UPDATE);
				STORE_CENTER->store(*ShipToBuild);
			}
			else
			{
				ShipToBuild->type(QUERY_DELETE);
				STORE_CENTER->store(*ShipToBuild);

				mShipBuildQ.remove_ship_to_build(ShipToBuild);
			}
		}
	}
//	SLOG("TEH FUNKY LOG %d", (-mLastTurnResource.get(RESOURCE_PRODUCTION) * 30/100));
	change_invested_ship_production(-mLastTurnResource.get(RESOURCE_PRODUCTION) * 30/100);


	// Fix for GB queue
	if (mShipBuildQ.length() == 0)
	{
		change_invested_ship_production(mShipProduction);
		set_ship_production(0);
		mStoreFlag += STORE_SHIP_PRODUCTION;
		mStoreFlag += STORE_INVESTED_SHIP_PRODUCTION;
	}

	mStoreFlag += STORE_INVESTED_SHIP_PRODUCTION;
}

int
	CPlayer::calc_real_ship_production()
{
	int
		MaxPPUsage = calc_ship_production();

	if (!MaxPPUsage) return 0;

	float
		BonusRatio = mInvestedShipProduction/MaxPPUsage;

	if (BonusRatio > 1) BonusRatio = 1;

	if (mAbility.has(ABILITY_EFFICIENT_INVESTMENT))
	{
		return (int)(calc_ship_production() * (100 + 50*BonusRatio*2) / 100);
	}
	else
	{
		return (int)(calc_ship_production() * (100 + 50*BonusRatio) / 100);
	}
}

bool
	CPlayer::calc_all_ships_upkeep(int *aReservedPP, int aCurrentProductionPoint, int *aNewMilitaryPoint, int *aGainingProductionPoint, int *aLosingProductionPoint, int *aLosingMilitaryPoint)
{
	long int
		Upkeep = 0;

	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)mFleetList.get(i);
		CMission &
		Mission = Fleet->get_mission();

		if (Fleet->get_status() == CFleet::FLEET_DEACTIVATED)
		{
			Upkeep += Fleet->calc_upkeep()/10;	// pay 10%
		}
		else if (Mission.get_mission() == CMission::MISSION_TRAIN)
		{
			Upkeep += Fleet->calc_upkeep() * 3;
		}
/*		else if (Mission.get_mission() == CMission::MISSION_DISPATCH_TO_ALLY)
		{
			if (MAX_PLAYER_MP - Upkeep < Fleet->calc_upkeep())
			{
				Upkeep = MAX_PLAYER_MP;
				break;
			}
			else
			{
				Upkeep += (long int) ( (double) Fleet->calc_upkeep() * (double) ((double) 1.5 * (double) Fleet->get_size()));
			}
		}      */
		else
		{
			Upkeep += Fleet->calc_upkeep();
		}
	}

	// calc anti fluff costs
	int ShipTotal = 0;
	float AntiFluffMod = 1.0f;
	for (int i=0 ; i<mDock.length() ; i++)
	{
		CDockedShip *
		Ship = (CDockedShip *)mDock.get(i);


		if (Ship->get_size() < 10)
		{
			ShipTotal += Ship->get_number();
		}
	}

	if(ShipTotal > 5000)
	{
		AntiFluffMod = 1.0f + ((float)ShipTotal * .001);
	}
	if (ShipTotal > 20000)
	{
		AntiFluffMod *= 1.5f;
	}
	// calc docked ship upkeep
	for (int i=0 ; i<mDock.length() ; i++)
	{
		CDockedShip *
		Ship = (CDockedShip *)mDock.get(i);
		CShipSize *
				Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(Ship->get_body());

		int
			ShipUpkeep = (int)(Ship->get_number()*Body->get_upkeep()/2);

		Upkeep += ShipUpkeep;
		if(Upkeep > 100000000 || Upkeep < 0)
		{
			Upkeep = 100000000;
		}
	}
	long long int tempUpkeep;
	tempUpkeep = (long long int)Upkeep * (long long int)AntiFluffMod;

	if(tempUpkeep > MAX_PLAYER_MP || tempUpkeep < 0)
	{
		tempUpkeep = MAX_PLAYER_MP;
	}
	Upkeep = tempUpkeep;

	if (Upkeep > *aNewMilitaryPoint)
	{
		Upkeep -= *aNewMilitaryPoint;
		*aLosingMilitaryPoint = *aNewMilitaryPoint;
		//*aNewMilitaryPoint = 0;
		Upkeep *= 2;

		if (aCurrentProductionPoint + *aReservedPP < Upkeep)
		{
			CFleetList
				CandidateFleetList;
			for (int i=0 ; i<mFleetList.length() ; i++)
			{
				CFleet *
				Fleet = (CFleet *)mFleetList.get(i);
				if (Fleet->get_status() == CFleet::FLEET_DEACTIVATED) continue;

				CDefensePlan *
				DefensePlan = NULL;
				for (int j=0 ; j<mDefensePlanList.length() ; j++)
				{
					CDefensePlan *
					TempDefensePlan = (CDefensePlan *)mDefensePlanList.get(j);
					int
						CapitalFleetID = TempDefensePlan->get_capital();
					if (Fleet->get_id() == CapitalFleetID)
					{
						DefensePlan = TempDefensePlan;
						break;
					}
				}

				if (DefensePlan == NULL)
				{
					CandidateFleetList.add_fleet(Fleet);
				}
			}

			if (CandidateFleetList.length() == 0)
			{
				int
					ScrapProduction = 0;

				if (mDock.length() > 0)
				{
					for (int i=0 ; i<mDock.length() ; i++)
					{
						CDockedShip *
						Ship = (CDockedShip *)mDock.get(i);
						int
							DestNumber = Ship->get_number()/10;
						if (DestNumber <= 0)
							DestNumber = 1;
						ScrapProduction += Ship->get_build_cost() * DestNumber/10;
					}

					*aGainingProductionPoint = ScrapProduction;
				}
			}

			CandidateFleetList.remove_without_free_all();

			return true;
		}
		else
		{
			for (int i=0 ; i<mFleetList.length() ; i++)
			{
				CFleet *
				Fleet = (CFleet *)mFleetList.get(i);
				if (Fleet->get_status() == CFleet::FLEET_DEACTIVATED)
				{
					Fleet->set_status(CFleet::FLEET_STAND_BY);
				}
			}
			*aLosingProductionPoint = Upkeep;
			return true;
		}
	}
	else
	{
		*aLosingMilitaryPoint = Upkeep;
		//*aNewMilitaryPoint -= Upkeep;
		return true;
	}
}

void
	CPlayer::update_tech()
{
	CTech *
	TargetTech = NULL;

	//if (mTargetTech)
		//TargetTech = mAvailableTechList.get_by_id(mTargetTech);

	//if (!TargetTech && (mAvailableTechList.length()>0))
	//{
	TargetTech = TECH_TABLE->get_by_id(mTargetTech);
	if(TargetTech)
	{
		TargetTech = TargetTech->get_prereq(TargetTech, this);
	}
			//(CTech *)mAvailableTechList.get(
							//number(mAvailableTechList.length())-1);
	//}
	if(!TargetTech)
	{
		TargetTech = (CTech *)mAvailableTechList.get(number(mAvailableTechList.length())-1);
	}
	if (TargetTech)
	{
		int
			ResearchCost = get_research_cost(TargetTech);

		if(mResearch >= ResearchCost) // learn tech
		{
			change_research(-(int)ResearchCost);
			discover_tech(TargetTech->get_id());
		}
	}
}

void
	CPlayer::save_planet()
{
	for (int i=0 ; i<mPlanetList.length() ; i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);
		if (Planet->is_changed() == false) continue;

		Planet->type(QUERY_UPDATE);
//		SLOG("database store a planet(%d)", Planet->get_id());
		STORE_CENTER->store(*Planet);
	}
}


CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CPlayer& aPlayer)
{
	aStoreCenter.store(aPlayer);

	return aStoreCenter;
}

const char*
	CPlayer::select_concentration_mode_html()
{
	static CString
		Buffer;
	Buffer.clear();

	Buffer = "<SELECT NAME=\"CONCENTRATION_MODE\" onChange=CM()>\n";

	for(int i=0; i<MODE_MAX; i++)
		Buffer.format("\t<OPTION %s VALUE=\"%d\">%s</OPTION>\n",
		(mMode == i) ? "SELECTED":"", i, get_mode_name(i));

	Buffer += "</SELECT>\n";

	return (char*)Buffer;
}

const char*
	CPlayer::available_science_list_html(CTech::ETechType aType)
{
	static CString
		HTML;
	HTML.clear();

	CTech
		*Tech;
	set_available_tech_list();
	for(int i=0; i<mAvailableTechList.length(); i++)
	{
		Tech = (CTech*)mAvailableTechList.get(i);
		if (Tech->get_type() != aType) continue;
		HTML.format("&nbsp;<A HREF=\"research_tech_detail.as?ID=%d\">%s</A><BR>\n",
			Tech->get_id(), Tech->get_name());
	}

	if (!HTML.length()) return " ";
	return (char*)HTML;
}

const char*
	CPlayer::known_science_list_html(CTech::ETechType aType)
{
	static CString
		HTML;
	HTML.clear();

	CKnownTech
		*Tech;

	for(int i=0; i<mTechList.length(); i++)
	{
		Tech = (CKnownTech*)mTechList.get(i);
		if (Tech->get_type() != aType) continue;
		HTML.format("&nbsp;<A HREF=\"/encyclopedia/tech/%d.html\">%s</A><BR>\n",
			Tech->get_id(), Tech->get_name());
	}

	if (!HTML.length()) return " ";
	return (char*)HTML;
}

/* start telecard 2000/10/04 */
const char *
	CPlayer::available_component_list_html()
{
	static CString
		Buf;
	Buf.clear();

	if(mComponentList.length() == 0)
		return "No Component Available.<BR>\n";

	for(int i=0; i<mComponentList.length(); i++)
	{
		CComponent *
		Component = (CComponent *)mComponentList.get(i);

		Buf.format("#%d %s<BR>\n",
			Component->get_id(), Component->get_name() );
	}

	return (char *)Buf;
}
/* end telecard 2000/10/04 */


const char*
	CPlayer::planet_management_html(int aStartIndex, int aNumber)
{
	static CString
		HTML;
	HTML.clear();

	if (!mPlanetList.length()) return " ";
	if (aStartIndex < 0 || aStartIndex >= mPlanetList.length()) return " ";

	int
		Count = 0;
	for(int i=aStartIndex; i<mPlanetList.length(); i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);
		HTML += Planet->html_management_record();
		Count++;

		if (Count >= aNumber) break;
	}

	return (char *)HTML;
}


bool
	CPlayer::set_target_tech( int aTechID )
{
	// check if the target tech is valid
	// if target tech id is invalid, reset target tech
	// INCOMPLETE

	mTargetTech = aTechID;
	mStoreFlag += STORE_TARGET_TECH;

	return true;
}

bool
	CPlayer::discover_tech(int aTechID, bool aIsInnate)
{
	CTech *
	NewTech = TECH_TABLE->get_by_id(aTechID);

	if (!NewTech)				 // not existing tech
		return false;

	if (mTechList.get_by_id(aTechID) && aIsInnate == false) 	// already know the tech
		return false;

	CKnownTech *
	KnownTech = new CKnownTech(NewTech);

	KnownTech->set_owner(mGameID);
	if (aIsInnate)
	{
		KnownTech->set_attribute(CTech::ATTR_INNATE);
	}

	mNewsCenter.known_tech_news(KnownTech);

	mTechList.add_known_tech(KnownTech);
	mTechList.type(QUERY_UPDATE);
	STORE_CENTER->store(mTechList);

	mTechCM += KnownTech->get_effect();
	build_control_model();

	CIntegerList &
	ComponentList = (CIntegerList &)KnownTech->get_component_list();

	static CString
		ComponentName,
		ComponentNews;
	ComponentName.clear();
	ComponentNews.clear();

	bool
		Comma = false;
	for (int i=0 ; i<ComponentList.length() ; i++)
	{
		CComponent *
		Component = COMPONENT_TABLE->get_by_id((int)ComponentList.get(i));
		if (!Component) continue;
		if (Component->is_secret()) continue;

		if (Comma) ComponentName += ", ";
		ComponentName += Component->get_name();
		Comma = true;
	}

	if (ComponentList.length() == 1)
	{
		ComponentNews.format(GETTEXT("The new component %1$s becomes available."),
			(char *)ComponentName);
		mNewsCenter.time_news((char *)ComponentNews);
	} else if (ComponentList.length() > 1)
	{
		ComponentNews.format(GETTEXT("The new components %1$s become available."),
			(char *)ComponentName);
		mNewsCenter.time_news((char *)ComponentNews);
	}

	if (mTargetTech == aTechID)
		reset_target_tech();

    #define SEAL_ONE 1734
    #define SEAL_TWO 1735
    #define SEAL_THREE 1736
    #define SEAL_FOUR 1737
    #define SEAL_FIVE 1738

    if (aTechID == SEAL_ONE ||
        aTechID == SEAL_TWO ||
        aTechID == SEAL_THREE ||
        aTechID == SEAL_FOUR ||
        aTechID == SEAL_FIVE) {
        
        CString warning;
        warning.clear();
        warning.format("WARNING : The player %s has broken a seal - %s.",
                        get_nick(),
                        KnownTech->get_name());
        for (int i = 0; i < PLAYER_TABLE->length(); i++) {
            if (PLAYER_TABLE->get_by_game_id(i)) {
                PLAYER_TABLE->get_by_game_id(i)->time_news( warning );
            }
        }
    }
    
    #undef SEAL_ONE
    #undef SEAL_TWO
    #undef SEAL_THREE
    #undef SEAL_FOUR
    #undef SEAL_FIVE

	//add project if given
	if(KnownTech->get_project()) {
		CProject *CurrentProject = mProjectList.get_by_id(KnownTech->get_project());
		mProjectList.add_project(CurrentProject);
		mNewsCenter.discovered_project_news(CurrentProject);
	}
	set_available_tech_list();
	return true;
}

void
	CPlayer::set_available_tech_list()
{
	// get highest known(not innate) tech level
	// search all tech whose level is equal or less than
	// highest know tech level+1
	// among such techs, list all techs whose prerequisits are
	// all known and who are not researched.
	mAvailableTechList.remove_all();

	for(int i=0; i<TECH_TABLE->length(); i++)
	{
		CTech *
		Tech = (CTech *)TECH_TABLE->get(i);
		int
			TechID = Tech->get_id();

		if ((!mTechList.get_by_id(TechID))&&(Tech->evaluate(this)))
		{
			mAvailableTechList.add_tech(Tech);
		}
	}
}

void
	CPlayer::set_project_list()
{
	mProjectList.remove_all();

	for(int i=0; i<PROJECT_TABLE->length(); i++)
	{
		CProject *
		Project = (CProject *)PROJECT_TABLE->get(i);
		if (Project->get_type() == TYPE_BM ||
			Project->get_type() == TYPE_COUNCIL) continue;
		int
			ProjectID = Project->get_id();
		if (mPurchasedProjectList.get_by_id(ProjectID)) continue;
		if (Project->evaluate(this)) mProjectList.add_project(Project);
	}
}

void
	CPlayer::set_component_list()
{
	mComponentList.remove_all();

	for(int i=0; i<COMPONENT_TABLE->length(); i++)
	{
		CComponent *
		Component = (CComponent*)COMPONENT_TABLE->get(i);

		if (Component->evaluate(this)) mComponentList.add_component(Component);
	}
}

void
	CPlayer::discover_basic_techs()
{
	for(int i=0; i < TECH_TABLE->length(); i++)
	{
		CTech
			*Tech = (CTech*)TECH_TABLE->get(i);

		if (Tech->is_attribute(CTech::ATTR_BASIC))
		{
			discover_tech(Tech->get_id());
		}
	}
}

// Learns all technology of level aLevel
// the CPlayer
void CPlayer::discover_tech_level(int aLevel) {
    CKnownTechList *KnownTechList = get_tech_list();

    for(int i=0; i < TECH_TABLE->length(); i++) {
        CTech *Tech = (CTech*)TECH_TABLE->get(i);

        if (Tech->get_level() <= aLevel) {
            if (!KnownTechList->get_by_id(Tech->get_id())) {
                discover_tech(Tech->get_id());
            }
        }
    }
}

bool
	CPlayer::lose_project(int aProject)
{
	CPurchasedProject *Project = mPurchasedProjectList.get_by_id(aProject);
	if (Project == NULL) return false;
	mProjectCM -= Project->get_effect();
	Project->type(QUERY_DELETE);
	STORE_CENTER->store(*Project);
	mPurchasedProjectList.remove_project(aProject);
	return true;
}

bool
	CPlayer::lose_all_project()
{
	for (int i=mPurchasedProjectList.length()-1 ; i>=0 ; i--)
	{
		CPurchasedProject *
		PurchasedProject = (CPurchasedProject *)mPurchasedProjectList.get(i);
		lose_project(PurchasedProject->get_id());
	}
	return true;
}

bool
	CPlayer::buy_project(int aProjectID, bool aPayPrice)
{

	CProject *
	Project = PROJECT_TABLE->get_by_id(aProjectID);

	if (!Project) return false;
	if (mPurchasedProjectList.get_by_id(aProjectID)) return false;

	int
		Cost;
	if (aPayPrice == true)
	{
		if (Project->get_type() == TYPE_PLANET)
		{
			int
				PlanetNumber = mPlanetList.length();
			Cost = Project->get_cost() * PlanetNumber;
		}
		else
		{
			Cost = Project->get_cost();
		}
	}
	else
	{
		Cost = 0;
	}

	if (mProduction < Cost) return false;

	CPurchasedProject *
	PurchasedProject = new CPurchasedProject(Project);
	PurchasedProject->set_owner(mGameID);
	PurchasedProject->type(QUERY_INSERT);
	STORE_CENTER->store(*PurchasedProject);

	mNewsCenter.purchased_project_news(PurchasedProject);

	mPurchasedProjectList.add_project(PurchasedProject);
	mProjectList.remove_project(aProjectID);

	change_reserved_production(-Cost);

	mProjectCM += PurchasedProject->get_effect();
	build_control_model();

	return true;
}

//start telecard 2001/03/14
void
	CPlayer::buy_bm_project( int aProjectID )
{
	CProject*
	project = PROJECT_TABLE->get_by_id( aProjectID );

	CPurchasedProject *
	purchasedProject = new CPurchasedProject(project);
	purchasedProject->set_owner(mGameID);
	purchasedProject->type(QUERY_INSERT);
	STORE_CENTER->store(*purchasedProject);

	mPurchasedProjectList.add_project(purchasedProject);

	mProjectCM += purchasedProject->get_effect();
	build_control_model();
}
//end telecard 2001/03/14

//start telecard 2001/04/02
void
	CPlayer::buy_ending_project( int aProjectID )
{
	CProject*
	project = PROJECT_TABLE->get_by_id( aProjectID );

	CPurchasedProject *
	purchasedProject = new CPurchasedProject(project);
	purchasedProject->set_owner(mGameID);
	purchasedProject->type(QUERY_INSERT);
	STORE_CENTER->store(*purchasedProject);

	mPurchasedProjectList.add_project(purchasedProject);

	mProjectCM += purchasedProject->get_effect();
	build_control_model();
}
//end telecard 2001/04/02

bool
	CPlayer::set_name(char *aName)
{
	mName = aName;
	refresh_nick();

	return true;
}

void
	CPlayer::set_race( int aRace )
{
	mRace = RACE_TABLE->get_by_id(aRace);
	if(!mRace)
	{
		mRace = RACE_TABLE->get_by_id(1);
		SLOG( "WRONG RACE : %s(%d) - %d",
			get_name(), get_game_id(), aRace );
	}
}

bool
	CPlayer::set_home_cluster_id(int aHomeClusterID)
{
	mHomeClusterID = aHomeClusterID;
	mStoreFlag += STORE_HOME_CLUSTER_ID;

	return true;
}

void
	CPlayer::init_race_innate()
{
	mAbility = mRace->get_ability();

	for(int i=0; i<mRace->get_innate_tech_count(); i++)
	{
		int TechID = mRace->get_innate_tech(i);
		if(!discover_tech(TechID, true))
			SLOG( "WRONG INNATE TECH : %s(%d) %s(%d) - %d",
			get_name(), get_game_id(), mRace->get_name(),
			mRace->get_id(), TechID );
	}
}

void
	CPlayer::init_admiral_innate()
{
	for(int i=0; i<INIT_ADMIRAL_NUMBER; i++)
	{
		CAdmiral *Admiral = new CAdmiral(this);

		mAdmiralPool.add_admiral(Admiral);
		Admiral->type(QUERY_INSERT);
		STORE_CENTER->store(*Admiral);
		mNewsCenter.admiral_news(Admiral);
	}
}

const char *
	CPlayer::generate_news()
{
	static CString
		News;
	News.clear();

	News = mNewsCenter.generate();

	News += "<BR>\n";

	News += "<TR>\n";
	News.format("<TD CLASS=\"maintext\" ALIGN=\"CENTER\">%s</TD>", GETTEXT("EVENT"));
	News += "</TR>\n";

	News += GALACTIC_EVENT_LIST->html("Galactic");

	CCluster *
	HomeCluster = UNIVERSE->get_by_id(mHomeClusterID);
	if (HomeCluster) News += HomeCluster->get_event_list()->html("Cluster");

	News += mEventList.html("System");
	News += "<BR><BR>\n";

	News += mPlanetList.generate_news();

	if (News.length()) return (char*)News;

	return " ";
}

void
	CPlayer::update_admiral_pool()
{

	if ((mAdmiralList.length() + mAdmiralPool.length()) >= CPlayer::mMaxAdmirals) return;

	mAdmiralTimer++;

	int
		Military = mControlModel.get_military();

	int
		CreateTime = CPlayer::mPeriodCreateAdmiral - (Military * CPlayer::mAdmiralMilitaryBonus);

	if (CreateTime < CPlayer::mMaxPeriodCreateAdmiral)
	{
		if (this->has_ability(ABILITY_FANATICAL_RECRUITING))
		{
			if (CPlayer::mMaxPeriodCreateAdmiral > 1)
			{
				CreateTime = CPlayer::mMaxPeriodCreateAdmiral - 1;
			}
		}
		else
		{
			CreateTime = CPlayer::mMaxPeriodCreateAdmiral;
		}
	}

//	SLOG("AdmiralTimer %d, CreateTime %d", mAdmiralTimer, CreateTime);
	if (mAdmiralTimer < CreateTime) return;

	CAdmiral
	*Admiral = new CAdmiral(this);
	mAdmiralPool.add_admiral(Admiral);
	Admiral->type(QUERY_INSERT);
	STORE_CENTER->store(*Admiral);
	mNewsCenter.admiral_news(Admiral);
	//SLOG("Player %s: New admiral %s", get_nick(), Admiral->get_name());

	mAdmiralTimer = 0;
}

const char *
	CPlayer::project_list_html()
{
	static CString
		Intro, List;
	Intro.clear();
	List.clear();

	for (int i=0; i<PROJECT_TABLE->length(); i++)
	{
		CProject *
		Project = (CProject *)PROJECT_TABLE->get(i);

		if (Project->get_type() == TYPE_BM ||
			Project->get_type() == TYPE_COUNCIL) continue;
		if (mPurchasedProjectList.get_by_id(Project->get_id())) continue;
		if (!Project->evaluate(this)) continue;

		CControlModel &
		Effect = (CControlModel &)Project->get_effect();

		List += "<TR>\n";

		List += "<TD CLASS=\"tabletxt\" WIDTH=\"144\" ALIGN=\"CENTER\">";
		List += Project->get_name();
		List += "</TD>\n";

		List += "<TD WIDTH=\"255\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">";
		List += Effect.get_effects_string_for_html();

		CPlayerEffectList *PlayerEffectList = Project->get_effect_list();

		bool
			AnyEffect = false;

		static CString OtherEffectList;
		OtherEffectList.clear();
		int j;
		for (j=0; j < PlayerEffectList->length(); j++)
		{
			CPlayerEffect *PlayerEffect = (CPlayerEffect *)PlayerEffectList->get(j);
			if (!PlayerEffect)
				continue;

			if (Effect.get_effects_string_for_html() && (!AnyEffect)) OtherEffectList += ",<BR>";
			if (AnyEffect) OtherEffectList += ",<BR>";
			AnyEffect = true;

			if ( PlayerEffect->get_type() == CPlayerEffect::PA_GAIN_ABILITY ) {
				OtherEffectList.format("%s", index_to_ability_name(PlayerEffect->get_argument1()));
			}
			else if  ( PlayerEffect->get_type() == CPlayerEffect::PA_LOSE_ABILITY ) {
				OtherEffectList.format("%s: %s", CPlayerEffect::get_type_name(PlayerEffect->get_type()), index_to_ability_name(PlayerEffect->get_argument1()));
			}
			else
			{
				OtherEffectList.format("%s: ", CPlayerEffect::get_type_name(PlayerEffect->get_type()));
				if (PlayerEffect->get_argument1() > 0) OtherEffectList += "+";
				OtherEffectList.format("%s", dec2unit(PlayerEffect->get_argument1()));
				if (PlayerEffect->get_apply() != CPlayerEffect::APPLY_ABSOLUTE)
					OtherEffectList += "%";
			}
		}

		List += OtherEffectList;

		List += "</TD>\n";

		int
			Cost;
		switch (Project->get_type())
		{
			case TYPE_PLANET:
				Cost = Project->get_cost() * mPlanetList.length();
				break;
			case TYPE_FIXED:
				Cost = Project->get_cost();
				break;
			default:
				Cost = 0;
		}

		List += "<TD WIDTH=\"103\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">";
		List += dec2unit(Cost);
		List += "</TD>\n";

		List += "<TD CLASS=\"tabletxt\" WIDTH=\"38\" ALIGN=\"CENTER\">\n";
		List.format("<INPUT TYPE=\"checkbox\" NAME=\"PROJECT%d\">",i);
		List += "</TD>\n";

		List += "</TR>\n";
	}
	if(!List) return(char *)List;
	Intro = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Intro += "<TR>\n";

	Intro += "<TH CLASS=\"tabletxt\" WIDTH=\"144\" BGCOLOR=\"#171717\">";
	Intro += GETTEXT("Project Name");
	Intro += "</TH>\n";

	Intro += "<TH WIDTH=\"255\" CLASS=\"tabletxt\" BGCOLOR=\"#171717\">";
	Intro += GETTEXT("Effect");
	Intro += "</TH>\n";

	Intro += "<TH WIDTH=\"103\" CLASS=\"tabletxt\" BGCOLOR=\"#171717\">";
	Intro += GETTEXT("Cost");
	Intro += "</TH>\n";

	Intro += "<TH CLASS=\"tabletxt\" WIDTH=\"38\" BGCOLOR=\"#171717\">";
	Intro += "&nbsp;";
	Intro += "</TH>\n";

	Intro += "</TR>\n";
	Intro += List;
	Intro += "</TABLE>\n";

	return (char *)Intro;
}

const char *
	CPlayer::purchased_project_list_html()
{
	CString Buf;
	Buf = "<SELECT NAME = \"PURCHASED PROJECT\">";

	for (int i=0; i<mProjectList.length(); i++)
		Buf.format("<OPTION VALUE = %d>%s (%d)</OPTION>\n",
		((CProject *)mPurchasedProjectList.get(i))->get_id(),
		((CProject *)mPurchasedProjectList.get(i))->get_name(),
		((CProject *)mPurchasedProjectList.get(i))->get_cost());

	Buf += "</SELECT>";

	return (char *)Buf;
}

bool
	CPlayer::set_council(CCouncil *aCouncil)
{
	SLOG("YOSHIKI : CPlayer::set_council()-1");
	if (aCouncil == NULL) return false;
	SLOG("YOSHIKI : CPlayer::set_council()-2");

	mCouncilID = aCouncil->get_id();
	mStoreFlag += STORE_COUNCIL_ID;

	SLOG("YOSHIKI : CPlayer::set_council()-3");
	CCouncil *
	Council = get_council();
	SLOG("YOSHIKI : CPlayer::set_council()-4");
	if (Council != NULL)
	{
		SLOG("YOSHIKI : CPlayer::set_council()-5");
		Council->add_member(this);
	}
	SLOG("YOSHIKI : CPlayer::set_council()-6");

	return true;
}

bool
	CPlayer::set_council(int aID)
{
	CCouncil *
	Council = COUNCIL_TABLE->get_by_id(aID);

	if (Council == NULL) return false;

	mCouncilID = Council->get_id();
	mStoreFlag += STORE_COUNCIL_ID;

	return true;
}

bool
	CPlayer::change_council_production(int aProduction)
{
	mStoreFlag += STORE_COUNCIL_PRODUCTION;
	safe_fast_change_type<int,MAX_COUNCIL_PP>(aProduction,mCouncilProduction);
	return true;
}

bool
	CPlayer::set_council_donation(int aDonation)
{
	safe_set_type<int,MAX_COUNCIL_PP>(aDonation, mCouncilDonation);
	mStoreFlag += STORE_COUNCIL_DONATION;
	return true;
}

bool
	CPlayer::change_council_donation(int aDonation)
{
	mStoreFlag += STORE_COUNCIL_DONATION;
	safe_fast_change_type<int,MAX_COUNCIL_PP>(aDonation,mCouncilDonation);
	return true;
}

bool
	CPlayer::set_council_vote_gain(int aGain)
{
	safe_set_type<int,MAX_VOTE_POWER>(aGain, mCouncilVoteGain);
	return true;
}

bool
	CPlayer::change_council_vote_gain(int aGain)
{
	safe_fast_change_type<int,MAX_VOTE_POWER>(aGain,mCouncilVoteGain);
	return true;
}

const char *
	CPlayer::get_race_name2()
{
	switch (get_race())
	{
		case CRace::RACE_HUMAN:
			return "human";
		case CRace::RACE_TARGOID:
			return "targoid";
		case CRace::RACE_BUCKANEER:
			return "buckaneer";
		case CRace::RACE_TECANOID:
			return "tecanoid";
		case CRace::RACE_EVINTOS:
			return "evintos";
		case CRace::RACE_AGERUS:
			return "agerus";
		case CRace::RACE_BOSALIAN:
			return "bosalian";
		case CRace::RACE_XELOSS:
			return "xeloss";
		case CRace::RACE_XERUSIAN:
			return "xerusian";
		case CRace::RACE_XESPERADOS:
			return "xesperados";
		// ---Change--- //
		case CRace::RACE_TRABOTULIN:
			return "trabotulin";
		// ---End Change--- //
		default:
			return NULL;
	}
}

bool
	CPlayer::refresh_power()
{
	if (mGameID == EMPIRE_GAME_ID) return true;

	mPower = mFleetList.get_power();
	
	// Allied Fleet List Power
	/*
	if (MAX_POWER - mPower < mAllyFleetList.get_power()) {
        mPower = MAX_POWER;
    } else {
        mPower += mAllyFleetList.get_power();
    }
    */

	if (MAX_POWER - mPower < mDock.get_power())
	{
		mPower = MAX_POWER;
	}
	else
	{
		mPower += mDock.get_power();

		if (MAX_POWER - mPower < mPlanetList.get_power())
		{
			mPower = MAX_POWER;
		}
		else
		{
			mPower += mPlanetList.get_power();

			if (MAX_POWER - mPower < mTechList.get_power())
			{
				mPower = MAX_POWER;
			}
			else
			{
				mPower += mTechList.get_power();
			}
		}
	}

	if (is_dead() == false)
	{
		CCouncil *
		Council = get_council();
		if (Council != NULL)
		{
			Council->refresh_power();
		}
	}

	return true;
}

int
	CPlayer::get_council_vote_power()
{
	int
		Social = mTechList.count_by_category(CTech::TYPE_SOCIAL);

	if ((long long int)mPower * (long long int)Social >
		(long long int)MAX_VOTE_POWER * 100) return MAX_VOTE_POWER;

	return (int)((long long int)mPower * (long long int)Social / 100);
}

bool
	CPlayer::set_council_production(int aProduction)
{
	safe_set_type<int,MAX_VOTE_POWER>(aProduction, mCouncilProduction);
	mStoreFlag += STORE_COUNCIL_PRODUCTION;
	return true;
}

/* start telecard 2001/01/29
void
CPlayer::new_player_assign_council()
{
	CCouncilTable
		*CouncilTable = COUNCIL_TABLE;

	int
		CouncilLessThan15 = 0;

	for(int i=0; i<CouncilTable->length(); i++)
	{
		CCouncil
			*Council = (CCouncil*)CouncilTable->get(i);

		if( Council->get_number_of_members() < 15 ) CouncilLessThan15++;
	}

	if (CouncilLessThan15 <= CouncilTable->length()/10)
	{
		// do nothing
	}
	else
	{
		int
			NthCouncil = number(CouncilLessThan15)-1;

		for(int i = 0; i < CouncilTable->length(); i++)
		{
			CCouncil
				*Council = (CCouncil*)CouncilTable->get(i);

			if (Council->get_number_of_members() < 15) NthCouncil--;

			if(!NthCouncil) break;
		}
	}
}
end telecard */

bool
	CPlayer::know_tech( int aTechID )
{
	if (!mTechList.get_by_id(aTechID)) return false;
	return true;
}

void
	CPlayer::forget_tech( int aTechID )
{
	CKnownTech* Tech = (CKnownTech*)mTechList.get_by_id(aTechID);
	if (!Tech) return;

	if (Tech->get_tech()->is_attribute(CTech::ATTR_BASIC)) return;

	mTechList.remove_known_tech(aTechID);
	mTechCM -= Tech->get_effect();

	mTechList.type(QUERY_UPDATE);
	STORE_CENTER->store(mTechList);
}

int
	CPlayer::calc_ship_production()
{
	int
		ret = 0;
	if (has_ability(ABILITY_MILITARISTIC_DOMINANCE))
		ret = (int)( (long long int) ((long long int)(mLastTurnResource.get(RESOURCE_PRODUCTION))*(30+(long long int)(mControlModel.get_military() + 2)*5)) /100);
	else
		ret = (int)( (long long int) ((long long int)(mLastTurnResource.get(RESOURCE_PRODUCTION))*(30+(long long int)(mControlModel.get_military())*5)) /100);
	if( ret < 0 ) return 0;
	return ret;
}

const char*
	CPlayer::independence_declaration(char *aName, const char *aSlogan)
{
	#define PERSONALISM_THEORY 		1111
	#define TOTALISM_THEORY			1110
	#define CLASSISM_THEORY			1109
	// 		check has tech
	// 		basic personalism(1111)
	//		totalism(1120)
	//		classism(1109)

	static CString
		Message;
	Message.clear();

	// send offer

	// remove council
	CCouncil *
	Council = get_council();
	if (Council != NULL)
	{
		SLOG("YOSHIKI : old council id = %d", Council->get_id());
		SLOG("YOSHIKI : old council member 1 = %d", Council->get_members()->length());
		Council->remove_member(get_game_id());
		SLOG("YOSHIKI : old council member 2 = %d", Council->get_members()->length());
	}

	// if old council's honor was higher than player's, reduce his honor by up to 10
	/*if (Council->get_honor() > get_honor())
	{
		int
			HonorLoss = 10;
		if ((mHonor - HonorLoss) < Council->get_honor()) HonorLoss = Council->get_honor() - mHonor;
		change_honor_with_news(-HonorLoss);
	}*/

	// create new council
	CCouncil *NewCouncil = new CCouncil();

	NewCouncil->set_name(aName);
	NewCouncil->set_slogan(aSlogan);
	if (mHonor < 50) NewCouncil->set_honor(mHonor);
	else NewCouncil->set_honor(50);

	SLOG("YOSHIKI : new council id = %d", NewCouncil->get_id());
	SLOG("YOSHIKI : new council member 1 = %d", NewCouncil->get_members()->length());
	NewCouncil->add_member(this);
	SLOG("YOSHIKI : new council member 2 = %d", NewCouncil->get_members()->length());
	NewCouncil->set_speaker(get_game_id());
	NewCouncil->set_home_cluster_id(get_home_cluster_id());

	set_council_donation(0);
	set_council_vote_gain(0);
	set_council_vote(0);
	set_council(NewCouncil);

	COUNCIL_TABLE->add_council(NewCouncil);
	COUNCIL_TABLE->add_council_rank(NewCouncil);

	NewCouncil->type(QUERY_INSERT);
	STORE_CENTER->store(*NewCouncil);

	Message.format(GETTEXT("Now you are the speaker of %1$s."),
		NewCouncil->get_nick());
	Message += "<BR>\n";
	Message.format(GETTEXT("Your council, %1$s, has been created. The slogan is \"%2$s\"."),
		NewCouncil->get_nick(),
		NewCouncil->get_slogan());

	// added by thedaz for create council forum ->
/*
	player.cc -> council 이 생길때 speaker 가 있음
	db: CouncilForum

	catagories 에 COUNCIL_ID 입력ed
	forums 에 COUNCIL_ID 입력ed
	forum_mods 에 speaker 인 사용자 넣기ed (user_id = mGameID)
	users 에 speaker 넣기ed
*/
/*	SLOG("THEDAZ: Create new council - independence declaration");

	char query[256];
	char str[256];
	char *council_name;
	char *passwd = "c5fe25896e49ddfe996db7508cf00534";

	council_name = add_slashes(NewCouncil->get_name());

	MYSQL db;

	mysql_init(&db);

	 ## DB CONNECT ## */
/*	if (!mysql_real_connect(&db, "localhost", "space", "fa75L5oC", "CouncilForum", 0, NULL, 0))
	{
		sprintf(str, "THEDAZ: Failed to connect to database: Error: %s (player.cc)\n", mysql_error(&db));
		SLOG(str);
	}

	sprintf(query, "INSERT INTO catagories (cat_id, cat_title, cat_order) VALUES ('%d', '%s', '1')", NewCouncil->get_id(), council_name);

	 ## INSERT catagories ## */
/*	if (mysql_query(&db, query) == -1)
	{
		sprintf(str, "THEDAZ: Failed to write database: Error: %s (player.cc)\n", mysql_error(&db));
		SLOG(str);
	}
	else
	{
		sprintf(str, "THEDAZ: Successed to write cat table (player.cc) -\t\t%d %s", NewCouncil->get_id(), council_name);
		SLOG(str);
	}

	sprintf(query, "INSERT INTO forums (forum_id, forum_name, forum_access, cat_id) VALUES ('%d', 'free board', '1', '%d')", NewCouncil->get_id(), NewCouncil->get_id());

	 ## INSERT forums ## */
/*	if (mysql_query(&db, query) == -1)
	{
		sprintf(str, "THEDAZ: Failed to write database: Error: %s (player.cc)\n", mysql_error(&db));
		SLOG(str);
	}
	else
	{
		sprintf(str, "THEDAZ: Successed to write forums table (player.cc) -\t\t%d %s", NewCouncil->get_id(), council_name);
		SLOG(str);
	}

	sprintf(query, "INSERT INTO forum_mods (forum_id, user_id, cat_id) VALUES ('%d', '%d', '%d')", NewCouncil->get_id(), mGameID, NewCouncil->get_id());

	 ## INSERT forum_mods ## */
/*	if (mysql_query(&db, query) == -1)
	{
		sprintf(str, "THEDAZ: Failed to write database: Error: %s (player.cc)\n", mysql_error(&db));
		SLOG(str);
	}
	else
	{
		sprintf(str, "THEDAZ: Successed to write forum_mods table (player.cc) -\t%d %s", NewCouncil->get_id(), council_name);
		SLOG(str);
	}

	sprintf(query, "INSERT INTO users (user_id, user_password, user_viewemail, user_level, cat_id) VALUES ('%d', '%s', '0', '2', '%d')", mGameID, passwd, NewCouncil->get_id());

	 ## INSERT users ## */
/*	if (mysql_query(&db, query) == -1)
	{
		sprintf(str, "THEDAZ: Failed to write database: Error: %s (player.cc)\n", mysql_error(&db));
		SLOG(str);
	}
	else
	{
		sprintf(str, "THEDAZ: Successed to write users table (player.cc) -\t\t%d %s", NewCouncil->get_id(), council_name);
		SLOG(str);
	}


	mysql_close(&db);
	// added by thedaz for create council forum <-*/

	return (char *)Message;
}

const char*
	CPlayer::get_honor_description()
{
	if (mHonor <= 5) return GETTEXT("Evil");
	else if (mHonor <= 10) return GETTEXT("Seed of Corruption");
	else if (mHonor <= 15) return GETTEXT("Sadistic");
	else if (mHonor <= 20) return GETTEXT("Black Hearted");
	else if (mHonor <= 25) return GETTEXT("Backstabber");
	else if (mHonor <= 30) return GETTEXT("Undependable");
	else if (mHonor <= 35) return GETTEXT("Untrustworthy");
	else if (mHonor <= 40) return GETTEXT("Unreliable");
	else if (mHonor <= 60) return GETTEXT("Average");
	else if (mHonor <= 65) return GETTEXT("Reliable");
	else if (mHonor <= 70) return GETTEXT("Honest");
	else if (mHonor <= 75) return GETTEXT("Faithful");
	else if (mHonor <= 80) return GETTEXT("Virtuous");
	else if (mHonor <= 85) return GETTEXT("Honorable");
	else if (mHonor <= 90) return GETTEXT("Incorruptible");
	else if (mHonor <= 95) return GETTEXT("Stainless");
	return GETTEXT("Saintly");
}

void
	CPlayer::set_honor(int aHonor)
{
	mStoreFlag += STORE_HONOR;
	mHonor = (aHonor<0) ? 0: (aHonor>100) ? 100: aHonor;
}

void
	CPlayer::return_privateer(CFleet *aFleet)
{
	CMission &Mission = aFleet->get_mission();

	CEngine *
	Engine = (CEngine *)COMPONENT_TABLE->get_by_id(aFleet->get_engine());
	assert(Engine);
	int
		ReturnTime = (9-Engine->get_level())*CGame::mSecondPerTurn;

	CPlayer *
	Player = PLAYER_TABLE->get_by_game_id(Mission.get_target());
	if (Player)
	{
		CPlanetList *
		PlanetList = Player->get_planet_list();
		CPlanet *
		Planet = (CPlanet *)PlanetList->get(number(PlanetList->length())-1);
		if (Planet)
			if (!Player->has_cluster(Planet->get_cluster_id()))
				ReturnTime *= 2;
			else ReturnTime *= 2;
	}
	else
	{
		ReturnTime *= 2;
	}

	if (mAbility.has(ABILITY_FAST_MANEUVER))
	{
		ReturnTime = ReturnTime * 70 / 100;
	}

	ReturnTime = calc_PA(ReturnTime, CPlayerEffect::PA_CHANGE_YOUR_FLEET_RETURN_TIME);
	aFleet->init_mission(CMission::MISSION_RETURNING, 0);
	Mission.set_terminate_time(CGame::get_game_time() + ReturnTime);
	time_news((char*)format(
			GETTEXT("%1$s fleet is returning from privateer mission."),
			aFleet->get_nick()));
}

const char *
	CPlayer::mission_handler()
{
	static CString
		Buf;
	Buf.clear();

	for (int i=mFleetList.length()-1; i>=0; i--)
	{
		CFleet *
		Fleet = (CFleet*)mFleetList.get(i);

		CAdmiral *
		Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());
		if (!Admiral)
		{
			SLOG("WRONG Admiral %d in %s of %s",
				Fleet->get_admiral_id(), Fleet->get_nick(), get_name());
			continue;
		}

		// for breeder male commander type
		if (Fleet->get_status() == CFleet::FLEET_COMMANDER_ABSENT)
		{
			Fleet->set_status( CFleet::FLEET_STAND_BY );
			continue;
		}
		if (Fleet->get_status() == CFleet::FLEET_STAND_BY)
		{
			if (Admiral->get_racial_ability() == CAdmiral::RA_BREEDER_MALE && number(50) == 1)
			{
				Fleet->set_status( CFleet::FLEET_COMMANDER_ABSENT );
				continue;
			}
		}

		if (Fleet->get_status() == CFleet::FLEET_PRIVATEER)
		{
			CMission &
			Mission = Fleet->get_mission();
			if (Mission.is_over())
			{
				ADMIN_TOOL->add_privateer_log(
					(char *)format("The fleet %s, whose owner is the player %s, doesn't have the privateer mission anymore, so it's returning.",
						Fleet->get_nick(), (char *)mNick));

				return_privateer(Fleet);
			}
			else
			{
				privateer(Fleet);
			}
		}

		if (!Fleet->under_mission()) continue;

		CMission &Mission = Fleet->get_mission();

		if (Mission.get_mission() == CMission::MISSION_PATROL)
			Admiral->gain_exp(CMission::mAdmiralExpPatrol);

		if (Mission.get_mission() == CMission::MISSION_STATION_ON_PLANET)
			if (get_turn()) Admiral->gain_exp(CMission::mAdmiralExpStation);

		if (Mission.get_mission() == CMission::MISSION_TRAIN)
		{
			if (get_turn())
			{

				if(Admiral->get_level() < CMission::mTrainMaxLevel)
				{
					Admiral->gain_exp(CMission::mAdmiralExpTrain);
				}
				Fleet->change_exp(CMission::mFleetExpTrain);
			}
		}

		if (Mission.is_over() && Mission.get_mission() == CMission::MISSION_EXPEDITION)
		{
			if ((Mission.get_terminate_time() == 0) && (CGame::mUpdateTurn)) Mission.set_terminate_time(CGame::get_game_time() + CMission::mExpeditionMissionTime);
			Admiral->gain_exp(CMission::mAdmiralExpExpedition);
			int
				ClusterID = find_new_planet();
			if (ClusterID > 0)
			{
				Buf.format(GETTEXT("Your fleet %1$s has found a new planet!"),
					Fleet->get_name());
				Buf += "<BR>\n";
				Buf += GETTEXT("They are returning with the information.");
				Buf += "<BR>\n";

				CPlanet
					*Planet = new CPlanet();
				assert(Planet);
				CCluster
					*Cluster = UNIVERSE->get_by_id(ClusterID);
				assert(Cluster);

				Planet->initialize();
				Planet->set_cluster(Cluster);
				Planet->set_name(Cluster->get_new_planet_name());
				add_planet(Planet);
				PLANET_TABLE->add_planet(Planet);
				Planet->start_terraforming();

				STORE_CENTER->store(*Planet);

				new_planet_news(Planet);

				Fleet->init_mission(CMission::MISSION_RETURNING_WITH_PLANET, 0);
			}
			else
			{
				Fleet->init_mission(CMission::MISSION_EXPEDITION, 0);
			}
		}

		if (Mission.is_over())
		{
			switch(Mission.get_mission())
			{
				/*case CMission::MISSION_TRAIN :
				{
					int
						NewExp = 10 + (mControlModel.get_military() * 2);
					if (NewExp < CMission::mFleetExpMinTrain) NewExp = CMission::mFleetExpMinTrain;
					Fleet->change_exp(NewExp);
					Buf.format(GETTEXT("Your fleet %1$s has gained %2$s points of exp."),
								Fleet->get_name(),
								dec2unit(NewExp));
					int
						AdmiralExp = Fleet->get_current_ship() *
										(Fleet->get_body()-4000) *
										(mControlModel.get_military() + 5);

					if(Admiral->get_level() < 13)
					{
						if (AdmiralExp < CMission::mAdmiralExpMinTrain + 500* Admiral->get_level()) AdmiralExp = CMission::mAdmiralExpMinTrain + 500* Admiral->get_level();
						Admiral->gain_exp(AdmiralExp);
					}

					Buf.format(GETTEXT("and %1$s has gained %2$s points of exp. from the training."),
						Admiral->get_name(),
						dec2unit(AdmiralExp));
					Buf += "<BR>\n";
				}
				break;*/

				case CMission::MISSION_PATROL :
					{
						CPlanet *
						Planet = mPlanetList.get_by_id(Mission.get_target());
						if (Planet)
						{
							Buf.format(GETTEXT("Your fleet %1$s has returned from the mission 'Patrol on the Planet %2$s'."),
								Fleet->get_name(),
								Planet->get_name());
							Buf += "<BR>\n";
						}
						else
						{
							Buf.format(GETTEXT("Your fleet %1$s has returned from the mission 'Patrol'."),
								Fleet->get_name());
							Buf += "<BR>\n";
						}
					}
					break;

				case CMission::MISSION_DISPATCH_TO_ALLY :
					{
						CPlayer *
						Ally = PLAYER_TABLE->get_by_game_id(Mission.get_target());
						if (Ally)
						{
							Buf.format(GETTEXT("Your fleet %1$s has returned from the ally %2$s."),
								Fleet->get_name(),
								Ally->get_nick());
							Buf += "<BR>\n";
						}
						else
						{
							Buf.format(GETTEXT("Your fleet %1$s has returned from the ally."),
								Fleet->get_name());
							Buf += "<BR>\n";
						}
					}
					break;
				case CMission::MISSION_SORTIE:
					{
						Buf.format(
							GETTEXT("Your fleet %1$s has returned without doing anything"
								" because you have not given any commands in 2 turns."),
							Fleet->get_name());
						Buf += "<BR>\n";
					}
					break;

				case CMission::MISSION_RETURNING :
					{
						Buf.format(GETTEXT("Your fleet %1$s has returned from a mission."),
							Fleet->get_name());
						Buf += "<BR>\n";

					}
					break;

				case CMission::MISSION_RETURNING_WITH_PLANET :
					{
						Buf.format(GETTEXT("Your fleet %1$s has returned with a new planet."),
							Fleet->get_name());
						Buf += "<BR>\n";
					}
					break;

				case CMission::MISSION_PRIVATEER:
				case CMission::MISSION_EXPEDITION:
				case CMission::MISSION_NONE:
				case CMission::MISSION_STATION_ON_PLANET:
				case CMission::MISSION_TRAIN:
					break;
			}
			Fleet->end_mission();
		}
	}
	return (char*)Buf;
}

int
	CPlayer::find_new_planet(bool aAlways)
{
	static int
		Chance[] = {
		10000,	// 0
		10000,	// 1
		7500,	// 2
		5000,	// 3
		5000,	// 4
		2833,	// 5
		2300,	// 6
		1909,	// 7
		1608,	// 8
		1366,	// 9
		1166,	// 10
		996,	// 11
		850,	// 12
		720,	// 13
		604,	// 14
		500,	// 15
		404,	// 16
		315,	// 17
		233,	// 18
		156,	// 19
		83	// 20
	};

	CCluster *Cluster =
		(CCluster*)mClusterList.get(number(mClusterList.length())-1);

	int nPlanet;
	if( Cluster )
		nPlanet =
			mPlanetList.count_planet_from_cluster(Cluster->get_id());
	else
		return get_home_cluster_id();

	int
		NumberOfPlayerPlanet = Cluster->get_planet_count() - CEmpire::mInitialNumberOfMagistratePlanet;

	if (Cluster->get_player_count()*20 < NumberOfPlayerPlanet || nPlanet > 20)
	{
		return -1;
	}
	else if (aAlways == true || number(10000) <= Chance[nPlanet])
	{
		return Cluster->get_id();
	}
	else
	{
		return -1;
	}
}

void
	CPlayer::change_honor(int aChange)
{
	int
		Diplomacy = mControlModel.get_diplomacy();
	int
		Change = aChange;
	if (Change < 0)
	{
		Change += Diplomacy;
		if (Change < aChange*2) Change = aChange*2;
		if (Change >= 0) Change = -1;
	}
	else if (Change > 0)
	{
		Change += Diplomacy;
		if (Change > aChange*2) Change = aChange*2;
		if (Change <= 0) Change = 1;
	}

	if ((mHonor == 100) && (Change < 0))
		Change = (Change < -1) ? (int)(Change/2):-1;
	if ((mHonor == 0) && (Change > 0))
		Change = (Change > 1) ? (int)(Change/2):1;

	set_honor(mHonor+Change);
}

void
	CPlayer::change_honor_with_news(int aChange)
{
	CString
		Honor = get_honor_description();

//	int
//		OldHonor = mHonor; //unused

	change_honor(aChange);

	if (!Honor.compare(get_honor_description()))
	{
		CString News;
		News.format(GETTEXT("Your honor has become %1$s by your breach of faith."),
			get_honor_description());
		//News.format("SUPPOSED HONOR LOSS: %d, REAL HONOR LOSS: %d", aChange, OldHonor-mHonor); // debug
		time_news((char*)News);
	}
}

void
	CPlayer::change_research_invest(int aAmount)
{
	mStoreFlag += STORE_RESEARCH_INVEST;
	safe_fast_change_type<int,MAX_RESEARCH_INVESTMENT_PP>(aAmount,mResearchInvest);
}

void
	CPlayer::add_action(CPlayerAction* aAction)
{
	mActionList.add_action(aAction);
}

void
	CPlayer::remove_action(int aActionID)
{
	mActionList.remove_action(aActionID);
}


bool
	CPlayer::check_council_donation_effect_timer()
{
	return (bool)mActionList.get_council_donation_action();
}

void
	CPlayer::set_council_donation_effect_timer()
{
	CPlayerActionCouncilDonation *Action
		= new CPlayerActionCouncilDonation();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	PLAYER_ACTION_TABLE->add_action(Action);
}

CPlayerAction*
CPlayer::check_break_ally_effect_timer(CPlayer *aPlayer)
{
	return mActionList.get_break_ally_action(aPlayer->get_game_id());
}

void
	CPlayer::set_break_ally_effect_timer(CPlayer *aPlayer)
{
	CPlayerActionBreakAlly *Action = new CPlayerActionBreakAlly();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->set_argument(aPlayer->get_game_id());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	PLAYER_ACTION_TABLE->add_action(Action);
}

CPlayerAction*
CPlayer::check_declare_hostile_effect_timer(CPlayer *aPlayer)
{
	return mActionList.get_declare_hostile_action(aPlayer->get_game_id());
}

void
	CPlayer::set_declare_hostile_effect_timer(CPlayer *aPlayer)
{
	CPlayerActionDeclareHostile *Action = new CPlayerActionDeclareHostile();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->set_argument(aPlayer->get_game_id());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	PLAYER_ACTION_TABLE->add_action(Action);
}


CPlayerAction*
CPlayer::check_break_pact_effect_timer(CPlayer *aPlayer)
{
	return mActionList.get_break_pact_action(aPlayer->get_game_id());
}

void
	CPlayer::set_break_pact_effect_timer(CPlayer *aPlayer)
{
	CPlayerActionBreakPact *Action = new CPlayerActionBreakPact();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->set_argument(aPlayer->get_game_id());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	PLAYER_ACTION_TABLE->add_action(Action);
}

void
	CPlayer::add_relation(CPlayerRelation* aRelation)
{
	mRelationList.add_relation(aRelation);
}

void
	CPlayer::remove_relation_non_bounty_all()
{
	if (mRelationList.length() == 0) return;

	for (int i=mRelationList.length()-1 ; i>=0 ; i--)
	{
		CPlayerRelation *
		Relation = (CPlayerRelation *)mRelationList.get(i);
		if (Relation->get_relation() == CRelation::RELATION_BOUNTY) continue;
		CPlayer *
		TargetPlayer;
		if (Relation->get_player1()->get_game_id() == mGameID)
		{
			TargetPlayer = Relation->get_player2();
		}
		else if (Relation->get_player2()->get_game_id() == mGameID)
		{
			TargetPlayer = Relation->get_player1();
		}
		else
		{
			SLOG("ERROR : Wrong Relation in CPlayer::remove_non_bounty_relation_all(), Relation->get_id() = %d, Relation->get_player1()->get_game_id() = %d, Relation->get_player2()->get_game_id() = %d, mGameID = %d", Relation->get_id(), Relation->get_player1()->get_game_id(), Relation->get_player2()->get_game_id(), mGameID);
			continue;
		}

		TargetPlayer->remove_relation(Relation->get_id());
		remove_relation(Relation->get_id());

		Relation->type(QUERY_DELETE);
		STORE_CENTER->store(*Relation);

		PLAYER_RELATION_TABLE->remove_relation(Relation->get_id());
	}
}

void
	CPlayer::remove_relation_all()
{
	if (mRelationList.length() == 0) return;

	for (int i=mRelationList.length()-1 ; i>=0 ; i--)
	{
		CPlayerRelation *
		Relation = (CPlayerRelation *)mRelationList.get(i);
		CPlayer *
		TargetPlayer;
		if (Relation->get_player1()->get_game_id() == mGameID)
		{
			TargetPlayer = Relation->get_player2();
		}
		else if (Relation->get_player2()->get_game_id() == mGameID)
		{
			TargetPlayer = Relation->get_player1();
		}
		else
		{
			SLOG("ERROR : Wrong Relation in CPlayer::remove_relation_all(), Relation->get_id() = %d, Relation->get_player1()->get_game_id() = %d, Relation->get_player2()->get_game_id() = %d, mGameID = %d", Relation->get_id(), Relation->get_player1()->get_game_id(), Relation->get_player2()->get_game_id(), mGameID);
			continue;
		}

		TargetPlayer->remove_relation(Relation->get_id());
		remove_relation(Relation->get_id());

		Relation->type(QUERY_DELETE);
		STORE_CENTER->store(*Relation);

		PLAYER_RELATION_TABLE->remove_relation(Relation->get_id());
	}
}

void
	CPlayer::remove_relation(int aRelationID)
{
	mRelationList.remove_relation(aRelationID);
}

void
	CPlayer::remove_relation_by_target_id(int aTargetID)
{
	mRelationList.remove_player_relation_by_target_id(aTargetID);
}

void
	CPlayer::add_diplomatic_message(CDiplomaticMessage* aMessage)
{
	mMessageBox.add_diplomatic_message(aMessage);

	// Check for auto-reply
	CDiplomaticMessage::EType MessageType;

	bool SendReply = false;

	if (mPreference && !is_dead())
	{
		switch ((int)aMessage->get_type())
		{
			case (int)CDiplomaticMessage::TYPE_SUGGEST_PACT:
				if (mPreference->has(CPreference::PR_ACCEPT_PACT))
				{
					MessageType = CDiplomaticMessage::TYPE_REPLY_SUGGEST_PACT;
					SendReply = true;
				}
				break;
			case (int)CDiplomaticMessage::TYPE_SUGGEST_ALLY:
				if (mPreference->has(CPreference::PR_ACCEPT_ALLY))
				{
					MessageType = CDiplomaticMessage::TYPE_REPLY_SUGGEST_ALLY;
					SendReply = true;
				}
				break;
			case (int)CDiplomaticMessage::TYPE_SUGGEST_TRUCE:
				if (mPreference->has(CPreference::PR_ACCEPT_TRUCE))
				{
					MessageType = CDiplomaticMessage::TYPE_REPLY_SUGGEST_TRUCE;
					SendReply = true;
				}
				break;
			default: break;
		};

		if ((SendReply == true) && (aMessage->get_status() != CDiplomaticMessage::STATUS_ANSWERED))
		{
       // update the current message
			improve_relationship(aMessage->get_type(), aMessage->get_sender());
			aMessage->set_status(CDiplomaticMessage::STATUS_ANSWERED);
			aMessage->type(QUERY_UPDATE);
			STORE_CENTER->store(*aMessage);
       // send the reply
			CDiplomaticMessage *SMessage = new CDiplomaticMessage();
			SMessage->initialize(MessageType, this, aMessage->get_sender(), "Auto-Reply: Accept");
			aMessage->get_sender()->add_diplomatic_message(SMessage);
			SMessage->type(QUERY_INSERT);
			STORE_CENTER->store(*SMessage);
		}
	}
}

CDiplomaticMessage *
CPlayer::get_diplomatic_message(int aMessageID)
{
	return mMessageBox.get_by_id(aMessageID);
}

void
	CPlayer::remove_diplomatic_message(int aMessageID)
{
	mMessageBox.remove_diplomatic_message(aMessageID);
}

void
	CPlayer::remove_from_database()
{
	mPlanetInvestPool = 0;
	STORE_CENTER->query("player",
		(char *)format("DELETE FROM player WHERE game_id = %d", get_game_id()));

	for (int i=mPlanetList.length()-1 ; i>=0 ; i--)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);

		CEmpirePlanetInfo *
		EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(Planet->get_id());
		if (EmpirePlanetInfo == NULL)
		{
			Planet->type(QUERY_DELETE);
			STORE_CENTER->store(*Planet);
		}
	}

	STORE_CENTER->query("tech",
		(char *)format("DELETE FROM tech WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("project",
		(char *)format("DELETE FROM project WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("admiral",
		(char *)format("DELETE FROM admiral WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("diplomatic_message",
		(char *)format("DELETE FROM diplomatic_message WHERE sender = %d or receiver = %d",
			get_game_id(), get_game_id()));

	STORE_CENTER->query("player_action",
		(char *)format("DELETE FROM player_action WHERE owner = %d", get_game_id()));

	if (mAdmission != NULL)
	{
		STORE_CENTER->query("admission",
			(char *)format("DELETE FROM admission WHERE player = %d", get_game_id()));
	}

	STORE_CENTER->query("class",
		(char *)format("DELETE FROM class WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("ship_building_q",
		(char *)format("DELETE FROM ship_building_q WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("docked_ship",
		(char *)format("DELETE FROM docked_ship WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("damaged_ship",
		(char *)format("DELETE FROM damaged_ship WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("fleet",
		(char *)format("DELETE FROM fleet WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("defense_fleet",
		(char *)format("DELETE FROM defense_fleet WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("plan",
		(char *)format("DELETE FROM plan WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("player_relation",
		(char *)format("DELETE FROM player_relation WHERE player1 = %d or player2 = %d",
			get_game_id(), get_game_id()));

	STORE_CENTER->query("damage",
		(char *)format("DELETE FROM damage WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("empire_action",
		(char *)format("DELETE FROM empire_action WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("player_effect",
		(char *)format("DELETE FROM player_effect WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("player_event",
		(char *)format("DELETE FROM player_event WHERE owner = %d", get_game_id()));

	STORE_CENTER->query("player_pref",
		(char *)format("DELETE FROM player_pref WHERE player_id = %d", get_game_id()));

}

void
	CPlayer::remove_from_memory()
{
	for (int i=mPlanetList.length()-1 ; i>=0 ; i--)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);

		CEmpirePlanetInfo *
		EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(Planet->get_id());
		if (EmpirePlanetInfo == NULL)
		{
			drop_planet(Planet);
			PLANET_TABLE->remove_planet(Planet->get_id());
		}
		else
		{
			if (EmpirePlanetInfo->get_planet_type() == CEmpire::LAYER_MAGISTRATE)
			{
				int
					ClusterID = EmpirePlanetInfo->get_position_arg();
				CMagistrate *
				Magistrate = MAGISTRATE_LIST->get_by_cluster_id(ClusterID);
				if (Magistrate == NULL)
				{
					drop_planet(Planet);

					Planet->type(QUERY_DELETE);
					STORE_CENTER->store(*Planet);

					PLANET_TABLE->remove_planet(Planet->get_id());
				}
				else
				{
					drop_planet(Planet);

					Planet->set_owner(EMPIRE);
					Magistrate->add_planet(Planet);

					EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);

					EmpirePlanetInfo->type(QUERY_UPDATE);
					STORE_CENTER->store(*EmpirePlanetInfo);

					Planet->type(QUERY_UPDATE);
					STORE_CENTER->store(*Planet);
				}
			}
			else if (EmpirePlanetInfo->get_planet_type() == CEmpire::LAYER_EMPIRE_PLANET)
			{
				drop_planet(Planet);

				Planet->set_owner(EMPIRE);
				EMPIRE->add_empire_planet(Planet);

				EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);

				EmpirePlanetInfo->type(QUERY_UPDATE);
				STORE_CENTER->store(*EmpirePlanetInfo);

				Planet->type(QUERY_UPDATE);
				STORE_CENTER->store(*Planet);
			}
			else
			{
				SLOG("ERROR : Wrong planet type in CPlayer::remove_from_memory(), EmpirePlanetInfo->get_planet_type() = %d, Planet->get_id() = %d", EmpirePlanetInfo->get_planet_type(), Planet->get_id());
			}
		}
	}

	mTechList.remove_all();

	mPurchasedProjectList.remove_all();

	mAdmiralList.remove_all();
	mAdmiralPool.remove_all();

	CCouncil *
	Council = get_council();
	if (Council != NULL)
	{
		Council->remove_member(get_game_id());
		if (Council->get_number_of_members() == 0)
		{
			COUNCIL_TABLE->remove_council(Council->get_id());
		}
	}
	mCouncilID = 0;

	mMessageBox.remove_all();

	for(int i=mActionList.length()-1 ; i>=0 ; i--)
	{
		CPlayerAction *
		Action = (CPlayerAction *)mActionList.get(i);

		mActionList.remove_action(Action->get_id());
		PLAYER_ACTION_TABLE->remove_action(Action->get_id());
	}

	if (mAdmission != NULL)
	{
		mAdmission->get_council()->remove_admission(this);
		delete mAdmission;
		mAdmission = NULL;
	}

	mShipDesignList.remove_all();

	mShipBuildQ.remove_all();

	mDock.remove_all();

	mRepairBay.remove_all();

	mFleetList.remove_all();

	mDefensePlanList.remove_all();

	mRelationList.remove_all();

	mEmpireActionList.remove_all();

	mEffectList.remove_all();

	mEventList.remove_all();
}

void
	CPlayer::remove_news_files()
{
	char
		GameIDString[100];
	CString
		NewsFile,
		FileNameBase;

	sprintf(GameIDString, "%d", mGameID);

	FileNameBase.format("%s/%c",
		ARCHSPACE->configuration().get_string("Game", "NewsFileDir"),
		GameIDString[0]);
	if (GameIDString[1] != '\0')
	{
		FileNameBase.format("/%c/%d", GameIDString[1], mGameID);
	}
	else
	{
		FileNameBase.format("/0/%d", mGameID);
	}

	NewsFile.format("%s.ability", (char *)FileNameBase);
	unlink((char *)NewsFile);

	NewsFile.clear();
	NewsFile.format("%s.tech", (char *)FileNameBase);
	unlink((char *)NewsFile);

	NewsFile.clear();
	NewsFile.format("%s.planet", (char *)FileNameBase);
	unlink((char *)NewsFile);

	NewsFile.clear();
	NewsFile.format("%s.project", (char *)FileNameBase);
	unlink((char *)NewsFile);

	NewsFile.clear();
	NewsFile.format("%s.admiral", (char *)FileNameBase);
	unlink((char *)NewsFile);

	NewsFile.clear();
	NewsFile.format("%s.time", (char *)FileNameBase);
	unlink((char *)NewsFile);
}

CPlayerRelation *
CPlayer::get_relation(CPlayer* aPlayer)
{
	CPlayerRelation *
	Relation = mRelationList.find_by_target_id(aPlayer->get_game_id());

	return Relation;
}

const char*
	CPlayer::diplomatic_message_select_html(CPlayer *aPlayer)
{
	static CString
		HTML;
	HTML.clear();

	CPlayerRelation *
	Relation = get_relation(aPlayer);
	if(!Relation)
	{
		Relation = new CPlayerRelation(this, aPlayer,
				CRelation::RELATION_TRUCE);

		aPlayer->add_relation(Relation);
		add_relation(Relation);
		PLAYER_RELATION_TABLE->add_relation(Relation);
		Relation->type(QUERY_INSERT);
		STORE_CENTER->store(*Relation);
	}

	if(Relation->get_relation() == CRelation::RELATION_HOSTILE)
	{
		HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";

		HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
			CDiplomaticMessage::TYPE_NORMAL,
			CDiplomaticMessage::get_type_description(CDiplomaticMessage::TYPE_NORMAL));

		HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
			CDiplomaticMessage::TYPE_SUGGEST_TRUCE,
			CDiplomaticMessage::get_type_description(CDiplomaticMessage::TYPE_SUGGEST_TRUCE));

		HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
			CDiplomaticMessage::TYPE_DEMAND_TRUCE,
			CDiplomaticMessage::get_type_description(CDiplomaticMessage::TYPE_DEMAND_TRUCE));
		HTML += "</SELECT>\n";
	}

	if (get_council_id() != aPlayer->get_council_id())
	{


		HTML = "<SELECT NAME=\"MESSAGE_TYPE\">\n";
		HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
			CDiplomaticMessage::TYPE_NORMAL,
			CDiplomaticMessage::get_type_description(CDiplomaticMessage::TYPE_NORMAL));
		if(Relation->get_relation() == CRelation::RELATION_TRUCE || Relation->get_relation() == CRelation::RELATION_NONE)
		{
			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
				CDiplomaticMessage::TYPE_DECLARE_HOSTILITY,
				CDiplomaticMessage::get_type_description(CDiplomaticMessage::TYPE_DECLARE_HOSTILITY));
		}
		if(Relation->get_relation() == CRelation::RELATION_HOSTILE)
		{
			HTML.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
				CDiplomaticMessage::TYPE_SUGGEST_TRUCE,
				CDiplomaticMessage::get_type_description(CDiplomaticMessage::TYPE_SUGGEST_TRUCE));
		}
		HTML += "</SELECT>\n";

	}
	else
	{


		if (Relation)
			HTML = CDiplomaticMessage::message_select_html(Relation->get_relation());
		else
			HTML = CDiplomaticMessage::message_select_html(CRelation::RELATION_TRUCE);
	}
	if (!HTML) return " ";
	return (const char*)HTML;
}

bool
	CPlayer::check_diplomatic_message(CDiplomaticMessage::EType aType, CPlayer* aPlayer)
{
	if (aType == CDiplomaticMessage::TYPE_NORMAL) return true;

	if (aPlayer->is_dead() == true) return false;

	if(aType == CDiplomaticMessage::TYPE_DECLARE_HOSTILITY)
	{
		return true;
	}

	if(aPlayer->get_council()->get_id() != get_council()->get_id() )
	{
		if((aType == CDiplomaticMessage::TYPE_DECLARE_HOSTILITY ) || (aType == CDiplomaticMessage::TYPE_SUGGEST_TRUCE ))
		{
		}
		else
		{
			return false;
		}
	}

	CPlayerRelation *
	Relation = get_relation(aPlayer);

	CRelation::ERelation RelationShip;

	if (!Relation)
		RelationShip = CRelation::RELATION_TRUCE;
	else
		RelationShip = Relation->get_relation();

	if (RelationShip == CRelation::RELATION_NONE)
	{
		Relation->set_relation(CRelation::RELATION_TRUCE);
	}

	if (RelationShip == CRelation::RELATION_WAR)
	{
		if (aType == CDiplomaticMessage::TYPE_SUGGEST_TRUCE)
			return true;
		if (aType == CDiplomaticMessage::TYPE_DEMAND_TRUCE)
			return true;
	} else if (RelationShip == CRelation::RELATION_TRUCE)
	{
		if (aType == CDiplomaticMessage::TYPE_DECLARE_WAR)
			return true;
		if (aType == CDiplomaticMessage::TYPE_SUGGEST_PACT)
			return true;
		if (aType == CDiplomaticMessage::TYPE_DECLARE_HOSTILITY)
			return true;
	} else if (RelationShip == CRelation::RELATION_PEACE)
	{
		if (aType == CDiplomaticMessage::TYPE_BREAK_PACT)
			return true;
		if (aType == CDiplomaticMessage::TYPE_SUGGEST_ALLY)
			return true;
	} else if (RelationShip == CRelation::RELATION_ALLY)
	{
		if (aType == CDiplomaticMessage::TYPE_BREAK_ALLY)
			return true;
	}else if (RelationShip == CRelation::RELATION_HOSTILE)
	{
		if (aType == CDiplomaticMessage::TYPE_SUGGEST_TRUCE)
			return true;
		if (aType == CDiplomaticMessage::TYPE_DECLARE_WAR)
			return true;
	}

	return false;
}

bool
	CPlayer::check_duplication_diplomatic_message(CDiplomaticMessage::EType aType, CPlayer *aPlayer)
{
	if (CDiplomaticMessage::check_need_required(aType) == false) return false;

	for (int i=0 ; i<mMessageBox.length() ; i++)
	{
		CDiplomaticMessage *
		Message = (CDiplomaticMessage*)mMessageBox.get(i);
		if (Message->get_type() == aType && aPlayer == Message->get_sender())
		{
			if (Message->check_need_required() == true)
			{
				return true;
			}
		}
	}
	return false;
}

const char *
	CPlayer::diplomatic_unread_message_table_html()
{
	if (!mMessageBox.length()) return NULL;

	static CString
		Table;
	Table.clear();

	for(int i=0; i<mMessageBox.length(); i++)
	{
		CDiplomaticMessage *Message =
			(CDiplomaticMessage*)mMessageBox.get(i);

		if (Message->get_status() == CDiplomaticMessage::STATUS_UNREAD)
			Table += Message->record_html();
	}

	if (!Table.length()) return NULL;

	return (char*)Table;
}

void
	CPlayer::declare_hostile(CPlayer *aPlayer)
{
	CPlayerRelation* Relation
		= mRelationList.find_by_target_id(aPlayer->get_game_id());

	if (Relation)
	{
		if (Relation->get_relation() != CRelation::RELATION_TRUCE)
		{
			SLOG("WARNING, call declare in NON TRUCE");
			return;
		}
		Relation->set_relation(CRelation::RELATION_HOSTILE);
		Relation->set_time(CGame::get_game_time());
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
	} else {
		Relation = new CPlayerRelation(this, aPlayer,
				CRelation::RELATION_HOSTILE);

		aPlayer->add_relation(Relation);
		add_relation(Relation);
		PLAYER_RELATION_TABLE->add_relation(Relation);
		Relation->type(QUERY_INSERT);
		STORE_CENTER->store(*Relation);
	}

	// new honor loss punishment system
	int	HonorLoss = 0;
/*	int	AttackRatio = (long long int)aPlayer->get_power()*100/(long long int)get_power();

	if (AttackRatio < 60) HonorLoss += 4;
	else if (AttackRatio < 70) HonorLoss += 3;
	else if (AttackRatio < 80) HonorLoss += 2;
	else if (AttackRatio < 90) HonorLoss += 1;
	else if (AttackRatio < 100) HonorLoss += 0;
*/
	// modify the loss by the difference of honor divided by 6
//	HonorLoss += (int)((aPlayer->get_honor()-get_honor())/24);

	// if the player has broken a pact recently
	CPlayerAction *Action = check_break_pact_effect_timer(aPlayer);
	if (Action)
	{
		if (Action->get_start_time() <= PERIOD_24HOURS*CGame::mSecondPerTurn)
			HonorLoss += 2;
		else if (Action->get_start_time() <= PERIOD_48HOURS*CGame::mSecondPerTurn)
			HonorLoss += 1;
		else // over 48 hours
			HonorLoss += 0;

		Action->type(QUERY_DELETE);
		STORE_CENTER->store(*Action);
		remove_action(Action->get_id());
		PLAYER_ACTION_TABLE->remove_action(Action->get_id());
	}

	// additional honor loss for pacifists
	//if (mAbility.has(ABILITY_PACIFIST)) HonorLoss += 2;

	set_declare_hostile_effect_timer(aPlayer);

	if (HonorLoss > 0) change_honor_with_news(-HonorLoss);

	time_news((char*)format(GETTEXT("You have declared hostile intents toward %1$s."),
			aPlayer->get_nick()));


	aPlayer->time_news((char*)format(GETTEXT("%1$s has declared hostile intents toward you."),
			get_nick()));
}

void
	CPlayer::declare_war(CPlayer *aPlayer)
{
	CPlayerRelation* Relation
		= mRelationList.find_by_target_id(aPlayer->get_game_id());

	if (Relation)
	{
		if (Relation->get_relation() != CRelation::RELATION_TRUCE &&
			Relation->get_relation() != CRelation::RELATION_HOSTILE)
		{
			SLOG("WARNING, call declare in NON TRUCE and NON HOSTILE");
			return;
		}
		Relation->set_relation(CRelation::RELATION_WAR);
		Relation->set_time(CGame::get_game_time());
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
	} else {
		Relation = new CPlayerRelation(this, aPlayer,
				CRelation::RELATION_WAR);

		aPlayer->add_relation(Relation);
		add_relation(Relation);
		PLAYER_RELATION_TABLE->add_relation(Relation);
		Relation->type(QUERY_INSERT);
		STORE_CENTER->store(*Relation);
	}

	// new honor loss punishment system
	int	HonorLoss = 0;
	int	AttackRatio = (long long int)aPlayer->get_power()*100/(long long int)get_power();

	if (AttackRatio < 60) HonorLoss += 4;
	else if (AttackRatio < 70) HonorLoss += 3;
	else if (AttackRatio < 80) HonorLoss += 2;
	else if (AttackRatio < 90) HonorLoss += 1;
	else if (AttackRatio < 100) HonorLoss += 0;

	// modify the loss by the difference of honor divided by 6
	HonorLoss += (int)((aPlayer->get_honor()-get_honor())/24);

	// if the player has broken a pact recently
	CPlayerAction *Action = check_break_pact_effect_timer(aPlayer);
	if (Action)
	{
		if (Action->get_start_time() <= PERIOD_24HOURS*CGame::mSecondPerTurn)
			HonorLoss += 2;
		else if (Action->get_start_time() <= PERIOD_48HOURS*CGame::mSecondPerTurn)
			HonorLoss += 1;
		else // over 48 hours
			HonorLoss += 0;

		Action->type(QUERY_DELETE);
		STORE_CENTER->store(*Action);
		remove_action(Action->get_id());
		PLAYER_ACTION_TABLE->remove_action(Action->get_id());
	}

	// additional honor loss for pacifists
	if (mAbility.has(ABILITY_PACIFIST)) HonorLoss += 2;

	if (HonorLoss > 0) change_honor_with_news(-HonorLoss);

	time_news((char*)format(GETTEXT("You have declared war with %1$s."),
			aPlayer->get_nick()));


	aPlayer->time_news((char*)format(GETTEXT("%1$s has declared war with you."),
			get_nick()));
}

void
	CPlayer::break_pact(CPlayer *aPlayer)
{
	CPlayerRelation* Relation
		= mRelationList.find_by_target_id(aPlayer->get_game_id());

	if (Relation)
	{
		if (Relation->get_relation() != CRelation::RELATION_PEACE)
		{
			SLOG("WARNING call break pact in NON PEACE");
			return;
		}
		Relation->set_relation(CRelation::RELATION_TRUCE);
		Relation->set_time(CGame::get_game_time());
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
	} else {
		SLOG("WARNING call break pact in NON_RELATION");
		return;
	}

	int
		HonorLoss = 0;

	// modify the loss by the difference of honor divided by 6
	HonorLoss += (int)((aPlayer->get_honor()-get_honor())/24);

	// if the player has broken an alliance recently
	CPlayerAction *Action = check_break_ally_effect_timer(aPlayer);
	if (Action)
	{
		if (Action->get_start_time() <= PERIOD_24HOURS*CGame::mSecondPerTurn)
			HonorLoss += 2;
		else if (Action->get_start_time() <= PERIOD_48HOURS*CGame::mSecondPerTurn)
			HonorLoss += 1;
		else // over 48 hours
			HonorLoss += 0;

		Action->type(QUERY_DELETE);
		STORE_CENTER->store(*Action);
		remove_action(Action->get_id());
		PLAYER_ACTION_TABLE->remove_action(Action->get_id());
	}

	if (HonorLoss > 0) change_honor_with_news(-HonorLoss);

	set_break_pact_effect_timer(aPlayer);

	time_news(
		(char *)format(GETTEXT("You have broken the pact with %1$s."),
			aPlayer->get_nick()));
	aPlayer->time_news(
		(char *)format(GETTEXT("%1$s has broken the pact."),
			get_nick()));

	CPlayer::clear_commerce_between(this, aPlayer);
}

void
	CPlayer::break_ally(CPlayer *aPlayer)
{
	CPlayerRelation *
	Relation = mRelationList.find_by_target_id(aPlayer->get_game_id());

	if (Relation == NULL)
	{
		SLOG("ERROR : Relation is NULL in CPlayer::break_ally()");
		return;
	}

	if (Relation->get_relation() != CRelation::RELATION_ALLY)
	{
		SLOG("ERROR : Relation->get_relation() != CRelation::RELATION_ALLY in CPlayer::break_ally()");
		return;
	}

	Relation->set_relation(CRelation::RELATION_PEACE);
	Relation->set_time(CGame::get_game_time());
	Relation->type(QUERY_UPDATE);
	STORE_CENTER->store(*Relation);

	int
		HonorLoss = 0;

	// modify the loss by the difference of honor divided by 6
	HonorLoss += (int)((aPlayer->get_honor()-get_honor())/12);

	if (HonorLoss > 0) change_honor_with_news(-HonorLoss);

	set_break_ally_effect_timer(aPlayer);

	for (int i=mAllyFleetList.length()-1 ; i>=0 ; i--)
	{
		CFleet *
		Fleet = (CFleet *)mAllyFleetList.get(i);
		if (Fleet->get_owner() == aPlayer->get_game_id())
		{
			Fleet->end_mission();

			aPlayer->time_news(
				(char *)format(GETTEXT("The fleet %1$s is returning from your ally %2$s because the alliance has been broken."),
					Fleet->get_nick(), (char *)mNick));
		}
	}

	CAllyFleetList *
	AllyFleetList = aPlayer->get_ally_fleet_list();
	for (int i=AllyFleetList->length()-1 ; i>=0 ; i--)
	{
		CFleet *
		Fleet = (CFleet *)AllyFleetList->get(i);
		if (Fleet->get_owner() == mGameID)
		{
			Fleet->end_mission();

			time_news(
				(char *)format(GETTEXT("The fleet %1$s is returning from your ally %2$s cause the alliance has been broken."),
					Fleet->get_nick(), aPlayer->get_nick()));
		}
	}

	time_news(
		(char *)format(GETTEXT("You have broken the alliance with %1$s."),
			aPlayer->get_nick()));
	aPlayer->time_news(
		(char *)format(GETTEXT("%1$s has broken the alliance."),
			get_nick()));
}

const char *
	CPlayer::get_message_list_html(int First, bool aIsForAdmin)
{
	static CString
		HTML;
	HTML.clear();

	if (mMessageBox.length() == 0) return NULL;

	CString
	Records;

	int
		Index = 0;

	while (Index < mMessageBox.length())
	{
		CDiplomaticMessage *
		Message = (CDiplomaticMessage *)mMessageBox.get(Index);
		assert(Message);
		if (Message->get_status() != CDiplomaticMessage::STATUS_WELL)
		{
			if ((time(0) - Message->get_time()) > ONE_WEEK)
			{
				Message->type(QUERY_DELETE);
				STORE_CENTER->store(*Message);
				mMessageBox.remove_diplomatic_message(Message->get_id());
				continue;
			}
		}
		Records += Message->record_html(aIsForAdmin);
		Index++;
	}

	if (Records.length() == 0) return NULL;

	HTML = "<TR>\n";
	HTML += "<TD ALIGN=center CLASS=maintext COLSPAN=3>\n";

	HTML += "<TABLE WIDTH=529 BORDER=0 CELLSPACING=0 CELLPADDING=0>\n";
	HTML += "<TR VALIGN=TOP>\n";

	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=244>&nbsp;</TD>\n";

	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=28 ALIGN=RIGHT>&nbsp;</TD>\n";

	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=257 ALIGN=RIGHT>";
	HTML += GETTEXT("Reply Required");
	HTML.format("<IMG SRC=%s/image/as_game/council/required1_icon.gif WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>\n&nbsp;&nbsp;&nbsp;&nbsp;",
		(char *)CGame::mImageServerURL);
	HTML += GETTEXT("Not Required");
	HTML.format("<IMG SRC=%s/image/as_game/council/notrequired1_icon.gif WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>",
		(char *)CGame::mImageServerURL);
	HTML += "</TD>\n";

	HTML += "</TR>\n";
	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML += "<TD ALIGN=center COLSPAN=3>\n";

	HTML += "<FORM METHOD=\"POST\" ACTION=\"read_diplomatic_message_delete.as\" name=\"MESSAGES\">";

	HTML += "<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	HTML += "<TR>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=35 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>&nbsp;%s</FONT>\n", GETTEXT("ID"));
	HTML += "</TH>\n";

	HTML += "<TH WIDTH=145 CLASS=tabletxt BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Name"));
	HTML += "</TH>\n";

	HTML += "<TH WIDTH=100 CLASS=tabletxt BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Date"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=105 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Type"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=55 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Status"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=55 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Read"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=55 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s <input type=\"checkbox\" onClick=allCheck()></FONT>\n", GETTEXT("Delete"));
	HTML += "</TH>\n";

	HTML += "</TR>\n";

	HTML += Records;

	HTML += "</TABLE>\n";

	HTML.format("<br><INPUT TYPE=Image SRC=\"%s/image/as_game/bu_delete.gif\" BORDER=0>",
		(char *)CGame::mImageServerURL);
	HTML+="</FORM>";
	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML += "<TD CLASS=maintext ALIGN=center COLSPAN=3>&nbsp;</TD>\n";
	HTML += "</TR>\n";



	return (char*)HTML;
}

char *
	CPlayer::get_message_list_by_sender_html(CPlayer *aSender, bool aIsForAdmin)
{
	static CString
		HTML;
	HTML.clear();

	CString
		Records;
	for (int i=0 ; i<PLAYER_TABLE->length() ; i++)
	{
		CPlayer *
		TempPlayer = (CPlayer *)PLAYER_TABLE->get(i);
		CDiplomaticMessageBox *
		MessageBox = TempPlayer->get_diplomatic_message_box();
		for (int j=0 ; j<MessageBox->length() ; j++)
		{
			CDiplomaticMessage *
			Message = (CDiplomaticMessage *)MessageBox->get(j);
			if (Message->get_sender()->get_game_id() == aSender->get_game_id())
			{
				Records += Message->record_html(aIsForAdmin);
			}
		}
	}

	if (Records.length() == 0) return NULL;

	HTML = "<TR>\n";
	HTML += "<TD ALIGN=center CLASS=maintext COLSPAN=3>\n";

	HTML += "<TABLE WIDTH=529 BORDER=0 CELLSPACING=0 CELLPADDING=0>\n";
	HTML += "<TR VALIGN=TOP>\n";

	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=244>&nbsp;</TD>\n";

	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=28 ALIGN=RIGHT>&nbsp;</TD>\n";

	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=257 ALIGN=RIGHT>";
	HTML += GETTEXT("Reply Required");
	HTML.format("<IMG SRC=%s/image/as_game/council/required1_icon.gif WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>\n&nbsp;&nbsp;&nbsp;&nbsp;",
		(char *)CGame::mImageServerURL);
	HTML += GETTEXT("Not Required");
	HTML.format("<IMG SRC=%s/image/as_game/council/notrequired1_icon.gif WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>",
		(char *)CGame::mImageServerURL);
	HTML += "</TD>\n";

	HTML += "</TR>\n";
	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML += "<TD ALIGN=center COLSPAN=3>\n";

	HTML += "<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	HTML += "<TR>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=35 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>&nbsp;%s</FONT>\n", GETTEXT("ID"));
	HTML += "</TH>\n";

	HTML += "<TH WIDTH=145 CLASS=tabletxt BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Name"));
	HTML += "</TH>\n";

	HTML += "<TH WIDTH=100 CLASS=tabletxt BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Date"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=105 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Type"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=55 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Status"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=55 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Read"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=55 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Delete"));
	HTML += "</TH>\n";

	HTML += "</TR>\n";

	HTML += Records;

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML += "<TD CLASS=maintext ALIGN=center COLSPAN=3>&nbsp;</TD>\n";
	HTML += "</TR>\n";

	return (char *)HTML;
}

const char*
	CPlayer::improve_relationship(CDiplomaticMessage::EType aType, CPlayer *aPlayer)
{
	static CString
		Ret;
	Ret.clear();

	CPlayerRelation* Relation
		= mRelationList.find_by_target_id(aPlayer->get_game_id());

	if (!Relation && aType != CDiplomaticMessage::TYPE_SUGGEST_PACT && aType != CDiplomaticMessage::TYPE_DECLARE_HOSTILITY)
	{
		Ret.format(GETTEXT("You don't have any relationship with %1$s."),
			aPlayer->get_nick());
		return (char*)Ret;
	}

	if (!Relation && aType == CDiplomaticMessage::TYPE_SUGGEST_PACT)
	{
		Relation = new CPlayerRelation(this, aPlayer,
				CRelation::RELATION_TRUCE);
		Relation->type(QUERY_INSERT);
		STORE_CENTER->store(*Relation);

		add_relation(Relation);
		aPlayer->add_relation(Relation);
		PLAYER_RELATION_TABLE->add_relation(Relation);
	}

	CPlayer
	*Player1 = Relation->get_player1(),
	*Player2 = Relation->get_player2();

	if ((Relation->get_relation() == CRelation::RELATION_PEACE)
		&& (aType == CDiplomaticMessage::TYPE_SUGGEST_ALLY))
	{
		Relation->set_relation(CRelation::RELATION_ALLY);
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);

		Player1->time_news(
			(char *)format(GETTEXT("Your relationship with %1$s has become \"%2$s\"."),
				Relation->get_player2()->get_nick(),
				Relation->get_relation_description()));
		Player2->time_news(
			(char *)format(GETTEXT("Your relationship with %1$s has become \"%2$s\"."),
				Relation->get_player1()->get_nick(),
				Relation->get_relation_description()));

		ADMIN_TOOL->add_player_relation_log(
			(char *)format("The relationship between the player %s and the player %s has changed into ally from peace.",
				Player1->get_nick(),
				Player2->get_nick()));

		return NULL;
	}
	else if (Relation->get_relation() == CRelation::RELATION_TRUCE &&
		aType == CDiplomaticMessage::TYPE_SUGGEST_PACT)
	{
		Relation->set_relation(CRelation::RELATION_PEACE);
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);

		SLOG("%s %s relation become peace, so START PLANET COMMERCE",
			Relation->get_player1()->get_nick(),
			Relation->get_player2()->get_nick());
		CPlayer::set_commerce_between(this, aPlayer);

		Player1->time_news(
			(char *)format(GETTEXT("Your relationship with %1$s has become \"%2$s\"."),
				Relation->get_player2()->get_nick(),
				Relation->get_relation_description()));
		Player2->time_news(
			(char *)format(GETTEXT("Your relationship with %1$s has become \"%2$s\"."),
				Relation->get_player1()->get_nick(),
				Relation->get_relation_description()));

		ADMIN_TOOL->add_player_relation_log(
			(char *)format("The relationship between the player %s and the player %s has changed into peace from truce.",
				Player1->get_nick(),
				Player2->get_nick()));

		return NULL;
	}
	else if ((Relation->get_relation() == CRelation::RELATION_WAR || Relation->get_relation() == CRelation::RELATION_HOSTILE) &&
		(aType == CDiplomaticMessage::TYPE_SUGGEST_TRUCE ||
			aType == CDiplomaticMessage::TYPE_DEMAND_TRUCE))

	{
		if (aType == CDiplomaticMessage::TYPE_DEMAND_TRUCE)
		{
			int
				Compensation = get_last_turn_production() * 5;

			Compensation = (Compensation > get_production()) ?
				get_production() : Compensation;

			change_reserved_production(-Compensation);
			aPlayer->change_reserved_production(Compensation);

			aPlayer->time_news(
				(char *)format(
					GETTEXT("You have received the compensation(%1$s PP) for war from %2$s."),
					dec2unit(Compensation), get_nick()));
			time_news(
				(char *)format(
					GETTEXT("You have given the compensation(%1$s PP) for war from %$2s."),
					dec2unit(Compensation), get_nick()));
		}
		if (Relation->get_relation() == CRelation::RELATION_HOSTILE)
		{
			CPlayerAction *
			Action = check_declare_hostile_effect_timer(aPlayer);
			if (Action != NULL)
			{
				Action->type(QUERY_DELETE);
				STORE_CENTER->store(*Action);
				remove_action(Action->get_id());
				PLAYER_ACTION_TABLE->remove_action(Action->get_id());
			}
		}

		Relation->set_relation(CRelation::RELATION_TRUCE);
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
		Player1->time_news(
			(char *)format(GETTEXT("Your relationship with %1$s has become \"%2$s\"."),
				Relation->get_player2()->get_nick(),
				Relation->get_relation_description()));
		Player2->time_news(
			(char *)format(GETTEXT("Your relationship with %1$s has become \"%2$s\"."),
				Relation->get_player1()->get_nick(),
				Relation->get_relation_description()));

		ADMIN_TOOL->add_player_relation_log(
			(char *)format("The relationship between the player %s and the player %s has changed into truce from war.",
				Player1->get_nick(),
				Player2->get_nick()));

		return NULL;
	}

	return GETTEXT("This message has unmatching message type");
}

void
	CPlayer::set_commerce_with(CPlayer *aPlayer)
{
	for(int i=0; i<aPlayer->get_planet_list()->length(); i++)
	{
		CPlanet *
		Planet1 = (CPlanet *)aPlayer->get_planet_list()->get(i);
		for(int j=0; j<mPlanetList.length(); j++)
		{
			CPlanet *
			Planet2 = (CPlanet *)mPlanetList.get(j);
			CPlanet::set_commerce_between(Planet1->get_id(), Planet2->get_id());
		}
	}
}

void
	CPlayer::clear_commerce_with(CPlayer *aPlayer)
{
	for(int i=0; i<aPlayer->get_planet_list()->length(); i++)
	{
		CPlanet *
		Planet1 = (CPlanet*)aPlayer->get_planet_list()->get(i);
		for(int j=0; j<mPlanetList.length(); j++)
		{
			CPlanet *
			Planet2 = (CPlanet *)mPlanetList.get(j);
			CPlanet::clear_commerce_between(Planet1->get_id(), Planet2->get_id());
		}
	}
}

bool
	CPlayer::is_border_area( int aPlanetID )
{
	CPlanet
		*Planet = mPlanetList.get_by_id( aPlanetID );
	if( Planet == NULL ) return false;

	if( Planet->get_order() < mPlanetList.length()/2 )
		return false;
	return true;
}

const char *
	CPlayer::html_select_border_planet()
{
	static CString
		Buf;
	Buf.clear();

	Buf = "<SELECT NAME=\"BORDER_PLANET\">\n";
	for( int i = mPlanetList.length()/2; i < mPlanetList.length(); i++ ){
		CPlanet
			*Planet = (CPlanet*)mPlanetList.get(i);

		Buf.format( "<OPTION VALUE=%d>%s</OPTION>\n", Planet->get_id(), Planet->get_name() );
	}
	Buf += "</SELECT>\n";

	return (char*)Buf;
}

CPlayer*
CPlayer::prev(CPlayer* aPlayer)
{
	if (aPlayer) mPrev = aPlayer;
	return mPrev;
}

CPlayer*
CPlayer::next(CPlayer* aPlayer)
{
	if (aPlayer) mNext = aPlayer;
	return mNext;
}

char *
	CPlayer::war_target_list_html()
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	List += "<TR BGCOLOR=\"#171717\">\n";

	List += "<TH WIDTH=\"25\" CLASS=\"tabletxt\">&nbsp;</TH>\n";

	List += "<TH CLASS=\"tabletxt\" WIDTH=\"127\">";
	List.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("Player"));
	List += "</TH>\n";

	List += "<TH WIDTH=\"139\" CLASS=\"tabletxt\">";
	List.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("Council"));
	List += "</TH>\n";

	List += "<TH WIDTH=\"118\" CLASS=\"tabletxt\">";
	List.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("Relation"));
	List += "</TH>\n";

	List += "<TH WIDTH=\"57\" CLASS=\"tabletxt\">";
	List.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("Planets"));
	List += "</TH>\n";

	List += "<TH WIDTH=\"70\" CLASS=\"tabletxt\">";
	List.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("Power"));
	List += "</TH>\n";

	List += "</TR>\n";

	bool
		AnyRelation = false;

	for (int i=0 ; i<mRelationList.length() ; i++)
	{
		CPlayerRelation *
		Relation = (CPlayerRelation *)mRelationList.get(i);

		if (Relation->get_relation() != CRelation::RELATION_WAR &&
			Relation->get_relation() != CRelation::RELATION_TOTAL_WAR &&
			Relation->get_relation() != CRelation::RELATION_BOUNTY &&
			Relation->get_relation() != CRelation::RELATION_HOSTILE) continue;

		CPlayer *
		TargetPlayer;
		CPlayer *
		Player1 = Relation->get_player1();
		CPlayer *
		Player2 = Relation->get_player2();

		if (Player1->get_game_id() == mGameID)
		{
			TargetPlayer = Player2;
		} else if (Player2->get_game_id() == mGameID)
		{
			TargetPlayer = Player1;
		} else continue;

		if (TargetPlayer->is_dead()) continue;

		if ((long long int)TargetPlayer->get_power()*100/(long long int)mPower > PLAYER_ATTACK_RESTRICTION_POWER_MAX ||
			(long long int)TargetPlayer->get_power()*100/(long long int)mPower < PLAYER_ATTACK_RESTRICTION_POWER_MIN) continue;

		CCouncil *
		TargetPlayerCouncil = TargetPlayer->get_council();
		CPlanetList *
		TargetPlayerPlanetList = TargetPlayer->get_planet_list();

		List += "<TR>\n";
		List += "<TD WIDTH=\"25\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">\n";
		List.format("<INPUT TYPE=\"radio\" NAME=\"TARGET_PLAYER_ID\" VALUE=\"%d\">\n",
			TargetPlayer->get_game_id());
		List += "</TD>\n";
		List.format("<TD CLASS=\"tabletxt\" WIDTH=\"127\">%s</TD>\n",
			TargetPlayer->get_nick());
		List.format("<TD WIDTH=\"139\" CLASS=\"tabletxt\" ALIGN=\"LEFT\">%s</TD>\n",
			TargetPlayerCouncil->get_nick());
		List.format("<TD WIDTH=\"118\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
			Relation->get_relation_description());
		List.format("<TD WIDTH=\"57\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d</TD>\n",
			TargetPlayerPlanetList->length());
		List.format("<TD WIDTH=\"70\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s ",
            		dec2unit(TargetPlayer->get_power()));
		List += "</TR>\n";

		AnyRelation = true;
	}

	CCouncilRelationList *
	CouncilRelationList = get_council()->get_relation_list();

	for (int i=0 ; i<CouncilRelationList->length() ; i++)
	{
		CCouncilRelation *
		Relation = (CCouncilRelation *)CouncilRelationList->get(i);

		if (Relation->get_relation() != CRelation::RELATION_WAR &&
			Relation->get_relation() != CRelation::RELATION_TOTAL_WAR) continue;

		CCouncil *
		TargetCouncil;
		CCouncil *
		Council1 = Relation->get_council1();
		CCouncil *
		Council2 = Relation->get_council2();

		if (Council1->get_id() == get_council()->get_id())
		{
			TargetCouncil = Council2;
		}
		else if (Council2->get_id() == get_council()->get_id())
		{
			TargetCouncil = Council1;
		}
		else continue;

		CPlayerList *
		MemberList = TargetCouncil->get_members();

		for (int i=0 ; i<MemberList->length() ; i++)
		{
			CPlayer *
			Member = (CPlayer *)MemberList->get(i);

			if ((long long int)Member->get_power()*100/(long long int)mPower > PLAYER_ATTACK_RESTRICTION_POWER_MAX ||
				(long long int)Member->get_power()*100/(long long int)mPower < PLAYER_ATTACK_RESTRICTION_POWER_MIN) continue;

			CPlanetList *
			MemberPlanetList = Member->get_planet_list();

			List += "<TR>\n";
			List += "<TD WIDTH=\"25\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">\n";
			List.format("<INPUT TYPE=\"radio\" NAME=\"TARGET_PLAYER_ID\" VALUE=\"%d\">\n",
				Member->get_game_id());
			List += "</TD>\n";
			List.format("<TD CLASS=\"tabletxt\" WIDTH=\"127\">%s</TD>\n",
				Member->get_nick());
			List.format("<TD WIDTH=\"139\" CLASS=\"tabletxt\" ALIGN=\"LEFT\">%s</TD>\n",
				TargetCouncil->get_nick());
			List.format("<TD WIDTH=\"118\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
				Relation->get_relation_description());
			List.format("<TD WIDTH=\"57\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d</TD>\n",
				MemberPlanetList->length());
			List.format("<TD WIDTH=\"70\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s ",
		                dec2unit(Member->get_power()));
			List += "</TR>\n";

			AnyRelation = true;
		}
	}

	List += "</TABLE>\n";

	if (AnyRelation)
	{
		return (char *)List;
	} else
	{
		return NULL;
	}
}

char *
	CPlayer::war_status_information_html()
{
	static CString
		Info;
	Info.clear();

	bool
		AnyWarStatus = false;

	for (int i=0 ; i<mRelationList.length() ; i++)
	{
		CPlayerRelation *
		Relation = (CPlayerRelation *)mRelationList.get(i);

		if (Relation->get_relation() != CRelation::RELATION_WAR &&
			Relation->get_relation() != CRelation::RELATION_TOTAL_WAR &&
			Relation->get_relation() != CRelation::RELATION_BOUNTY &&
			Relation->get_relation() != CRelation::RELATION_HOSTILE) continue;

		AnyWarStatus = true;
	}

	if (!AnyWarStatus)
	{
		Info = "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
		Info += "<TR>\n";
		Info.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("There are no players at war with you."));
		Info += "</TR>\n";
		Info += "</TABLE>\n";

		return (char *)Info;
	}

	Info = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Info += "<TR BGCOLOR=\"#171717\">\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"136\" COLOR=\"#666666\">";
	Info += GETTEXT("Player");
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"61\" CLASS=\"tabletxt\" COLOR=\"#666666\">";
	Info += GETTEXT("Race");
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"61\" CLASS=\"tabletxt\" COLOR=\"#666666\">";
	Info += GETTEXT("Status");
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"122\" CLASS=\"tabletxt\" COLOR=\"#666666\">";
	Info += GETTEXT("Council");
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"50\" COLOR=\"#666666\">";
	Info += GETTEXT("Planets");
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"52\" COLOR=\"#666666\">";
	Info += GETTEXT("Power (Virtual)");
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"57\" COLOR=\"#666666\">";
	Info += GETTEXT("Ranking");
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"55\" COLOR=\"#666666\">";
	Info += GETTEXT("Honor");
	Info += "</TH>\n";

	Info += "</TR>\n";

	for (int i=0 ; i<mRelationList.length() ; i++)
	{
		CPlayerRelation *
		Relation = (CPlayerRelation *)mRelationList.get(i);

		if (Relation->get_relation() != CRelation::RELATION_WAR &&
			Relation->get_relation() != CRelation::RELATION_TOTAL_WAR &&
			Relation->get_relation() != CRelation::RELATION_BOUNTY &&
			Relation->get_relation() != CRelation::RELATION_HOSTILE) continue;
		CPlayer *
		Player;
		CPlayer *
		Player1 = Relation->get_player1();
		CPlayer *
		Player2 = Relation->get_player2();

		if( Player1->is_dead() || Player2->is_dead() ) continue;

		if (Player1->get_game_id() == mGameID)
		{
			Player = Player2;
		} else if (Player2->get_game_id() == mGameID)
		{
			Player = Player1;
		} else continue;

		CCouncil *
		PlayerCouncil = Player->get_council();
		CPlanetList *
		PlayerPlanetList = Player->get_planet_list();

		Info += "<TR>\n";

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\">&nbsp;%s</TD>",
			Player->get_nick());

		Info.format("<TD WIDTH=\"61\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
			Player->get_race_name());

		Info.format("<TD WIDTH=\"61\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
			Relation->get_relation_description());

		Info.format("<TD WIDTH=\"122\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
			PlayerCouncil->get_nick());


		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"50\" ALIGN=\"CENTER\">%d</TD>\n",
			PlayerPlanetList->length());

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"52\" ALIGN=\"CENTER\">%s",
			dec2unit(Player->get_power()));
			
		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"57\" ALIGN=\"CENTER\">%s</TD>\n",
			"1");

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"55\" ALIGN=\"CENTER\">%d</TD>\n",
			Player->get_honor());

		Info += "</TR>\n";
	}

	Info += "</TABLE>\n";

	return (char *)Info;
}

char *
	CPlayer::battle_record_list_html(bool aFromHead, int aStartBattleRecordIndex, int aNumberOfBattleRecord, int *aTotalRecord)
{
	static CString
		List;
	List.clear();

	CBattleRecordTable
		BattleRecordList;

	for (int i=0 ; i<BATTLE_RECORD_TABLE->length() ; i++)
	{
		CBattleRecord *
		Record = (CBattleRecord *)BATTLE_RECORD_TABLE->get(i);

		if (Record->get_attacker_id() != mGameID &&
			Record->get_defender_id() != mGameID) continue;

		BattleRecordList.add_battle_record(Record);
	}

	if (aStartBattleRecordIndex == -1) aStartBattleRecordIndex = BattleRecordList.length() - 1;
	if (aStartBattleRecordIndex < 0 ||
		aStartBattleRecordIndex >= BattleRecordList.length())
	{
		*aTotalRecord = 0;
		return GETTEXT("There is no battle record you have.");
	}

	List = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	List += "<TR BGCOLOR=\"#171717\">\n";

	List += "<TH CLASS=\"tabletxt\" WIDTH=\"100\" COLOR=\"#666666\">";
	List += GETTEXT("Attack");
	List += "</TH>\n";

	List += "<TH WIDTH=\"100\" CLASS=\"tabletxt\" COLOR=\"#666666\">";
	List += GETTEXT("Defense");
	List += "</TH>\n";

	List += "<TH WIDTH=\"168\" CLASS=\"tabletxt\" COLOR=\"#666666\">";
	List += GETTEXT("Date");
	List += "</TH>\n";

	List += "<TH CLASS=\"tabletxt\" WIDTH=\"60\" COLOR=\"#666666\">";
	List += GETTEXT("Type");
	List += "</TH>\n";

	List += "<TH CLASS=\"tabletxt\" WIDTH=\"70\" COLOR=\"#666666\">";
	List += GETTEXT("Result");
	List += "</TH>\n";

	List += "<TH CLASS=\"tabletxt\" WIDTH=\"52\" COLOR=\"#666666\">";
	List += GETTEXT("View");
	List += "</TH>\n";

	List += "</TR>\n";

	int
		Index = aStartBattleRecordIndex,
	Number = 0;
	bool
		AnyRecord = false;
	while (Index >= 0 && Index < BattleRecordList.length() && Number < aNumberOfBattleRecord)
	{
		CBattleRecord *
		Record = (CBattleRecord *)BattleRecordList.get(Index);

		List += "<TR>\n";

		List.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s (#%d)</TD>\n",
			Record->get_attacker_name(), Record->get_attacker_id());

		List.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s (#%d)</TD>\n",
			Record->get_defender_name(), Record->get_defender_id());

		char TimeString[60];
		struct tm *Time;
		time_t RecordTime = Record->get_time();
		Time = localtime(&RecordTime);
		strftime(TimeString, 60, "%Y/%m/%d&nbsp;%r", Time);

		List.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
			TimeString);

		List.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
			Record->get_war_type_string());

		if (Record->get_winner() == 0)
		{
			List.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
				GETTEXT("Draw"));
		} else if (Record->get_winner() == Record->get_attacker_id())
		{
			List.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
				GETTEXT("Attacker Win"));
		} else
		{
			List.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
				GETTEXT("Defender Win"));
		}

		List += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">\n";
		List.format("<A HREF=\"battle_report_detail.as?LOG_ID=%d\">", Record->get_id());
		List.format("<IMG SRC=\"%s/image/as_game/bu_view.gif\" WIDTH=\"44\" HEIGHT=\"11\" BORDER=\"0\">\n",
			(char *)CGame::mImageServerURL);
		List += "</A>\n";
		List += "</TD>\n";

		List += "</TR>\n";

		if (aFromHead == true)
		{
			Index++;
		}
		else
		{
			Index--;
		}

		Number++;
		AnyRecord = true;
	}

	List += "</TABLE>\n";

	*aTotalRecord = BattleRecordList.length();

	if (AnyRecord == false)
	{
		List = "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
		List += "<TR>\n";

		List.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
			GETTEXT("There are no battle reports related with you."));

		List += "</TR>\n";
		List += "</TABLE>\n";

		BattleRecordList.remove_without_free_all();

		return (char *)List;
	}
	else
	{
		BattleRecordList.remove_without_free_all();

		return (char *)List;
	}
}

char *
	CPlayer::available_spy_list_html()
{
	static CString
		List;
	List.clear();

	bool
		AnySpy = false;

	List = "<TABLE WIDTH=\"300\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";

	for (int i=0 ; i<SPY_TABLE->length() ; i++)
	{
		CSpy *
		Spy = (CSpy *)SPY_TABLE->get(i);

		if (Spy->get_cost() > get_production()) continue;

		if (!Spy->evaluate(this))
		{
			continue;
		}

#define METEOR_STRIKE			8012
#define STELLAR_BOMBARDMENT		8014

		if (mAbility.has(ABILITY_NO_SPY))
		{
			if (Spy->get_id() != METEOR_STRIKE && Spy->get_id() != STELLAR_BOMBARDMENT)
			{
				continue;
			}
		}

#undef METEOR_STRIKE
#undef STELLAR_BOMBARDMENT

		if (mAbility.has(ABILITY_PACIFIST) && Spy->get_type() == CSpy::TYPE_ATROCIOUS)
		{
			continue;
		}

		List += "<TR>\n";

		List += "<TD>\n";
		if (AnySpy)
		{
			List.format("<INPUT TYPE=radio NAME=\"SPY_ID\" VALUE=\"%d\">\n",
				Spy->get_id());
		}
		else
		{
			List.format("<INPUT TYPE=radio NAME=\"SPY_ID\" VALUE=\"%d\" CHECKED>\n",
				Spy->get_id());
		}
		List += "</TD>\n";

		List.format("<TD CLASS=\"tabletxt\">%s</TD>\n", Spy->get_name());

		List += "</TR>\n";

		AnySpy = true;
	}

	List += "</TABLE>\n";

	if (!AnySpy) return NULL;

	return (char *)List;
}

/*char *
	CPlayer::get_detailed_commerce_info_html()
{
	static CString
		Info;
	Info.clear();

	Info.clear();
	Info = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Info += "<TR BGCOLOR=\"#171717\">\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"136\" COLOR=\"#666666\" align=\"center\">";
	Info += "Name";
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"59\" CLASS=\"tabletxt\" COLOR=\"#666666\" align=\"center\">";
	Info += "Size";
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"121\" CLASS=\"tabletxt\" COLOR=\"#666666\" align=\"center\">";
	Info += "Resource";
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"69\" COLOR=\"#666666\" align=\"center\">";
	Info += "<input type=\"checkbox\" onClick=allCheck()>";
	Info += "</TH>\n";
	Info += "</TR>\n";

	for (int i=0 ; i<mPlanetList.length() ; i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);

		Info += "<TR>\n";

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\">&nbsp;<a href=\"planet_detail.as?PLANET_ID=%d\">%s</a></TD>",
			Planet->get_id(), Planet->get_name());

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\">&nbsp;%s</TD>",
			Planet->get_size_description());

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\">&nbsp;%s</TD>",
			Planet->get_resource_description());

		if (Planet->get_planet_invest_pool()) {
			Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\" align=\"center\">&nbsp;<input type=\"checkbox\" CHECKED name=\"PLANET_ID%d\" VALUE=\"ON\"></TD>",
				Planet->get_id());
		} else {
			Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\" align=\"center\">&nbsp;<input type=\"checkbox\" UNCHECKED name=\"PLANET_ID%d\" VALUE=\"ON\"></TD>",
				Planet->get_id());
		}

		Info += "</TR>\n";
		}
	Info += "</TABLE>\n";

}*/

bool
	CPlayer::has_effect(int aEffect, int aTarget)
{
	for (int i=0 ; i<mEffectList.length() ; i++)
	{
		CPlayerEffect *
		Effect = (CPlayerEffect *)mEffectList.get(i);

		if (Effect->get_type() == aEffect)
		{
			if (aTarget == 0) return true;
			if (Effect->get_target() == aTarget) return true;
		}
	}

	return false;
}

int
	CPlayer::calc_PA(int aValue, int aEffect)
{
	int
		P = 0,
	A = 0;

	for (int i=0 ; i<mEffectList.length() ; i++)
	{
		CPlayerEffect *
		Effect = (CPlayerEffect *)mEffectList.get(i);

		if (Effect->get_type() != aEffect) continue;

		if (Effect->get_apply() == CPlayerEffect::APPLY_ABSOLUTE)
			A += Effect->get_argument1();
		else
			P += Effect->get_argument1();
	}
	return ((aValue*(100 + P)/100)) + A;
}

int
	CPlayer::calc_minus_PA(int aValue, int aEffect)
{
	int
		P = 0,
	A = 0;

	for (int i=0 ; i<mEffectList.length() ; i++)
	{
		CPlayerEffect *
		Effect = (CPlayerEffect *)mEffectList.get(i);

		if (Effect->get_type() != aEffect) continue;
		if (Effect->get_apply() == CPlayerEffect::APPLY_ABSOLUTE)
			A += Effect->get_argument1();
		else
			P += Effect->get_argument1();
	}

	return (aValue*(100 - P)/100) - A;
}

const char*
	CPlayer::check_attackable(CPlayer *aPlayer)
{
	static CString
		RetMsg;
	RetMsg.clear();

	if ((long long int)aPlayer->get_power()*100/(long long int)mPower > PLAYER_ATTACK_RESTRICTION_POWER_MAX)
	//if (aPlayer->get_power()*100/mPower > PLAYER_ATTACK_RESTRICTION_POWER_MAX)

	{
		RetMsg.format(
			GETTEXT("You cannot attack %1$s because he/she is too powerful."),
			aPlayer->get_nick());
	}
	else if ((long long int)aPlayer->get_power()*100/(long long int)mPower < PLAYER_ATTACK_RESTRICTION_POWER_MIN)
	//else if (aPlayer->get_power()*100/mPower < PLAYER_ATTACK_RESTRICTION_POWER_MIN)
	{
		RetMsg.format(
			GETTEXT("You cannot attack %1$s because he/she is too weak."),
			aPlayer->get_nick());
	}
	else if (aPlayer->get_protected_mode() != PROTECTED_NONE)
	{
		switch(aPlayer->get_protected_mode())
		{
			case PROTECTED_PLANET_LOSS:
				//do nothing for now. Should be changed to be in here though.
				break;

#ifdef PROTECTION_24HOURS
			case PROTECTED_24HOURS:
				RetMsg.format(
					GETTEXT("Empire fleets are on %1$s's orbit, "
						"%2$s is protected for 24 hours by Empire."),
					aPlayer->get_nick(), aPlayer->get_nick());
				break;
#endif
			case PROTECTED_NONE:
				break;
#ifdef PROTECTION_BEGINNER
			case PROTECTED_BEGINNER:
				RetMsg.format(
					GETTEXT("Empire fleets are on %1$s's orbit, "
						"%2$s is protected by the Empire."),
					aPlayer->get_nick(), aPlayer->get_nick());
				break;
#endif
		}
	}
	else if (!CGame::mUpdateTurn)
	{
		RetMsg.format(
			GETTEXT("You cannot attack %1$s because time has not yet started."),
			aPlayer->get_nick());
	}

	if (!RetMsg.length()) return NULL;

	return (char*)RetMsg;
}
const char*
	CPlayer::check_attackableHi(CPlayer *aPlayer)
{
	static CString
		RetMsg;
	RetMsg.clear();

    // Target is too powerful for you to attack
	if ((long long int)aPlayer->get_power()*100/(long long int)mPower > PLAYER_ATTACK_RESTRICTION_POWER_MAX_HI)
	//if (aPlayer->get_power()*100/mPower > PLAYER_ATTACK_RESTRICTION_POWER_MAX)

	{
		RetMsg.format(
			GETTEXT("You cannot attack %1$s because he/she is too powerful."),
			aPlayer->get_nick());
	}
	// Target is too weak for you to attack
	else if ((long long int)aPlayer->get_power()*100/(long long int)mPower < PLAYER_ATTACK_RESTRICTION_POWER_MIN_HI)
	//else if (aPlayer->get_power()*100/mPower < PLAYER_ATTACK_RESTRICTION_POWER_MIN)
	{
		RetMsg.format(
			GETTEXT("You cannot attack %1$s because he/she is too weak."),
			aPlayer->get_nick());
	}
	else if (aPlayer->get_protected_mode() != PROTECTED_NONE)
	{
		switch(aPlayer->get_protected_mode())
		{
			case PROTECTED_PLANET_LOSS:
				//do nothing for now.
				break;

#ifdef PROTECTION_24HOURS
			case PROTECTED_24HOURS:
				RetMsg.format(
					GETTEXT("Empire fleets are on %1$s's orbit, "
						"%2$s is protected for 24 hours by Empire."),
					aPlayer->get_nick(), aPlayer->get_nick());
				break;
#endif
			case PROTECTED_NONE:
				break;
#ifdef PROTECTION_BEGINNER
			case PROTECTED_BEGINNER:
				RetMsg.format(
					GETTEXT("Empire fleets are on %1$s's orbit, "
						"%2$s is protected by the Empire."),
					aPlayer->get_nick(), aPlayer->get_nick());
				break;
#endif
		}
	}
	else if (!CGame::mUpdateTurn)
	{
		RetMsg.format(
			GETTEXT("You cannot attack %1$s because time has not yet started."),
			aPlayer->get_nick());
	}

	if (!RetMsg.length()) return NULL;

	return (char*)RetMsg;
}

const char*
	CPlayer::check_privateer_attackable(CPlayer *aPlayer)
{
	static CString
		RetMsg;
	RetMsg.clear();

	if (get_power()*2 < aPlayer->get_power())
		RetMsg.format(
		GETTEXT("You cannot attack %1$s because he/she is too powerful."),
		aPlayer->get_nick());
	else if (aPlayer->get_power()*2 < get_power())
		RetMsg.format(
		GETTEXT("You cannot attack %1$s because he/she is too weak."),
		aPlayer->get_nick());
	else if (!CGame::mUpdateTurn)
	{
		RetMsg.format(
			GETTEXT("You cannot attack %1$s because time has not yet started."),
			aPlayer->get_nick());
	}

	switch(aPlayer->get_protected_mode())
	{
		case PROTECTED_PLANET_LOSS:
			//do nothing for now.
			break;
#ifdef PROTECTION_24HOURS
		case PROTECTED_24HOURS:
			RetMsg.format(
				GETTEXT("Empire fleets are on %1$s's orbit, "
					"%2$s is protected for 24 hours by Empire."),
				aPlayer->get_nick(), aPlayer->get_nick());
			break;
#endif
		case PROTECTED_NONE:
			break;
#ifdef PROTECTION_BEGINNER
		case PROTECTED_BEGINNER:
			RetMsg.format(
				GETTEXT("Empire fleets are on %1$s's orbit, "
					"%2$s is protected by the Empire."),
				aPlayer->get_nick(), aPlayer->get_nick());
			break;
#endif
	}

	if (!RetMsg.length()) return NULL;

	return (char*)RetMsg;
}

bool
	CPlayer::check_attackable_power(CPlayer *aPlayer)
{
	if (get_power()*2 < aPlayer->get_power())
		return false;
	else if (aPlayer->get_power()*2 < get_power())
		return false;
	return true;
}

void
	CPlayer::privateer(CFleet *aFleet)
{
	CMission &
	Mission = aFleet->get_mission();
	CPlayer *
	Player = PLAYER_TABLE->get_by_game_id(Mission.get_target());
	if (!Player)
	{
		CString Report;
		Report.format(GETTEXT("Could not find the target player, %1$s is returning."),
			aFleet->get_nick());
		time_news((char*)Report);

		return_privateer(aFleet);
		return;
	}

	if (Player->is_dead())
	{
		CString Report;
		Report.format(GETTEXT("Target player is dead, %1$s is returning."),
			aFleet->get_nick());
		time_news((char*)Report);

		return_privateer(aFleet);
		return;
	}

	int
		FleetID = aFleet->get_id();
	CPlanetList *
	PlanetList = Player->get_planet_list();
	bool
		Privateerable = false;

	for(int i=0; i<PlanetList->length(); i++)
	{
		CPlanet *
		Planet = (CPlanet *)PlanetList->get(i);
		if (!Planet->get_privateer())
		{
			Privateerable = true;
			break;
		}
	}

	if (!Privateerable)
	{
		CString Report;
		Report.format(GETTEXT("The target player has no planets you can privateer on, %1$s is returning."),
			aFleet->get_nick());
		time_news((char*)Report);

		return_privateer(aFleet);

		ADMIN_TOOL->add_privateer_log(
			(char *)format("The fleet %s, whose owner is the player %s, is returning from the privateer mission because the player %s has no planets to privateer.",
				aFleet->get_nick(),
				(char *)mNick,
				Player->get_nick()));
		return;
	}

	int
		PlanetIndex = number(PlanetList->length())-1;
	CPlanet *
	Planet = (CPlanet*)PlanetList->get(PlanetIndex);
	assert(Planet);
	//SLOG("PRIVATEER PLANET: %s, %d", Planet->get_name(), PlanetIndex);

	CBattle
		Battle(CBattle::WAR_PRIVATEER, this, Player, Planet);

	Battle.init_privateer(aFleet);
	if (Battle.privateer_fleet_detection_check(true))
	{
		CString
			FleetNick = aFleet->get_nick();

		//SLOG("PRIVATEER BATTLE");
		Battle.update_fleet_after_battle();
		CString Report = Battle.get_report();
		Report.format(GETTEXT("%1$s has detected %2$s with his/her alarm system."),
			Planet->get_name(), (char*)FleetNick);
		Report += "<BR>\n";
		Report += GETTEXT("You were attacked by the defensive fleets on the planet.");
		Report += "<BR>\n";

		CFleet *
		Fleet = mFleetList.get_by_id(FleetID);
		if (Fleet)
		{
			Report.format(GETTEXT("%1$s has survived successfully under fire."),
				(char *)FleetNick);
			Report += "<BR>\n";

			ADMIN_TOOL->add_privateer_log(
				(char *)format("The fleet %s, whose owner is the player %s, has failed to privateer on the planet %s whose owner is the player %s, but it has survived.",
					(char *)FleetNick,
					(char *)mNick,
					Planet->get_name(),
					Player->get_nick()));

			return_privateer(Fleet);
		}
		else
		{
			Report.format(GETTEXT("%1$s has sunk under fire."),
				(char *)FleetNick);
			Report += "<BR>\n";

			ADMIN_TOOL->add_privateer_log(
				(char *)format("The fleet %s, whose owner is the player %s, has failed to privateer on the planet %s whose owner is the player %s, and it has sunk under fire.",
					(char*)FleetNick,
					(char *)mNick,
					Planet->get_name(),
					Player->get_nick()));
		}
		time_news((char*)Report);
		return;
	}
	else
	{
		CString Report = Battle.get_report();

		//SLOG("EARN PP");
		int Amount = 0;
		int AmountEarned = 0;
		int Exp_Gain = 0;
		CString Output;
		if (!Planet->get_privateer())
		{
			CAdmiral *Admiral =
				mAdmiralList.get_by_id(aFleet->get_admiral_id());
			if (Admiral == NULL)
			{
				SLOG("Admiral is NULL in CPlayer::privateer()");
				return;
			}

			int Capa = aFleet->get_current_ship() * (aFleet->get_body()-4000);
			Capa /= 2;
			if (Capa > CMission::mMaxPrivateerCapacity) Capa = CMission::mMaxPrivateerCapacity;

			//SLOG("EARN CAPA:%d", Capa);

			// Max of 1, Min -1 (+100%, -100%)
			double PrivStealBonus =  0.0;

			// give famous privateers over 15 privateer auto 100%
			if (Admiral->get_overall_attack() >= 15 && Admiral->get_racial_ability() == CAdmiral::RA_FAMOUS_PRIVATEER)
				PrivStealBonus = 1.0;
			else
			{
				PrivStealBonus = Admiral->get_overall_attack()/20.0;
				if (PrivStealBonus < -1.0)
					PrivStealBonus = -1.0;
				else if (PrivStealBonus > 0.9)
					PrivStealBonus = 0.9;
				for (int i=21; PrivStealBonus <= 1.0 && i < Admiral->get_overall_attack(); i++)
					PrivStealBonus += 0.02;
			}

			if (PrivStealBonus < -1.0)
				PrivStealBonus = -1.0;
			else if (PrivStealBonus > 1.0)
				PrivStealBonus = 1.0;
            	// Amount stolen from the planet -- based on ship size and number of ships
			Amount = (int)((long long int)Planet->get_pp_per_turn() * (long long int)Capa / 100);
            	// Amount player keeps -- based on Privateer Ability
			AmountEarned = int((Amount/2.0) + (Amount/2.0)*PrivStealBonus);
//		SLOG("EARN AMOUNT:%d", Amount);
//		SLOG("REAL EARN AMOUNT:%d", AmountEarned);

			Planet->start_privateer();
			Planet->set_privateer_amount(Amount);
			change_reserved_production(AmountEarned);

		// Commie earns 0.1 to 1.0% of PP stolen in EXP
			double Exp_Gain_Percent = (0.001*number(10));
			if (Exp_Gain_Percent < 0.005) Exp_Gain_Percent = 0.005;

			Exp_Gain = (int)(AmountEarned*10*Exp_Gain_Percent);
			if (Exp_Gain < CMission::mAdmiralExpMinPrivateer) Exp_Gain = CMission::mAdmiralExpMinPrivateer;
			if (Exp_Gain > CMission::mAdmiralExpMaxPrivateer) Exp_Gain = CMission::mAdmiralExpMaxPrivateer;
			Admiral->gain_exp(Exp_Gain);

			Output.clear();
			if (AmountEarned < Amount)
			{
				Output.format("The privateer fleet %s has gained %d PP on %s, "
					"and the fleet admiral has gained %d points of exp.<br>"
					"Our experts suggest that not all the PP is there...",
					aFleet->get_nick(),
					AmountEarned,
					Planet->get_name(),
					Exp_Gain);
				Output += "<BR>\n";
			}
			else
			{
				Output.format("The privateer fleet %s has gained %d PP on %s, "
					"and the fleet admiral has gained %d exp.",
					aFleet->get_nick(),
					AmountEarned,
					Planet->get_name(),
					Exp_Gain);
				Output += "<BR>\n";
			}
			Report += GETTEXT((char *)Output);
			ADMIN_TOOL->add_privateer_log(
				(char *)format("The fleet %s, whose owner is the player %s, has arrived at the planet %s whose owner is the player %s, and gained %d PP.",
					aFleet->get_nick(),
					(char *)mNick,
					Planet->get_name(),
					Player->get_nick(),
					AmountEarned));
		}
		else
		{
			//SLOG("Privateer protected");
			Report.format(GETTEXT("The planet %1$s was already under privateering."),
				Planet->get_name());
			Report += "<BR>\n";

			ADMIN_TOOL->add_privateer_log(
				(char *)format("The fleet %s, whose owner is the player %s, has arrived at the planet %s whose owner is the player %s, but the planet was already under privateering.",
					aFleet->get_nick(),
					(char *)mNick,
					Planet->get_name(),
					Player->get_nick()));
		}

		if (Battle.privateer_fleet_detection_check(false))
		{
			if (Amount)
			{
				Output.clear();
				Output.format("%s succeeded in the privateer of %s. %d PP has been stolen from your planet.",
					get_nick(),
					Planet->get_name(),
					Amount);
				Player->time_news(GETTEXT((char *)Output));
			}
			else
				Player->time_news(
					(char *)format(GETTEXT("%1$s succeeded in the privateer of %2$s. But nothing has been stolen from your planet."),
						get_nick(),Planet->get_name()));
			Report.format(GETTEXT("You are detected after the privateering.<BR>"
					"%1$s knew you were the privateer."),
				Player->get_nick());
		}
		else
		{
			if (Amount)
			{
				Output.clear();
				Output.format("Someone succeeded in the privateer of %s. %d PP has been stolen from your planet.",
					Planet->get_name(),
					Amount);
				Player->time_news(GETTEXT((char *)Output));			}
			else
				Player->time_news((char *)format(GETTEXT("Someone succeeded in the privateer of %1$s. But nothing has been stolen from you."),
						Planet->get_name()));

			Report.format(GETTEXT("You were not detected out after privateer.<BR>"
					"%1$s did not know your identity."),
				Player->get_nick());
		}

		time_news((char*)Report);
	}
}

void
	CPlayer::store()
{
	for(int i=0; i<mAdmiralList.length(); i++)
	{
		CAdmiral
			*Admiral = (CAdmiral*)mAdmiralList.get(i);

		if(Admiral->is_changed())
		{
			Admiral->type(QUERY_UPDATE);
			STORE_CENTER->store(*Admiral);
		}
	}

	for(int i = 0; i<mAdmiralPool.length(); i++)
	{
		CAdmiral
			*Admiral = (CAdmiral*)mAdmiralPool.get(i);
		if (Admiral->is_changed())
		{
			Admiral->type(QUERY_UPDATE);
			STORE_CENTER->store(*Admiral);
		}
	}

	// update fleet
	for( int i = 0; i < mFleetList.length(); i++ ){
		CFleet
			*Fleet = (CFleet*)mFleetList.get(i);
		if( Fleet->is_changed() ){
			Fleet->type(QUERY_UPDATE);
			STORE_CENTER->store(*Fleet);
		}
	}

	type(QUERY_UPDATE);
	*STORE_CENTER << *this;

	save_planet();
}

void
	CPlayer::initialize_tech_control_model()
{
	mTechCM.clear();

	for (int i=0 ; i<mTechList.length() ; i++)
	{
		CKnownTech *
		KnownTech = (CKnownTech *)mTechList.get(i);
		mTechCM += KnownTech->get_effect();
	}
}

char *
	CPlayer::process_spy(int aSpyID, CPlayer *aTargetPlayer, bool *aSuccess)
{
	*aSuccess = false;

	CSpy *
	Spy = SPY_TABLE->get_by_id(aSpyID);
	if (!Spy) return NULL;

	if (!aTargetPlayer) return NULL;

	if (Spy->get_prerequisite())
	{
		CKnownTech *
		KnownTech = mTechList.get_by_id(Spy->get_prerequisite());

		if (!KnownTech)
		{
			return GETTEXT("You don't know the prerequisite technology"
					" to perform this special operation.");
		}
	}

#define ARTIFICIAL_DISEASE				8009
#define RED_DEATH						8010

#define GENERAL_INFORMATION_GATHERING	8001
#define DETAILED_INFORMATION_GATHERING	8002
#define STEAL_SECRET_INFO				8003
#define COMPUTER_VIRUS_INFILTRATION		8004
#define DEVASTATING_NETWORK_WORM		8005
#define STEAL_COMMON_TECHNOLOGY			8008
#define STEAL_IMPORTANT_TECHNOLOGY		8016
#define STEAL_SECRET_TECHNOLOGY			8017

	CControlModel *
	TargetControlModel = aTargetPlayer->get_control_model();
	int
		TargetSpyCM = TargetControlModel->get_real_spy();
	int
		TargetStatus = aTargetPlayer->get_security_status();
	int
		TargetAlertness = aTargetPlayer->get_alertness();
	int
		Difficulty = Spy->get_difficulty();
	int
		AttackRoll,
		DefensiveRoll;

	switch (Spy->get_id())
	{
		case ARTIFICIAL_DISEASE :
		case RED_DEATH :

			if (mAbility.has(ABILITY_GENETIC_ENGINEERING_SPECIALIST))
			{
				AttackRoll = 50 + number(( (mControlModel.get_real_spy() + 3) * 10)) + ( (mControlModel.get_real_spy() + 3) * 10);
			}
			else
			{
				AttackRoll = 50 + number(( (mControlModel.get_real_spy() + 1) * 10)) + ( (mControlModel.get_real_spy() + 1) * 10);
			}

			break;

		case GENERAL_INFORMATION_GATHERING :
		case DETAILED_INFORMATION_GATHERING :
		case STEAL_SECRET_INFO :
		case COMPUTER_VIRUS_INFILTRATION :
		case DEVASTATING_NETWORK_WORM :
		case STEAL_COMMON_TECHNOLOGY :
		case STEAL_IMPORTANT_TECHNOLOGY:
		case STEAL_SECRET_TECHNOLOGY:
			if (mAbility.has(ABILITY_INFORMATION_NETWORK_SPECIALIST))
			{
				AttackRoll = 50 + number(( (mControlModel.get_real_spy() + 5) * 10)) + ( (mControlModel.get_real_spy() + 5) * 10);
			}
			else
			{
				AttackRoll = 50 + number(( (mControlModel.get_real_spy() + 1) * 10)) + ( (mControlModel.get_real_spy() + 1) * 10);
			}

			break;

		default :
			AttackRoll = 50 + number(( (mControlModel.get_real_spy() + 1) * 10)) + ( (mControlModel.get_real_spy() + 1) * 10);
	}

#undef ARTIFICIAL_DISEASE
#undef RED_DEATH

#undef GENERAL_INFORMATION_GATHERING
#undef DETAILED_INFORMATION_GATHERING
#undef STEAL_SECRET_INFO
#undef COMPUTER_VIRUS_INFILTRATION
#undef DEVASTATING_NETWORK_WORM
#undef STEAL_COMMON_TECHNOLOGY
#undef STEAL_IMPORTANT_TECHNOLOGY
#undef STEAL_SECRET_TECHNOLOGY

	int
		AlertnessChangeForFailure,
		AlertnessChangeForSuccess;

	if (Difficulty > 100)
	{
		AlertnessChangeForFailure = (10 + TargetStatus/10)*100/100;
	}
	else if (Difficulty < 0)
	{
		AlertnessChangeForFailure = (10 + TargetStatus/10)*0/100;
	}
	else
	{
		AlertnessChangeForFailure = (10 + TargetStatus/10)*Difficulty/100;
	}

	AlertnessChangeForSuccess = 10 + (TargetStatus + Difficulty)/10;

	// attack success decision
	// Important Spy Defense Mod
	DefensiveRoll = (25 + (TargetSpyCM)) *
		(100 + (TargetStatus * 2) + TargetAlertness)/100 *
		(100 + Difficulty)/100;

	if (AttackRoll <= DefensiveRoll)	// attack failure
	{
		DefensiveRoll = (25 + (TargetSpyCM*5)) *
			(100 + TargetStatus + TargetAlertness)/100 *
			(100 + 50)/100;
		if (AttackRoll <= DefensiveRoll)	// escape failure
		{
			aTargetPlayer->time_news(
				(char *)format(GETTEXT("You caught a spy from %1$s!"),
					(char *)mNick));

			aTargetPlayer->change_alertness(AlertnessChangeForFailure);

			CRelation *
			Relation = get_relation(aTargetPlayer);
			if (Relation)
			{
				if (Relation->get_relation() == CRelation::RELATION_ALLY)
				{
					break_ally(aTargetPlayer);
				}
			}

			int
				HonorDiff = Spy->get_honor_loss(this, aTargetPlayer, Spy->get_type());
			if (HonorDiff > 0)
			{
				set_honor(mHonor - HonorDiff);

				ADMIN_TOOL->add_spy_log(
					(char *)format("%s : The spy from the player %s failed to spy the player %s. The spy has been executed and the player %s lost %d points of honor.",
						Spy->get_name(),
						(char *)mNick,
						aTargetPlayer->get_nick(),
						(char *)mNick,
						HonorDiff));

				return (char *)format(GETTEXT("It was failure! Your spy couldn't escape,"
						" and you lost %1$d of honor!"),
					HonorDiff);
			}
			else
			{
				ADMIN_TOOL->add_spy_log(
					(char *)format("%s : The spy from the player %s failed to spy the player %s. The spy has been executed but the player %s didn't lose honor.",
						Spy->get_name(),
						(char *)mNick,
						aTargetPlayer->get_nick(),
						(char *)mNick));

				return GETTEXT("It was failure! Your spy couldn't escape, but you didn't lose honor anyway.");
			}
		}
		else	// escape success
		{
			time_news(GETTEXT("Your spy couldn't even attempt the action"
					" because of strong security systems."));

			aTargetPlayer->change_alertness(AlertnessChangeForFailure);

			ADMIN_TOOL->add_spy_log(
				(char *)format("%s : The spy from the player %s failed to spy the player %s, but the spy succeeded to escape.",
					Spy->get_name(),
					(char *)mNick,
					aTargetPlayer->get_nick()));

			return GETTEXT("It was failure! But your spy tried to escape and succeeded.");
		}
	}

	static CString
		Result;
	Result.clear();

	CString
		SpyResult;
	SpyResult = Spy->perform(this, aTargetPlayer, aSuccess);

	aTargetPlayer->change_alertness(AlertnessChangeForSuccess);

	// concealment success decision
	if (number(100) <= 25)	// concealment failure
	{
		aTargetPlayer->time_news(
			(char *)format(GETTEXT("You were targetted by a spying attack from %1$s, but you were safe."),
				(char *)mNick));

		int
			HonorDiff = Spy->get_honor_loss(this, aTargetPlayer, Spy->get_type());
		if (HonorDiff > 0)
		{
			set_honor(mHonor - HonorDiff);

			ADMIN_TOOL->add_spy_log(
				(char *)format("%s : The spy was successful. The spy from the player %s failed to conceal himself from the player %s. The player %s lost %d points of honor.",
					Spy->get_name(),
					(char *)mNick,
					aTargetPlayer->get_nick(),
					(char *)mNick,
					HonorDiff));

			Result.format(GETTEXT("The spy was successful!"
					" Your spy couldn't conceal himself,"
					" and you lost %1$d of honor!"),
				HonorDiff);
			Result += "<BR><BR>\n";
			Result += (char *)SpyResult;

			return (char *)Result;
		}
		else
		{
			ADMIN_TOOL->add_spy_log(
				(char *)format("%s : The spy was successful. The spy from the player %s failed to conceal himself from the player %s, but the player %s didn't lose honor.",
					Spy->get_name(),
					(char *)mNick,
					aTargetPlayer->get_nick(),
					(char *)mNick));

			Result = GETTEXT("The spy was successful! Your spy couldn't conceal himself, but you didn't lose honor anyway.");
			Result += "<BR><BR>\n";
			Result += (char *)SpyResult;

			return (char *)Result;
		}
	}

	// Now, the spy attack is concealed.....
	// And the defender doesn't know by whom he/she was attacked.....

	// blame success decision
	if (number(100) <= 50)	// blame failure
	{
		aTargetPlayer->time_news(
			GETTEXT("You are targeted by a spying attack,"
				" but you don't know who initiated the attack."));

		ADMIN_TOOL->add_spy_log(
			(char *)format("%s : The spy was successful. The spy from the player %s failed to blame someone, but the player %s didn't lose honor.",
				Spy->get_name(),
				(char *)mNick,
				(char *)mNick));

		Result = GETTEXT("The spy was successful!"
				" Your spy concealed himself, but couldn't blame anyone."
				" Anyway, you didn't lose any honor.");
		Result += "<BR><BR>\n";
		Result += (char *)SpyResult;

		return (char *)Result;
	}

	// wrong attacker informing decision
	DefensiveRoll = (25 + (TargetSpyCM*5)) *
		(100 + TargetStatus + 0)/100 *
		(100 + Difficulty)/100;

	if (AttackRoll <= DefensiveRoll)	// wrong attacker informing failure
	{
		aTargetPlayer->time_news(
			GETTEXT("You are targeted by a spying attack, but you don't know who initiated the attack."));

		ADMIN_TOOL->add_spy_log(
			(char *)format("%s : The spy was successful. The spy from the player %s failed to give the player %s wrong information, but the player %s din't lose honor.",
				Spy->get_name(),
				(char *)mNick,
				aTargetPlayer->get_nick(),
				(char *)mNick));

		Result = GETTEXT("It was successful!"
				" Your spy concealed himself,"
				" but couldn't give a target player wrong information."
				" Anyway, you didn't lose any honor.");
		Result += "<BR><BR>\n";
		Result += (char *)SpyResult;

		return (char *)Result;
	}

	CPlayerList
	CandidateList;
	CPlayerList *
	Members = get_council()->get_members();

	for (int i=0 ; i<Members->length() ; i++)
	{
		CPlayer *
		Member = (CPlayer *)Members->get(i);
		if (Member->get_game_id() == mGameID) continue;

		CRelation *
		Relation = get_relation(Member);
		if (!Relation) continue;

		if (Relation->get_relation() == CRelation::RELATION_TRUCE ||
			Relation->get_relation() == CRelation::RELATION_WAR)
		{
			CandidateList.add_player(Member);
		}
	}

	int
		NumberOfCandidate = CandidateList.length();

	if (NumberOfCandidate == 0)
	{
		aTargetPlayer->time_news(GETTEXT("You are targeted by a spying attack, but you don't know who initiated the attack."));

		ADMIN_TOOL->add_spy_log(
			(char *)format("%s : The spy was successful. The spy from the player %s succeeded to blame someone, but just there was no candidate. And the player %s didn't lose honor.",
				Spy->get_name(),
				(char *)mNick,
				(char *)mNick));

		Result = GETTEXT("Your spy could blame someone,"
				" but he couldn't find any candidate."
				" Anyway it was successful!");
		Result += "<BR><BR>\n";
		Result += (char *)SpyResult;
	}
	else
	{
		// Now the defender is informed that a random player attacked him/her.....

		CPlayer *
		VictimPlayer = (CPlayer *)CandidateList.get(number(NumberOfCandidate) - 1);

		aTargetPlayer->time_news((char *)format(GETTEXT("You were targetted by a spying attack from %1$s."),
				VictimPlayer->get_nick()));

		int
			OldVictimHonor = VictimPlayer->get_honor();
		int
			HonorDiff = Spy->get_honor_loss(this, VictimPlayer, Spy->get_type());
		if (HonorDiff > 0)
		{
			VictimPlayer->set_honor(OldVictimHonor - HonorDiff);
			VictimPlayer->time_news(
				(char *)format(GETTEXT("You are framed by someones spy, and you lost %1$s of honor!"),
					dec2unit(HonorDiff)));
		}
		else
		{
			VictimPlayer->time_news(GETTEXT("You are framed by someone's spy, but you didn't lose any honor."));
		}

		ADMIN_TOOL->add_spy_log(
			(char *)format("%s : The spy was successful. The spy from the player %s succeeded to blame the player %s, And the player %s didn't lose honor.",
				Spy->get_name(),
				(char *)mNick,
				VictimPlayer->get_nick(),
				(char *)mNick));

		Result = GETTEXT("It was perfectly successful! Your spy tried to blame someone, and he found a victim.");
		Result += "<BR><BR>\n";
		Result += (char *)SpyResult;
	}

	return (char *)Result;
}

bool
	CPlayer::can_buy_event(CEventInstance* EventInstance)
{
	CEventEffectList* EffectList;
	EffectList = EventInstance->get_event()->get_effect_list();
	CEventEffect* Effect;
	int TotalEventPP = mProduction;
	int P = 0;
	int A = 0;

	for(int i=0; i<EffectList->length(); i++)
	{
		Effect = (CEventEffect *)EffectList->get(i);

		if(Effect->get_type() == CPlayerEffect::PA_CHANGE_PRODUCTION)
		{
			if(Effect->has_attribute(CEventEffect::EE_BEGIN_ONLY))
			{
				if(Effect->get_apply() == CPlayerEffect::APPLY_ABSOLUTE)
				{
					A = Effect->get_argument1();
					TotalEventPP += A;
				}
				else
				{
					P =	TotalEventPP * Effect->get_argument1() / 100;
					TotalEventPP += P;
				}
			}
		}
	}

	if(TotalEventPP >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CPlayer::can_request_admission()
{
	if (mAdmissionTimeLimit == -1) // npos
		return true;

	if (mAdmissionTimeLimit <= time(0))
	{
		enable_admission();
		return true;
	}
	return false;
}

void CPlayer::disable_admission()
{
	mAdmissionTimeLimit = 0;
	mStoreFlag += STORE_ADMISSION_TIME_LIMIT;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

void CPlayer::disable_admission(time_t aTime)
{
	mAdmissionTimeLimit = time(0) + aTime;
	mStoreFlag += STORE_ADMISSION_TIME_LIMIT;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

void CPlayer::enable_admission()
{
	mAdmissionTimeLimit = -1; // npos
	mStoreFlag += STORE_ADMISSION_TIME_LIMIT;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

const char *CPlayer::admission_limit_time_left()
{
	time_t Current = mAdmissionTimeLimit-time(0);
	return (char *)format(GETTEXT("The player %1$s currently cannot request council admission for another %2$s."),
		(char *)mNick, timetostring(Current));
}

int
	CPlayer::get_degree_of_empire_relation()
{
	if( mEmpireRelation <= -96 )
		return DM_PURSUED_ANYWHERE_IN_THE_UNIVERSE;
	if( mEmpireRelation <= -91 )
		return DM_PURSUED_THROUGHOUT_THE_GALAXY;
	if( mEmpireRelation <= -86 )
		return DM_LARGE_BOUNTY_ON_YOUR_HEAD;
	if( mEmpireRelation <= -81 )
		return DM_KNOWN_THROUGHOUT_THE_GALAXY;
	if( mEmpireRelation <= -76 )
		return DM_PURSUED_THROUGHOUT_THE_CLUSTER;
	if( mEmpireRelation <= -71 )
		return DM_KNOWN_THROUGHOUT_THE_CLUSTER;
	if( mEmpireRelation <= -66 )
		return DM_LOCALLY_PURSUED;
	if( mEmpireRelation <= -61 )
		return DM_LIGHT_BOUNTY_OFFERED;
	if( mEmpireRelation <= -41 )
		return DM_ENEMY_OF_THE_EMPIRE;
	if( mEmpireRelation <= -36 )
		return DM_DANGEROUS_CRIMINAL;
	if( mEmpireRelation <= -31 )
		return DM_PURSUED_CRIMINAL;
	if( mEmpireRelation <= -26 )
		return DM_LIGHTLY_CHASED;
	if( mEmpireRelation <= -21 )
		return DM_CRIMINAL;
	if( mEmpireRelation <= -16 )
		return DM_ANNOYANCE_TO_THE_EMPIRE;
	if( mEmpireRelation <= -11 )
		return DM_THORN_IN_THE_EMPIRES_SIDE;
	if( mEmpireRelation <= -6 )
		return DM_IGNORED_FUGITIVE;
	if( mEmpireRelation <= -1 )
		return DM_EXILED_SUBJECT;
	if( mEmpireRelation <= 5 )
		return DA_EXILE_IS_BEING_PROCESSED;
	if( mEmpireRelation <= 10 )
		return DA_IN_DANGER_OF_EXILE;
	if( mEmpireRelation <= 15 )
		return DA_THIRD_WARNING;
	if( mEmpireRelation <= 20 )
		return DA_SECOND_WARNING;
	if( mEmpireRelation <= 25 )
		return DA_FIRST_WARNING;
	if( mEmpireRelation <= 30 )
		return DA_DISLIKED_IN_COURT;
	if( mEmpireRelation <= 35 )
		return DA_ON_THE_EMPIRES_BAD_SIDE;
	if( mEmpireRelation <= 40 )
		return DA_IGNORED_BY_THE_MAGISTRATE;
	if( mEmpireRelation <= 60 )
		return DA_AVERAGE_SUBJECT;
	if( mEmpireRelation <= 65 )
		return DA_FRIEND_OF_THE_MAGISTRATE;
	if( mEmpireRelation <= 70 )
		return DA_ON_THE_EMPIRES_GOOD_SIDE;
	if( mEmpireRelation <= 75 )
		return DA_WELL_KNOWN_IN_COURT;
	if( mEmpireRelation <= 80 )
		return DA_FRIEND_OF_THE_EMPEROR;
	if( mEmpireRelation <= 85 )
		return DA_TRUSTED_SUBJECT;
	if( mEmpireRelation <= 90 )
		return DA_UPSTANDING_SUBJECT;
	if( mEmpireRelation <= 95 )
		return DA_EXEMPLARY_SUBJECT;
	return DA_PERFECT_SUBJECT;
}

const char *
	CPlayer::get_degree_name_of_empire_relation()
{
	if (get_degree_of_empire_relation() == DA_EXILE_IS_BEING_PROCESSED)
		return GETTEXT("Exile is being processed");
	if (get_degree_of_empire_relation() == DA_IN_DANGER_OF_EXILE)
		return GETTEXT("In Danger of Exile");
	if (get_degree_of_empire_relation() == DA_THIRD_WARNING)
		return GETTEXT("Third Warning");
	if (get_degree_of_empire_relation() == DA_SECOND_WARNING)
		return GETTEXT("Second Warning");
	if (get_degree_of_empire_relation() == DA_FIRST_WARNING)
		return GETTEXT("First Warning");
	if (get_degree_of_empire_relation() == DA_DISLIKED_IN_COURT)
		return GETTEXT("Disliked in Court");
	if (get_degree_of_empire_relation() == DA_ON_THE_EMPIRES_BAD_SIDE)
		return GETTEXT("On the Empire's Bad Side");
	if (get_degree_of_empire_relation() == DA_IGNORED_BY_THE_MAGISTRATE)
		return GETTEXT("Ignored by the Magistrate");
	if (get_degree_of_empire_relation() == DA_AVERAGE_SUBJECT)
		return GETTEXT("Average Subject");
	if (get_degree_of_empire_relation() == DA_FRIEND_OF_THE_MAGISTRATE)
		return GETTEXT("Friend of the Magistrate");
	if (get_degree_of_empire_relation() == DA_ON_THE_EMPIRES_GOOD_SIDE)
		return GETTEXT("On the Empire's Good Side");
	if (get_degree_of_empire_relation() == DA_WELL_KNOWN_IN_COURT)
		return GETTEXT("Well Known in Court");
	if (get_degree_of_empire_relation() == DA_FRIEND_OF_THE_EMPEROR)
		return GETTEXT("Friend of the Emperor");
	if (get_degree_of_empire_relation() == DA_TRUSTED_SUBJECT)
		return GETTEXT("Trusted Subject");
	if (get_degree_of_empire_relation() == DA_UPSTANDING_SUBJECT)
		return GETTEXT("Upstanding Subject");
	if (get_degree_of_empire_relation() == DA_EXEMPLARY_SUBJECT)
		return GETTEXT("Exemplary Subject");
	if (get_degree_of_empire_relation() == DA_PERFECT_SUBJECT)
		return GETTEXT("Perfect Subject");
	if (get_degree_of_empire_relation() == DM_EXILED_SUBJECT)
		return GETTEXT("Exiled Subject");
	if (get_degree_of_empire_relation() == DM_IGNORED_FUGITIVE)
		return GETTEXT("Ignored Subject");
	if (get_degree_of_empire_relation() == DM_THORN_IN_THE_EMPIRES_SIDE)
		return GETTEXT("Thorn in the Empire's Side");
	if (get_degree_of_empire_relation() == DM_ANNOYANCE_TO_THE_EMPIRE)
		return GETTEXT("Annoyance to the Empire");
	if (get_degree_of_empire_relation() == DM_CRIMINAL)
		return GETTEXT("Criminal");
	if (get_degree_of_empire_relation() == DM_LIGHTLY_CHASED)
		return GETTEXT("Lightly Chased");
	if (get_degree_of_empire_relation() == DM_PURSUED_CRIMINAL)
		return GETTEXT("Pursued Criminal");
	if (get_degree_of_empire_relation() == DM_DANGEROUS_CRIMINAL)
		return GETTEXT("Dangerous Criminal");
	if (get_degree_of_empire_relation() == DM_ENEMY_OF_THE_EMPIRE)
		return GETTEXT("Enemy of the Empire");
	if (get_degree_of_empire_relation() == DM_LIGHT_BOUNTY_OFFERED)
		return GETTEXT("Light Bounty Offered");
	if (get_degree_of_empire_relation() == DM_LOCALLY_PURSUED)
		return GETTEXT("Locally Pursued");
	if (get_degree_of_empire_relation() == DM_KNOWN_THROUGHOUT_THE_CLUSTER)
		return GETTEXT("Known throughout the Cluster");
	if (get_degree_of_empire_relation() == DM_PURSUED_THROUGHOUT_THE_CLUSTER)
		return GETTEXT("Pursued throughout the Cluster");
	if (get_degree_of_empire_relation() == DM_KNOWN_THROUGHOUT_THE_GALAXY)
		return GETTEXT("Known throughout the Galaxy");
	if (get_degree_of_empire_relation() == DM_LARGE_BOUNTY_ON_YOUR_HEAD)
		return GETTEXT("Large Bounty on Your Head");
	if (get_degree_of_empire_relation() == DM_PURSUED_THROUGHOUT_THE_GALAXY)
		return GETTEXT("Pursued throughout the Galaxy");
	if (get_degree_of_empire_relation() == DM_PURSUED_ANYWHERE_IN_THE_UNIVERSE)
		return GETTEXT("Pursued Anywhere in the Universe");

	return NULL;
}

int
	CPlayer::get_court_rank()
{
	if( has_ability( ABILITY_DUKE ) ) return CR_DUKE;
	if( has_ability( ABILITY_MARQUIS ) ) return CR_MARQUIS;
	if( has_ability( ABILITY_EARL ) ) return CR_EARL;
	if( has_ability( ABILITY_VISCOUNT ) ) return CR_VISCOUNT;
	if( has_ability( ABILITY_BARON ) ) return CR_BARON;
	if( has_ability( ABILITY_ROGUE_DUKE ) ) return CR_ROGUE_DUKE;
	if( has_ability( ABILITY_ROGUE_MARQUIS ) ) return CR_ROGUE_MARQUIS;
	if( has_ability( ABILITY_ROGUE_EARL ) ) return CR_ROGUE_EARL;
	if( has_ability( ABILITY_ROGUE_VISCOUNT ) ) return CR_ROGUE_VISCOUNT;
	if( has_ability( ABILITY_ROGUE_BARON ) ) return CR_ROGUE_BARON;

	return CR_NONE;
}

void
	CPlayer::set_court_rank( int aRank )
{
	if (aRank <= CR_NONE || aRank > CR_ROGUE_BARON)
	{
		SLOG("ERROR : Wrong aRank in CPlayer::set_court_rank(), aRank = %d", aRank);
		return;
	}

	mAbility -= ABILITY_DUKE;
	mAbility -= ABILITY_MARQUIS;
	mAbility -= ABILITY_EARL;
	mAbility -= ABILITY_VISCOUNT;
	mAbility -= ABILITY_BARON;
	mAbility -= ABILITY_ROGUE_DUKE;
	mAbility -= ABILITY_ROGUE_MARQUIS;
	mAbility -= ABILITY_ROGUE_EARL;
	mAbility -= ABILITY_ROGUE_VISCOUNT;
	mAbility -= ABILITY_ROGUE_BARON;

	switch (aRank)
	{
		case CR_DUKE :
			mAbility += ABILITY_DUKE;
			break;
		case CR_MARQUIS :
			mAbility += ABILITY_MARQUIS;
			break;
		case CR_EARL :
			mAbility += ABILITY_EARL;
			break;
		case CR_VISCOUNT :
			mAbility += ABILITY_VISCOUNT;
			break;
		case CR_BARON :
			mAbility += ABILITY_BARON;
			break;
		case CR_ROGUE_DUKE :
			mAbility += ABILITY_ROGUE_DUKE;
			break;
		case CR_ROGUE_MARQUIS :
			mAbility += ABILITY_ROGUE_MARQUIS;
			break;
		case CR_ROGUE_EARL :
			mAbility += ABILITY_ROGUE_EARL;
			break;
		case CR_ROGUE_VISCOUNT :
			mAbility += ABILITY_ROGUE_VISCOUNT;
			break;
		case CR_ROGUE_BARON :
			mAbility += ABILITY_ROGUE_BARON;
			break;
		default :
			break;
	}
	mStoreFlag += STORE_ABILITY;

	mNick.clear();

	if( get_court_rank() > CR_NONE ){
		CCluster
			*Cluster = UNIVERSE->get_by_id(mHomeClusterID);
		if( Cluster == NULL )
			mNick.format( "%s the %s (#%d)", (char *)mName, get_court_rank_name(), mGameID );
		else
			mNick.format( "%s the %s of %s (#%d)", (char *)mName, get_court_rank_name(), Cluster->get_name(), mGameID );
	} else {
		mNick.format("%s (#%d)", (char *)mName, mGameID);
	}
}

int
	CPlayer::get_degree_of_amity_bonus_from_rank()
{
	static int degree_of_amity_bonus[] = {
		0,
		25,
		20,
		15,
		10,
		5,
		-100,
		-100,
		-100,
		-100,
		-100
	};

	return degree_of_amity_bonus[get_court_rank()];
}

int
	CPlayer::get_support_bonus_from_rank()
{
	static int support_bonus[] = {
		0,
		50,
		40,
		30,
		20,
		10,
		-100,
		-100,
		-100,
		-100,
		-100
	};

	return support_bonus[get_court_rank()];
}

int
	CPlayer::get_relation_lost_per_day_by_rank()
{
	static float relation_lost[] = {
		0,
		0,
		0,
		0,
		0,
		0,
		-5,
		-5,
		-4,
		-3,
		-3
	};
	/*float aHonor = (float)mHonor;
	if (aHonor < 1) aHonor = 1;
	float lost = 1/(aHonor/50) * relation_lost[get_court_rank()];
	if (lost < -5) lost = -5;*/
	float lost = relation_lost[get_court_rank()];
	return (int)lost;
}

int
	CPlayer::get_tribute_bonus_by_rank()
{
	static int tribute_bonus[] = {
		0,
		25,
		20,
		15,
		10,
		5,
		-25,
		-20,
		-15,
		-10,
		-5
	};

	return tribute_bonus[get_court_rank()];
}

const char *
	CPlayer::get_court_rank_name(int aRank)
{
	if (aRank == -1) aRank = get_court_rank();

	if (aRank == CR_NONE)
	{
		return GETTEXT("None");
	}
	else if (aRank == CR_DUKE)
	{
		return GETTEXT("Duke");
	}
	else if (aRank == CR_MARQUIS)
	{
		return GETTEXT("Marquis");
	}
	else if (aRank == CR_EARL)
	{
		return GETTEXT("Earl");
	}
	else if (aRank == CR_VISCOUNT)
	{
		return GETTEXT("Viscount");
	}
	else if (aRank == CR_BARON)
	{
		return GETTEXT("Baron");
	}
	else if (aRank == CR_ROGUE_DUKE)
	{
		return GETTEXT("Rogue Duke");
	}
	else if (aRank == CR_ROGUE_MARQUIS)
	{
		return GETTEXT("Rogue Marquis");
	}
	else if (aRank == CR_ROGUE_EARL)
	{
		return GETTEXT("Rogue Earl");
	}
	else if (aRank == CR_ROGUE_VISCOUNT)
	{
		return GETTEXT("Rogue Viscount");
	}
	else if (aRank == CR_ROGUE_BARON)
	{
		return GETTEXT("Rogue Baron");
	}
	else
	{
		return NULL;
	}
}

void
	CPlayer::change_empire_relation( int aChange )
{
	int
		OrigRelation = mEmpireRelation;

	mEmpireRelation += aChange;

	if( OrigRelation > 0 && mEmpireRelation <= 0 ){
		switch( get_court_rank() ){
			case CR_DUKE :
				set_court_rank( CR_ROGUE_DUKE );
				time_news( GETTEXT("You lost your rank. Now, you are the rogue duke, the enemy of empire.") );
				break;
			case CR_MARQUIS :
				set_court_rank( CR_ROGUE_MARQUIS );
				time_news( GETTEXT("You lost your rank. Now, you are the rogue marquis, the enemy of empire.") );
				break;
			case CR_EARL :
				set_court_rank( CR_ROGUE_EARL );
				time_news( GETTEXT("You lost your rank. Now, you are the rogue earl, the enemy of empire.") );
				break;
			case CR_VISCOUNT :
				set_court_rank( CR_ROGUE_VISCOUNT );
				time_news( GETTEXT("You lost your rank. Now, you are the rogue viscount, the enemy of empire.") );
				break;
			case CR_BARON :
				set_court_rank( CR_ROGUE_BARON );
				time_news( GETTEXT("You lost your rank. Now, you are the rogue baron, the enemy of empire.") );
				break;
		}
	}

	/*if( OrigRelation == 0 && mEmpireRelation >= 0 )
		mEmpireRelation = 0;
	if( OrigRelation < 0 && mEmpireRelation >= 0 )
		mEmpireRelation = -1;*/

	if( mEmpireRelation > 100 ) mEmpireRelation = 100;
	if( mEmpireRelation < -100 ) mEmpireRelation = -100;

	mStoreFlag += STORE_EMPIRE_RELATION;
}

bool
	CPlayer::test_degree_of_amity()
{
	if( get_empire_relation() < 50 ) return false;

	int
		Test = (get_empire_relation()-50)*2 + get_degree_of_amity_bonus_from_rank();
	if( has_ability(ABILITY_DIPLOMAT) )
		Test = Test*(20+mControlModel.get_diplomacy()+3)/20;
	else
		Test = Test*(20+mControlModel.get_diplomacy())/20;

	if( number(100) <= Test ) return true;
	return false;
}

CEmpireAction *
CPlayer::grant_boon( int aAction )
{
	static int additional_relation_drop[] = {
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		-5,
		0,
		0,
		-5,
		-5,
		-10,
		-15,
		-25
	};

	int
		Amount = 0;
	int
		Reward,
		SupportCheck;

	if( has_ability(ABILITY_DIPLOMAT) )
		SupportCheck = (int)(10*(get_control_model()->get_diplomacy()+3.5)+3);
	else
		SupportCheck = (int)(10*(get_control_model()->get_diplomacy()+0.5)+3);

	SupportCheck = number(SupportCheck/2) + SupportCheck/2;

//	CTech
//			*Tech = get_available_tech_list()->get_by_level(2+number(2)); //unused

	if( SupportCheck <= 11 ){
		Reward = CEmpireAction::EA_REWARD_LOW_PRODUCTION;
	} else if( ( SupportCheck <= 21 ) && (get_available_tech_list()->get_by_level(2)) ){
		Reward = CEmpireAction::EA_REWARD_LEVEL2_TECH;
	} else if( SupportCheck <= 32 ){
		Reward = CEmpireAction::EA_REWARD_LEVEL13_COMMANDER;
	} else if( SupportCheck <= 42 ){
		Reward = CEmpireAction::EA_REWARD_CLASS1_FLEET;
	} else if( SupportCheck <= 52 ){
		Reward = CEmpireAction::EA_REWARD_MEDIUM_PRODUCTION;
	} else if( ( SupportCheck <= 62 ) && (get_available_tech_list()->get_by_level(3)) && (get_available_tech_list()->get_by_level(4))){
		Reward = CEmpireAction::EA_REWARD_LEVEL34_TECH;
	} else if( SupportCheck <= 73 ){
		Reward = CEmpireAction::EA_REWARD_LEVEL46_COMMANDER;
	} else if( SupportCheck <= 84 ){
		Reward = CEmpireAction::EA_REWARD_CLASS2_FLEET;
	} else if( SupportCheck <= 95 ){
		Reward = CEmpireAction::EA_REWARD_HIGH_PRODUCTION;
	} else if( ( SupportCheck <= 105 ) && (get_available_tech_list()->get_by_level(5)) && (get_available_tech_list()->get_by_level(6))){
		Reward = CEmpireAction::EA_REWARD_LEVEL56_TECH;
	} else if( SupportCheck <= 115 ){
		Reward = CEmpireAction::EA_REWARD_CLASS3_FLEET;
	} else if( SupportCheck <= 122 ){
		Reward = CEmpireAction::EA_REWARD_LEVEL710_COMMANDER;
	} else if( ( SupportCheck <= 128 ) && (get_available_tech_list()->get_by_level(7)) && (get_available_tech_list()->get_by_level(8))){
		Reward = CEmpireAction::EA_REWARD_LEVEL78_TECH;
	} else if( SupportCheck <= 133 ){
		Reward = CEmpireAction::EA_REWARD_CLASS4_FLEET;
	} else if( SupportCheck <= 138 ){
		Reward = CEmpireAction::EA_REWARD_LEVEL1115_COMMANDER;
	} else if( SupportCheck <= 143 ){
		Reward = CEmpireAction::EA_REWARD_CLASS5_FLEET;
	} else {
		Reward = CEmpireAction::EA_REWARD_PLANET;
	}

	switch( Reward ){

		case CEmpireAction::EA_REWARD_LOW_PRODUCTION :
			Reward = CEmpireAction::EA_REWARD_LOW_PRODUCTION;
			Amount = dice(10,2)*5000;
			change_reserved_production(Amount);
			break;

		case CEmpireAction::EA_REWARD_MEDIUM_PRODUCTION :
			Amount = (number(20)+30)*5000;
			change_reserved_production(Amount);
			break;

		case CEmpireAction::EA_REWARD_HIGH_PRODUCTION :
			Amount = (number(5)+5)*50000;
			change_reserved_production(Amount);
			break;

		case CEmpireAction::EA_REWARD_LEVEL2_TECH :
			{
				CTech
					*Tech = get_available_tech_list()->get_by_level(2);
				if( Tech ){
					discover_tech( Tech->get_id() );
					Amount = Tech->get_id();
					break;
				}
			}

		case CEmpireAction::EA_REWARD_LEVEL34_TECH :
			{
				CTech
					*Tech = get_available_tech_list()->get_by_level(2+number(2));
				if( Tech ){
					discover_tech( Tech->get_id() );
					Amount = Tech->get_id();
					break;
				}
			}

		case CEmpireAction::EA_REWARD_LEVEL56_TECH :
			{
				CTech
					*Tech = get_available_tech_list()->get_by_level(4+number(2));
				if( Tech ){
					discover_tech( Tech->get_id() );
					Amount = Tech->get_id();
					break;
				}
			}

		case CEmpireAction::EA_REWARD_LEVEL78_TECH :
			{
				CTech
					*Tech = get_available_tech_list()->get_by_level(6+number(2));
				if( Tech ){
					discover_tech( Tech->get_id() );
					Amount = Tech->get_id();
					break;
				}
			}

		case CEmpireAction::EA_REWARD_LEVEL13_COMMANDER :
			{
				CAdmiral
					*Admiral = new CAdmiral(this);
				Admiral->give_level(number(3));
				get_admiral_pool()->add_admiral(Admiral);
				Admiral->type(QUERY_INSERT);
				STORE_CENTER->store(*Admiral);
				new_admiral_news(Admiral);
				Amount = Admiral->get_id();
				break;
			}

		case CEmpireAction::EA_REWARD_LEVEL46_COMMANDER :
			{
				CAdmiral
					*Admiral = new CAdmiral(this);
				Admiral->give_level(3+number(3));
				get_admiral_pool()->add_admiral(Admiral);
				Admiral->type(QUERY_INSERT);
				STORE_CENTER->store(*Admiral);
				new_admiral_news(Admiral);
				Amount = Admiral->get_id();
				break;
			}

		case CEmpireAction::EA_REWARD_LEVEL710_COMMANDER :
			{
				CAdmiral
					*Admiral = new CAdmiral(this);
				Admiral->give_level(6+number(4));
				get_admiral_pool()->add_admiral(Admiral);
				Admiral->type(QUERY_INSERT);
				STORE_CENTER->store(*Admiral);
				new_admiral_news(Admiral);
				Amount = Admiral->get_id();
				break;
			}

		case CEmpireAction::EA_REWARD_LEVEL1115_COMMANDER :
			{
				CAdmiral
					*Admiral = new CAdmiral(this);
				Admiral->give_level(10+number(5));
				get_admiral_pool()->add_admiral(Admiral);
				Admiral->type(QUERY_INSERT);
				STORE_CENTER->store(*Admiral);
				Amount = Admiral->get_id();
				break;
			}

		case CEmpireAction::EA_REWARD_PLANET :
			{
				int
					ClusterID = find_new_planet(true);
				if( ClusterID > 0 ){
					CPlanet
						*Planet = new CPlanet();
					CCluster
						*Cluster = UNIVERSE->get_by_id(ClusterID);
					Planet->initialize();
					Planet->set_cluster(Cluster);
					Planet->set_name(Cluster->get_new_planet_name());
					add_planet(Planet);
					PLANET_TABLE->add_planet(Planet);
					Planet->start_terraforming();
					new_planet_news(Planet);
					Amount = Planet->get_id();

					Planet->type(QUERY_INSERT);
					STORE_CENTER->store(*Planet);
				}
				break;
			}

		case CEmpireAction::EA_REWARD_CLASS1_FLEET :
		case CEmpireAction::EA_REWARD_CLASS2_FLEET :
		case CEmpireAction::EA_REWARD_CLASS3_FLEET :
		case CEmpireAction::EA_REWARD_CLASS4_FLEET :
		case CEmpireAction::EA_REWARD_CLASS5_FLEET :
			{

				int
					classIndex = number(2);

				switch( Reward ) {
					case CEmpireAction::EA_REWARD_CLASS1_FLEET :
						break;
					case CEmpireAction::EA_REWARD_CLASS2_FLEET :
						{
							classIndex += 2;
							break;
						}
					case CEmpireAction::EA_REWARD_CLASS3_FLEET :
						{
							classIndex += 4;
							break;
						}
					case CEmpireAction::EA_REWARD_CLASS4_FLEET :
						{
							classIndex += 6;
							break;
						}
					case CEmpireAction::EA_REWARD_CLASS5_FLEET :
						{
							classIndex += 8;
							break;
						}
				}

				CAdmiral
					*Admiral = new CAdmiral(this);
				Admiral->give_level(classIndex+number(3));
				get_admiral_list()->add_admiral(Admiral);
				Admiral->type(QUERY_INSERT);
				STORE_CENTER->store(*Admiral);

				CShipDesign *
				shipDesign = create_empire_design(classIndex);

				CFleet*
				fleet = new CFleet;

				int day = GAME->get_game_time();
				day = day/60;
				day = day/60;
				day = day/24;
				if (day > 15)
					day = 15;

				int base[10] = { 4, 4, 3, 3, 2, 1, 2, 1, 1, 1 };
				int baseDice[10] = { 4, 3, 3, 2, 3, 3, 2, 2, 0, 0 };
				int dayDice[10] = { 3, 6, 4, 2, 2, 2, 3, 2, 2, 0 };
				int dayRoll[10] = { 1, 2, 2, 2, 2, 2, 3, 3, 3, -1 };

				int ships = base[classIndex-1]
					+ number( baseDice[classIndex-1] );
				for(int i=0 ; i<day/dayRoll[classIndex-1] ; i++)
					ships += number( dayDice[classIndex-1] );

				if(ships > Admiral->get_fleet_commanding()) ships = Admiral->get_fleet_commanding();
				if(ships < 1) ships = 1;

				CString
				name = shipDesign->get_name();
				name.format( " * %d", ships );

				fleet->set_ship_class( shipDesign );
				fleet->set_owner( get_game_id() );
				fleet->set_id( get_fleet_list()->get_new_fleet_id() );
				fleet->set_name( name );
				fleet->set_admiral( Admiral->get_id() );
				fleet->set_max_ship( ships );
				fleet->change_exp(10);
				fleet->set_current_ship( ships );

				get_fleet_list()->add_fleet(fleet);

				Amount = fleet->get_id();

				Admiral->set_fleet_number(fleet->get_id());

				Admiral->type(QUERY_UPDATE);
				*STORE_CENTER << *Admiral;
				shipDesign->type(QUERY_INSERT);
				*STORE_CENTER << *shipDesign;
				fleet->type(QUERY_INSERT);
				*STORE_CENTER << *fleet;

				break;
			}

	}

	change_empire_relation( additional_relation_drop[Reward] );

	CEmpireAction
		*Action = new CEmpireAction();

	Action->set_id( get_empire_action_list()->get_new_id() );
	Action->set_owner( get_game_id() );
	Action->set_action( aAction );
	Action->set_amount( Amount );
	Action->set_target( Reward );
	Action->set_answer( CEmpireAction::EA_ANSWER_COMPLETE );
	Action->set_time();

	Action->save_new_action();
	get_empire_action_list()->add_empire_action(Action);

	return Action;
}

void
	CPlayer::expire_empire_demand( int aHowOld )
{
	time_t now = time(0);

	for (int i=get_empire_action_list()->length()-1 ; i>=0 ; i--)
	{
		CEmpireAction *
		Action = (CEmpireAction *)get_empire_action_list()->get(i);

		if (Action->get_time() < now-aHowOld &&
			Action->get_answer() == CEmpireAction::EA_ANSWER_WAITING)
		{
			Action->reject_demand(this);
		}
	}

}

void
	CPlayer::expire_event()
{
	time_t
		Now = CGame::get_game_time();

	for (int i = mEventList.length()-1 ; i>=0 ; i--)
	{
		CEventInstance *
		Event = (CEventInstance *)mEventList.get(i);

		if (Event->get_life() < Now)
		{
//			SLOG( "EVENT DEBUG : %s removed", Event->get_name() );
			Event->type(QUERY_DELETE);
			STORE_CENTER->store(*Event);
			mEventList.remove_event_instance(Event);
			continue;
		}
		if (Event->is_changed() == true)
		{
			Event->type(QUERY_UPDATE);
			STORE_CENTER->store(*Event);
		}
	}
}

char *
	CPlayer::apply_effect(CPlayerEffect *aEffect)
{
//	SLOG("CPlayer::apply_effect(aEffect)");
	static CString
		Buf;
	Buf.clear();
//	SLOG("Effect Type:%d", aEffect->get_type());
	switch (aEffect->get_type())
	{
		case CPlayerEffect::PA_CHANGE_PLANET_RESOURCE :
			{
				CPlanet *
				Planet = get_planet(aEffect->get_target());
				if (Planet != NULL)
				{
					Planet->change_resource(aEffect->get_argument1());
					Buf.format(GETTEXT("The resource status of the planet %1$s has been changed to %2$s."),
						Planet->get_name(),
						Planet->get_resource_description());
				}
			}
			break;

		case CPlayerEffect::PA_SWITCH_PLANET_ORDER :
			{
				CPlanet
					*Planet1 = get_planet(aEffect->get_argument1()),
				*Planet2 = get_planet(aEffect->get_argument2());
				if (Planet1 != NULL && Planet2 != NULL)
				{
					int
						TmpOrder;
					TmpOrder = Planet1->get_order();
					Planet1->set_order(Planet2->get_order());
					Planet2->set_order(TmpOrder);
					Buf.format(GETTEXT("%1$s's order and %2$s's order have been exchanged."),
						Planet1->get_name(), Planet2->get_name());
				}
			}
			break;

		case CPlayerEffect::PA_SET_FLEET_MISSION :
			{
				CFleet *
				Fleet = mFleetList.get_by_id(aEffect->get_target());
				if (Fleet)
				{
					if (Fleet->get_status() == CFleet::FLEET_STAND_BY)
					{
						Fleet->init_mission((CMission::EMissionType)aEffect->get_argument1(), 0);
						Buf.format(GETTEXT("%1$s has been dispatched for mission \"%2$s\"."),
							Fleet->get_name(),
							Fleet->get_mission().get_mission_name());
					}
				}
			}
			break;

		case CPlayerEffect::PA_LOSE_PLANET :
			{
				CPlanet *
				Planet = get_planet(aEffect->get_target());
				if (Planet != NULL && get_planet_list()->length() > 1)
				{
					drop_planet(Planet);
					BLACK_MARKET->add_new_item(Planet);
					Buf.format(GETTEXT("You have lost the planet %1$s."), Planet->get_name());
				}
			}
			break;

		case CPlayerEffect::PA_DESTROY_ALL_DOCKED_SHIP :
			{
				for (int i=mDock.length()-1 ; i>=0; i--)
				{
					CDockedShip *
					Ship = (CDockedShip *)mDock.get(i);
					Ship->type(QUERY_DELETE);
					STORE_CENTER->store(*Ship);

					mDock.remove_docked_ship(Ship);
				}
				Buf += GETTEXT("You have lost all ships in your dock.");
			}
			break;

		case CPlayerEffect::PA_WIN_AND_GAIN_PLANET :
			{
//			Buf += GETTEXT("You have won and gained a planet (broken).");
			}
			break;

		case CPlayerEffect::PA_LOSE_TECH :
			{
		    // note: rewrote to not use known_tech() and forget_tech()
				CKnownTechList *TargetTechList = &mTechList;
				CKnownTech *TempTech = TargetTechList->get_by_id(aEffect->get_target());
				if (TempTech != NULL && TempTech->get_level() > 4)
				{
					Buf.format(GETTEXT("You lost the tech %1$s."), TempTech->get_name());
					TargetTechList->remove_known_tech(aEffect->get_target());
				}
			}
			break;

		case CPlayerEffect::PA_LOSE_PROJECT :
			{
				CPurchasedProject *
				Project = mPurchasedProjectList.get_by_id(aEffect->get_target());
				if (Project != NULL)
				{
					Buf.format(GETTEXT("You have lost the project %1$s."), Project->get_name());
					mPurchasedProjectList.remove_project(aEffect->get_target());
				}
			}
			break;

		case CPlayerEffect::PA_PLANET_LOST_BUILDING :
			{
				CPlanet *
				Planet = get_planet(aEffect->get_target());
				if (Planet != NULL)
				{
					switch(aEffect->get_argument2())
					{
						case BUILDING_FACTORY :
						case BUILDING_MILITARY_BASE :
						case BUILDING_RESEARCH_LAB :
							{
								int
									Lost,
									Building = Planet->get_building().get((EResourceType)aEffect->get_target());
								if (aEffect->get_apply() == CPlayerEffect::APPLY_ABSOLUTE)
								{
									Lost = aEffect->get_argument1();
								}
								else
								{
									Lost = Building*(aEffect->get_argument1())/100;
								}
								Planet->get_building().change((EResourceType)aEffect->get_target(), Lost);
								Buf.format(GETTEXT("You have lost %1$d %2$s on %3$s."),
									-Lost,
									CResource::get_building_name(aEffect->get_target()),
									Planet->get_name());
							}
							break;

						case -1 :
							{
								for (int i=BUILDING_FACTORY ; i<=BUILDING_RESEARCH_LAB ; i++)
								{
									int
										Lost,
										Building = Planet->get_building().get((EResourceType)i);
									if (aEffect->get_apply() == CPlayerEffect::APPLY_ABSOLUTE)
									{
										Lost = aEffect->get_argument1();
									}
									else
									{
										Lost = Building*(aEffect->get_argument1())/100;
									}
									Planet->get_building().change((EResourceType)i, Lost);
									Buf.format(GETTEXT("You have lost %1$d %2$s on %3$s."),
										-Lost,
										CResource::get_building_name(i),
										Planet->get_name());
								}
							}
							break;

						default :
							break;
					}
				}
			}
			break;

		case CPlayerEffect::PA_SKIP_TURN :
			{
//			Buf += GETTEXT("Turn skipped.");
			}
			break;

		case CPlayerEffect::PA_CHANGE_CONTROL_MODEL :
			{
//			Buf += GETTEXT("Control Model Effected.");
			}
			break;

		case CPlayerEffect::PA_CHANGE_PRODUCTION :
			{
				int
					DiffProduction = 0;
				if (aEffect->get_apply() == CPlayerEffect::APPLY_ABSOLUTE)
				{
//			    CEvent *Event = EVENT_TABLE->get_by_id(aEffect->get_source());
//             	SLOG("Apply absolute!!! Event: %s", Event->get_name());
					DiffProduction = aEffect->get_argument1();
					change_reserved_production(aEffect->get_argument1());
				}
				else
				{
					int
						OldProduction = mProduction;
					DiffProduction = OldProduction * aEffect->get_argument1() / 100;
					change_reserved_production(DiffProduction);
				}

				if (DiffProduction > 0)
				{
					if (aEffect->get_source_type() == CPlayerEffect::ST_EVENT)
						Buf.format(GETTEXT("You have gained %1$s PP due to an event."),
						dec2unit(DiffProduction));
//			    else
//					Buf.format(GETTEXT("You have gained %1$s PP due to an event."),
//							dec2unit(DiffProduction));
				}
				else if (DiffProduction < 0)
				{
					if (aEffect->get_source_type() == CPlayerEffect::ST_EVENT)
						Buf.format( GETTEXT("You have lost %1$s PP due to an event."),
						dec2unit(-DiffProduction));
//			    else
//					Buf.format(GETTEXT("You have lost %1$s PP."),
//							dec2unit(-DiffProduction));
				}
			}
			break;

		case CPlayerEffect::PA_CHANGE_ALL_COMMANDER_ABILITY :
			{
				if (aEffect->get_target() >= 0 || aEffect->get_target() <= 11)
				{
					for (int i=0 ; i<mAdmiralList.length() ; i++)
					{
						CAdmiral *
						Admiral = (CAdmiral *)mAdmiralList.get(i);
						Admiral->set_level_by_effect(aEffect->get_target(), aEffect->get_argument1());
					}
					for (int i=0 ; i<mAdmiralPool.length() ; i++)
					{
						CAdmiral *
						Admiral = (CAdmiral *)mAdmiralPool.get(i);
						Admiral->set_level_by_effect(aEffect->get_target(), aEffect->get_argument1());
					}
				}
			//Buf += GETTEXT("All of your Commanders have been effected by something.");
			}
			break;

		case CPlayerEffect::PA_GAIN_TECH :
			{
				CTech *
				NewTech = TECH_TABLE->get_by_id(aEffect->get_target());
				if (NewTech != NULL)
				{
					if (know_tech(aEffect->get_target()) == false)
					{
						discover_tech(aEffect->get_target());
						Buf.format(GETTEXT("You have discovered a new tech %1$s."),
							NewTech->get_name());
					}
				}
			}
			break;

		case CPlayerEffect::PA_GAIN_FLEET :
			{
				int
					classIndex = number(aEffect->get_target());
				CAdmiral
					*Admiral = new CAdmiral(this);
				Admiral->give_level(aEffect->get_argument1() + number(aEffect->get_argument2() - aEffect->get_argument1()));
				get_admiral_list()->add_admiral(Admiral);
				Admiral->type(QUERY_INSERT);
				STORE_CENTER->store(*Admiral);

				CShipDesign *
				shipDesign = create_empire_design(classIndex);

				CFleet*
				fleet = new CFleet;

				int ships = Admiral->get_fleet_commanding();

				CString
					name = shipDesign->get_name();
				name.format( " * %d", ships );

				fleet->set_ship_class( shipDesign );
				fleet->set_owner( get_game_id() );
				fleet->set_id( get_fleet_list()->get_new_fleet_id() );
				fleet->set_name( name );
				fleet->set_admiral( Admiral->get_id() );
				fleet->set_max_ship( ships );
				fleet->change_exp(10);
				fleet->set_current_ship( ships );

				get_fleet_list()->add_fleet(fleet);

				Admiral->set_fleet_number(fleet->get_id());

				Admiral->type(QUERY_UPDATE);
				*STORE_CENTER << *Admiral;
				shipDesign->type(QUERY_INSERT);
				*STORE_CENTER << *shipDesign;
				fleet->type(QUERY_INSERT);
				*STORE_CENTER << *fleet;


				Buf.format(GETTEXT("You have gained the %1$s fleet."), fleet->get_name());
			}
			break;

		case CPlayerEffect::PA_GAIN_PROJECT :
			{
				CProject *
				Project = PROJECT_TABLE->get_by_id(aEffect->get_target());
				if (Project != NULL)
				{
					if (mPurchasedProjectList.get_by_id(aEffect->get_target()) == NULL)
					{
						buy_project(aEffect->get_target(), false);
						Buf.format(GETTEXT("You have gained a new project %1$s."),
							Project->get_name());
					}
				}
			}
			break;

		case CPlayerEffect::PA_GAIN_SECRET_PROJECT :
			{
			/*CProject *
				Project = PROJECT_TABLE->get_by_id(aEffect->get_target());*/
			#define NUMBER_OF_SECRETBM_PROJECTS		11

				static int BM_Secret_Projects[NUMBER_OF_SECRETBM_PROJECTS + 1] = {
					0,
					7007,
					8000,
					7101,
					7021,
					8001,
					8002,
					8003,
					8004,
					8005,
					8006,
					8007
				};

				int RandSecretProject = BM_Secret_Projects[number(NUMBER_OF_SECRETBM_PROJECTS)];
				int nSecretProjectsTried = 0;
				bool bBought = false;

				while (bBought == false)
				{
					if (mPurchasedProjectList.get_by_id(RandSecretProject) == NULL)
					{
						CProject *
						Project = PROJECT_TABLE->get_by_id(RandSecretProject);
						buy_project(RandSecretProject, false);
						Buf.format(GETTEXT("You have gained a new project %1$s."),
							Project->get_name());
						bBought = true;
					}
					else
					{
						nSecretProjectsTried++;
					}

					if (nSecretProjectsTried >= 1)
					{
						RandSecretProject = BM_Secret_Projects[nSecretProjectsTried];
						nSecretProjectsTried++;
					}

					if (nSecretProjectsTried > NUMBER_OF_SECRETBM_PROJECTS)
					{
						bBought = true;
					}
				}
			}
			break;

		case CPlayerEffect::PA_GUARDED_BY_IMPERIAL_FLEET :
			{
//			Buf += GETTEXT("You are being guarded by imperial fleets.");
			}
			break;

		case CPlayerEffect::PA_CHANGE_EMPIRE_RELATION :
			{
				change_empire_relation(aEffect->get_argument1());
				if (aEffect->get_argument1() > 0)
				{
					Buf += GETTEXT("Your relationship with the Empire has been improved.");
				}
				else if (aEffect->get_argument1() < 0)
				{
					Buf += GETTEXT("Your relationship with the Empire has become worse.");
				}
			}
			break;

		case CPlayerEffect::PA_COMMANDER_LEVEL_UP :
			{
				CAdmiral *
				Admiral = Admiral = mAdmiralList.get_by_id(aEffect->get_target());
				if (Admiral == NULL)
				{
					Admiral = mAdmiralPool.get_by_id(aEffect->get_target());
				}

				if (Admiral != NULL)
				{
					Admiral->give_level(aEffect->get_argument1());
					Buf.format(GETTEXT("Your admiral %1$s has gained a level."),
						Admiral->get_name());
				}
			}
			break;

		case CPlayerEffect::PA_GRANT_BOON :
			{
				grant_boon(CEmpireAction::ED_GRANT_BOON);
				Buf += GETTEXT("Emperor is so pleased with your deed. He granted you a small boon.");
			}
			break;

		case CPlayerEffect::PA_LOSE_COMMANDER :
			{
				int
					Lose;
				if (aEffect->get_apply() == CPlayerEffect::APPLY_ABSOLUTE)
				{
					Lose = aEffect->get_argument1();
				}
				else
				{
					Lose = mAdmiralPool.length()*aEffect->get_argument1()/100;
				}
				if (Lose > mAdmiralPool.length()) Lose = mAdmiralPool.length();

				if (Lose > 0)
				{
					for (int i=Lose-1 ; i>=0 ; i--)
					{
						CAdmiral *
						Admiral = (CAdmiral *)get_admiral_pool()->get(i);
						Admiral->type(QUERY_DELETE);
						STORE_CENTER->store(*Admiral);

						mAdmiralPool.remove_admiral(Admiral->get_id());
					}
					Buf.format(GETTEXT("You have lost %1$s admiral%2$s from your pool."), dec2unit(Lose), plural(Lose));
				}
			}
			break;

		case CPlayerEffect::PA_CHANGE_PLANET_CONTROL_MODEL:
			{
//			Buf += GETTEXT("A planet control model changed.");
				break;
			}
		case CPlayerEffect::PA_CHANGE_PLANET_POPULATION :
			{
				CPlanet *
				Planet = get_planet(aEffect->get_target());
				if (Planet != NULL)
				{
					int
						Change;
					if (aEffect->get_apply() == CPlayerEffect::APPLY_ABSOLUTE)
					{
						Change = aEffect->get_argument1()*1000;
					}
					else
					{
						Change = Planet->get_population()*aEffect->get_argument1()/100;
					}
					Planet->change_population(Change);

					if (Change > 0)
					{
						Buf.format(GETTEXT("Your planet %1$s has gained %2$s k people."),
							Planet->get_name(),
							dec2unit(Change));
					}
					else if (Change < 0)
					{
						Buf.format(GETTEXT("Your planet %1$s has lost %2$s k people."),
							Planet->get_name(),
							dec2unit(-Change));
					}
				}
			}
			break;

		case CPlayerEffect::PA_LOSE_PLANET_GRAVITY_CONTROL :
			{
				CPlanet *
				Planet = get_planet(aEffect->get_target());
				if (Planet != NULL)
				{
					if (Planet->has_attribute(CPlanet::PA_GRAVITY_CONTROLED) == true)
					{
						Planet->remove_attribute(CPlanet::PA_GRAVITY_CONTROLED);
						Buf.format(GETTEXT("Your planet %1$s has lost the gravity control center."),
							Planet->get_name());
					}
				}
			}
			break;

		case CPlayerEffect::PA_GAIN_PLANET_GRAVITY_CONTROL :
			{
				CPlanet *
				Planet = get_planet(aEffect->get_target());
				if (Planet != NULL)
				{
					if (Planet->has_attribute(CPlanet::PA_GRAVITY_CONTROLED) == false)
					{
						Planet->set_attribute(CPlanet::PA_GRAVITY_CONTROLED);
						Buf.format(GETTEXT("Your planet %1$s has gained the gravity control center."),
							Planet->get_name());
					}
				}
			}
			break;

		case CPlayerEffect::PA_CHANGE_SHIP_ABILITY_ON_PLANET :
			{
//		    Buf += GETTEXT("Change Ship Ability on Planet (BROKEN)");
			}
			break;

		case CPlayerEffect::PA_CHANGE_YOUR_FLEET_RETURN_TIME :
			{
//		    Buf += GETTEXT("Change Fleet Return Time (WORKS)");
			}
			break;
		case CPlayerEffect::PA_DAMAGE_FLEET :
			{
				CFleet *
				Fleet = mFleetList.get_by_id(aEffect->get_target());
				if (Fleet != NULL)
				{
					if (Fleet->get_current_ship() > 1)
					{
						CShipSize *
						Size = SHIP_SIZE_TABLE->get_by_id(Fleet->get_body());
						CArmor *
						Armor = (CArmor *)COMPONENT_TABLE->get_by_id(Fleet->get_armor());
						int
							HP = (int)(Size->get_space() * Armor->get_hp_multiplier());
						HP /= 4;
						for (int i=0 ; i<Fleet->get_current_ship()-1 ; i++)
						{
							CDamagedShip *
							Ship = new CDamagedShip((CShipDesign *)Fleet, HP);
							mRepairBay.add_damaged_ship(Ship);

							Ship->type(QUERY_INSERT);
							STORE_CENTER->store(*Ship);
						}
						Fleet->set_current_ship(1);
						Buf.format(GETTEXT("Your fleet %1$s is severely damaged."),
							Fleet->get_name());
					}
				}
			}
			break;

		case CPlayerEffect::PA_CHANGE_HONOR :
			{
				change_honor(aEffect->get_argument1());
				if (aEffect->get_argument1() > 0)
				{
					Buf += GETTEXT("Everyone is admiring you.");
				}
				else if (aEffect->get_argument1() < 0)
				{
					Buf += GETTEXT("Everyone is laughing at you.");
				}
			}
			break;

		case CPlayerEffect::PA_SHOW_PLAYER :
			{
//		    Buf += GETTEXT("Show Player");
			}
			break;

		case CPlayerEffect::PA_COUNCIL_DECLARE_TOTAL_WAR :
			{
				CCouncil *
				Foe = COUNCIL_TABLE->get_by_id(aEffect->get_target());
				if (Foe != NULL)
				{
					if (get_council()->get_speaker_id() == get_game_id())
					{
						CCouncilRelation *
						Relation = get_council()->get_relation(Foe);
						if (Relation != NULL)
						{
							if (Relation->get_relation() != CRelation::RELATION_TOTAL_WAR)
							{
								get_council()->declare_total_war(Foe);
								Buf.format(GETTEXT("You declared total war to %1$s by council-wide pressing."),
									Foe->get_nick());
							}
						}
					}
				}
			}
			break;

		case CPlayerEffect::PA_WIN_OR_LOSE_PLANET :
			{
//		    Buf += GETTEXT("Win or Lose Planet (BROKEN)");
			}
			break;
		case CPlayerEffect::PA_CHANGE_CONCENTRATION_MODE :
			{
				if (aEffect->get_target() < 0)
				{
					set_mode(number(3));
				}
				else if (aEffect->get_target() < MODE_MAX)
				{
					set_mode(aEffect->get_target());

					switch (aEffect->get_source_type())
					{
						case CPlayerEffect::ST_SPY :
							Buf += GETTEXT("Your concentration mode has been changed a spy action.");
							break;

						case CPlayerEffect::ST_EVENT :
							Buf += GETTEXT("Your concentration mode has been changed by an event.");
							break;

						case CPlayerEffect::ST_PROJECT :
							Buf += GETTEXT("Your concentration mode has been changed by a project.");
							break;

						default :
							Buf += GETTEXT("Your concentration mode has changed.");
							break;
					}
				}
			}
			break;

		case CPlayerEffect::PA_RECRUIT_ENEMY_COMMANDER :
			{
//		    Buf += GETTEXT("Recruit Enemy Commander (BROKEN)");
			}
			break;

		case CPlayerEffect::PA_KILL_COMMANDER_AND_DISBAND_FLEET :
			{
				CFleet *
				Fleet = mFleetList.get_by_id(aEffect->get_target());
				if (Fleet != NULL)
				{
					CAdmiral *
					Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());

					Buf.format(GETTEXT("You have lost the fleet %1$s and its commander %2$s."),
						Fleet->get_name(), Admiral->get_name());

					mDock.change_ship((CShipDesign *)Fleet, Fleet->get_current_ship());

					Fleet->type(QUERY_DELETE);
					STORE_CENTER->store(*Fleet);
					mFleetList.remove_fleet(Fleet->get_id());

					Admiral->type(QUERY_DELETE);
					STORE_CENTER->store(*Admiral);
					mAdmiralList.remove_admiral(Admiral->get_id());
				}
			}
			break;
		case CPlayerEffect::PA_GAIN_ABILITY :
			{
				if (!has_ability((EAbility)aEffect->get_argument1()))
				{
					set_ability((EAbility)aEffect->get_argument1());
					Buf.format(GETTEXT("You have gained the ability %1$s."),
						index_to_ability_name(aEffect->get_argument1()));
				}
			}    break;
		case CPlayerEffect::PA_LOSE_ABILITY :
			{
				if (has_ability((EAbility)aEffect->get_argument1()))
				{
					remove_ability((EAbility)aEffect->get_argument1());
					Buf.format(GETTEXT("You have lost the ability %1$s."),
						index_to_ability_name(aEffect->get_argument1()));
				}
			}    break;
		case CPlayerEffect::PA_INVASION_FROM_EMPIRE :
			{
//		    Buf += GETTEXT("Invasion from Empire (BROKEN)");
			}
			break;
		case CPlayerEffect::PA_GAIN_COMMANDER :
			{
				CAdmiral *
				Admiral = new CAdmiral(this);
				Admiral->give_level(aEffect->get_argument1());

				Admiral->type(QUERY_INSERT);
				STORE_CENTER->store(*Admiral);
				mAdmiralPool.add_admiral(Admiral);

				new_admiral_news(Admiral);
			}
			break;
	}
//	SLOG("Event Message: %s",(char*)Buf);
//	SLOG("END OF CPlayer::apply_effect(aEffect)");
	return (char *)Buf;
}

char *
	CPlayer::activate_event(CEventInstance *aInstance, const char *aAnswer)
{
	static CString
		Buf;
	Buf.clear();

	if (is_dead() == true) return NULL;

	CEvent *
	Event = aInstance->get_event();
//	int
//		Life = aInstance->get_life(); //unused
	CEventEffectList *
	EffectList = Event->get_effect_list();

	int
		RandomPlanet = 0,
	RandomPlanet2 = 0;
	int
		RandomFleet = 0,
	RandomTech = 0,
	RandomProject = 0,
	RandomAdmiral = 0,
	RandomPoolAdmiral = 0,
	RandomFleetAdmiral = 0,
	RandomNewTech = 0,
	RandomNewProject = 0,
	RandomNewSecretProject = 0,
	RandomCouncil = 0,
	RandomPlayer = 0;

	if (mPlanetList.length() > 0)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(number(mPlanetList.length()) - 1);
		RandomPlanet = Planet->get_id();
	}
	if (mPlanetList.length() >= 2)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(number(mPlanetList.length()) - 1);
		if (Planet->get_id() != RandomPlanet)
		{
			RandomPlanet2 = Planet->get_id();
		}
	}
	if (mFleetList.length() > 0)
	{
		CFleet *
		Fleet = (CFleet *)mFleetList.get(number(mFleetList.length()) - 1);
		RandomFleet = Fleet->get_id();
	}
	if (mTechList.length() > 0)
	{
		CKnownTech *
		Tech = (CKnownTech *)mTechList.get(number(mTechList.length()) - 1);
		RandomTech = Tech->get_id();
	}
	if (mPurchasedProjectList.length() > 0)
	{
		CPurchasedProject *
		Project = (CPurchasedProject *)mPurchasedProjectList.get(number(mPurchasedProjectList.length()) - 1);
		RandomProject = Project->get_id();
	}
	if (number(2) == 1 && mAdmiralList.length() > 0)
	{
		CAdmiral *
		Admiral = (CAdmiral *)mAdmiralList.get(number(mAdmiralList.length()) - 1);
		RandomAdmiral = Admiral->get_id();
	}
	else if (mAdmiralPool.length() > 0)
	{
		CAdmiral *
		Admiral = (CAdmiral *)mAdmiralPool.get(number(mAdmiralPool.length()) - 1);
		RandomAdmiral = Admiral->get_id();
	}
	if (mAdmiralPool.length() > 0)
	{
		CAdmiral *
		Admiral = (CAdmiral *)mAdmiralPool.get(number(mAdmiralPool.length()) - 1);
		RandomPoolAdmiral = Admiral->get_id();
	}
	if (mAdmiralList.length() > 0)
	{
		CAdmiral *
		Admiral = (CAdmiral *)mAdmiralList.get(number(mAdmiralList.length()) - 1);
		RandomFleetAdmiral = Admiral->get_id();
	}
	if (mAvailableTechList.length() > 0)
	{
		CTech *
		Tech = (CTech *)mAvailableTechList.get(number(mAvailableTechList.length()) - 1);
		RandomNewTech = Tech->get_id();
	}
	if (mProjectList.length() > 0)
	{
		CProject *
		Project = (CProject *)mProjectList.get(number(mProjectList.length()) - 1);
		RandomNewProject = Project->get_id();
	}

	CCouncil *
	Council = (CCouncil *)COUNCIL_TABLE->get(number(COUNCIL_TABLE->length()) - 1);
	if (Council->get_id() != get_council()->get_id())
	{
		RandomCouncil = Council->get_id();
	}

	if (get_council()->get_number_of_members() > 1)
	{
		CPlayerList *
		CouncilMembers = get_council()->get_members();
		CPlayer *
		Player = (CPlayer *)CouncilMembers->get(number(CouncilMembers->length()) - 1);
		if (Player->get_game_id() != mGameID)
		{
			RandomPlayer = Player->get_game_id();
		}
	}
//	SLOG("Event Effects: length = %d", EffectList->length());
	for (int i=0 ; i<EffectList->length() ; i++)
	{
		CEventEffect *
		Effect = (CEventEffect *)EffectList->get(i);

		if (Effect->has_attribute(CEventEffect::EE_CHANCE) == true)
		{
			if (Effect->get_chance() < number(100)) continue;
		}

		if (Event->has_type(CEvent::EVENT_ANSWER) == true && aAnswer != NULL)
		{
			if (Effect->has_attribute(CEventEffect::EE_ANSWER_YES) == true && strcasecmp(aAnswer, "yes") != 0) continue;
			if (Effect->has_attribute(CEventEffect::EE_ANSWER_NO) == true && strcasecmp(aAnswer, "no") != 0) continue;
		}
//		if (mGameID == 41)
//		SLOG("Event Effect -- ID: %d Target: %d Arg1: %d Arg2: %d Duration: %d",
//            Effect->get_id(), Effect->get_target(), Effect->get_argument1(),
//            Effect->get_argument2(), Effect->get_duration());

		CPlayerEffect *
		NewEffect = new CPlayerEffect();
		*NewEffect = *(CPlayerEffect *)Effect;

		NewEffect->set_savable();
		NewEffect->set_id(mEffectList.get_new_id());
		NewEffect->set_owner(mGameID);
		NewEffect->set_source(CPlayerEffect::ST_EVENT, Event->get_id());

// All events must have a duration ;/
//		if (Effect->has_attribute(CEventEffect::EE_DIFFERENT_DURATION) == true)
//		{
		NewEffect->set_life(CGame::get_game_time() + Effect->get_duration()*CGame::mSecondPerTurn);
//		}
//		else
//		{
//			NewEffect->set_life(Effect->get_duration()); // why would you ever use life ;/
//		}
//		SLOG("LIFE: %d DURATION: %d",NewEffect->get_life(), Effect->get_duration());
		switch (NewEffect->get_type())
		{
			case CPlayerEffect::PA_CHANGE_PLANET_RESOURCE :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_SWITCH_PLANET_ORDER :
				NewEffect->set_argument1(RandomPlanet);
				NewEffect->set_argument2(RandomPlanet2);
				break;

			case CPlayerEffect::PA_SET_FLEET_MISSION :
				NewEffect->set_target(RandomFleet);
				break;

			case CPlayerEffect::PA_LOSE_PLANET :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_DESTROY_ALL_DOCKED_SHIP :
				break;

			case CPlayerEffect::PA_WIN_AND_GAIN_PLANET :
				break;

			case CPlayerEffect::PA_LOSE_TECH :
				NewEffect->set_target(RandomTech);
				break;

			case CPlayerEffect::PA_LOSE_PROJECT :
				NewEffect->set_target(RandomProject);
				break;

			case CPlayerEffect::PA_PLANET_LOST_BUILDING :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_SKIP_TURN :
				break;

			case CPlayerEffect::PA_CHANGE_CONTROL_MODEL :
				break;

			case CPlayerEffect::PA_CHANGE_PRODUCTION :
				break;

			case CPlayerEffect::PA_CHANGE_ALL_COMMANDER_ABILITY :
				break;

			case CPlayerEffect::PA_GAIN_TECH :
				NewEffect->set_target(RandomNewTech);
				break;

			case CPlayerEffect::PA_GAIN_FLEET :
				break;

			case CPlayerEffect::PA_GAIN_PROJECT :
				NewEffect->set_target(RandomNewProject);
				break;

			case CPlayerEffect::PA_GAIN_SECRET_PROJECT :
				NewEffect->set_target(RandomNewSecretProject);
				break;

			case CPlayerEffect::PA_GUARDED_BY_IMPERIAL_FLEET :
				break;

			case CPlayerEffect::PA_CHANGE_EMPIRE_RELATION :
				break;

			case CPlayerEffect::PA_COMMANDER_LEVEL_UP :
				NewEffect->set_target(RandomAdmiral);
				break;

			case CPlayerEffect::PA_GRANT_BOON :
				break;

			case CPlayerEffect::PA_LOSE_COMMANDER :
				NewEffect->set_target(RandomPoolAdmiral);
				break;

			case CPlayerEffect::PA_CHANGE_PLANET_CONTROL_MODEL :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_CHANGE_PLANET_POPULATION :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_LOSE_PLANET_GRAVITY_CONTROL :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_GAIN_PLANET_GRAVITY_CONTROL :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_CHANGE_SHIP_ABILITY_ON_PLANET :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_CHANGE_YOUR_FLEET_RETURN_TIME :
				break;

			case CPlayerEffect::PA_DAMAGE_FLEET :
				NewEffect->set_target(RandomFleet);
				break;

			case CPlayerEffect::PA_CHANGE_HONOR :
				break;

			case CPlayerEffect::PA_SHOW_PLAYER :
				NewEffect->set_target(RandomPlayer);
				break;

			case CPlayerEffect::PA_COUNCIL_DECLARE_TOTAL_WAR :
				NewEffect->set_target(RandomCouncil);
				break;

			case CPlayerEffect::PA_WIN_OR_LOSE_PLANET :
				break;

			case CPlayerEffect::PA_CHANGE_CONCENTRATION_MODE :
				break;

			case CPlayerEffect::PA_RECRUIT_ENEMY_COMMANDER :
				NewEffect->set_target(RandomPlayer);
				break;

			case CPlayerEffect::PA_KILL_COMMANDER_AND_DISBAND_FLEET :
				NewEffect->set_target(RandomFleet);
				break;

			case CPlayerEffect::PA_INVASION_FROM_EMPIRE :
				NewEffect->set_target(RandomPlanet);
				break;

			case CPlayerEffect::PA_GAIN_COMMANDER :
				break;

			case CPlayerEffect::PA_PARALYZE_PLANET :
				break;

			case CPlayerEffect::PA_PRODUCE_MP_PER_TURN :
				break;

			case CPlayerEffect::PA_PRODUCE_RP_PER_TURN :
				break;

			case CPlayerEffect::PA_CONSUME_PP_PER_TURN :
				break;

			case CPlayerEffect::PA_IMPERIAL_RETRIBUTION :
				break;

			default :
				break;
		}

//		SLOG("Player Effect -- ID: %d Target: %d Arg1: %d Arg2: %d Life: %d",
//            NewEffect->get_id(), NewEffect->get_target(), NewEffect->get_argument1(),
//            NewEffect->get_argument2(), NewEffect->get_life());
//	    Buf.format(GETTEXT("%1$s :: "),Event->get_name());
		if (Effect->has_attribute(CEventEffect::EE_BEGIN_ONLY) == true)
		{
//			SLOG("APPLY START");
			Buf += apply_effect(NewEffect);
//			SLOG("APPLY STOP");
			Buf += "<BR>\n";

			delete NewEffect;
			continue;
		}
		else
		{
			mEffectList.add_player_effect(NewEffect);
			NewEffect->type(QUERY_INSERT);
			STORE_CENTER->store(*NewEffect);
		}
	}

	return (char *)Buf;
}

void
	CPlayer::upload_project_effect()
{
	for (int i=0 ; i<mPurchasedProjectList.length() ; i++)
	{
		CPurchasedProject *
		Project = (CPurchasedProject *)mPurchasedProjectList.get(i);
		CPlayerEffectList *
		EffectList = Project->get_effect_list();
		if (EffectList->length() == 0) continue;

		for (int i=0 ; i<EffectList->length() ; i++)
		{
			CPlayerEffect *
			Effect = (CPlayerEffect *)EffectList->get(i);
			CPlayerEffect *
			NewEffect = new CPlayerEffect(this, Effect);
			NewEffect->set_life(0);		// will disappear after turn always
			NewEffect->set_source(CPlayerEffect::ST_PROJECT, Project->get_id());

			switch (NewEffect->get_type())
			{
				case CPlayerEffect::PA_CHANGE_PLANET_RESOURCE :
				case CPlayerEffect::PA_PARALYZE_PLANET :
				case CPlayerEffect::PA_LOSE_PLANET :
				case CPlayerEffect::PA_PLANET_LOST_BUILDING :
				case CPlayerEffect::PA_CHANGE_PLANET_CONTROL_MODEL :
				case CPlayerEffect::PA_CHANGE_PLANET_POPULATION :
				case CPlayerEffect::PA_LOSE_PLANET_GRAVITY_CONTROL :
				case CPlayerEffect::PA_GAIN_PLANET_GRAVITY_CONTROL :
				case CPlayerEffect::PA_CHANGE_SHIP_ABILITY_ON_PLANET :
				case CPlayerEffect::PA_INVASION_FROM_EMPIRE :
					{
						int
							RandomPlanet = 0;

						if (get_planet_list()->length() > 0)
						{
							CPlanet *
							Planet = (CPlanet*)(get_planet_list()->get(number(get_planet_list()->length())-1));
							RandomPlanet = Planet->get_id();
						}
						NewEffect->set_target(RandomPlanet);
					}
					break;

				case CPlayerEffect::PA_SWITCH_PLANET_ORDER :
					{
						int
							RandomPlanet = 0,
						RandomPlanet2 = 0;

						if( get_planet_list()->length() > 0 ){
							CPlanet
								*Planet = (CPlanet*)(get_planet_list()->get(number(get_planet_list()->length())-1));
							RandomPlanet = Planet->get_id();
						}
						if( get_planet_list()->length() > 1 ){
							CPlanet
								*Planet = (CPlanet*)(get_planet_list()->get(number(get_planet_list()->length())-1));
							if( Planet->get_id() != RandomPlanet )
								RandomPlanet2 = Planet->get_id();
						}
						NewEffect->set_argument1(RandomPlanet);
						NewEffect->set_argument2(RandomPlanet2);
					}
					break;
				case CPlayerEffect::PA_SET_FLEET_MISSION :
				case CPlayerEffect::PA_DAMAGE_FLEET :
				case CPlayerEffect::PA_KILL_COMMANDER_AND_DISBAND_FLEET :
					{
						int
							RandomFleet = 0;
						if( get_fleet_list()->length() > 0 ){
							CFleet
								*Fleet = (CFleet*)(get_fleet_list()->get(number(get_fleet_list()->length())-1));
							RandomFleet = Fleet->get_id();
						}
						NewEffect->set_target(RandomFleet);
					}
					break;
				case CPlayerEffect::PA_LOSE_TECH :
					{
						int
							RandomTech = 0;
						if( get_tech_list()->length() > 0 ){
							CKnownTech
								*Tech = (CKnownTech*)(get_tech_list()->get(number(get_tech_list()->length())-1));
							RandomTech = Tech->get_id();
						}
						NewEffect->set_target(RandomTech);
					}
					break;
				case CPlayerEffect::PA_LOSE_PROJECT :
					{
						int
							RandomProject = 0;
						if( get_purchased_project_list()->length() > 0 ){
							CPurchasedProject
								*Project = (CPurchasedProject*)(get_purchased_project_list()->get(number(get_purchased_project_list()->length())-1));
							RandomProject = Project->get_id();
						}
						NewEffect->set_target(RandomProject);
					}
					break;
				case CPlayerEffect::PA_GAIN_TECH :
					{
						int
							RandomNewTech = 0;
						if( get_available_tech_list()->length() > 0 ){
							CTech
								*Tech = (CTech*)(get_available_tech_list()->get(number(get_available_tech_list()->length())-1));
							RandomNewTech = Tech->get_id();
						}
						NewEffect->set_target(RandomNewTech);
					}
					break;
				case CPlayerEffect::PA_GAIN_PROJECT :
					{
						int
							RandomNewProject = 0;
						if( get_project_list()->length() > 0 ){
							CProject
								*Project = (CProject*)(get_project_list()->get(number(get_project_list()->length())-1));
							RandomNewProject = Project->get_id();
						}
						NewEffect->set_target(RandomNewProject);
					}
					break;
				case CPlayerEffect::PA_GAIN_SECRET_PROJECT :
					{
						int
							RandomNewSecretProject = 0;

						NewEffect->set_target(RandomNewSecretProject);
					}
					break;
				case CPlayerEffect::PA_COMMANDER_LEVEL_UP :
					{
						int
							RandomAdmiral = 0;
						if( number(2) == 1 && get_admiral_list()->length() > 0 ){
							CAdmiral
								*Admiral = (CAdmiral*)(get_admiral_list()->get(number(get_admiral_list()->length())-1));
							RandomAdmiral = Admiral->get_id();
						} else if( get_admiral_pool()->length() > 0 ){
							CAdmiral
								*Admiral = (CAdmiral*)(get_admiral_pool()->get(number(get_admiral_pool()->length())-1));
							RandomAdmiral = Admiral->get_id();
						}
						NewEffect->set_target(RandomAdmiral);
					}
					break;
				case CPlayerEffect::PA_SHOW_PLAYER :
				case CPlayerEffect::PA_RECRUIT_ENEMY_COMMANDER :
					{
						int
							RandomPlayer = 0;
						if (get_council()->get_number_of_members() > 1)
						{
							CPlayer *
							Player = (CPlayer *)(get_council()->get_members()->get(number(get_council()->get_number_of_members())-1));
							if (Player->get_game_id() != mGameID)
							{
								RandomPlayer = Player->get_game_id();
							}
						}
						NewEffect->set_target(RandomPlayer);
					}
					break;
				case CPlayerEffect::PA_COUNCIL_DECLARE_TOTAL_WAR :
					{
						int
							RandomCouncil = 0;
						CCouncil *
						Council = (CCouncil*)(COUNCIL_TABLE->get(number(COUNCIL_TABLE->length())-1));
						if (Council->get_id() != get_council()->get_id())
							RandomCouncil = Council->get_id();
						NewEffect->set_target(RandomCouncil);
					}
					break;
				case CPlayerEffect::PA_CHANGE_YOUR_FLEET_RETURN_TIME :
					{
                    // this effect needs to stay around the length of the entire turn
                    // /2 just in case of lag to keep from having it double up possibly?
						NewEffect->set_life(CGame::get_game_time()+(CGame::mSecondPerTurn/2));
					}
					break;
				case CPlayerEffect::PA_LOSE_COMMANDER :
					{
						int
							RandomPoolAdmiral = 0;
						if( get_admiral_pool()->length() > 0 ){
							CAdmiral
								*Admiral = (CAdmiral*)(get_admiral_pool()->get(number(get_admiral_pool()->length())-1));
							RandomPoolAdmiral = Admiral->get_id();
						}
						NewEffect->set_target(RandomPoolAdmiral);
					}
					break;
			}

			mEffectList.add_player_effect(NewEffect);
		}
	}
}

bool
	CPlayer::has_siege_blockade_restriction()
{
	if (CGame::mSiegeBlockadeRestriction)
	{
		CPlayerActionSiegeBlockadeRestriction *
		Restriction = (CPlayerActionSiegeBlockadeRestriction *)PLAYER_ACTION_TABLE->get_by_owner_type(this, CAction::ACTION_PLAYER_SIEGE_BLOCKADE_RESTRICTION);

		if (Restriction == NULL)
		{
			return false;
		}
		else
		{
			if ((Restriction->get_start_time() + CGame::mSiegeBlockadeRestrictionDuration) < CGame::get_game_time())
			{
				return false;
			}
			return true;
		}
	}
	return false;
}

bool
	CPlayer::has_siege_blockade_protection()
{
	if (CGame::mSiegeBlockadeProtection)
	{
		CPlayerActionSiegeBlockadeProtection *
		Protection = (CPlayerActionSiegeBlockadeProtection *)PLAYER_ACTION_TABLE->get_by_owner_type(this, CAction::ACTION_PLAYER_SIEGE_BLOCKADE_PROTECTION);

		if (Protection == NULL)
		{
			return false;
		}
		else
		{
			if ((Protection->get_start_time() + CGame::mSiegeBlockadeProtectionDuration) < CGame::get_game_time())
			{
				return false;
			}
			return true;
		}
	}
	return false;
}

bool
	CPlayer::set_siege_blockade_restriction()
{
	if (CGame::mSiegeBlockadeRestriction)
	{
		CPlayerActionSiegeBlockadeRestriction *
		Restriction = new CPlayerActionSiegeBlockadeRestriction();
		Restriction->set_owner(this);
		Restriction->set_start_time(CGame::get_game_time());

		/*Restriction->type(QUERY_INSERT);
		STORE_CENTER->store(*Restriction);*/

		mActionList.add_action(Restriction);
		PLAYER_ACTION_TABLE->add_action(Restriction);
	}
	return true;
}

bool
	CPlayer::set_siege_blockade_protection()
{
	if (CGame::mSiegeBlockadeProtection)
	{
		CPlayerActionSiegeBlockadeProtection *
		Protection = new CPlayerActionSiegeBlockadeProtection();
		Protection->set_owner(this);
		Protection->set_start_time(CGame::get_game_time());

		/*Protection->type(QUERY_INSERT);
		STORE_CENTER->store(*Protection);*/

		mActionList.add_action(Protection);
		PLAYER_ACTION_TABLE->add_action(Protection);
	}
	return true;
}

int
CPlayer::get_siege_blockade_restriction_time()
{
	if (CGame::mSiegeBlockadeRestriction)
	{
		CPlayerActionSiegeBlockadeRestriction *
		Restriction = (CPlayerActionSiegeBlockadeRestriction *)PLAYER_ACTION_TABLE->get_by_owner_type(this, CAction::ACTION_PLAYER_SIEGE_BLOCKADE_RESTRICTION);

		if (Restriction == NULL)
		{
			return 0;
		}
		else
		{
			return (CGame::mSiegeBlockadeRestrictionDuration - (CGame::get_game_time() - Restriction->get_start_time()));
		}
	}
	return 0;
}

int
CPlayer::get_siege_blockade_protection_time()
{
	if (CGame::mSiegeBlockadeProtection)
	{
		CPlayerActionSiegeBlockadeProtection *
		Protection = (CPlayerActionSiegeBlockadeProtection *)PLAYER_ACTION_TABLE->get_by_owner_type(this, CAction::ACTION_PLAYER_SIEGE_BLOCKADE_PROTECTION);

		if (Protection == NULL)
		{
			return 0;
		}
		else
		{
			return (CGame::get_game_time() - Protection->get_start_time());
		}
	}
	return 0;
}


void
	CPlayer::load_news()
{

	FILE
		*fp;
	CString
		News,
		NewsFile,
		FileNameBase;
	char
		buf[1000];

	FileNameBase = get_news_file_name();

	NewsFile.clear();
	NewsFile.format( "%s.ability", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "r" );
	if( fp ){
		News.clear();
		while( fgets( buf, 999, fp ) ){
			News += buf;
		}
		mNewsCenter.set_ability((char*)News);
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.tech", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "r" );
	if( fp ){
		News.clear();
		while( fgets( buf, 999, fp ) ){
			News += buf;
		}
		mNewsCenter.set_tech((char*)News);
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.planet", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "r" );
	if( fp ){
		News.clear();
		while( fgets( buf, 999, fp ) ){
			News += buf;
		}
		mNewsCenter.set_planet((char*)News);
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.project", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "r" );
	if( fp ){
		News.clear();
		while( fgets( buf, 999, fp ) ){
			News += buf;
		}
		mNewsCenter.set_project((char*)News);
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.admiral", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "r" );
	if( fp ){
		News.clear();
		while( fgets( buf, 999, fp ) ){
			News += buf;
		}
		mNewsCenter.set_admiral((char*)News);
		fclose(fp);
	}

	NewsFile.clear();
	NewsFile.format( "%s.time", (char*)FileNameBase );
	fp = fopen( (char*)NewsFile, "r" );
	if( fp ){
		News.clear();
/*		while( fgets( buf, 999, fp ) ){
			News += buf;
		}*/
		mNewsCenter.set_time_news(fp);
		fclose(fp);
	}

}

const char *
	CPlayer::get_news_file_name()
{
	char
		buf[100];
	static CString
		FileName;
	FileName.clear();

	sprintf( buf, "%d", get_game_id() );

	FileName.format( "%s/%c", ARCHSPACE->configuration().get_string( "Game", "NewsFileDir" ), buf[0] );
	if( buf[1] )
		FileName.format( "/%c/%d", buf[1], get_game_id() );
	else
		FileName.format( "/0/%d", get_game_id() );

	return (char*)FileName;
}

CPlanet *
CPlayer::get_target_empire_planet()
{
	if (mPlanetList.length() == 0) return NULL;

	return (CPlanet *)mPlanetList.get(0);
}

bool
	CPlayer::add_empire_planet(CPlanet *aPlanet)
{
	if (mEmpirePlanetList.add_planet(aPlanet) == -1) return false;

	return true;
}

bool
	CPlayer::remove_empire_planet(int aPlanetID)
{
	return mEmpirePlanetList.remove_planet(aPlanetID);
}

int
	CPlayer::get_number_of_empire_planets()
{
	return mEmpirePlanetList.length();
}

CSortedPlanetList *
CPlayer::get_lost_empire_planet_list()
{
	static CSortedPlanetList
		LostPlanetList;
	LostPlanetList.remove_all();

	CEmpirePlanetInfoList *
	EmpirePlanetInfoList = EMPIRE->get_empire_planet_info_list();

	for (int i=0 ; i<EmpirePlanetInfoList->length() ; i++)
	{
		CEmpirePlanetInfo *
		EmpirePlanetInfo = (CEmpirePlanetInfo *)EmpirePlanetInfoList->get(i);
		if (EmpirePlanetInfo->get_planet_type() != CEmpire::LAYER_EMPIRE_PLANET) continue;
		if (EmpirePlanetInfo->get_owner_id() == EMPIRE_GAME_ID) continue;

		CPlanet *
		Planet = PLANET_TABLE->get_by_id(EmpirePlanetInfo->get_planet_id());
		if (Planet == NULL)
		{
			SLOG("ERROR : Planet is NULL in CPlayer::get_lost_empire_planet_list(), EmpirePlanetInfo->get_planet_id() = %d", EmpirePlanetInfo->get_planet_id());
			continue;
		}

		LostPlanetList.add_planet(Planet);
	}

	return &LostPlanetList;
}

int
	CPlayer::get_number_of_lost_empire_planets()
{
	static CSortedPlanetList
		LostPlanetList;
	LostPlanetList.remove_all();

	CEmpirePlanetInfoList *
	EmpirePlanetInfoList = EMPIRE->get_empire_planet_info_list();

	for (int i=0 ; i<EmpirePlanetInfoList->length() ; i++)
	{
		CEmpirePlanetInfo *
		EmpirePlanetInfo = (CEmpirePlanetInfo *)EmpirePlanetInfoList->get(i);
		if (EmpirePlanetInfo->get_planet_type() != CEmpire::LAYER_EMPIRE_PLANET) continue;
		if (EmpirePlanetInfo->get_owner_id() == EMPIRE_GAME_ID) continue;

		CPlanet *
		Planet = PLANET_TABLE->get_by_id(EmpirePlanetInfo->get_planet_id());
		if (Planet == NULL)
		{
			SLOG("ERROR : Planet is NULL in CPlayer::get_number_of_lost_empire_planets(), EmpirePlanetInfo->get_planet_id() = %d", EmpirePlanetInfo->get_planet_id());
			continue;
		}

		LostPlanetList.add_planet(Planet);
	}

	return LostPlanetList.length();
}

bool
	CPlayer::initialize(CMySQL *aMySQL)
{
	SLOG("START : Initialize BlackMarket");
	mBlackMarket.initialize(aMySQL);
	SLOG("END : Initialize BlackMarket");

	CAdmiralList *
	BlackMarketAdmiralList = BLACK_MARKET->get_admiral_list();
	CFleetList *
	BlackMarketFleetList = BLACK_MARKET->get_fleet_list();
	CSortedPlanetList *
	BlackMarketPlanetList = BLACK_MARKET->get_planet_list();

	SLOG("START : Initialize Empire");

	mEmpireAdmiralInfoList.initialize(aMySQL);
	mEmpireFleetInfoList.initialize(aMySQL);
	mEmpirePlanetInfoList.initialize(aMySQL);

	initialize_magistrate(aMySQL);
	initialize_fortress(aMySQL);
	initialize_empire_capital_planet(aMySQL);

	if (mEmpireAdmiralInfoList.length() == 0 ||
		mEmpireFleetInfoList.length() == 0 ||
		mEmpirePlanetInfoList.length() == 0)
	{
		mEmpireAdmiralInfoList.remove_all_from_db();
		mEmpireAdmiralInfoList.remove_all();

		mEmpireFleetInfoList.remove_all_from_db();
		mEmpireFleetInfoList.remove_all();

		mEmpirePlanetInfoList.remove_all_from_db();
		mEmpirePlanetInfoList.remove_all();

		/* Magistrate */
		for (int i=0 ; i<mMagistrateList.length() ; i++)
		{
			CMagistrate *
			Magistrate = (CMagistrate *)mMagistrateList.get(i);
			CCluster *
			Cluster = UNIVERSE->get_by_id(Magistrate->get_cluster_id());
			Magistrate->initialize(Cluster->get_id());
		}

		CCluster *
		EmpireCluster = UNIVERSE->get_by_id(EMPIRE_CLUSTER_ID);

		/* Empire Planet */
		/* Store Empire Planet */
		for (int i=0 ; i<CEmpire::mInitialNumberOfEmpirePlanet ; i++)
		{
			CPlanet *
			Planet = new CPlanet();
			Planet->initialize();
			Planet->set_owner(0);
			Planet->set_order(i);
			Planet->set_cluster(EmpireCluster);
			Planet->set_name(EmpireCluster->get_new_planet_name());
			Planet->set_max_population();
			Planet->init_planet_news_center();

			Planet->type(QUERY_INSERT);
			STORE_CENTER->store(*Planet);

			PLANET_TABLE->add_planet(Planet);
			mEmpirePlanetList.add_planet(Planet);
		}

		/* Store Empire Planet Info */
		for (int i=0 ; i<mEmpirePlanetList.length() ; i++)
		{
			CPlanet *
			Planet = (CPlanet *)mEmpirePlanetList.get(i);

			CEmpirePlanetInfo *
			EmpirePlanetInfo = new CEmpirePlanetInfo();
			EmpirePlanetInfo->set_planet_id(Planet->get_id());
			EmpirePlanetInfo->set_planet_type(CEmpire::LAYER_EMPIRE_PLANET);

			EmpirePlanetInfo->type(QUERY_INSERT);
			STORE_CENTER->store(*EmpirePlanetInfo);

			mEmpirePlanetInfoList.add_empire_planet_info(EmpirePlanetInfo);
		}

		/* Fortress */
		for (int i=0 ; i<mFortressList.length() ; i++)
		{
			CFortress *
			Fortress = (CFortress *)mFortressList.get(i);
			Fortress->initialize(Fortress->get_layer(), Fortress->get_sector(), Fortress->get_order(), EMPIRE_GAME_ID);
		}

		/* Empire Capital Planet */
		mEmpireCapitalPlanet.initialize();

		/* Last Game Time */
		CGameStatus::mLastGameTime = time(0);
		CGameStatus * gstat = new CGameStatus();
		gstat->type(QUERY_INSERT);
		STORE_CENTER->store(*gstat);
		delete gstat;

	}
	else
	{
		//aMySQL->query("LOCK TABLE admiral READ");
		aMySQL->query("SELECT * FROM admiral WHERE owner = 0");
		aMySQL->use_result();

		while (aMySQL->fetch_row())
		{
			CAdmiral *
			Admiral = new CAdmiral(aMySQL->row());
			if (BlackMarketAdmiralList->get_by_id(Admiral->get_id()) != NULL) continue;

			CEmpireAdmiralInfo *
			EmpireAdmiralInfo = mEmpireAdmiralInfoList.get_by_admiral_id(Admiral->get_id());
			if (EmpireAdmiralInfo == NULL)
			{
				BLACK_MARKET->add_item(Admiral);
				continue;
			}
			else
			{
				switch (EmpireAdmiralInfo->get_admiral_type())
				{
					case CEmpire::LAYER_MAGISTRATE :
						{
							int
								ClusterID = EmpireAdmiralInfo->get_position_arg1();
							CMagistrate *
							Magistrate = mMagistrateList.get_by_cluster_id(ClusterID);
							Magistrate->add_admiral(Admiral);
						}
						break;

					case CEmpire::LAYER_FORTRESS :
						{
							int
								Layer = EmpireAdmiralInfo->get_position_arg1(),
							Sector = EmpireAdmiralInfo->get_position_arg2(),
							Order = EmpireAdmiralInfo->get_position_arg3();
							CFortress *
							Fortress = mFortressList.get_by_layer_sector_order(Layer, Sector, Order);
							Fortress->add_admiral(Admiral);
						}
						break;

					case CEmpire::LAYER_EMPIRE_CAPITAL_PLANET :
						{
							mEmpireCapitalPlanet.add_admiral(Admiral);
						}
						break;

					default :
						{
							SLOG("ERROR : Wrong get_admiral_type() in EmpireAdmiralInfo! get_admiral_id() = %d, get_admiral_type() = %d", EmpireAdmiralInfo->get_admiral_id(), EmpireAdmiralInfo->get_admiral_type());
						}
						break;
				}
			}
		}

		aMySQL->free_result();
		//aMySQL->query("UNLOCK TABLES");

		//aMySQL->query("LOCK TABLE fleet READ");
		aMySQL->query("SELECT * FROM fleet WHERE owner = 0");
		aMySQL->use_result();

		while (aMySQL->fetch_row())
		{
			int
				FleetID = as_atoi(aMySQL->row(CFleet::STORE_ID));
			if (BlackMarketFleetList->get_by_id(FleetID) != NULL) continue;
			CFleet *
			Fleet = new CFleet(EMPIRE_SHIP_DESIGN_TABLE, aMySQL->row());

			CEmpireFleetInfo *
			EmpireFleetInfo = mEmpireFleetInfoList.get_by_fleet_id(Fleet->get_id());
			if (EmpireFleetInfo == NULL)
			{
				SLOG("ERROR : No EmpireFleetInfo found in mEmpireFleetInfoList! Fleet ID = %d", Fleet->get_id());

				delete Fleet;
				continue;
			}
			else
			{
				switch (EmpireFleetInfo->get_fleet_type())
				{
					case CEmpire::LAYER_MAGISTRATE :
						{
							int
								ClusterID = EmpireFleetInfo->get_position_arg1();
							CMagistrate *
							Magistrate = mMagistrateList.get_by_cluster_id(ClusterID);
							Magistrate->add_fleet(Fleet);
						}
						break;

					case CEmpire::LAYER_FORTRESS :
						{
							int
								Layer = EmpireFleetInfo->get_position_arg1(),
							Sector = EmpireFleetInfo->get_position_arg2(),
							Order = EmpireFleetInfo->get_position_arg3();
							CFortress *
							Fortress = mFortressList.get_by_layer_sector_order(Layer, Sector, Order);
							Fortress->add_fleet(Fleet);
						}
						break;

					case CEmpire::LAYER_EMPIRE_CAPITAL_PLANET :
						{
							mEmpireCapitalPlanet.add_fleet(Fleet);
						}
						break;

					default :
						break;
				}
			}
			CEmpire::update_max_empire_fleet_id(Fleet);
		}

		aMySQL->free_result();
		//aMySQL->query("UNLOCK TABLES");

		//aMySQL->query("LOCK TABLE planet READ");
		aMySQL->query("SELECT * FROM planet WHERE owner = 0");
		aMySQL->use_result();

		while (aMySQL->fetch_row())
		{
			int
				PlanetID = as_atoi(aMySQL->row(CPlanet::STORE_ID));
			if (BlackMarketPlanetList->get_by_id(PlanetID) != NULL) continue;
			CPlanet *
			Planet = new CPlanet(aMySQL->row());

			CEmpirePlanetInfo *
			EmpirePlanetInfo = mEmpirePlanetInfoList.get_by_planet_id(Planet->get_id());
			if (EmpirePlanetInfo == NULL)
			{
				SLOG("ERROR : No EmpirePlanetInfo found in mEmpirePlanetInfoList! Planet ID = %d", Planet->get_id());

				delete Planet;
				continue;
			}

			switch (EmpirePlanetInfo->get_planet_type())
			{
				case CEmpire::LAYER_MAGISTRATE :
					{
						int
							ClusterID = EmpirePlanetInfo->get_position_arg();
						CMagistrate *
						Magistrate = mMagistrateList.get_by_cluster_id(ClusterID);
						if (Magistrate == NULL)
						{
							EmpirePlanetInfo->set_planet_type(CEmpire::LAYER_EMPIRE_PLANET);
							mEmpirePlanetList.add_planet(Planet);

							EmpirePlanetInfo->type(QUERY_UPDATE);
							STORE_CENTER->store(*EmpirePlanetInfo);
						}
						else
						{
							Magistrate->add_planet(Planet);
						}

					}
					break;

				case CEmpire::LAYER_EMPIRE_PLANET :
					{
						mEmpirePlanetList.add_planet(Planet);
					}
					break;

				default :
					break;
			}

			PLANET_TABLE->add_planet(Planet);
		}

		aMySQL->free_result();
		//aMySQL->query("UNLOCK TABLES");
	}

	SLOG("END : Initialize Empire");

	return true;
}

bool
	CPlayer::initialize_magistrate(CMySQL *aMySQL)
{
	SLOG("START : CPlayer::initialize_magistrate()");

	/* Create magistrates */
	for (int i=0 ; i<UNIVERSE->length() ; i++)
	{
		CCluster *
		Cluster = (CCluster *)UNIVERSE->get(i);
		if (Cluster->get_id() == 0) continue;

		CMagistrate *
		Magistrate = new CMagistrate();
		Magistrate->set_cluster_id(Cluster->get_id());

		mMagistrateList.add_magistrate(Magistrate);
	}

	SLOG("END : CPlayer::initialize_magistrate()");
	return true;
}

bool
	CPlayer::initialize_fortress(CMySQL *aMySQL)
{
	SLOG("START : CPlayer::initialize_fortress()");

	for (int i=1 ; i<=4 ; i++)
	{
		for (int j=1 ; j<=pow(2.0, (5-i)) ; j++)
		{
			for (int k=1 ; k<=pow(2.0, (5-i)) ; k++)
			{
				//aMySQL->query("LOCK TABLE fortress READ");
				aMySQL->query((char *)format("SELECT * FROM fortress WHERE layer = %d AND sector = %d AND fortress_order = %d", i, j, k));
				aMySQL->use_result();

				if (aMySQL->fetch_row() == NULL)
				{
					aMySQL->free_result();
					//aMySQL->query("UNLOCK TABLES");

					//aMySQL->query("LOCK TABLE fortress WRITE");
					aMySQL->query((char *)format("INSERT INTO fortress (layer, sector, fortress_order, owner) VALUES (%d, %d, %d, 0)", i, j, k));
					//aMySQL->query("UNLOCK TABLES");

					CFortress *
					Fortress = new CFortress();
					Fortress->set_layer(i);
					Fortress->set_sector(j);
					Fortress->set_order(k);
					Fortress->set_owner_id(EMPIRE_GAME_ID);

					mFortressList.add_fortress(Fortress);
				}
				else
				{
					CFortress *
					Fortress = new CFortress();
					Fortress->set_layer(as_atoi(aMySQL->row(CFortress::STORE_LAYER)));
					Fortress->set_sector(as_atoi(aMySQL->row(CFortress::STORE_SECTOR)));
					Fortress->set_order(as_atoi(aMySQL->row(CFortress::STORE_ORDER)));
					Fortress->set_owner_id(as_atoi(aMySQL->row(CFortress::STORE_OWNER_ID)));

					mFortressList.add_fortress(Fortress);

					aMySQL->free_result();
					//aMySQL->query("UNLOCK TABLES");
				}
			}
		}
	}

	SLOG("END : CPlayer::initialize_fortress()");
	return true;
}

bool
	CPlayer::initialize_empire_capital_planet(CMySQL *aMySQL)
{
	SLOG("START : CPlayer::initialize_empire_capital_planet()");

	//aMySQL->query("LOCK TABLE empire_capital_planet READ");
	aMySQL->query("SELECT * FROM empire_capital_planet");
	aMySQL->use_result();

	if (aMySQL->fetch_row() == NULL)
	{
		aMySQL->free_result();
		//aMySQL->query("UNLOCK TABLES");

		//aMySQL->query("LOCK TABLE empire_capital_planet WRITE");
		aMySQL->query((char *)format("INSERT INTO empire_capital_planet (owner_id) VALUES (%d)", EMPIRE_GAME_ID));
		//aMySQL->query("UNLOCK TABLES");

		EMPIRE_CAPITAL_PLANET->set_owner_id(EMPIRE_GAME_ID);
	}
	else
	{
		EMPIRE_CAPITAL_PLANET->set_owner_id(atoi(aMySQL->row(0)));

		aMySQL->free_result();
		//aMySQL->query("UNLOCK TABLES");
	}

	SLOG("END : CPlayer::initialize_empire_capital_planet()");
	return true;
}

bool
	CPlayer::is_dismissable()
{
#ifdef COUNCIL_PLAYER_DISMISSAL
	if (get_game_id() == get_council()->get_speaker_id()) {
		return false;
	}
	if ((mTurn-mNewsCenter.get_turn())*CGame::mSecondPerTurn > 60*60*24*3) {
		return true;
	}
#endif
	return false;
}

int
CPlayer::add_planet_invest_pool(int amount) {

	if ( mPlanetInvestPool + amount < 0 ) {
		amount = mPlanetInvestPool + amount;
		mPlanetInvestPool = 0;
		mStoreFlag += STORE_PLANET_INVEST_POOL;
		return amount;
	}
	if ( mPlanetInvestPool + amount > MAX_PLANET_INVESTMENT_PP ) {
		amount = mPlanetInvestPool + amount - MAX_PLANET_INVESTMENT_PP;
		mPlanetInvestPool = MAX_PLANET_INVESTMENT_PP;
		mStoreFlag += STORE_PLANET_INVEST_POOL;
		return amount;
	}
	mPlanetInvestPool += amount;
	mStoreFlag += STORE_PLANET_INVEST_POOL;
	return 0;
}

void
CPlayer::add_planet_invest_pool_usage(int amount) {
	if ( mPlanetInvestPoolUsage + amount < 0 ) {
		mPlanetInvestPoolUsage = 0;
		mStoreFlag += STORE_PLANET_INVEST_POOL;
		return;
	}
	if ( mPlanetInvestPoolUsage + amount > MAX_PLANET_INVESTMENT_PP ) {
		mPlanetInvestPool = MAX_PLANET_INVESTMENT_PP;
		mStoreFlag += STORE_PLANET_INVEST_POOL;
		return;
	}
	mPlanetInvestPoolUsage += amount;
	return;
}

char *
CPlayer::get_planets_invest_pool_html() {
	static CString
		Info;
	Info.clear();
	Info = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Info += "<TR BGCOLOR=\"#171717\">\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"136\" COLOR=\"#666666\" align=\"center\">";
	Info += "Name";
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"59\" CLASS=\"tabletxt\" COLOR=\"#666666\" align=\"center\">";
	Info += "Size";
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"121\" CLASS=\"tabletxt\" COLOR=\"#666666\" align=\"center\">";
	Info += "Resource";
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"69\" COLOR=\"#666666\" align=\"center\">";
	Info += "<input type=\"checkbox\" onClick=allCheck()>";
	Info += "</TH>\n";
	Info += "</TR>\n";

	for (int i=0 ; i<mPlanetList.length() ; i++)
	{
		CPlanet *
		Planet = (CPlanet *)mPlanetList.get(i);

		Info += "<TR>\n";

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\">&nbsp;<a href=\"planet_detail.as?PLANET_ID=%d\">%s</a></TD>",
			Planet->get_id(), Planet->get_name());

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\">&nbsp;%s</TD>",
			Planet->get_size_description());

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\">&nbsp;%s</TD>",
			Planet->get_resource_description());

		if (Planet->get_planet_invest_pool()) {
			Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\" align=\"center\">&nbsp;<input type=\"checkbox\" CHECKED name=\"PLANET_ID%d\" VALUE=\"ON\"></TD>",
				Planet->get_id());
		} else {
			Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\" align=\"center\">&nbsp;<input type=\"checkbox\" UNCHECKED name=\"PLANET_ID%d\" VALUE=\"ON\"></TD>",
				Planet->get_id());
		}

		Info += "</TR>\n";
	}
	Info += "</TABLE>\n";

	return (char *)Info;
}

CTechList*
CPlayer::get_available_tech_list()
{
	set_available_tech_list();
	return &mAvailableTechList;
}

const char *
	CPlayer::current_bounties_html()
{
	if(mBountyList.size() == 0) return NULL;

	static CString aResult;
	aResult.clear();

	aResult = "<TABLE WIDTH=\"250\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\"><TR BGCOLOR=\"#171717\"><TH WIDTH=\"118\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">Name</FONT></TH><TH WIDTH=\"118\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">Empire Points</FONT></TH></TR>";

	list<CBounty*> *aBountyList = mBountyList.get_current_bounties();

	list<CBounty*>::iterator current;
	CPlayer *aPlayer;
	for (current = aBountyList->begin(); current != aBountyList->end(); current++)
	{
		aPlayer = (*current)->get_target_player();
		if (!aPlayer)
		{
			SLOG("CPlayer::current_bounties_html() : null aPlayer");
			continue;
		}
		if (aPlayer->is_dead())
		{
			SLOG("CPlayer::current_bounties_html() : aPlayer is dead");
			continue;
		}
		aResult.format("<tr><td><a href=\"../info/player_search_result.as?PLAYER_ID=%d\">%s (#%d)</a></td><td ALIGN=\"CENTER\">%s</td></tr>",aPlayer->get_game_id(),aPlayer->get_name(),aPlayer->get_game_id(),dec2unit((*current)->get_empire_points()));
	}

	aResult += "</TABLE>";

	return (const char*)aResult;
}

const char *
	CPlayer::get_cluster_list_string()
{
	static CString aResult;
	aResult.clear();
	for (int i=0 ; i<mClusterList.length() ; i++)
	{
		CCluster *
		Cluster = (CCluster *)mClusterList.get(i);
		if (i > 0) aResult += ",&nbsp;";
		aResult += (CString)Cluster->get_name();

	}
	return aResult;
}

const char *
	CPlayer::possible_bounties_html()
{
	std::list<COfferedBounty*> *aBountyVector = OFFERED_BOUNTY_TABLE->get_available_bounties(this);
	if ((aBountyVector == NULL)||(aBountyVector->size() == 0)) return NULL;

	static CString aResult;
	aResult.clear();
	CPlayer *aPlayer;

	aResult = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\"><TR BGCOLOR=\"#171717\"><TH WIDTH=\"57\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">Select</FONT></TH><TH WIDTH=\"118\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">Name</FONT></TH><TH WIDTH=\"118\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">Power</FONT></TH><TH WIDTH=\"118\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">Council</FONT></TH></TR>";
	std::list<COfferedBounty*>::iterator BountyItr;
	for (BountyItr = aBountyVector->begin(); BountyItr != aBountyVector->end(); BountyItr++)
	{
		if ((*BountyItr) == NULL) return "There was a funky error";
		aPlayer = (*BountyItr)->get_player();
		aResult.format("<tr><td ALIGN=\"CENTER\"><input type=radio name=TARGET value=%d></td><td><a href=\"../info/player_search_result.as?PLAYER_ID=%d\">%s (#%d)</a></td><td ALIGN=\"CENTER\">%s</td><td>%s (#%d)</td></tr>",aPlayer->get_game_id(),aPlayer->get_game_id(),aPlayer->get_name(),aPlayer->get_game_id(),dec2unit(aPlayer->get_power()),aPlayer->get_council()->get_name(),aPlayer->get_council_id());
	}

	aResult += "</TABLE>";
	//i think this was leaking without a delete, not a big leak but a constant leak ever page view
	delete aBountyVector;
	return (const char*)aResult;
}

CShipDesign *
CPlayer::create_empire_design(int classIndex)
{

	int classLevelTable[5][5] = {
		{ 25, 30, 20, 15, 10 },
		{ 20, 30, 25, 15, 10 },
		{ 15, 35, 25, 15, 10 },
		{ 10, 30, 25, 25, 10 },
		{  0, 25, 40, 25, 10 }
	};
	int levelIndex = number(100);
	for(int i=0 ; i<5; i++)
	{
		levelIndex -= classLevelTable[(classIndex-1)/2][i];
		if( levelIndex <= 0 )
		{
			levelIndex = i+1;
			break;
		}
	}

	CShipSize *
	shipSize = SHIP_SIZE_TABLE->get_by_id( 4000 + classIndex );

	CShipDesign *
	shipDesign = new CShipDesign;

	shipDesign->set_design_id( get_ship_design_list()->max_design_id()+1 );

	CString
		name;
	name.format( "Imperial %s Mk. %d", shipSize->get_name(), levelIndex );

	shipDesign->set_name( name );
	shipDesign->set_owner( get_game_id() );
	shipDesign->set_body( shipSize->get_id() );
	CComponentList
		*weaponList = new CComponentList,
	*deviceList = new CComponentList;
	for(int i=0 ; i<COMPONENT_TABLE->length() ; i++)
	{
		CComponent*
		component = (CComponent*)COMPONENT_TABLE->get(i);
		if( component->get_level() == levelIndex )
		{
			switch( component->get_category() )
			{
				case CComponent::CC_ARMOR:
					{
						shipDesign->set_armor( component->get_id() );
						break;
					}
				case CComponent::CC_ENGINE:
					{
						shipDesign->set_engine( component->get_id() );
						break;
					}
				case CComponent::CC_COMPUTER:
					{
						shipDesign->set_computer( component->get_id() );
						break;
					}
				case CComponent::CC_SHIELD:
					{
						shipDesign->set_shield( component->get_id() );
						break;
					}
			} //switch
		} //if

		if ((component->get_level() <= levelIndex ) && (component->get_category()==CComponent::CC_DEVICE))
			deviceList->add_component( component );
		if ((component->get_level() == levelIndex ) && (component->get_category()==CComponent::CC_WEAPON))
			weaponList->add_component( component );
	} //for

	for (int i=0 ; i<shipSize->get_weapon() ; i++)
	{
		CWeapon *
		weapon = (CWeapon *)weaponList->get(number(weaponList->length()) - 1);
		if (weapon == NULL)
		{
			SLOG("ERROR : weapon is NULL in CBlackMarket::create_new_fleet()");
			continue;
		}
		if ((weapon->get_id() != 6308) // != Nahodoom Ray Launcher
			&& (weapon->get_id() != 6110) // != Psi Storm Launcher
			&& (weapon->get_id() != 6208) // != Psionic Pulse Shocker
			&& (weapon->get_id() != 6205) // != Death Spore
			&& (weapon->get_id() != 6105)) // != Psi Blaster

		{
			shipDesign->set_weapon(i, weapon->get_id());
			int
				WeaponNumber = shipSize->get_slot()/weapon->get_space();
			if (WeaponNumber < 1) WeaponNumber = 1;
			shipDesign->set_weapon_number(i, WeaponNumber);
		}
		else i--;
	}

	CIntegerList
		DeviceIDList;

	int MaxDevices = shipSize->get_device();
	if ((levelIndex == 1) && (MaxDevices > 2)) MaxDevices = 2;

	for (int i=0 ; i<MaxDevices ; i++)
	{
		CDevice *
		device = (CDevice *)deviceList->get(number(deviceList->length()) - 1);
		if (device == NULL)
		{
			SLOG("ERROR : device is NULL in CEmpireAction::create_empire_design()");
			continue;
		}

		if ((DeviceIDList.find_sorted_key((void *)device->get_id()) == -1) // disable duplicates
			&& (device->get_id() != 5525) // != Space Mining Module
			&& (device->get_id() != 5503) // != Psi Drive
			&& (device->get_id() != 5506) // != Psi Control System
			&& (device->get_id() != 5507) // != Insanity Field Generator
			&& (device->get_id() != 5519) // != Mind Tracker
			&& (device->get_id() != 5526)) // != Anti-Psi Device
		{
			if ( ((device->get_id() == 5512) && (classIndex > 5)) // Cloaking Field Generator
				|| ((device->get_id() == 5514) && (classIndex < 6)) ) // Auto-Repair Device
				i--;
			else
			{
				DeviceIDList.insert_sorted((void *)device->get_id());
				shipDesign->set_device(i, device->get_id());
			}
		}
		else i--;
	}
	shipDesign->set_build_cost(shipSize->get_cost());
	shipDesign->make_empire_design();
	get_ship_design_list()->add_ship_design(shipDesign);
	return shipDesign;
}
