#if !defined(__ARCHSPACE_PLAYER_H__)
#define __ARCHSPACE_PLAYER_H__

#include "universe.h"
#include "planet.h"
#include "tech.h"
#include "project.h"
#include "component.h"
#include "ship.h"
#include "action.h"
#include "relation.h"
#include "defense.plan.h"
#include "message.h"
#include "empire.h"
#include "event.h"
#include "magistrate.h"
#include "fortress.h"
#include "blackmarket.h"
#include "spy.h"
#include "rank.h"
#include "bounty.h"
#include "preference.h"

class CCouncil;
class CRace;
class CAdmission;
class CRankTable;

//------------------------------------------------------- CPlayer

struct LostPlanet
{
  time_t time;
  LostPlanet *next;
};

extern TZone gPlayerZone;
/**
*/

class CPlayer: public CStore
{
	public:
		CPlayer(MYSQL_ROW aRow);
		CPlayer(int aPortalID, int aGameID,
					const char *aName, int aRace, CCouncil* aCouncil);
		/* For Empire */
		CPlayer(int aGameID);
		virtual ~CPlayer();

		virtual const char *table() { return "player"; }
		virtual CString& query();

		enum EConcentrationMode
		{
			MODE_BALANCED = 0,
			MODE_INDUSTRY,
			MODE_MILITARY,
			MODE_RESEARCH,
			MODE_MAX
		};

		enum {
			DA_EXILE_IS_BEING_PROCESSED,
			DA_IN_DANGER_OF_EXILE,
			DA_THIRD_WARNING,
			DA_SECOND_WARNING,
			DA_FIRST_WARNING,
			DA_DISLIKED_IN_COURT,
			DA_ON_THE_EMPIRES_BAD_SIDE,
			DA_IGNORED_BY_THE_MAGISTRATE,
			DA_AVERAGE_SUBJECT,
			DA_FRIEND_OF_THE_MAGISTRATE,
			DA_ON_THE_EMPIRES_GOOD_SIDE,
			DA_WELL_KNOWN_IN_COURT,
			DA_FRIEND_OF_THE_EMPEROR,
			DA_TRUSTED_SUBJECT,
			DA_UPSTANDING_SUBJECT,
			DA_EXEMPLARY_SUBJECT,
			DA_PERFECT_SUBJECT,
			DM_EXILED_SUBJECT,
			DM_IGNORED_FUGITIVE,
			DM_THORN_IN_THE_EMPIRES_SIDE,
			DM_ANNOYANCE_TO_THE_EMPIRE,
			DM_CRIMINAL,
			DM_LIGHTLY_CHASED,
			DM_PURSUED_CRIMINAL,
			DM_DANGEROUS_CRIMINAL,
			DM_ENEMY_OF_THE_EMPIRE,
			DM_LIGHT_BOUNTY_OFFERED,
			DM_LOCALLY_PURSUED,
			DM_KNOWN_THROUGHOUT_THE_CLUSTER,
			DM_PURSUED_THROUGHOUT_THE_CLUSTER,
			DM_KNOWN_THROUGHOUT_THE_GALAXY,
			DM_LARGE_BOUNTY_ON_YOUR_HEAD,
			DM_PURSUED_THROUGHOUT_THE_GALAXY,
			DM_PURSUED_ANYWHERE_IN_THE_UNIVERSE
		};

		enum { // court rank
			CR_NONE,
			CR_DUKE,
			CR_MARQUIS,
			CR_EARL,
			CR_VISCOUNT,
			CR_BARON,
			CR_ROGUE_DUKE,
			CR_ROGUE_MARQUIS,
			CR_ROGUE_EARL,
			CR_ROGUE_VISCOUNT,
			CR_ROGUE_BARON
		};

		enum EProtectedMode
		{
			PROTECTED_NONE = 0,
			PROTECTED_PLANET_LOSS = 1,
#if defined(PROTECTION_24HOURS) && defined(PROTECTION_BEGINNER)
			PROTECTED_24HOURS,
#endif
#if defined(PROTECTION_24HOURS) && !defined(PROTECTION_BEGINNER)
			PROTECTED_24HOURS
#endif
#ifdef PROTECTION_BEGINNER
			PROTECTED_BEGINNER
#endif
		};

	protected:
		enum EStoreFlag
		{
			STORE_GAME_ID = 0,
			STORE_PORTAL_ID,
			STORE_NAME,
			STORE_HOME_CLUSTER_ID,
			STORE_LAST_LOGIN,
			STORE_LAST_LOGIN_IP,
			STORE_MODE,
			STORE_RACE,
			STORE_HONOR,
			STORE_RESEARCH_INVEST,
			STORE_TICK,
			STORE_TURN,
			STORE_PRODUCTION,
			STORE_SHIP_PRODUCTION,
			STORE_INVESTED_SHIP_PRODUCTION,
			STORE_RESEARCH,
			STORE_ABILITY,
			STORE_TARGET_TECH,
			STORE_ADMIRAL_TIMER,
			STORE_HONOR_TIMER,

			STORE_LAST_TURN_PRODUCTION,
			STORE_LAST_TURN_RESEARCH,
			STORE_LAST_TURN_MILITARY,

