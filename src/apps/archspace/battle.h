#if !defined(__ARCHSPACE_BATTLE_H__)
#define __ARCHSPACE_BATTLE_H__

#include "component.h"
#include "define.h"
#include "store.h"
#include "defense.plan.h"

#define NUMBER_OF_ORDER	11

class CBattleFleet;
class CBattle;
class CBattleRecord;
class CBattleFleetList;
class CFleet;
class CAdmiral;
class CPlayer;
class CAdmiralList;
class CPlanet;
class CFleetList;

/**
*/
// Defines a vector, x-length mX and y-length mY
// pointing in direction 
class CVector {
  public:
	CVector();
	CVector(int aX, int aY);
	~CVector();
  private:
	int mX,
	    mY;
	
	double mDirection;
  public:
	void set_vector(int aX, int aY, double aDirection);
	
	int get_x() { return mX; }
	int get_y() { return mY; }
	int get_direction() { return (int)mDirection; }
	
	void turn(double aDegree);
	void turn_to(CVector *aVector, double aMaxTurn = 360.0);
	void move(int aDeltaX, int aDeltaY);
	void move(int aLength);
	int distance(CVector *aVector);
	int delta_direction(CVector *aVector);

	void rotate(int aDirection);
	void rotate_about(int aDirection, CVector *aFixedPoint);

	bool is_in_range(CVector *aVector, int aDistance, int aDirection);
	bool is_heading_to(CVector *aVector);

	bool touch_border();
};

extern TZone gTargetFleetZone;

/**
*/

class CTargetFleet {
  public:
	CTargetFleet();
	CTargetFleet(CBattleFleet *aMe, CBattleFleet *aEnemy);
	~CTargetFleet();
  private:
	CBattleFleet *mTarget;
	int mDistance;
	int mDangerRating;
  public:
	CBattleFleet *get_fleet() { return mTarget; }
	int get_danger_rating() { return mDangerRating; }

	RECYCLE(gTargetFleetZone);
};

/**
*/
class CTargetFleetList: public CList {
  public:
	CTargetFleetList();
	virtual ~CTargetFleetList();
  protected:
	virtual bool free_item(TSomething aItem);
	virtual const char *debug_info() const { return "target fleet list"; }
};

/**
*/
class CTurret: public CWeapon {
  public:
	CTurret();
	~CTurret();

	void init( CWeapon *aWeapon, int aNumber );
	bool is_empty() { return !mInited; }
	bool ready() { if( mCooling == 0 ) return true; return false; }

	void set_cooling( int aCooling ) { mCooling = aCooling; }

	void cooling() { if( mCooling > 0 ) mCooling--; }
	void recharge( CBattleFleet *aFleet );

	int get_number() { return mNumber; }

  private:
    int  mNumber,
         mCooling;
    bool mInited;
};

const int MaxBattleFleet = 100;


extern TZone gBattleFleetZone;
/**
*/

class CBattleFleet : public CVector
{
public:
	CBattleFleet();
	~CBattleFleet();
	static const int JUMP_OUT_TICKS = 30;
	static const int REFIRE_ASSAULT_MOD = 4;
	enum {
		COMMAND_NORMAL,
		COMMAND_FORMATION,
		COMMAND_PENETRATE,
		COMMAND_FLANK,
		COMMAND_RESERVE,
		COMMAND_FREE,
		COMMAND_ASSAULT,
		COMMAND_STAND_GROUND,
		STATUS_BERSERK,
		STATUS_DISORDER,
		STATUS_ROUT,
		STATUS_RETREAT,
		STATUS_PANIC,
		STATUS_ANNIHILATED,
		STATUS_ANNIHILATED_THIS_TURN,
		STATUS_RETREATED_THIS_TURN,
		STATUS_RETREATED
	};
	enum {
		SUBSTATUS_NONE,
		SUBSTATUS_TURNING_TO_CENTER,
		SUBSTATUS_PENETRATION,
		SUBSTATUS_CHARGE,
		SUBSTATUS_MOVE_STRAIGHT,
		SUBSTATUS_TURN_TO_FORWARD,
		SUBSTATUS_TURN_TO_BACKWARD,
		SUBSTATUS_TURN_TO_NEAREST_BORDER,
		SUBSTATUS_JUMP_OUT
	};
	enum {
		MORALE_NORMAL,
		MORALE_WEAK_BREAK,
		MORALE_NORMAL_BREAK,
		MORALE_COMPLETE_BREAK
	};

