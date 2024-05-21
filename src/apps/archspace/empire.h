#if !defined(__ARCHSPACE_EMPIRE_H__)
#define __ARCHSPACE_EMPIRE_H__

#include "fleet.h"
#include "admiral.h"

class CFleet;

class CEmpire
{
	public :
		enum EEmpireLayerType
		{
			LAYER_MAGISTRATE = 0,
			LAYER_EMPIRE_PLANET,
			LAYER_FORTRESS,
			LAYER_EMPIRE_CAPITAL_PLANET
		};

		enum EEmpireFleetGroupType
		{
			EMPIRE_FLEET_GROUP_TYPE_MAGISTRATE = 0,
			EMPIRE_FLEET_GROUP_TYPE_MAGISTRATE_COUNTERATTACK,
			EMPIRE_FLEET_GROUP_TYPE_EMPIRE_PLANET,
			EMPIRE_FLEET_GROUP_TYPE_EMPIRE_PLANET_COUNTERATTACK,
			EMPIRE_FLEET_GROUP_TYPE_FORTRESS,
			EMPIRE_FLEET_GROUP_TYPE_EMPIRE_CAPITAL_PLANET
		};

	public :
		static int
			mInitialNumberOfMagistratePlanet,
			mInitialNumberOfEmpirePlanet;
		static int
			mFleetRegenCycleInTurn;
		static int
			mAmountOfMagistrateShipRegen,
			mAmountOfFortress1ShipRegen,
			mAmountOfFortress2ShipRegen,
			mAmountOfFortress3ShipRegen,
			mAmountOfFortress4ShipRegen,
			mAmountOfCapitalPlanetShipRegen;
		static int
			mFortress1AdmiralFleetCommandingBonus,
			mFortress2AdmiralFleetCommandingBonus,
			mFortress3AdmiralFleetCommandingBonus,
			mFortress4AdmiralFleetCommandingBonus,
			mCapitalPlanetAdmiralFleetCommandingBonus,
			mEmpireInvasionLimit,
			mEmpireInvasionLimitDuration;
		static int
			mMaxEmpireFleetID;

	public :
		static bool update_max_empire_fleet_id(CFleet *aFleet);
};

class CEmpireCapitalPlanet : public CStore
{
	public :
		enum EStoreFlag
		{
			STORE_OWNER_ID
		};

	public :
		CEmpireCapitalPlanet();
		virtual ~CEmpireCapitalPlanet();

		virtual const char *table() { return "empire_capital_planet"; }
		virtual CString &query();

		int get_owner_id() { return mOwnerID; }
		bool set_owner_id(int aOwnerID);

		char *get_name() { return (char *)mName; }

		CAdmiralList *get_admiral_list() { return &mAdmiralList; }
		CFleetList *get_fleet_list() { return &mFleetList; }

		bool add_admiral(CAdmiral *aAdmiral);
		bool remove_admiral(int aAdmiralID);

		bool add_fleet(CFleet *aFleet);
		bool remove_fleet(int aFleetID);

		bool is_dead();

		bool regen_empire_fleets();

		bool update_DB();

		bool initialize();

	private :
		int
			mOwnerID;
		CString
			mName;
		CAdmiralList
			mAdmiralList;
		CFleetList
			mFleetList;
};

extern TZone gEmpireAdmiralInfo;

class CEmpireAdmiralInfo : public CStore
{
	public:
		enum EStoreFlag
		{
			STORE_ADMIRAL_ID = 0,
			STORE_ADMIRAL_TYPE,
			STORE_POSITION_ARG1,
			STORE_POSITION_ARG2,
			STORE_POSITION_ARG3
		};

	public:
		CEmpireAdmiralInfo();
		CEmpireAdmiralInfo(MYSQL_ROW aRow);
		virtual ~CEmpireAdmiralInfo();

		virtual const char *table() { return "empire_admiral_info"; }
		virtual CString& query();

	public:
		int get_admiral_id() { return mAdmiralID; }
		int get_admiral_type() { return mAdmiralType; }
		int get_position_arg1() { return mPositionArg1; }
		int get_position_arg2() { return mPositionArg2; }
		int get_position_arg3() { return mPositionArg3; }