			STORE_COUNCIL_ID,
			STORE_COUNCIL_VOTE,
			STORE_COUNCIL_PRODUCTION,
			STORE_COUNCIL_DONATION,
			STORE_SECURITY_LEVEL,
			STORE_ALERTNESS,
			STORE_EMPIRE_RELATION,
			STORE_PROTECTED_MODE,
			STORE_PROTECTED_TERMINATE_TIME,

			STORE_NEWS_TURN,
			STORE_NEWS_PRODUCTION,
			STORE_NEWS_RESEARCH,
			STORE_NEWS_POPULATION,

			STORE_PLANET_INVEST_POOL,
			STORE_ADMISSION_TIME_LIMIT,
		};

	private:
		int mGameID;
		int	mPortalID;

		time_t mTick;
		int	mTurn;
		time_t
			mLastLogin;
		CString
			mLastLoginIP;
		time_t mAdmissionTimeLimit;
		CRace *mRace;

		int mTargetTech;  	// target tech ID
							// 0 - free research
							// non-0 - target tech ID research
		int
			mResearch,
			mShipProduction,
			mInvestedShipProduction;

		int
			mProduction;

		int mPopulationIncreased;
		int	mHonor;
		

		int mResearchInvest;

		int mAdmiralTimer;
		int mHonorTimer;
		LostPlanet *mFirstLostPlanet;
		LostPlanet *mLastLostPlanet;
		int mLostPlanets;

		CPreference *mPreference;
		CString mName;
		CString mNick;

		CCommandSet mAbility;

		CClusterList mClusterList;
		CPlanetList mPlanetList;

		CControlModel mProjectCM;
		CControlModel mTechCM;
		CControlModel mControlModel;

		CResource mLastTurnResource;
		CResource mNewResource;

		CKnownTechList mTechList;
		CTechList mAvailableTechList;

		CPurchasedProjectList mPurchasedProjectList;
		CProjectList mProjectList;

		CAdmiralList mAdmiralList;
		CAdmiralList mAdmiralPool;

		CComponentList mComponentList;
		CShipDesignList mShipDesignList;
		CShipBuildQ mShipBuildQ;
		CDock mDock;
		CRepairBay mRepairBay;

		CPlayerActionList mActionList;
		CPlayerRelationList mRelationList;

		CDefensePlanList mDefensePlanList;

		CDiplomaticMessageBox mMessageBox;

		int
			mCouncilID;

		int
			mHomeClusterID;

		CAdmission *mAdmission;

		int mCouncilProduction;
		int mCouncilDonation;
		int mCouncilVoteGain;
		int mCouncilVote;

		int mSecurityLevel;
		int mAlertness;
		int mEmpireRelation;
		CEmpireActionList mEmpireActionList;

		int mPower;

		CNewsCenter mNewsCenter;

		CFleetList mFleetList;
		CAllyFleetList mAllyFleetList;

		EConcentrationMode mMode;

		EProtectedMode mProtectedMode;
		time_t mProtectedTerminateTime;

		CPlayer *mPrev;
		CPlayer *mNext;

		CPlayerEffectList
			mEffectList;

		CEventInstanceList
			mEventList;

		int mPlanetInvestPool;
		int mPlanetInvestPoolUsage;

		CBountyList
			mBountyList;

	/* For Empire */
	private:
		CEmpireAdmiralInfoList
			mEmpireAdmiralInfoList;
		CEmpireFleetInfoList
			mEmpireFleetInfoList;
		CEmpirePlanetInfoList
			mEmpirePlanetInfoList;

		CSortedPlanetList
			mEmpirePlanetList;

		CMagistrateList
			mMagistrateList;
		CFortressList
			mFortressList;
		CEmpireCapitalPlanet
			mEmpireCapitalPlanet;

		CBlackMarket
			mBlackMarket;

	public: // get
		inline int get_portal_id() const;
		inline int get_game_id() const;

		inline const char* get_name() const;
		const char *get_nick() const { return (char *)mNick; }
		bool refresh_nick();
		const char *get_real_name() const { return (char *)mName; }
		time_t get_last_login() { return mLastLogin; }
		time_t get_admission_time_limit() { return mAdmissionTimeLimit; }
		char *get_last_login_ip() { return mLastLoginIP; }
		int get_race();
		char *get_race_name();
		const char *get_race_name2();
		inline CRace *get_race_data();

		inline CPlanet *get_planet(int aPlanetID);
		bool is_border_area( int aPlanetID );
		const char *html_select_border_planet();

		int calc_population();
		char *calc_population_with_unit();
		int get_research_lab();

		int get_production() { return mProduction; }
		bool set_production(int aProduction);

		inline int get_ship_production();
		inline int get_invested_ship_production();
		inline int get_research();
		int get_research_cost(CTech *aTech);
		inline int get_target_tech();
		inline int get_last_turn_production();
		inline int get_last_turn_research();
		inline int get_last_turn_military();
		inline int get_last_turn_population_increased();	//telecard 2001/04/02
		inline int get_honor();
	
		const char* get_cluster_list_string();
		const char* get_honor_description();
		

		inline int get_research_invest();

		inline time_t get_tick();
		inline int get_turn();

		inline int get_mode();
		const char *get_mode_name();

		inline EProtectedMode get_protected_mode();