	enum {
		WEAK_CLOAKING = 1,
		COMPLETE_CLOAKING,
		WEAK_CLOAKING_DETECTION,
		COMPLETE_CLOAKING_DETECTION,
		PSI_RACE,
		ENHANCED_PSI,
		ENHANCED_MOBILITY
	};

private:
	int
		mID;
	CString
		mNick;
	int
		mOwner;
	CFleet *
	mFleet;
	int
		mShieldStrength[MaxBattleFleet],
		mShieldMaxStrength,
		mShieldSolidity,
		mShieldRechargeRate,
		mHP[MaxBattleFleet],
		mMaxHP,
		mSize,
		mStatus,
		mTurretRefireTimer,
		mPreCharge,
		mExploit;
	int
		mTarget;
	int
		mPenetration,
		mRedZoneRadius,
		mEngaged,
		mSubStatus,
		mMaxShip,
		mExp,
		mOriginalPower,
		mTotalHP;
	int
		mMoraleModifier,
		mBerserkModifier,
		mMoraleStatus,
		mStatusLastingTurn,
		mJumpWaitTurns,
		mRemainingStealthTurns;
	double
		mRatio;
	float
		mMorale;
	int
		mAdmiralExp,
		mKilledShip,
		mKilledFleet;
	CCommandSet
		mAttribute;

	CAdmiral
		*mCommander;

	CArmor
		*mArmor;
	CEngine
		*mEngine;
	CComputer
		*mComputer;
	CShield
		*mShield;
	CTurret
		mTurret[WEAPON_MAX_NUMBER];
	CDevice
		*mDevice[DEVICE_MAX_NUMBER];

	CBattleFleet
		*mEnemy;
	CBattle
		*mBattle;
	bool
		mTouchedBorder,
		mIsDetected,
		mIsEncountered,
		mIsCapital;

	CTargetFleetList
		mEncounterList,
		mFireRangeList;

	CFleetEffectListDynamic
		mStaticEffectList,
		mDynamicEffectList;

	static char *mOrderName[];
public:
	int get_id() { return mID; }
	void set_id(int aID) { mID = aID; }
	double get_bat_ratio();
	int get_real_id();
	int get_owner() { return mOwner; }
	int get_real_owner();
	void tick_refire_timer() {if(mTurretRefireTimer > 0) mTurretRefireTimer--;}
	int get_turret_refire(){return mTurretRefireTimer;}
	const char *get_nick() { return (char*)mNick; }
	const char *get_class_name();
	const char *get_size_name();
	int get_body() { return mSize+4000; }
	int get_size() { return mSize; }

	int get_command() { return mStatus; }
	void set_command(int aStatus);
	int get_status() { return mStatus; }
	void set_status(int aStatus, int aLastingTurn = 1800);
	inline void set_status(int aStatus, float aLastingTurn) { set_status(aStatus,(int)aLastingTurn); }
	bool recover_status();
	void remove_abnormal_status() { set_status(COMMAND_FREE); }

	const char *get_order_name() { return mOrderName[mStatus]; }

	int get_exploit() { return mExploit; }
	void reset_exploit() { mExploit = 1; }
	void add_exploit( int aNewExploit ) { mExploit += aNewExploit; }

	int get_power();
	int get_original_power() { return mOriginalPower; }
	int get_penetration() { return mPenetration; }
	void set_penetration( int aPenetration) { mPenetration = aPenetration; }

	int get_admiral_exp() { return mAdmiralExp; }
	int get_killed_ship() { return mKilledShip; }
	int get_killed_fleet() { return mKilledFleet; }

	CAdmiral *get_admiral() { return mCommander; }
	CFleet *get_fleet() { return mFleet; }

	CTurret *get_turret( int aIndex ) { return mTurret+aIndex; }
	int calc_danger_rating( CBattleFleet *aEnemy );

	int get_target() { return mTarget; }
	void set_target(int aTarget) { mTarget = aTarget; }

