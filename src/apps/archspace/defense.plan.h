#if !defined(__ARCHSPACE_DEFENSE_H__)
#define __ARCHSPACE_DEFENSE_H__

#include "store.h"

class CFleetList;

extern TZone gDefenseFleetZone;

class CDefenseFleet: public CStore
{
	public:
		CDefenseFleet() {}
		CDefenseFleet( MYSQL_ROW aRow );
		virtual ~CDefenseFleet() {}
	/* from CStore */
		virtual const char *table() { return "defense_fleet"; }
		virtual CString& query();
		friend CStoreCenter& operator<<(CStoreCenter& aStoreCenter, CDefenseFleet& aFleet);
		enum
		{
			FIELD_OWNER = 0,
			FIELD_PLAN_ID,
			FIELD_FLEET_ID,
			FIELD_FLEET_COMMAND,
			FIELD_FLEET_X,
			FIELD_FLEET_Y
		};
		enum
		{
			COMMAND_NORMAL = 0,
			COMMAND_FORMATION,
			COMMAND_PENETRATE,
			COMMAND_FLANK,
			COMMAND_RESERVE,
			COMMAND_FREE,
			COMMAND_ASSAULT,
			COMMAND_STAND_GROUND,
			COMMAND_MAX
		};
	private:
		int
			mOwner,
			mPlanID,
			mCommand,
			mX,
			mY;
		int
			mFleetID;

		enum EStoreFlag
		{
			STORE_OWNER,
			STORE_PLAN_ID,
			STORE_FLEET_ID,
			STORE_COMMAND,
			STORE_X,
			STORE_Y
		};

	public:
		int get_owner() { return mOwner; }
		int get_plan_id() { return mPlanID; }
		int get_fleet_id() { return mFleetID; }
		int get_command() { return mCommand; }
		static char *get_command_string_upper_case(int aCommand);
		static char *get_command_string_normal(int aCommand);
		int get_x() { return mX; }
		int get_y() { return mY; }
		const char* get_option_string();
	
		void set_owner( int aID ) { mStoreFlag += STORE_OWNER; mOwner = aID; }
		void set_plan_id( int aID ) { mStoreFlag += STORE_PLAN_ID; mPlanID = aID; }
		void set_fleet_id(int aID)
			{ mStoreFlag += STORE_FLEET_ID; mFleetID = aID; }
		void set_command(int aCommand)
			{ mStoreFlag += STORE_COMMAND; mCommand = aCommand; }
		void set_x(int x) { mStoreFlag += STORE_X; mX = x; }
		void set_y(int y) { mStoreFlag += STORE_Y; mY = y; }

		bool convert_coordinates();

		static int get_fleet_command_from_string(char *aFleetCommandString);

	RECYCLE(gDefenseFleetZone);
};

/**
*/
class CDefenseFleetList: public CSortedList
{
	public:
		CDefenseFleetList() {};
		virtual ~CDefenseFleetList() {};

		int add_defense_fleet(CDefenseFleet *aFleet);
		bool remove_defense_fleet(int aFleetID);
		bool remove_without_free_defense_fleet(int aFleetID);
		CDefenseFleet *get_by_id(int aFleetID);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "defense fleet list"; }
};

extern TZone gDefensePlanZone;
/**
*/

class CDefensePlan: public CStore
{
	public:
		CDefensePlan();
		CDefensePlan( MYSQL_ROW aRow);
		virtual ~CDefensePlan() {}
	/* from CStore */
		virtual const char *table() { return "plan"; }
		virtual CString& query();
		friend CStoreCenter& operator<<(CStoreCenter& aStoreCenter, CDefensePlan& aPlan);
	private:
		int
			mOwner,
			mID,
			mType,
			mEnemy,
			mMin,
			mMax,
			mAttackType;
		int
			mCapital;

		CDefenseFleetList
			mFleetList;
		CString
			mName;
		enum
		{
			FIELD_OWNER,
			FIELD_ID,
			FIELD_TYPE,
			FIELD_NAME,
			FIELD_CAPITAL,
			FIELD_ENEMY,
			FIELD_MIN,
			FIELD_MAX,
			FIELD_ATTACK_TYPE
		};
		enum EStoreFlag
		{
			STORE_OWNER,
			STORE_ID,
			STORE_TYPE,
			STORE_NAME,
			STORE_CAPITAL,
			STORE_ENEMY,
			STORE_MIN,
			STORE_MAX,
			STORE_ATTACK_TYPE
		};

