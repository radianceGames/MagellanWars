#if !defined(__ARCHSPACE_COMPONENT_H__)
#define __ARCHSPACE_COMPONENT_H__

#include "prerequisite.h"
#include "define.h"

extern TZone gFleetEffectZone;

/**
*/
class CFleetEffect
{
public :
	CFleetEffect();
	CFleetEffect( int aType, int aAmount, int aApply );
	~CFleetEffect();

	enum {	// FE = Fleet Effect
	    // 0 - 9
		FE_AR,
		FE_WEAPON_AR,
		FE_COMPUTER,
		FE_DR,
		FE_ARMOR_DR,
		FE_MOBILITY,
		FE_SPEED,
		FE_HP,
		FE_DETECTION_RANGE,
		FE_MORALE,
		// 10 - 19
		FE_COMPLETE_CLOAKING,
		FE_WEAK_CLOAKING,
		FE_COMPLETE_CLOAKING_DETECTION,
		FE_WEAK_CLOAKING_DETECTION,
		FE_SHIELD_SOLIDITY,
		FE_SHIELD_STRENGTH,
		FE_IMPENETRABLE_SHIELD,
		FE_IMPENETRABLE_ARMOR,
		FE_SHIELD_RECHARGE_RATE,
		FE_BEAM_DAMAGE,
		// 20 - 29
		FE_BEAM_AR,		// 20
		FE_DR_AGAINST_BEAM,
		FE_BEAM_DEFENSE,
		FE_MISSILE_DAMAGE,
		FE_MISSILE_AR,
		FE_DR_AGAINST_MISSILE,
		FE_MISSILE_DEFENSE,
		FE_PROJECTILE_DAMAGE,
		FE_PROJECTILE_AR,
		FE_DR_AGAINST_PROJECTILE,
		// 30 - 39
		FE_PROJECTILE_DEFENSE,	// 30
		FE_PSI_ATTACK,
		FE_PSI_DEFENSE,
		FE_GENERIC_DEFENSE,
		FE_CHAIN_REACTION,
		FE_REPAIR,
		FE_REPAIR_SPEED,
		FE_NON_REPAIRABLE,
		FE_COOLING_TIME,
		FE_BEAM_COOLING_TIME,
		// 40 - 49
		FE_MISSILE_COOLING_TIME,	// 40
		FE_PROJECTILE_COOLING_TIME,
		FE_STEALTH,
		FE_MORALE_MODIFIER,
		FE_BERSERK_MODIFIER,
		FE_EFFICIENCY,
		FE_TRAINED,
		FE_DAMAGE,
		FE_PSI_DAMAGE,
		FE_PSI_STATIC_DAMAGE,
		// 50 - 59
		FE_CRITICAL_HIT,         //50
		FE_MISINTERPRET,
		FE_COMMANDER_SURVIVAL,
		FE_NEVER_BERSERK,
		FE_NEVER_RETREAT_ROUT,
		FE_PSI_DEFENSE_PANALTY,
		FE_PSI_NEUTRALIZATION_FIELD,
		FE_SPACE_MINING,
		FE_SYNGERY,
		FE_SHIELD_INTEGRITY,
		// 60 - 67
		FE_FAST_RETREAT,
		FE_FRONT_ARMOR,
		FE_SIDE_ARMOR,
		FE_REAR_ARMOR,
		FE_NUL_S_DISTORTION,
		FE_NUL_S_PIERCE,
		FE_NUL_A_PIERCE,
		FE_DANGER_RATING,
				// WE = Weapon Effect
		WE_SHIELD_PIERCING,
		WE_ARMOR_PIERCING,
		WE_ADDITIONAL_DAMAGE_TO_BIO_ARMOR,
		WE_SHIELD_DISTORTION,
		WE_SHIELD_OVERHEAT,
		WE_CORROSIVITY,
		WE_PSI,
		WE_PSI_EMPOWER,
		WE_DEATH,
		WE_COMPLETE_ARMOR_PIERCING,
		WE_COMPLETE_SHIELD_PIERCING,
		WE_COMPLETE_SHIELD_DISTORTION,
		FE_MAX
	};
	enum {
		AT_ABSOLUTE,
		AT_PROPORTIONAL,
		AT_MAX
	};
	enum {
		LOCAL_EFFECT,
		AREA_EFFECT_TARGET_ALLY,
		AREA_EFFECT_TARGET_ENEMY,
		AREA_EFFECT_TARGET_ALL,
		AE_LOCAL_EFFECT,
		TARGET_MAX
	};

protected :
	int
		mType,
		mApplyType,
		mPeriod,
		mAmount,
		mTarget,
		mCharge,
		mRange;
	static char *mEffectName[];
	static char *mTargetName[];
	static char *mTypeName[];
public :
	void set_type( char *aTypeName );
	void set_type( int aType ) { mType = aType; }
	void set_amount( int aAmount ) { mAmount = aAmount; }
	void set_apply_type( char *aTypeName );
	void set_apply_type( int aType ) { mApplyType = aType; }
	void set_period( int aPeriod ) { mPeriod = aPeriod; }
	void set_range( int aRange ) { mRange = aRange; }
	void set_target( char *aTargetName );
	void set_target( int aTarget ) { mTarget = aTarget; }
	int get_type() { return mType; }
	int get_apply_type() { return mApplyType; }
	int get_period() { return mPeriod; }
	int get_amount() { return mAmount; }
	int get_target() { return mTarget; }
	int get_range() { return mRange; }