	int get_active_ratio();
	int count_active_ship();
	int get_max_ship() { return mMaxShip; }

	bool init(CBattle *aBattle, CPlayer *aOwner, CFleet *aFleet, CAdmiralList *aAdmiralList, int aID);
	void init_common();

	void set_capital_fleet() { mIsCapital = true; }
	bool is_capital_fleet() { return mIsCapital; }

	/*void set_detected() { mIsDetected = true; }
	void reset_detected() { mIsDetected = false; }  // Not used anyomre..
	bool is_detected() { return mIsDetected; }*/

	void set_encountered() { mIsEncountered = true; }
	void reset_encountered() { mIsEncountered = false; }
	bool is_encountered() { return mIsEncountered; }

	void set_touched_border() { mTouchedBorder = true; }
	void reset_touched_border() { mTouchedBorder = false; }
	bool touched_border() { return mTouchedBorder; }

	void reset_substatus() { mSubStatus = SUBSTATUS_NONE; }
	void set_substatus( int aStatus ) { mSubStatus = aStatus; }
	int get_substatus() { return mSubStatus; }

	void set_engaged() { mEngaged = 50; }
	void loose_engaged() { if( mEngaged > 0 ) mEngaged--; }
	void reset_engaged() { mEngaged = 0; mRemainingStealthTurns = 0;}
	bool is_engaged() { return (bool)mEngaged; }
	bool is_precharging() { if (mEncounterList.length() == 0 && !is_engaged()){mPreCharge++; return true;} return false;}
	bool is_disabled();

	int get_efficiency();
	int get_speed();
	double get_mobility();
	int get_detection_range();
	int get_red_zone_radius() { return mRedZoneRadius; }
	int get_total_hp() { return mTotalHP; }
	int get_exp() { return mExp; }
	float get_morale() { return mMorale; }
	float change_morale( float aMorale );
	void set_morale_status( int aStatus ) { mMoraleStatus = aStatus; }
	int get_morale_status() { return mMoraleStatus; }
	int get_morale_modifier() { return mMoraleModifier; }
	int get_berserk_modifier() { return mBerserkModifier; }
	int get_jump_timer() {return mJumpWaitTurns;}
	bool tick_jump_timer();

	void turn_to( CVector *aVector ) { CVector::turn_to( aVector, get_mobility() ); }
	void move() { CVector::move( get_speed() ); }
	void trace( CVector *aVector );

	int calc_effective_reach_time( CVector *aVector );

	void attack(CBattleRecord *aRecord, CBattleFleet *aEnemy);
	void fire( CBattleRecord *aRecord, CTurret *aTurret, CBattleFleet *aEnemy );
	int calc_AR( CTurret *aTurret );
	int calc_DR( CTurret *aTurret );
	bool damage_first_active( int aDam, bool PSI = false );
	bool damage_random_active( int aDam, bool PSI = false );

	void damage( CTurret *aTurret, CBattleFleet *aEnemy, int aHitChance, int aCritChance, int *aTotalDam, int *aHitCount, int *aMissCount, int *aSunkenCount );

	char *get_order_name(int aOrder);

	bool on_path( CVector *aPoint );
	bool path_meet_border();
	bool path_meet_vertical_border();
	void get_path( int *aLeftX, int *aRightX, int *aTopY, int *aBottomY );

	bool on_fire_range( CBattleFleet *aEnemy );
	bool on_complete_fire_range( CBattleFleet *aEnemy );

	void clear_encounter_list() { mEncounterList.remove_all(); }
	void add_to_encounter_list( CBattleFleet *aEnemy );

	CBattleFleet *get_enemy() { return mEnemy; }
	void set_enemy( CBattleFleet *aEnemy ) { mEnemy = aEnemy; }
	void reset_enemy() { mEnemy = NULL; }

	CBattleFleet *find_target();
	CBattleFleet *get_highest_DAR_encounter_fleet();
	CBattleFleet *get_least_ERT_engaged_fleet(CBattleFleetList *aEnemyList, int aRange = 10000);
	CBattleFleet *get_least_ERT_fleet(CBattleFleetList *aEnemyList);