		bool set_admiral_id(int aAdmiralID);
		bool set_admiral_type(int aAdmiralType);
		bool set_position_arg1(int aPositionArg1);
		bool set_position_arg2(int aPositionArg2);
		bool set_position_arg3(int aPositionArg3);

	private:
		int
			mAdmiralID,
			mAdmiralType,
			mPositionArg1,
			mPositionArg2,
			mPositionArg3;
};

class CEmpireAdmiralInfoList: public CSortedList
{
	public:
		CEmpireAdmiralInfoList();
		virtual ~CEmpireAdmiralInfoList();

		bool add_empire_admiral_info(CEmpireAdmiralInfo *aEmpireAdmiralInfo);
		bool remove_empire_admiral_info(int aAdmiralID);

		CEmpireAdmiralInfo *get_by_admiral_id(int aAdmiralID);

		bool initialize(CMySQL *aMySQL);
		bool remove_all_from_db();

		bool update_DB();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

extern TZone gEmpireFleetInfo;

class CEmpireFleetInfo : public CStore
{
	public:
		enum EStoreFlag
		{
			STORE_FLEET_ID = 0,
			STORE_FLEET_TYPE,
			STORE_POSITION_ARG1,
			STORE_POSITION_ARG2,
			STORE_POSITION_ARG3
		};

	public:
		CEmpireFleetInfo();
		CEmpireFleetInfo(MYSQL_ROW aRow);
		virtual ~CEmpireFleetInfo();

		virtual const char *table() { return "empire_fleet_info"; }
		virtual CString& query();

	public:
		int get_fleet_id() { return mFleetID; }
		int get_fleet_type() { return mFleetType; }
		int get_position_arg1() { return mPositionArg1; }
		int get_position_arg2() { return mPositionArg2; }
		int get_position_arg3() { return mPositionArg3; }

		bool set_fleet_id(int aFleetID);
		bool set_fleet_type(int aFleetType);
		bool set_position_arg1(int aPositionArg1);
		bool set_position_arg2(int aPositionArg2);
		bool set_position_arg3(int aPositionArg3);

	private:
		int
			mFleetID,
			mFleetType,
			mPositionArg1,
			mPositionArg2,
			mPositionArg3;
};

class CEmpireFleetInfoList: public CSortedList
{
	public:
		CEmpireFleetInfoList();
		virtual ~CEmpireFleetInfoList();

		bool add_empire_fleet_info(CEmpireFleetInfo *aEmpireFleetInfo);
		bool remove_empire_fleet_info(int aFleetID);

		CEmpireFleetInfo *get_by_fleet_id(int aFleetID);

		bool initialize(CMySQL *aMySQL);
		bool remove_all_from_db();

		bool update_DB();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

extern TZone gEmpirePlanetInfo;

class CEmpirePlanetInfo : public CStore
{
	public:
		enum EStoreFlag
		{
			STORE_PLANET_ID = 0,
			STORE_OWNER_ID,
			STORE_PLANET_TYPE,
			STORE_POSITION_ARG
		};

	public:
		CEmpirePlanetInfo();
		CEmpirePlanetInfo(MYSQL_ROW aRow);
		virtual ~CEmpirePlanetInfo();

		virtual const char *table() { return "empire_planet_info"; }
		virtual CString& query();

	public:
		int get_planet_id() { return mPlanetID; }
		int get_owner_id() { return mOwnerID; }
		int get_planet_type() { return mPlanetType; }
		int get_position_arg() { return mPositionArg; }

		bool set_planet_id(int aPlanetID);
		bool set_owner_id(int aOwnerID);
		bool set_planet_type(int aPlanetType);
		bool set_position_arg(int aPositionArg);

	private:
		int
			mPlanetID,
			mOwnerID;
		int
			mPlanetType,
			mPositionArg;
};

class CEmpirePlanetInfoList: public CSortedList
{
	public:
		CEmpirePlanetInfoList();
		virtual ~CEmpirePlanetInfoList();

		bool add_empire_planet_info(CEmpirePlanetInfo *aEmpirePlanetInfo);
		bool remove_empire_planet_info(int aAdmiralID);