		CCouncil *get_council();
		int get_council_id() { return mCouncilID; }
		inline int get_council_vote();
		int get_council_production() { return mCouncilProduction; }
		inline int get_council_donation();
		inline int get_max_donation();
		inline int get_council_vote_gain();

		int get_home_cluster_id() { return mHomeClusterID; }

		CPreference *get_preference() { return mPreference; }
		int get_security_level() { return mSecurityLevel; }
		int get_security_status()
			{ return CSpy::get_security_status(mSecurityLevel); }
		int get_alertness() { return mAlertness; }
		int get_empire_relation() { return mEmpireRelation; }
		int get_degree_of_empire_relation();
		const char *get_degree_name_of_empire_relation();
		int get_court_rank();
		void set_court_rank( int aRank );
		const char *get_court_rank_name( int aRank = -1 );
		void change_empire_relation( int aChange );
		CEmpireActionList *get_empire_action_list() { return &mEmpireActionList; }
		CEventInstanceList *get_event_list() { return &mEventList; }
		bool test_degree_of_amity();
		int get_degree_of_amity_bonus_from_rank();
		int get_support_bonus_from_rank();
		int get_relation_lost_per_day_by_rank();
		int get_tribute_bonus_by_rank();
		CEmpireAction *grant_boon( int aAction );
		void expire_empire_demand( int aHowOld );
		void expire_event();

		inline bool has_ability(const EAbility aAbility);
		inline CAdmission *get_admission();
		CComponentList *get_component_list() { set_component_list(); return &mComponentList; }
		CShipDesignList *get_ship_design_list() { return &mShipDesignList; }
		CShipBuildQ *get_build_q() { return &mShipBuildQ; }
		CDock *get_dock() { return &mDock; }
		CRepairBay *get_repair_bay() { return &mRepairBay; }
		CControlModel *get_control_model() { return &mControlModel; }
		CControlModel *get_project_CM() { return &mProjectCM; }
		CControlModel *get_tech_CM() { return &mTechCM; }
		CFleetList *get_fleet_list() { return &mFleetList; }
		CAllyFleetList *get_ally_fleet_list() { return &mAllyFleetList; }
		CAdmiralList *get_admiral_list() { return &mAdmiralList; }
		CAdmiralList *get_admiral_pool() { return &mAdmiralPool; }
		CDefensePlanList *get_defense_plan_list() { return &mDefensePlanList; }

		CProjectList *get_project_list() { set_project_list(); return &mProjectList; }
		CPurchasedProjectList *get_purchased_project_list()
			{ return &mPurchasedProjectList; }
		CKnownTechList *get_tech_list() { return &mTechList; }
		CTechList *get_available_tech_list();
		CDiplomaticMessageBox *get_diplomatic_message_box() { return &mMessageBox; }
		CPlayerActionList *get_action_list() { return &mActionList; }
		CPlayerRelationList *get_relation_list() { return &mRelationList; }

		CResource *get_new_resource() { return &mNewResource; }
		bool is_dismissable();

		inline int get_planet_invest_pool() { return mPlanetInvestPool; }
		inline int get_planet_invest_pool_usage() { return mPlanetInvestPoolUsage; }
		inline void set_planet_invest_pool_usage(int amount) { mPlanetInvestPoolUsage = amount; }
		void add_planet_invest_pool_usage(int amount);
		int add_planet_invest_pool(int amount);
		inline void set_planet_invest_pool(int amount) { mPlanetInvestPool = amount; }

		char *get_planets_invest_pool_html();
		inline CBounty* get_bounty_by_player_id(int aID);
		inline void remove_bounties_on_player(int aID);

		inline void lost_planet();
		inline bool is_protected();
		inline void clear_lost_planet();

        CShipDesign *create_empire_design (int classIndex);

		static int mPeriodCreateAdmiral;
		static int mAdmiralMilitaryBonus;
		static int mMaxPeriodCreateAdmiral;
		static int mMaxAdmirals;
		static int mExtraBlackMarketAdmirals;
		static int mRetireTimeLimit;
        static int mAdmissionLimitTime;
        static int mHonorIncreaseTurns;
        static int mProtectionPlanetLossTime;
        static int mProtectionPlanetLossAmount;
	/* For Empire */
	public:
		CPlanet *get_target_empire_planet();

		CEmpireAdmiralInfoList *get_empire_admiral_info_list()
				{ return &mEmpireAdmiralInfoList; }
		CEmpireFleetInfoList *get_empire_fleet_info_list()
				{ return &mEmpireFleetInfoList; }
		CEmpirePlanetInfoList *get_empire_planet_info_list()
				{ return &mEmpirePlanetInfoList; }

		bool add_empire_planet(CPlanet *aPlanet);
		bool remove_empire_planet(int aPlanetID);
		CSortedPlanetList *get_empire_planet_list() { return &mEmpirePlanetList; }
		int get_number_of_empire_planets();

		CSortedPlanetList *get_lost_empire_planet_list();
		int get_number_of_lost_empire_planets();

		CMagistrateList *get_magistrate_list() { return &mMagistrateList; }
		CFortressList *get_fortress_list() { return &mFortressList; }
		CEmpireCapitalPlanet *get_empire_capital_planet() { return &mEmpireCapitalPlanet; }
		CBlackMarket *get_black_market() { return &mBlackMarket; }