	void weapon_cooling();
	void repair( int aPercent );
	void reorganize_after_battle( CPlayer *aOwner );
	void drop_ship_under_25();


	void raid_attack(CBattleFleet *aEnemy);
	void raid_fire(CTurret *aTurret, CBattleFleet *aEnemy);

	void siege(CPlanet *aPlanet, CString &aReport);

	float calc_PA_float( float aValue, int aEffect );
	float calc_minus_PA_float( float aValue, int aEffect );

	int calc_PA( int aValue, int aEffect );
	int calc_minus_PA( int aValue, int aEffect );
	int calc_0base_PA( int aValue, int aEffect );
	bool has_effect( int aEffect );
	bool under_area_effect();
	int get_effect_amount(int aEffect);

	bool is_psi_race() { return mAttribute.has( PSI_RACE ); }
	bool has_enhanced_psi() { return mAttribute.has( ENHANCED_PSI ); }
	bool has_enhanced_mobility() { return mAttribute.has( ENHANCED_MOBILITY ); }
	bool is_cloaked() { return mAttribute.has( COMPLETE_CLOAKING ) || mAttribute.has( WEAK_CLOAKING ); }
	bool is_full_cloaked() { return mAttribute.has( COMPLETE_CLOAKING ); }
	bool is_weak_cloaked() { return mAttribute.has( WEAK_CLOAKING ); }
	//bool is_stealthed() { if (mEngaged < (50-mRemainingStealthTurns)) return false; else return true; }
	bool is_stealthed();
	bool can_see( CBattleFleet *aEnemy );
	void remove_cloaking() { mAttribute -= WEAK_CLOAKING; mAttribute -= COMPLETE_CLOAKING; }
	void charge_effect();

	int get_shield_solidity() { return mShieldSolidity; }
	void recharge_shield();
	void repair_in_battle();

	CFleetEffect *get_area_effect();
	CFleetEffectListDynamic *get_static_effect_list() { return &mStaticEffectList; }
	void add_dynamic_effect( CFleetEffect *aEffect ){ mDynamicEffectList.push(aEffect); }
	void add_static_effect( CFleetEffect *aEffect ){ mStaticEffectList.push(aEffect); }
	void clear_dynamic_effect() { mDynamicEffectList.remove_all(); }

	int get_hp( int i ) { return mHP[i]; }
	void change_hp( int i, int aHP );
	int get_max_hp() { return mMaxHP; }

	int get_shield_strength( int i ) { return mShieldStrength[i]; }
	void change_shield_strength( int i, int aShield );

	CArmor *get_armor() { return mArmor; }
	CShield *get_shield() { return mShield; }
	CComputer *get_computer() { return mComputer; }
	CEngine *get_engine() { return mEngine; }
	CDevice *get_device(int i) { return mDevice[i]; }


	RECYCLE(gBattleFleetZone);
};


extern TZone gFormationPointZone;

/**
*/
class CFormationPoint : public CVector
{
public :
	CFormationPoint();
	CFormationPoint( CBattleFleet *aFleet );
	virtual ~CFormationPoint();
private :
	int mFleetID;
public :
	int get_id() { return mFleetID; }
	void set_id( int aNewID ) { mFleetID = aNewID; }

	RECYCLE(gFormationPointZone);
};

/**
*/
class CFormationInfo : public CSortedList
{
public :
	CFormationInfo();
	CFormationInfo( int aSide );
	virtual ~CFormationInfo();

protected:
	virtual bool free_item(TSomething aItem);
	virtual int compare( TSomething aItem1, TSomething aItem2) const;
	virtual int compare_key( TSomething aItem, TConstSomething aKey) const;
	virtual const char *debug_info() const { return "formation info"; }
private :
	int
		mSide;
public :
	int add_formation_point(CFormationPoint *aPoint);

	void set_side( int aSide ) { mSide = aSide; }
	int get_side() { return mSide; }
};

/**
*/
class CBattleFleetList:public CSortedList
{
public:
	CBattleFleetList();
	virtual ~CBattleFleetList();
	void init( CPlayer *aOwner, int aSide );

	int add_battle_fleet(CBattleFleet *Fleet);
	bool remove_battle_fleet(int aFleetID);

	CBattleFleet *get_by_id(int aBattleFleetID);