	void charge() { mCharge++; }
	void use() { mCharge = 0; }
	bool ready() { return (mCharge >= mPeriod); }

	const char *print_html();

	RECYCLE(gFleetEffectZone);
};

/**
*/
class CFleetEffectListStatic : public CList
{
public :
	CFleetEffectListStatic();
	virtual ~CFleetEffectListStatic();

	CFleetEffectListStatic &operator=(CFleetEffectListStatic &aList);
protected:
	virtual bool free_item(TSomething aItem);
	virtual const char *debug_info() const { return "fleet effect list static"; }
};

/**
*/
class CFleetEffectListDynamic : public CList
{
public :
	CFleetEffectListDynamic();
	virtual ~CFleetEffectListDynamic();

	CFleetEffectListDynamic &operator+=(CFleetEffectListStatic &aList);
	CFleetEffectListDynamic &operator+=(CFleetEffectListDynamic &aList);

//		void add_fleet_effect(CFleetEffect *aEffect);
protected:
	virtual bool free_item(TSomething aItem);
	virtual const char *debug_info() const { return "fleet effect list dynamic"; }
};

/**
*/
class CComponent : public CPrerequisiteList
{
private:

	int
		mID,			//component index number
		mLevel;			//level

	CString
		mName,		//name
		mDescription;	//description

	CFleetEffectListStatic
		mEffectList;

	CCommandSet
		mAttribute;

	static char *
		mCategoryName[];
	static char *
		mCategoryNameNormal[];
	static char *
		mAttributeName[];
protected:
	int
		mCategory;		//category
public:
	CComponent();
	~CComponent() {}

	const char *get_name() { return (char *)mName; }
	const char *get_description() { return (char *)mDescription; }
	int get_id() { return mID; }
	int get_level() { return mLevel; }
	int get_category() { return mCategory; }
	const char *get_category_name( int aCategory );
	const char *get_category_name();
	char *get_category_name_normal();
	static char *get_category_name_normal(int aCategoty);

	void set_id( int aID ) { mID = aID; };
	void set_name( char *aName ) { mName = aName; }
	void set_description( char *aDescription )
	{ mDescription = aDescription; }
	void set_level( int aLevel ) { mLevel = aLevel; }
	void set_category(char *aCategoryName );

	const char *html_print_row();

	void add_effect( CFleetEffect *aEffect ) { mEffectList.push(aEffect); }
	int effect_length() { return mEffectList.length(); }
	CFleetEffect *get_effect( int i ) { return (CFleetEffect*)mEffectList.get(i); }
	CFleetEffectListStatic &get_effect_list() { return mEffectList; }
	int effect_amount( int aEffect );
	bool has_effect( int aEffect );

	bool has_attribute( int aAttr ) { return mAttribute.has(aAttr); }
	void set_attribute( char *aAttrStr );
	void set_attribute( int aAttr ) { mAttribute += aAttr; }

