#if !defined(__ARCHSPACE_GAME_H__)
#define __ARCHSPACE_GAME_H__

#include "ending.h"
#include "player.h"
#include "portal.h"

class CPlayerTable;
class CPersonalEndingTable;
class CRaceTable;
class CCouncilTable;
class CPlayerActionTable;
class CBattleRecordTable;
class CPlayerRelationTable;
class CCouncilActionTable;
class CCouncilRelationTable;
class CSpyTable;
class CEventTable;
class CAdminTool;
class CGlobalEnding;
class CAdminTool;
class CAdmiralNameTable;
class CBountyTable;
class CProjectList;

class CGame
{
	public :
		enum ELanguage
		{
			LANG_EN = 0,
			LANG_KO
		};

	public:
		static ELanguage mLanguage;
		static CString mImageServerURL;
		static CString mForumServerURL;

		static time_t mGameStartTime;
		static time_t mServerStartTime;
		static time_t mSecondPerTurn;
		static bool mUpdateTurn;
		static int mMaxUser;
		static int mSiegeBlockadeRestrictionDuration;
		static int mSiegeBlockadeProtectionDuration;
		static bool mSiegeBlockadeRestriction;
		static bool mSiegeBlockadeProtection;

		static int mMinAvailableCluster;
		static int mMinCouncilCount;

		static void lock();
		static void unlock();

		static void spawn_update_thread();
		static void kill_update_thread();

		static int mTechRate;
		static int mCombatReturnRate;
		static time_t mLastCouncilElection;


	public:
		CGame();
		~CGame();

		char *get_charset();

		CPlayer *get_player_by_portal_id(int aPortalID);
		CPlayer *get_player_by_game_id(int aGameID);
		CPlayer *get_player_by_name(const char *aName);

		bool initialize(CIniFile *aConfig);
		bool connect_database();
		void remove_old_player();

		bool script_table();
        bool database_table();

	public:
		CPlayer* create_new_player(int aPortalID, const char *aName,
													int aRace);

		CCouncil *create_new_council(CCluster *aCluster, char *aName = NULL);
		bool create_new_action( CPlayer *Owner, int aAction,
											int aTarget = -1 );

		static inline time_t get_game_time();
		static inline int get_game_time_in_weeks();
		static inline int get_game_time_in_days();

	public:
		CUniverse *universe() { return mUniverse; }
		CPlayerTable *player_table() { return mPlayerTable; }
		CPlanetTable *planet_table() { return mPlanetTable; }
		CPortalUserTable *portal_table() { return mPortalUserTable; }
		CTechTable *tech_table() { return mTechTable; }
		CProjectTable *project_table() { return mProjectTable; }
		CProjectList *ending_project_list() { return mEndingProjectList; }
//		CProjectTable *secret_project_table() { return mSecretProjectTable; }
//		CPersonalEndingTable *personal_ending_table() { return mPersonalEndingTable; }
//		CProjectTable *ending_project_table() { return mEndingProjectTable; }
		CRaceTable *race_table() { return mRaceTable; }
		CCouncilTable *council_table() { return mCouncilTable; }
		CComponentTable *component_table() { return mComponentTable; }
		CShipSizeTable *ship_table() { return mShipSizeTable; }
		CAdmiralNameTable *admiral_name_table() { return mAdmiralNameTable; }
		CPlayerActionTable *player_action_table() { return mPlayerActionTable; }
		CBattleRecordTable *battle_record_table() { return mBattleRecordTable; }
		CPlayerRelationTable *player_relation_table() { return mPlayerRelationTable; }
		CCouncilActionTable *council_action_table() { return mCouncilActionTable; }
		CCouncilRelationTable *council_relation_table() { return mCouncilRelationTable; }
		CSpyTable *spy_table() { return mSpyTable; }
		CEventTable *event_table() { return mEventTable; }
		CEventInstanceList *galactic_event_list() { return mGalacticEventList; }
		CShipDesignList *get_empire_ship_design_table() { return mEmpireShipDesignTable; }
		CAdminTool *get_admin_tool() { return mAdminTool; }
		CBountyTable *bounty_table() { return mBountyTable; }
		COfferedBountyTable *offered_bounty_table() { return mOfferedBountyTable; }
		bool set_global_ending_data();
		CGlobalEnding *get_global_ending_data() { return &mGlobalEndingData; }
		CIniFile *get_config() { return mConfig; }
        CMySQLPool *get_mysql_pool() { return mMySQLPool; }