	int get_new_id();

	void auto_deployment(CBattle *aBattle, CFleetList *aFleetList, CAdmiralList *aAdmiralList);
	void add_formation_info( CBattleFleet *aBattleFleet );

	bool lost();

		// for test
	int get_detected_fleet();

	enum {
		FORMATION_NONE,
		FORMATION_DISBAND,
		FORMATION_ENCOUNTER,
		FORMATION_REFORMATION
	};

protected:
	virtual bool free_item(TSomething aItem);
	virtual int compare( TSomething aItem1, TSomething aItem2 ) const;
	virtual int compare_key( TSomething aItem, TConstSomething aKey ) const;
	virtual const char *debug_info() const { return "battle fleet list"; }
private:
	CPlayer
		*mOwner;
	int
		mSide;
	int
		mFormationStatus,
		mFormationSpeed;
	CFormationInfo
		mFormationInfo;
	int
		mTotalPower;
	CBattleFleet
		*mCapitalFleet;
public:
	void set_capital_fleet( CBattleFleet *aFleet ){ mCapitalFleet = aFleet; }
	CBattleFleet *get_capital_fleet() { return mCapitalFleet; }
	CAdmiral *get_capital_commander() { return mCapitalFleet->get_admiral(); }

	int get_formation_speed() { return mFormationSpeed; }
	void set_formation_speed();
	int get_formation_status() { return mFormationStatus; }
	void set_formation_status( int aStatus ) { mFormationStatus = aStatus; }
	int get_side() { return mSide; }

	CPlayer *get_owner() { return mOwner; }

	void update_fleet_after_battle(CPlayer *aEnemy, int aWarType, bool aWin);
	void update_empire_fleet_after_battle(CPlayer *aEnemy, int aWarType, bool aWin);

	CFormationPoint *get_formation_point( int aIndex ) { return (CFormationPoint*)mFormationInfo.get(aIndex); }
	int formation_length() { return mFormationInfo.length(); }
	int get_total_power() { return mTotalPower; }
	void calc_total_power();
	int get_current_power();
	int count_total_ships();
	int count_total_ship_class();
	void set_newMorale(int newMorale);
	void update_morale(float aMoraleUp, float aCapitalMorale, float aFleetMorale);
	int get_capital_fleet_id();
	CBattleFleet *search_capital_fleet();

	bool deploy_extra_fleet(CBattle *aBattle, CPlanet *aPlanet = NULL);
};

extern TZone gBattleRecordZone;
/**
*/
class CBattleRecord : public CStore
{
public :
	enum EStoreFlag
	{
		STORE_ID = 0,
		STORE_ATTACKER_ID,
		STORE_DEFENDER_ID,
		STORE_ATTACKER_NAME,
		STORE_DEFENDER_NAME,
		STORE_ATTACKER_RACE,
		STORE_DEFENDER_RACE,
		STORE_ATTACKER_COUNCIL,
		STORE_DEFENDER_COUNCIL,
		STORE_TIME,
		STORE_WAR_TYPE,
		STORE_IS_DRAW,
		STORE_WINNER,
		STORE_PLANET_ID,
		STORE_BATTLE_FIELD_NAME,
		STORE_ATTACKER_GAIN,
		STORE_ATTACKER_LOSE_FLEET,
		STORE_ATTACKER_LOSE_ADMIRAL,
		STORE_DEFENDER_LOSE_FLEET,
		STORE_DEFENDER_LOSE_ADMIRAL,
		STORE_RECORD_FILE,
		STORE_THERE_WAS_BATTLE
	};

	CBattleRecord();
	CBattleRecord(CPlayer *aAttacker, CPlayer *aDefender, int aWarType, void *aBattleField);
	CBattleRecord(MYSQL_ROW aRow);
	virtual ~CBattleRecord();
	virtual const char *table() { return "battle_record"; }
	virtual CString &query();

private :
	int
		mID,
		mAttackerID,
		mDefenderID;
	CString
		mAttackerName,
		mDefenderName;
	int
		mAttackerRace,
		mDefenderRace;
	int
		mAttackerCouncil,
		mDefenderCouncil;
	time_t
		mTime;
	int
		mWarType;
	bool
		mIsDraw;
	int
		mWinner;
	int
		mPlanetID;
	CString
		mBattleFieldName;

private :
	CString
		mAttackerGain,
		mAttackerLoseFleet,
		mAttackerLoseAdmiral,
		mDefenderLoseFleet,
		mDefenderLoseAdmiral;
	CString
		mRecordFile;
	int
		mThereWasBattle;
	int
		mFireID,
		mTurn;	// temporary value
	CString
		mBuf;

public :
	int get_id() { return mID; }
	void set_id(int aID);