		bool initialize(CMySQL *aMySQL);

	public: // set
		inline void set_portal_id(int aPortalID);
		inline void set_game_id(int aGameID);

		bool set_name(char *aName);
		void set_race(int aRace);

		bool set_home_cluster_id(int aHomeClusterID);

		void set_last_login(int aLastLogin)
			{ mLastLogin = aLastLogin; mStoreFlag += STORE_LAST_LOGIN; }
		void set_last_login_ip(char *aLastLoginIP)
			{ mLastLoginIP = aLastLoginIP; mStoreFlag += STORE_LAST_LOGIN_IP; }
		bool change_reserved_production(int aProduction);
		bool set_ship_production(int aProduction);
		bool change_ship_production(int aProduction);
		bool change_invested_ship_production(int aProduction);
		bool change_research(int aResearch);

		int calc_ship_production();

		inline bool change_last_turn_production(int aProduction);
		inline bool change_last_turn_research(int aResearch);
		inline bool change_last_turn_military(int aMilitary);

		void set_project_list();
		void set_component_list();

		void set_honor(int aHonor);
	
		void change_honor(int aChange);
	
		void change_honor_with_news(int aChange);
	

		void change_research_invest(int aAmount);

		inline void set_tick(int aTick);

		inline void spend_turn();
		inline void set_mode( int aMode );
		void set_protected_mode(EProtectedMode aMode);

		bool set_target_tech( int aTechID );
		inline void reset_target_tech();
		void set_available_tech_list();

 		inline void set_preference(CPreference *aPreference);
		bool set_council(CCouncil *aCouncil);
		bool set_council(int aID);
		inline void set_council_vote(int aID);
		int get_council_vote_power();
		bool set_council_production(int aProduction);
		bool change_council_production(int aProduction);
		bool set_council_donation(int aDonation);
		bool change_council_donation(int aDonation);
		bool set_council_vote_gain(int aGain);
		bool change_council_vote_gain(int aGain);

		void set_security_level(int aLevel)
			{ mSecurityLevel = aLevel; mStoreFlag += STORE_SECURITY_LEVEL; }
		void set_alertness(int aAlertness)
			{ mAlertness = aAlertness; mStoreFlag += STORE_ALERTNESS; }
		inline void change_alertness(int aAlertness);
		void set_empire_relation(int aRelation)
			{ mEmpireRelation = aRelation; mStoreFlag += STORE_EMPIRE_RELATION; }

		inline void set_ability(const EAbility aAbility);
		inline void remove_ability(const EAbility aAbility);
		inline void set_admission(CAdmission *aAdmission) ;
		inline void reset_admission();

		inline bool is_dead();
		inline bool is_complete_dead();
		void set_dead(char *aReasonString = NULL);
		inline void set_complete_dead();
		inline void resurrect();
	public: // etc
        void init_race_innate();
		inline void add_admiral_list(CAdmiral *aAdmiral);
		inline void add_admiral_pool(CAdmiral *aAdmiral);
		void init_admiral_innate();

		bool add_planet(CPlanet *aPlanet);
		bool add_planet_when_loading(CPlanet *aPlanet);
		bool drop_planet(CPlanet *aPlanet);

		void update_turn();
		void update_tech();
		void update_admiral_pool();
		void build_control_model();
		void build_ship();
		int calc_real_ship_production();
		bool calc_all_ships_upkeep(int *aReservedPP, int aCurrentProductionPoint, int *aNewMilitaryPoint, int *aGainingProductionPoint, int *aLosingProductionPoint, int *aLosingMilitaryPoint);
		int pay_fleet_upkeep(int aNetIncome);
		void repair_damaged_ship();
		bool simulate_repair_damaged_ship(int *aReservedPP, int *aMilitaryPoint, int *aLosingProductionPoint, int *aLosingMilitaryPoint);
		const char *mission_handler();
		int find_new_planet(bool aAlways = false);

		void save_planet(CStoreCenter &aStoreCenter);

		bool discover_tech(int aTechID, bool aIsInnate = false);
		inline void add_tech (CKnownTech *aTech);
		bool know_tech( int aTechID );
		void forget_tech( int aTechID );
		inline int count_tech_by_category( int aCategory );
		bool buy_project(int aProject, bool aPayPrice = true);
		void buy_bm_project(int aProject);	//telecard 2001/03/14
		void buy_ending_project(int aProject);	//telecard 2001/04/02
		bool lose_project(int aProject);
		bool lose_all_project();
		inline void add_project(CPurchasedProject *aProject);
		inline bool is_project_list_empty();
		inline bool is_available_project(int aProjectID);//depreciated
		inline bool has_project(int aProjectID);
		void save_planet();
		void discover_basic_techs();
		void discover_tech_level(int aLevel);

		inline CPlanetList *get_planet_list();
		inline CClusterList &cluster_list();	//telecard 2001/03/31

		void new_player_assign_council();

		int get_power() { return mPower; }

		bool refresh_power();

		const char* independence_declaration(char *aName, const char *aSlogan);
		inline bool has_tech(int aTechID);

		void add_action(CPlayerAction* aAction);
		void remove_action(int aActionID);