	protected:
		CUniverse				*mUniverse;
		CPlayerTable 			*mPlayerTable;
		CPlanetTable 			*mPlanetTable;
        CPortalUserTable        *mPortalUserTable;
		CTechTable 				*mTechTable;
		CProjectTable			*mProjectTable;
//		CProjectTable			*mSecretProjectTable;
//		CPersonalEndingTable	*mPersonalEndingTable;
//		CProjectTable			*mEndingProjectTable;
		CRaceTable 				*mRaceTable;
		CCouncilTable			*mCouncilTable;
		CComponentTable			*mComponentTable;
		CShipSizeTable			*mShipSizeTable;
		CAdmiralNameTable		*mAdmiralNameTable;
		CPlayerActionTable  	*mPlayerActionTable;
		CPlayerRelationTable	*mPlayerRelationTable;
		CCouncilActionTable		*mCouncilActionTable;
		CCouncilRelationTable	*mCouncilRelationTable;
		CBattleRecordTable		*mBattleRecordTable;
		CSpyTable				*mSpyTable;
		CEventTable				*mEventTable;
		CEventInstanceList		*mGalacticEventList;
		CShipDesignList			*mEmpireShipDesignTable;
		CProjectList			*mEndingProjectList;

		CAdminTool *
			mAdminTool;

		CIniFile *mConfig;

		CShipDesign
			mInitialDesign1,
			mInitialDesign2;

		CBountyTable		*mBountyTable;
		COfferedBountyTable	*mOfferedBountyTable;

	private :
		CGlobalEnding
			mGlobalEndingData;
        CMySQLPool *mMySQLPool;

	protected:
		bool tech_encyclopedia();
		bool race_encyclopedia();
		bool project_encyclopedia();
		bool load_tick(CMySQL &aMySQL);
		bool load_project(CMySQL &aMySQL);
		bool load_admission(CMySQL &aMySQL);
		bool load_diplomatic_message(CMySQL &aMySQL);
		bool load_council_message(CMySQL &aMySQL);
		bool load_bounty(CMySQL &aMySQL);
		bool component_encyclopedia();	/* telecard 2000/10/02 */
		bool ship_encyclopedia();	/* telecard 2000/10/05 */
		bool spy_encyclopedia();

		static pth_mutex_t mMutex;
		static pth_t mUpdateThread;
};

inline time_t
CGame::get_game_time()
{
	//if (!mUpdateTurn) return 0;
	return time(0)-mServerStartTime+mGameStartTime;
}

// Returns the game time to the nearest day
inline int CGame::get_game_time_in_days() {
    return get_game_time()/ONE_DAY;
}

// Returns the game time to the nearest week
inline int CGame::get_game_time_in_weeks() {
    return get_game_time()/ONE_WEEK;
}

/**************************************************************************/
class CGameStatus : public CStore
{
	public:
		enum
		{
			STORE_LAST_GAME_TIME = 0
		};

	public:
		static time_t mLastGameTime;

	public:
		CGameStatus();
		virtual ~CGameStatus();
		virtual CString& query();
		virtual const char *table() { return "game_status"; }
		static inline time_t get_down_time();
		static void load(CMySQL * aMySQL);

};

inline time_t
CGameStatus::get_down_time()
{
	time_t DownTime = time(0) - mLastGameTime;
	if (DownTime < 0 || DownTime > 60*60*24*7) return 0;
	else return DownTime;
}


#endif