	enum EComponentCategory {
		CC_BEGIN = 0,
		CC_ARMOR = 0,
		CC_COMPUTER,
		CC_SHIELD,
		CC_ENGINE,
		CC_DEVICE,
		CC_WEAPON,
		CC_MAX
	};

	enum {
		CA_BEGIN = 1,
		CA_PSI_RACE_ONLY = 1,
		CA_BIO_ARMOR_ONLY,
		CA_NON_BIO_ARMOR_ONLY,
		CA_REACTIVE_ARMOR_ONLY,
		CA_NORMAL_ARMOR_ONLY,
		CA_HYBRID_ARMOR_ONLY,
		CA_CUSTOMIZABLE_ARMOR_ONLY,
		CA_MAX
	};
};

enum EWeaponType {
	WT_BEGIN = 0,
	WT_BEAM = 0,
	WT_MISSILE,
	WT_PROJECTILE,
	WT_FIGHTER,
	WT_MAX
};

extern	TZone gWeaponZone;
/**
*/
class CWeapon: public CComponent
{
private:
	int
		mWeaponType,	//weapon type
		mAttackingRate,	//attacking rate
		mDamageRoll,	//roll
		mDamageDice,	//dice, damage=[roll]D[dice]
		mSpace,			//space
		mCoolingTime,	//cooling time
		mRange,			//range
		mAngleOfFire,	//angle of fire, from front
		mSpeed;			//speed, move distance per turn

	static char *
		mWeaponTypeName[];
	static char *
		mWeaponTypeNameNormal[];
public:
	CWeapon();

	int get_weapon_type() { return mWeaponType; }
	char *get_weapon_type_name_normal();

	int get_attacking_rate() { return mAttackingRate; }
	int get_damage_roll() { return mDamageRoll; }
	int get_damage_dice() { return mDamageDice; }
	int get_space() { return mSpace; }
	int get_cooling_time() { return mCoolingTime; }
	int get_range() { return mRange; }
	int get_angle_of_fire() { return mAngleOfFire; }
	int get_speed() { return mSpeed; }

	void set_weapon_type( char *aWeaponType );
	void set_attacking_rate( int aAttackingRate ) { mAttackingRate = aAttackingRate; }
	void set_damage_roll( int aDamageRoll ) { mDamageRoll = aDamageRoll; }
	void set_damage_dice( int aDamageDice ) { mDamageDice = aDamageDice; }
	void set_space( int aSpace ) { mSpace = aSpace; }
	void set_cooling_time( int aCoolingTime ) { mCoolingTime = aCoolingTime; }
	void set_range( int aRange ) { mRange = aRange; }
	void set_angle_of_fire( int aAngleOfFire ) { mAngleOfFire = aAngleOfFire; }
	void set_speed( int aSpeed ) { mSpeed = aSpeed; }

	const char *html_print_row();

	RECYCLE(gWeaponZone);
};

enum EAromorType {
	AT_BEGIN = 0,
	AT_NORM = 0,
	AT_BIO,
	AT_REACTIVE,
	AT_HYBRID,
	AT_CUSTOMIZABLE,
	AT_MAX
};


extern TZone gArmorZone;
/**
*/
class CArmor: public CComponent
{	private:
	int
		mArmorType,
		mDefenseRate;
	double
		mHPmultiplier;
	static char *
		mArmorTypeName[];
	static char *
		mArmorTypeNameNormal[];
public:
	CArmor();

	int get_armor_type() { return mArmorType; }
	char *get_armor_type_name_normal();

	int get_defense_rate() { return mDefenseRate; }
	double get_hp_multiplier() { return mHPmultiplier; }

	void set_armor_type( char *aArmorType );
	void set_defense_rate( int aDefenseRate ) { mDefenseRate = aDefenseRate; }
	void set_hp_multiplier( double aHPmultiplier ) { mHPmultiplier = aHPmultiplier; }

	const char *html_print_row();

	RECYCLE(gArmorZone);
};

extern TZone gDeviceZone;
/**
*/
class CDevice: public CComponent
{
private:
	int
		mMinClass,	//required min class to load this device
		mMaxClass;	//required max class to load this device
public:
	CDevice();