		void add_relation(CPlayerRelation* aRelation);
		void remove_relation_non_bounty_all();
		void remove_relation_all();
		void remove_relation(int aRelationID);
		void remove_relation_by_target_id(int aTargetID);

		void add_diplomatic_message(CDiplomaticMessage* aMessage);
		CDiplomaticMessage* get_diplomatic_message(int aMessageID);
		void remove_diplomatic_message(int aMessageID);

		bool check_diplomatic_message(CDiplomaticMessage::EType aType, CPlayer* aPlayer);
		bool check_duplication_diplomatic_message(CDiplomaticMessage::EType aType, CPlayer* aPlayer);

		bool check_council_donation_effect_timer();
		void set_council_donation_effect_timer();

		CPlayerAction* check_break_pact_effect_timer(CPlayer *aPlayer);
		void set_break_pact_effect_timer(CPlayer *aPlayer);
		CPlayerAction* check_break_ally_effect_timer(CPlayer *aPlayer);
		void set_break_ally_effect_timer(CPlayer *aPlayer);
        CPlayerAction* check_declare_hostile_effect_timer(CPlayer *aPlayer);
        void set_declare_hostile_effect_timer(CPlayer *aPlayer);

		void declare_war(CPlayer *aPlayer);
		void declare_hostile(CPlayer *aPlayer);
		void break_pact(CPlayer *aPlayer);
		void break_ally(CPlayer *aPlayer);

		void store();

		void initialize_tech_control_model();

		char *process_spy(int aSpyID, CPlayer *aTargetPlayer, bool *aSuccess);

		bool can_buy_event(CEventInstance* EventInstance);

		inline void remove_bounty(int aID);
		inline bool accept_bounty(COfferedBounty* aBounty);
		inline bool insert_bounty(CBounty *aBounty);
		inline int active_bounties();
		const char *current_bounties_html();
		const char *possible_bounties_html();

		bool can_request_admission();
		void disable_admission();
		void disable_admission(time_t aTime);
		void enable_admission();
		const char* admission_limit_time_left();
	public:
		const char* check_attackable(CPlayer *aPlayer);
		const char* check_attackableHi(CPlayer *aPlayer);
		const char* check_privateer_attackable(CPlayer *aPlayer);
		bool check_attackable_power(CPlayer *aPlayer);

		void privateer(CFleet *aFleet);
		void return_privateer(CFleet *aFleet);
		bool has_cluster(int aClusterID);
		bool add_cluster(CCluster* aCluster);
		bool remove_cluster(int aClusterID);

		void rearrange_cluster();

		void add_cluster_list(CClusterList& aClusterList);

		void remove_from_database();
		void remove_from_memory();
		void remove_news_files();

	public:
		inline void new_planet_news(CPlanet* aPlanet);
		inline void time_news(const char* aNews);
		inline void new_admiral_news(CAdmiral *aAdmiral);

		const char *get_news_file_name();
		void load_news();

		CPlayerRelation *get_relation(CPlayer *aPlayer);

		const char* improve_relationship(
					CDiplomaticMessage::EType aType, CPlayer *aPlayer);

		void set_commerce_with(CPlayer *aPlayer);
		void clear_commerce_with(CPlayer *aPlayer);
	public:
		const char *available_science_list_html(CTech::ETechType aType);
		const char *known_science_list_html(CTech::ETechType aType);

		const char* select_concentration_mode_html();
		const char* available_component_list_html(); /* telecard 2000/10/04 */
		const char* generate_news();
		const char *project_list_html();
		const char *purchased_project_list_html();
		const char *planet_management_html(int aStartIndex, int aNumber);

		const char *diplomatic_message_select_html(CPlayer *aPlayer);
		const char *diplomatic_unread_message_table_html();
		const char *get_message_list_html(int First, bool aIsForAdmin);
		static char *get_message_list_by_sender_html(CPlayer *aSender, bool aIsForAdmin);

		CPlayer *prev(CPlayer* aPlayer=NULL);
		CPlayer *next(CPlayer* aPlayer=NULL);

		char *war_target_list_html();
		char *war_status_information_html();
		char *battle_record_list_html(bool aFromHead, int aStartBattleRecordIndex, int aNumberOfBattleRecord, int *aTotalRecord);

		char *available_spy_list_html();

		CPlayerEffectList *get_effect_list() { return &mEffectList; }

		bool has_effect(int aEffect, int aTarget = 0);

		int calc_PA(int aValue, int aEffect);
		int calc_minus_PA(int aValue, int aEffect);

		char *apply_effect(CPlayerEffect *aEffect);
		char *activate_event(CEventInstance *aInstance, const char *aAnswer = NULL);
		void upload_project_effect();

		bool has_siege_blockade_restriction();
		bool has_siege_blockade_protection();
		bool set_siege_blockade_restriction();
		bool set_siege_blockade_protection();
		int get_siege_blockade_restriction_time();
		int get_siege_blockade_protection_time();

	protected:
		static bool set_commerce_between(CPlayer *aPlayer1, CPlayer *aPlayer2);
		static bool clear_commerce_between(CPlayer *aPlayer1, CPlayer *aPlayer2);

		const char *get_mode_name(int aMode);