	public:
		enum EPlanType
		{
			GENERIC_PLAN = 0,
			SPECIAL_PLAN
		};
		enum
		{
			CAPITAL_FLEET = 0
		};
		enum EEnemyRace
		{
			RACE_HUMAN = 1,
			RACE_TARGOID = 2,
			RACE_BUCKANEER = 4,
			RACE_TECANOID = 8,
			RACE_EVINTOS = 16,
			RACE_AGERUS = 32,
			RACE_BOSALIAN = 64,
			RACE_XELOSS = 128,
			RACE_XERUSIAN = 256,
			RACE_XESPERADOS= 512,
			RACE_ALL = RACE_HUMAN | RACE_TARGOID | RACE_BUCKANEER | RACE_TECANOID | RACE_EVINTOS | RACE_AGERUS | RACE_BOSALIAN | RACE_XELOSS | RACE_XERUSIAN | RACE_XESPERADOS
		};

		enum EAttackType
		{
			TYPE_SIEGE = 1,
			TYPE_BLOCKADE = 2,
			TYPE_ALL = TYPE_SIEGE | TYPE_BLOCKADE
		};

		int get_owner() { return mOwner; }
		int get_id() { return mID; }
		int get_type() { return mType; }
		const char* get_name() { return (char*)mName; }
		CDefenseFleetList *get_fleet_list() { return &mFleetList; }
		int get_min() { return mMin; }
		int get_max() { return mMax; }
		int get_capital() { return mCapital; }
		int get_enemy() { return mEnemy; }
		int get_attack_type() { return mAttackType; }

		void set_owner( int aOwner ) { mStoreFlag += STORE_OWNER; mOwner = aOwner; }
		void set_id( int aID ) { mStoreFlag += STORE_ID; mID = aID; }
		void set_type( int aType ) { mStoreFlag += STORE_TYPE; mType = aType; }
		void set_name( char *aName ) { mStoreFlag += STORE_NAME; mName = aName; }
		bool set_fleet( int index, CDefenseFleet* aFleet );
		void set_enemy( int aEnemy ) { mStoreFlag += STORE_ENEMY; mEnemy = aEnemy; }
		void set_min( int aMin ) { mStoreFlag += STORE_MIN; mMin = aMin; }
		void set_max( int aMax ) { mStoreFlag += STORE_MAX; mMax = aMax; }
		void set_capital(int aCapital)
			{ mStoreFlag += STORE_CAPITAL; mCapital = aCapital; }
		void set_attack_type(int aAttackType)
			{ mStoreFlag += STORE_ATTACK_TYPE; mAttackType = aAttackType; }

		bool is_plan_for( int aEnemy );
		bool add_defense_fleet(CDefenseFleet *aFleet);
		bool remove_defense_fleet(int aIndex);

		int get_fleets_number();

		char *get_for_which_race();
		char *get_for_which_attack();

		bool convert_coordinates();

		bool is_there_stacked_fleet();

	/* For Empire */
	public :
		bool init_for_empire(CFleetList *aFleetList, bool aOffenseSide);

	RECYCLE(gDefensePlanZone);
};

/**
*/
class CDefensePlanList: public CSortedList
{
	public:
		CDefensePlanList() {};
		virtual ~CDefensePlanList() {};
		int add_defense_plan(CDefensePlan* aPlan);
		bool remove_defense_plan(int aPlanID);
		CDefensePlan *get_by_id(int aPlanID);
		CDefensePlan* get_generic_plan();
		int get_new_id();

		CDefensePlan *get_optimal_plan(int aRace, int aEnemyPower, int aMyPower,
										CDefensePlan::EAttackType aAttackType);
		char *special_defense_plan_info_html();

	protected:
		virtual bool free_item( TSomething aItem );
		virtual int compare( TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key( TSomething aItem, 
								TConstSomething aKey) const;
		virtual const char *debug_info() const { return "defense plan list"; }
};

#endif

/* end of file defense.plan.h */