	int get_turn() { return mTurn; }
	void set_turn( int aTurn ) { mTurn = aTurn; }

	bool there_was_battle() { return (bool)mThereWasBattle; }
	void set_there_was_battle() { mThereWasBattle = 1; }

	int get_attacker_id() { return mAttackerID; }
	int get_defender_id() { return mDefenderID; }
	const char *get_attacker_name() { return (char*)mAttackerName; }
	const char *get_defender_name() { return (char*)mDefenderName; }
	char *get_record_file() { return (char *)mRecordFile; }
	int get_time() { return mTime; }

	int get_fire_id() { return mFireID; }
	int get_new_fire_id() { return ++mFireID; }
	void set_attacker_race( int aRace ) { mAttackerRace = aRace; }
	int get_attacker_race() { return mAttackerRace; }
	void set_defender_race( int aRace ) { mDefenderRace = aRace; }
	int get_defender_race() { return mDefenderRace; }
	void set_attacker_council( int aCouncil ) { mAttackerCouncil = aCouncil; }
	int get_attacker_council() { return mAttackerCouncil; }
	void set_defender_council( int aCouncil ) { mDefenderCouncil = aCouncil; }
	int get_defender_council() { return mDefenderCouncil; }

	void set_war_type(int aWarType) { mWarType = aWarType; }
	int get_war_type() { return mWarType; }
	char *get_war_type_string();

	bool is_draw() { return mIsDraw; }
	void set_draw();
	void reset_draw();

	int get_winner() { return mWinner; }
	void set_winner(int aID) { mWinner = aID; }

	void set_planet_id( int aID ) { mPlanetID = aID; }
	int get_planet_id() { return mPlanetID; }

	void set_battle_field_name(char *aName) { mBattleFieldName = aName; }
	char *get_battle_field_name() { return (char *)mBattleFieldName; }

	void set_attacker_gain( char *aStr ) { mAttackerGain = aStr; }
	const char *get_attacker_gain() { return (char *)mAttackerGain; }
	void set_attacker_lose_fleet( char *aStr ) { mAttackerLoseFleet = aStr; }
	const char *get_attacker_lose_fleet() { return (char *)mAttackerLoseFleet; }
	void set_attacker_lose_admiral( char *aStr ) { mAttackerLoseAdmiral = aStr; }
	const char *get_attacker_lose_admiral() { return (char *)mAttackerLoseAdmiral; }
	void set_defender_lose_fleet( char *aStr ) { mDefenderLoseFleet = aStr; }
	const char *get_defender_lose_fleet() { return (char *)mDefenderLoseFleet; }
	void set_defender_lose_admiral( char *aStr ) { mDefenderLoseAdmiral = aStr; }
	const char *get_defender_lose_admiral() { return (char *)mDefenderLoseAdmiral; }

public :
	void add_fleet( CBattleFleet *aFleet );
	void add_location( CBattleFleet *aFleet );
	void disable_fleet( CBattleFleet *aFleet );
	void add_fire( CBattleFleet *aAttacker, CBattleFleet *aTarget, CTurret *aTurret, int aHitChance );
	void add_hit( CBattleFleet *aAttacker, CBattleFleet *aTarget, int aHitCount, int aMissCount, int aTotalDamage, int aSunkCount );

	void add_buf( char *aStr );
	void save();
	RECYCLE(gBattleRecordZone);
};

extern TZone gBattleZone;
/**
*/
class CBattle {
  public:
	CBattle();
	CBattle(int aWarType, CPlayer *aAttacker, CPlayer *aDefender, void *aBattleField);
	~CBattle();
  private:
	int
		mTurn,
		mWarType;
	CPlayer
		*mAttacker,
		*mDefender;