		CEmpirePlanetInfo *get_by_planet_id(int aPlanetID);

		bool initialize(CMySQL *aMySQL);
		bool remove_all_from_db();

		bool update_DB();

		int get_number_of_planets_by_owner(CPlayer *aPlayer, int aPlanetType);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

extern TZone gEmpireActionZone;

class CEmpireAction : public CStore
{
	private :
		int
			mID,
			mOwner,
			mAction,
			mTarget,
			mAnswer,
			mAmount;
		time_t
			mTime;
	public :
		enum {
			EA_ACTION_BRIBE,
			EA_ACTION_REQUEST_BOON,
			EA_ACTION_REQUEST_MEDIATION,
			EA_ACTION_INVADE_MAGISTRATE_DOMINION,
			EA_ACTION_INVADE_EMPIRE,
			ED_DEMAND_TRIBUTE,
			ED_DEMAND_LEAVE_COUNCIL,
			ED_DEMAND_DECLARE_WAR,
			ED_DEMAND_MAKE_TRUCE,
			ED_DEMAND_FREE_SUBORDINARY_COUNCIL,
			ED_DEMAND_ARMAMENT_REDUCTION,
			ED_GRANT_RANK,
			ED_GRANT_BOON
		};

		enum {
			EA_ANSWER_WAITING,
			EA_ANSWER_YES,
			EA_ANSWER_NO,
			EA_ANSWER_COMPLETE
		};

		enum {
			EA_REWARD_DENIED,
			EA_REWARD_LOW_PRODUCTION,
			EA_REWARD_LEVEL2_TECH,
			EA_REWARD_LEVEL13_COMMANDER,
			EA_REWARD_CLASS1_FLEET,
			EA_REWARD_MEDIUM_PRODUCTION, 
			EA_REWARD_LEVEL34_TECH,
			EA_REWARD_LEVEL46_COMMANDER,
			EA_REWARD_CLASS2_FLEET,
			EA_REWARD_HIGH_PRODUCTION, 
			EA_REWARD_CLASS3_FLEET,
			EA_REWARD_LEVEL56_TECH,
			EA_REWARD_LEVEL710_COMMANDER,
			EA_REWARD_CLASS4_FLEET,
			EA_REWARD_LEVEL78_TECH,
			EA_REWARD_LEVEL1115_COMMANDER,
			EA_REWARD_CLASS5_FLEET,
			EA_REWARD_PLANET
		};

		enum {
			STORE_ID,
			STORE_OWNER,
			STORE_ACTION,
			STORE_TARGET,
			STORE_AMOUNT,
			STORE_ANSWER,
			STORE_TIME
		};

		CEmpireAction();
		CEmpireAction(MYSQL_ROW aRow);
		virtual ~CEmpireAction();

		virtual const char *table() { return "empire_action"; }
		virtual CString &query();

		int get_id() { return mID; }
		bool set_id(int aID);

		int get_owner() { return mOwner; }
		bool set_owner(int aOwner);

		int get_action() { return mAction; }
		bool set_action(int aAction);
		const char *get_action_name();

		int get_target() { return mTarget; }
		bool set_target(int aTarget);

		int get_amount() { return mAmount; }
		bool set_amount(int aAmount);
		const char *get_target_amount_str(CPlayer *aPlayer);

		int get_answer() { return mAnswer; }
		bool set_answer(int aAnswer);
		const char *get_answer_name();

		time_t get_time() { return mTime; }
		bool set_time();
		const char *get_time_str();

		void save_new_action();

		const char *accept_demand( CPlayer *aPlayer );
		const char *reject_demand( CPlayer *aPlayer );

		RECYCLE(gEmpireActionZone);
};

class CEmpireActionList : public CSortedList
{
	public:
		CEmpireActionList();
		virtual ~CEmpireActionList();

		CEmpireAction *get_by_id( int aID );
		bool add_empire_action(CEmpireAction *aEmpireAction);
		bool remove_empire_action(CEmpireAction *aEmpireAction);

		void flush_db();

		int get_new_id();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;

		virtual const char *debug_info() const { return "empire action list"; }
};

#endif