		void get_extra_income_by_effect();
		void pay_extra_cost_by_effect();
		int update_security(int aBalance);

		void process_imperial_retribution();

		friend class CNewsCenter;
		friend CStoreCenter& operator<<(CStoreCenter& aStoreCenter, CPlayer& aPlayer);
	RECYCLE(gPlayerZone);

	/* For Empire */
	protected :
		bool initialize_magistrate(CMySQL *aMySQL);
		bool initialize_fortress(CMySQL *aMySQL);
		bool initialize_empire_capital_planet(CMySQL *aMySQL);
};

inline int
CPlayer::get_portal_id() const
{
	return mPortalID;
}

inline int
CPlayer::get_game_id() const
{
	return mGameID;
}

inline const char*
CPlayer::get_name() const
{
	static CString Name;
	Name = mName;
	Name.htmlspecialchars();
	Name.nl2br();
	return (char*)Name;
}

/*
inline const char* CPlayer::get_nick() const {
     refresh_nick();
     return (char *)mNick;
}  */

inline CRace*
CPlayer::get_race_data()
{
	return mRace;
}

inline CPlanet*
CPlayer::get_planet(int aPlanetID)
{
	return mPlanetList.get_by_id(aPlanetID);
}

inline int
CPlayer::get_ship_production()
{
	return mShipProduction;
}

inline int
CPlayer::get_invested_ship_production()
{
	return mInvestedShipProduction;
}

inline int
CPlayer::get_research()
{
	return mResearch;
}

inline int
CPlayer::get_last_turn_production()
{
	return mLastTurnResource.get(RESOURCE_PRODUCTION);
}

inline int
CPlayer::get_last_turn_research()
{
	return mLastTurnResource.get(RESOURCE_RESEARCH);
}

inline int
CPlayer::get_last_turn_military()
{
	return mLastTurnResource.get(RESOURCE_MILITARY);
}

inline int
CPlayer::get_last_turn_population_increased()
{
	return mPopulationIncreased;
}

inline bool
CPlayer::change_last_turn_production(int aProduction)
{
	return mLastTurnResource.change(RESOURCE_PRODUCTION, aProduction);
}

inline bool
CPlayer::change_last_turn_research(int aResearch)
{
	return mLastTurnResource.change(RESOURCE_RESEARCH, aResearch);
}

inline bool
CPlayer::change_last_turn_military(int aMilitary)
{
	return mLastTurnResource.change(RESOURCE_MILITARY, aMilitary);
}

inline int
CPlayer::get_target_tech()
{
	return mTargetTech;
}

inline time_t
CPlayer::get_tick()
{
	return mTick;
}

inline int
CPlayer::get_turn()
{
	return mTurn;
}

inline int
CPlayer::get_mode()
{
	return (int)mMode;
}

inline CPlayer::EProtectedMode
CPlayer::get_protected_mode()
{
	return mProtectedMode;
}

inline int
CPlayer::get_council_vote()
{
	return mCouncilVote;
}

inline int
CPlayer::get_council_donation()
{
	return mCouncilDonation;
}

inline int
CPlayer::get_max_donation()
{
	return (mProduction / 10);
}

inline int
CPlayer::get_council_vote_gain()
{
	return mCouncilVoteGain;
}

inline bool
CPlayer::has_ability(const EAbility aAbility)
{
	return mAbility.has(aAbility);
}

inline bool
CPlayer::is_dead()
{
	if (mGameID == 0)
	{
		return mEmpireCapitalPlanet.is_dead();
	}
	else
	{
		return mAbility.has(ABILITY_DEAD)||mAbility.has(ABILITY_COMPLETE_DEAD);
	}
}

inline bool
CPlayer::is_complete_dead()
{
	return mAbility.has(ABILITY_COMPLETE_DEAD);
}

inline void
CPlayer::set_complete_dead()
{
	mStoreFlag += STORE_ABILITY;
	mAbility += ABILITY_COMPLETE_DEAD;
}

inline void
CPlayer::resurrect()
{
	mStoreFlag += STORE_ABILITY;
	mAbility -= ABILITY_DEAD;
	mAbility -= ABILITY_COMPLETE_DEAD;
}

inline CAdmission *
CPlayer::get_admission()
{
	return mAdmission;
}

inline int
CPlayer::get_honor()
{
	return mHonor;
}

inline int
CPlayer::get_research_invest()
{
	return mResearchInvest;
}


inline void
CPlayer::set_portal_id(int aPortalID)
{
	mPortalID = aPortalID;
}

inline void
CPlayer::set_game_id(int aGameID)
{
	mGameID = aGameID;
}

inline void
CPlayer::set_tick(int aTick)
{
	mTick = aTick;
	mStoreFlag += STORE_TICK;
}

inline void
CPlayer::reset_target_tech()
{
	mTargetTech = 0;
	mStoreFlag += STORE_TARGET_TECH;
}

inline void
CPlayer::set_mode( int aMode )
{
	mStoreFlag += STORE_MODE;
	mMode = (EConcentrationMode)aMode;
}

inline void
CPlayer::spend_turn()
{
	mTurn++;
}

inline void
CPlayer::set_preference(CPreference *aPreference)
{
    // does garbage collection take care of this? // shouldn't rely on garbage collection :-P
    if (mPreference != NULL)
        delete mPreference;
    mPreference = aPreference;
}