	void *
		mBattleField;

	CBattleFleetList
		mOffenseFleetList,
		mDefenseFleetList;
	CBattleRecord *
	mRecord;
	CString
		mReport;

public:
	enum
	{
		WAR_SIEGE = 0,
		WAR_PRIVATEER,
		WAR_RAID,
		WAR_BLOCKADE,
		
		WAR_RATED_SIEGE,
		WAR_RATED_BLOCKADE,

		WAR_MAGISTRATE,
		WAR_MAGISTRATE_COUNTERATTACK,
		WAR_EMPIRE_PLANET,
		WAR_EMPIRE_PLANET_COUNTERATTACK,
		WAR_FORTRESS,
		WAR_EMPIRE_CAPITAL_PLANET
	};

	enum {
		SIDE_OFFENSE,
		SIDE_DEFENSE
	};

	static bool mBattleSimulationEnabled;

	CBattleRecord *get_record() { return mRecord; }

	void set_attacker( CPlayer *aPlayer );
	void set_defender( CPlayer *aPlayer );
	
	int get_attacker_id();
	double get_ratio();

	
	// ---Change--- //
	int get_turn() { return mTurn; }
	void set_turn(int aTurn) { mTurn = aTurn; }
	// ---End Change--- //

	int get_war_type() { return mWarType; }
	void set_war_type(int aWarType) { mWarType = aWarType; }

	void *get_battle_field() { return mBattleField; }
	void set_battle_field(CPlanet *aPlanet) { mBattleField = (void *)aPlanet; }
	char *get_battle_field_name();

	bool init_battle_fleet(CDefensePlan *aOffensePlan, CFleetList *aOffenseFleetList, CAdmiralList *aOffenseAdmiralList, CDefensePlan *aDefensePlan, CFleetList *aDefenseFleetList, CAdmiralList *aDefenseAdmiralList);

	bool add_offense_battle_fleet(CFleet *aFleet);

	bool deploy_by_plan(CPlayer *aPlanOwner, CDefensePlan *aPlan, CFleetList *aFleetList, CAdmiralList *aAdmiralList, CBattleFleetList *aBattleFleetList);
	bool run_step();

	void update_penetration();
	void prepare_turn(CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void one_side_run(CBattleFleetList *aActive, CBattleFleetList *aPassive);

	void fleet_run_normal(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_stand_ground(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_free(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_penetrate(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_assault(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_flank(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_formation(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_berserk(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_disorder(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_retreat(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_rout(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);
	void fleet_run_panic(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive);

	CBattleFleetList& get_offense_battle_fleet_list()
	{ return mOffenseFleetList; }
	CBattleFleetList& get_defense_battle_fleet_list()
	{ return mDefenseFleetList; }

	char *get_result_report();
	bool attacker_win();
	bool is_rated_siege_battle();
	bool is_rated_blockade_battle();
	int calc_battle_rating();
	double get_weighted_average_ship_class();
	int count_total_ships();

	const char *get_report() { return (char*)mReport; }

	bool init_raid(CFleet *aFleet);
	bool raid_fleet_detection_check();
	bool raid_fleet_war();
	bool raid_siege_war();
	bool raid_bomb();

	bool init_privateer(CFleet *aFleet);
	bool privateer_fleet_detection_check(bool aBool);

	bool siege_war();

	void update_fleet_after_battle();
	void update_empire_fleet_after_battle();
	void save();

	void finish_report_after_battle();

	RECYCLE(gBattleZone);
};

/**
*/
class CBattleRecordTable : public CSortedList
{
public:
	CBattleRecordTable();
	virtual ~CBattleRecordTable();

	bool remove_battle_record(int aBattleRecordID);
	int add_battle_record(CBattleRecord *aBattleRecord);
	CBattleRecord *get_by_id( int aBattleRecordID );
	bool load(CMySQL &aMySQL);
protected:
	virtual bool free_item(TSomething aItem);
	virtual int compare( TSomething aItem1, TSomething aItem2 ) const;
	virtual int compare_key( TSomething aItem, TConstSomething aKey ) const;
	virtual const char *debug_info() const { return "battle record table"; }
};

#endif