	int get_min_class() { return mMinClass; }
	int get_max_class() { return mMaxClass; }

	void set_min_class( int aMinClass ) { mMinClass = aMinClass; }
	void set_max_class( int aMaxClass ) { mMaxClass = aMaxClass; }

	const char *html_print_row();

	RECYCLE(gDeviceZone);
};

extern TZone gEngineZone;
/**
*/
class CEngine: public CComponent
{	private:
	int
		mBattleSpeed[MAX_SHIP_CLASS],
		mBattleMobility[MAX_SHIP_CLASS],
		mCruiseSpeed;
public:
	CEngine();

	int get_battle_speed( int aLevel ) { return mBattleSpeed[aLevel]; }
	int get_battle_mobility( int aLevel ) { return mBattleMobility[aLevel]; }
	int get_cruise_speed() { return mCruiseSpeed; }


	void set_battle_speed( int aLevel, int aBattleSpeed ) { mBattleSpeed[aLevel] = aBattleSpeed; }
	void set_battle_mobility( int aLevel, int aBattleMobility ) { mBattleMobility[aLevel] = aBattleMobility; }
	void set_cruise_speed( int aCruiseSpeed ) { mCruiseSpeed = aCruiseSpeed; }

	const char *html_print_row();

	RECYCLE(gEngineZone);
};

extern TZone gComputerZone;
/**
*/
class CComputer: public CComponent
{
private:
	int
		mAttackingRate,	//attacking rate
		mDefenseRate;	//defense rate

public:
	CComputer();

	int get_attacking_rate() { return mAttackingRate; }
	int get_defense_rate() { return mDefenseRate; }

	void set_attacking_rate( int aAttackingRate ) { mAttackingRate = aAttackingRate; }
	void set_defense_rate( int aDefenseRate ) { mDefenseRate = aDefenseRate; }

	const char *html_print_row();

	RECYCLE(gComputerZone);
};

extern TZone gShieldZone;

/**
*/
class CShield: public CComponent
{
private:
	int
		mDeflectionSolidity,
		mStrength[MAX_SHIP_CLASS],
		mRechargeRate[MAX_SHIP_CLASS];

public:
	CShield();

	int get_deflection_solidity() { return mDeflectionSolidity; }
	int get_strength( int aClass ) { return mStrength[aClass]; }
	int get_recharge_rate( int aClass ) { return mRechargeRate[aClass]; }

	void set_deflection_solidity( int aDeflectionSolidity ) { mDeflectionSolidity = aDeflectionSolidity; }
	void set_strength( int aClass, int aStrength ) { mStrength[aClass] = aStrength; }
	void set_recharge_rate( int aClass, int aRechargeRate ) { mRechargeRate[aClass] = aRechargeRate; }

	const char *html_print_row();

	RECYCLE(gShieldZone);
};

/**
*/
class CComponentList: public CSortedList
{
public:
	CComponentList();
	virtual ~CComponentList();

	int add_component(CComponent *aComponent);
	bool remove_component(int aComponentID);

	CComponent* get_by_id(int aComponentID);
	CComponent* get_best_component( int aCategory );

// bug! never use!
//		CComponentList* get_by_category( int aCategory );

	char *armor_list_html();
	char *weapon_list_html(int aNumber, int aSpacePerSlot);
	char *device_list_html(int aNumber);

	int get_weapon_level();
protected:
	virtual bool free_item(TSomething aItem);
	virtual int compare(TSomething aItem1, TSomething aItem2) const;
	virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
	virtual const char *debug_info() const { return "component list"; }
};

/**
*/
class CComponentTable: public CSortedList
{
public:
	CComponentTable();
	virtual ~CComponentTable();

	int add_component(CComponent *aComponent);
	bool remove_component(int aComponentID);

	CComponent* get_by_id(int aComponentID);

protected:
	virtual bool free_item(TSomething aItem);
	virtual int compare(TSomething aItem1, TSomething aItem2) const;
	virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
	virtual const char *debug_info() const { return "component table"; }
};

/**
*/


/* end of file component.h */

#endif