inline void
CPlayer::set_council_vote(int aID)
{
	mStoreFlag += STORE_COUNCIL_VOTE;
	mCouncilVote = aID;
}

inline void
CPlayer::change_alertness(int aAlertness)
{
	mAlertness += aAlertness;
	if (mAlertness < 0) mAlertness = 0;
	mStoreFlag += STORE_ALERTNESS;
}

inline void
CPlayer::set_ability(const EAbility aAbility)
{
	mStoreFlag += STORE_ABILITY;
	mAbility += aAbility;
}

inline void
CPlayer::remove_ability(const EAbility aAbility)
{
	mStoreFlag += STORE_ABILITY;
	mAbility -= aAbility;
}

inline void
CPlayer::set_admission( CAdmission *aAdmission )
{
	mAdmission = aAdmission;
}

inline void
CPlayer::reset_admission()
{
	mAdmission = NULL;
}

inline void
CPlayer::add_admiral_list(CAdmiral *aAdmiral)
{
	mAdmiralList.add_admiral(aAdmiral);
}

inline void
CPlayer::add_admiral_pool(CAdmiral *aAdmiral)
{
	mAdmiralPool.add_admiral(aAdmiral);
}

inline void
CPlayer::add_tech(CKnownTech *aTech)
{
	mTechList.add_known_tech(aTech);
	mTechCM += aTech->get_effect();
}

inline bool
CPlayer::has_tech(int aTechID)
{
	return (bool)mTechList.get_by_id(aTechID);
}

inline int
CPlayer::count_tech_by_category( int aCategory )
{
	return mTechList.count_by_category( aCategory );
}

inline void
CPlayer::add_project(CPurchasedProject *aProject)
{
	mPurchasedProjectList.add_project(aProject);
	mProjectCM += aProject->get_effect();
}

inline bool
CPlayer::is_project_list_empty()
{
	if (mProjectList.length()) return false;

	return true;
}

inline bool
CPlayer::is_available_project(int aProjectID)
{
	if(mProjectList.get_by_id(aProjectID)) return true;
	return false;
}

inline bool
CPlayer::has_project(int aProjectID)
{
	if(mPurchasedProjectList.get_by_id(aProjectID)) return true;
	return false;
}

inline CPlanetList *
CPlayer::get_planet_list()
{
	return &mPlanetList;
}

inline CClusterList&
CPlayer::cluster_list()
{
	return mClusterList;
}

inline void
CPlayer::new_planet_news(CPlanet* aPlanet)
{
	mNewsCenter.new_planet_news(aPlanet);
}

inline void
CPlayer::time_news(const char* aNewsString)
{
	if (aNewsString == NULL) return;

	mNewsCenter.time_news(aNewsString);
}

inline void
CPlayer::new_admiral_news(CAdmiral *aAdmiral)
{
	mNewsCenter.admiral_news(aAdmiral);
}

inline bool
CPlayer::insert_bounty(CBounty *aBounty)
{
	return mBountyList.insert_bounty(aBounty);
}

inline CBounty*
CPlayer::get_bounty_by_player_id(int aID)
{
	return mBountyList.get_bounty_by_player_id(aID);
}

inline bool
CPlayer::accept_bounty(COfferedBounty* aBounty)
{
	return mBountyList.accept_bounty(aBounty,this);
}

inline void
CPlayer::remove_bounties_on_player(int aID)
{
	mBountyList.remove_bounties_on_player(aID);
}

inline void
CPlayer::remove_bounty(int aID)
{
	mBountyList.remove_bounty(aID);
}

inline int
CPlayer::active_bounties()
{
	return mBountyList.size();
}

inline void CPlayer::lost_planet()
{
 if (mFirstLostPlanet == NULL)
 {
     mFirstLostPlanet = new LostPlanet();
     mFirstLostPlanet->next = NULL;
     mFirstLostPlanet->time = time(0);
     mLostPlanets = 1;
     mLastLostPlanet = mFirstLostPlanet;
 }
 else
 {
     LostPlanet *aLostPlanet = new LostPlanet();
     mLastLostPlanet->next = aLostPlanet;
     mLastLostPlanet = aLostPlanet;
     mLastLostPlanet->next = NULL;
     mLastLostPlanet->time = time(0);
     mLostPlanets++;
 }
}

inline void CPlayer::clear_lost_planet()
{
    LostPlanet *aLostPlanet = NULL;
    if (mFirstLostPlanet == NULL)
        return;
    mLostPlanets = 0;
    while (mFirstLostPlanet != NULL)
    {
     aLostPlanet = mFirstLostPlanet->next;
     delete mFirstLostPlanet;
     mFirstLostPlanet = aLostPlanet;
    }
}

inline bool CPlayer::is_protected()
{
    LostPlanet *aLostPlanet = NULL;

    if (mFirstLostPlanet && (mLostPlanets*1.0)/(mPlanetList.length()+mLostPlanets) >= (double)CPlayer::mProtectionPlanetLossAmount/(double)100)
	{
		if (time(0) - mLastLostPlanet->time >= CPlayer::mProtectionPlanetLossTime)
	    {

	    	clear_lost_planet();
	    }
	    else
	    {
	    	set_protected_mode(PROTECTED_PLANET_LOSS);
	    	return true;
	    }
	}
    else
    {
        LostPlanet *aPrevLostPlanet = NULL;
        for (aLostPlanet = mFirstLostPlanet; aLostPlanet != NULL; aLostPlanet = aLostPlanet->next)
        {
          if (time(0) - aLostPlanet->time >= CPlayer::mProtectionPlanetLossTime)
          {
              if (aPrevLostPlanet != NULL && aLostPlanet->next != NULL)
              {
                  aPrevLostPlanet->next = aLostPlanet->next;
              }
              else if (aPrevLostPlanet != NULL)
              {
		  		  aPrevLostPlanet->next = NULL;
                  mLastLostPlanet = aPrevLostPlanet;
              }
              else if (aLostPlanet->next != NULL)
              {
                  mFirstLostPlanet = aLostPlanet->next;
              }
	      else
	      {
		  mFirstLostPlanet = NULL;
		  mLastLostPlanet = NULL;
	      }

              mLostPlanets--;
              delete aLostPlanet;
          }
          aPrevLostPlanet = aLostPlanet;
        }
    }
   if (get_protected_mode() == PROTECTED_PLANET_LOSS) set_protected_mode(PROTECTED_NONE);
   return false;
}

//--------------------------------------------------------CPlayerList

extern TZone gPlayerListZone;
/**
*/
class CPlayerList: public CSortedList
{
	public:
		CPlayerList();
		virtual ~CPlayerList();

		bool add_player(CPlayer* aPlayer);
		bool remove_player(int aID);

		CPlayer *get_by_game_id(int aGameID);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "player list"; }

	RECYCLE(gPlayerListZone);
};


#define MAX_HASHTABLE 			512

#define INDEX_PORTAL_ID			1
#define INDEX_NAME				2

/**
*/
class CPlayerTable: public CSortedList
{
	class CPortalIDHashList: public CSortedList
	{
		public:
			CPortalIDHashList();
			virtual ~CPortalIDHashList();
		protected:
			virtual bool free_item(TSomething aItem);
			virtual int compare(TSomething aItem1, TSomething aItem2) const;
			virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
			virtual const char *debug_info() const { return "player table hash list"; }
	};

	class CNameHashList: public CSortedList
	{
		public:
			CNameHashList();
			virtual ~CNameHashList();
		protected:
			virtual bool free_item(TSomething aItem);
			virtual int compare(TSomething aItem1, TSomething aItem2) const;
			virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
			virtual const char *debug_info() const { return "player table hash list"; }
	};


	class CGameIDList: public CList
	{
		public:
			CGameIDList();
			virtual ~CGameIDList();
		protected:
			virtual bool free_item(TSomething aItem);
			virtual const char *debug_info() const { return "player table game id"; }
	};

	public:
		CPlayerTable();
		virtual ~CPlayerTable();

		bool load(CMySQL &aMySQL);


		int add_player(CPlayer* aPlayer);
		bool remove_player(int aGameID);

//		void remove_old_player();

		inline int get_max_id();
		CPlayer* get_by_game_id(int aGameID);
		CPlayer* get_by_portal_id(int aPortalID);
		CPlayer* get_by_name(const char* aName);

		void give_planet_to_owner( CPlanet *aPlanet );

		inline void insert_update_list(CPlayer *aPlayer);
		void delete_update_list(CPlayer *aPlayer);
		static void *update(void *aArg);

		char *select_player_except_one( CPlayer *aPlayer );
		void verify_player();

		void refresh_rank_table();

		void add_player_rank(CPlayer *aPlayer);
		void remove_player_rank(int aPlayerID);

		CRankTable *get_overall_rank_table() { return &mOverallRankTable; }
		CRankTable *get_fleet_rank_table() { return &mFleetRankTable; }
		CRankTable *get_planet_rank_table() { return &mPlanetRankTable; }
		CRankTable *get_tech_rank_table() { return &mTechRankTable; }

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "player table"; }

		bool load_tech(CMySQL &aMySQL);
		bool load_admiral(CMySQL &aMySQL);
		bool load_ship_design(CMySQL &aMySQL);
		bool load_ship_build_q(CMySQL &aMySQL);
		bool load_dock(CMySQL &aMySQL);
		bool load_repair_bay(CMySQL &aMySQL);
		bool load_fleet(CMySQL &aMySQL);
		bool load_plan(CMySQL &aMySQL);
		bool load_empire_action(CMySQL &aMySQL);
		bool load_event(CMySQL &aMySQL);
		bool load_effect(CMySQL &aMySQL);
		bool load_preferences(CMySQL &aMySQL);

	protected:
		int mMaxID;

		CPortalIDHashList
			mPortalIDIndex[MAX_HASHTABLE];
		CNameHashList
			mNameIndex[MAX_HASHTABLE];
		CGameIDList
			mGameIDIndex;
		CRankTable
			mOverallRankTable,
			mFleetRankTable,
			mPlanetRankTable,
			mTechRankTable;

		static CPlayer *mUpdatePlayer;
};

inline int
CPlayerTable::get_max_id()
{
	return mMaxID;
}

#endif
