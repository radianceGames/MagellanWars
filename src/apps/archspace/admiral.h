#if !defined(__ARCHSPACE_ADMIRAL_H__)
#define __ARCHSPACE_ADMIRAL_H__

#include "store.h"

class CPlayer;

//---------------------------------------------------- CAdmiral
// ---Change--- //
#define MAX_RACE 							10
// ---End Change--- //
#define MAX_ADMIRAL_TYPE 					9

#define NUMBER_OF_ADMIRAL_SKILL_LEVEL		26
#define MAX_RACIAL_SKILL					3
#define MAX_LEVEL							20

extern TZone gAdmiralZone;
/**
*/

class CAdmiral: public CStore
{
public:
	enum EArmadaClass
	{
		AC_A = 0,
		AC_B,
		AC_C,
		AC_D,
		AC_MAX
	};

	enum
	{
			// attack abilities
		//SIEGE_PLANET = 0,
		//BLOCKADE,
		//RAID,
		//PRIVATEER,

			// defense abilities
		//SIEGE_REPELLING,
		//BREAK_BLOCKADE,
		//PREVENT_RAID,
		OFFENSE = 0,
		DEFENSE,

			// effectiveness abilities
		MANEUVER,
		DETECTION,
		//INTERPRETATION,

		MAX_SKILL
	};

	enum
	{
		LEVEL = 0,
		SKILL_UP_LEVEL,

		MAX_ELEMENT
	};

	enum EAdmiralStoreFlag
	{
		STORE_ID = 0,
		STORE_OWNER,
		STORE_RACE,
		STORE_TYPE,
		STORE_NAME,
		STORE_EXP,
		STORE_LEVEL,

		STORE_FLEET_NUMBER,

		// command abilities
		STORE_ARMADA_COMMANDING,
		STORE_FLEET_COMMANDING,
		STORE_EFFICIENCY,

		//Combat Abilitys
		STORE_OFFENSE,
		STORE_OFFENSE_UP_LEVEL,
		STORE_DEFENSE,
		STORE_DEFENSE_UP_LEVEL,
		// attack abilities
		//STORE_SIEGE_PLANET,
		//STORE_SIEGE_PLANET_UP_LEVEL,
		//STORE_BLOCKADE,
		//STORE_BLOCKADE_UP_LEVEL,
		//STORE_RAID,
		//STORE_RAID_UP_LEVEL,
		//STORE_PRIVATEER,
		//STORE_PRIVATEER_UP_LEVEL,

		// defense abilities
		//STORE_SIEGE_REPELLING,
		//STORE_SIEGE_REPELLING_UP_LEVEL,
		//STORE_BREAK_BLOCKADE,
		//STORE_BREAK_BLOCKADE_UP_LEVEL,
		//STORE_PREVENT_RAID,
		//STORE_PREVENT_RAID_UP_LEVEL,

		// effectiveness abilities
		STORE_MANEUVER,
		STORE_MANEUVER_UP_LEVEL,
		STORE_DETECTION,
		STORE_DETECTION_UP_LEVEL,
		//STORE_INTERPRETATION,
		//STORE_INTERPRETATION_UP_LEVEL,

		// special abilities
		STORE_COMMON_ABILITY,
		STORE_RACE_ABILITY
	};

	enum { // starting circumstance
		SC_SUPERCOMMANDER,
		SC_EXCELLENT,
		SC_VERY_GOOD,
		SC_GOOD,
		SC_AVERAGE,
		SC_POOR,
		SC_BAD,
		SC_VERY_BAD,
		SC_CANNON_FODDER,
		SC_MAX
	};

	enum {	// special ability
		SA_ENGINEERING_SPECIALIST,
		SA_SHIELD_SYSTEM_SPECIALIST,
		SA_MISSILE_SPECIALIST,
		SA_BALLISTIC_EXPERT,
		SA_ENERGY_SYSTEM_SPECIALIST,
//			SA_VETERAN_FIGHTER_PILOT,
		SA_MAX
	};

	enum {	// racial ability
		RA_IRRATIONAL_TACTICS,
		RA_INTUITION,
		RA_LONE_WOLF,
		RA_DNA_POISON_REPLICATER,
		RA_BREEDER_MALE,
		RA_CLONAL_DOUBLE,
		RA_XENOPHOBIC_FANATIC,
		RA_MENTAL_GIANT,
		RA_ARTIFACT_CRYSTAL,
		RA_PSYCHIC_PROGENITOR,
		RA_ARTIFACT_COOLING_ENGINE,
		RA_LYING_DORMANT,
		RA_MISSILE_CRATERS,
		RA_METEOR_DRONES,
		RA_CYBER_SCAN_UNIT,
		RA_TRAJECTORY_AUGMENTATION,
		RA_PATTERN_BROADCASTER,
		RA_FAMOUS_PRIVATEER,
		RA_COMMERCE_KING,
		RA_RETREAT_SHIELD,
		RA_GENETIC_THROWBACK,
		RA_RIGID_THINKING,
		RA_MANAGEMENT_PROTOCOL,
		RA_BLITZKRIEG,
		RA_TACTICAL_GENIUS,  //--NEW--
		RA_SHIELD_DISRUPTER,
		RA_DEFENSIVE_MATRIX,
		RA_CONSCIOUSNESS_CRYSTAL,
		// ---Change--- //
		RA_CRUSADER,
		RA_IMPINGEMENT_NEUTRALIZATION_FIELD,
		RA_ARMADA_SYNERGY_SPECIALIST,
		// ---End Change--- //
		RA_MAX
	};

private:
	int mOwner;
	int mID;
	int mRace;
	CString mName;
	int mStartingCircumstance;
	int	mExp;
	int mLevel;
	int mFleetNumber;

		// command abilities
	int
		mArmadaCommanding,	// default : 0 to 3
		mFleetCommanding,	// default : 6 to 11
		mEfficiency;		// default : 25 to 55%, min : 0%, max : 100%

		// attack, defense and effectiveness abilities
	int mSkill[MAX_SKILL][MAX_ELEMENT];
	int mSkillByEffect[MAX_SKILL+2];

		// special abilities
	int mSpecialAbility;
	int	mRacialAbility;

	static int mMaxID;

	static int
		mPlusFleetCommanding[MAX_LEVEL+1],
		mExpLevelTable[MAX_LEVEL+1],
		mPossibleRacialSkill[MAX_RACE][MAX_RACIAL_SKILL],
		mRaceInitialSkill[MAX_RACE][MAX_SKILL],
		mArmadaCommandingData[AC_MAX][NUMBER_OF_ADMIRAL_SKILL_LEVEL];

	static char
		*mStartingCircumstanceName[],
		*mSpecialAbilityName[],
		*mRacialAbilityName[],
		*mSpecialAbilityAcronym[],
		*mRacialAbilityAcronym[];

public:
	CAdmiral(CPlayer *aPlayer);
	CAdmiral(MYSQL_ROW aRow);
	CAdmiral(int aLevel, int geniusLevel, int aFleetCommandingBonus, int aRace);
			/* aRace = -1 means a random race. */
	virtual ~CAdmiral() {}

	virtual const char *table() { return "admiral"; }
	virtual CString& query();

protected:
	int get_starting_circumstance_number(int aGenius);

public: // get

	bool level_up();
	void give_level( int aLevel );

	int get_race() { return mRace; }
	char *get_name() { return (char *)mName; }
	char *get_nick();
	inline int get_id() { return mID; }
	inline int get_owner() { return mOwner; }
	inline int get_exp() { return mExp; }
	inline int get_level() { return mLevel; }
	inline int get_fleet_number() { return mFleetNumber; }
	inline int get_starting_circumstance() { return mStartingCircumstance; }
	char *get_starting_circumstance_name();

	int get_armada_commanding();
	char *get_armada_commanding_name();
	int get_real_armada_commanding() { return mArmadaCommanding; }

	int get_armada_commanding_effect(int aSkillType);
	int get_armada_commanding_effect_to_efficiency();

	int get_fleet_commanding();
	int get_real_fleet_commanding() { return mFleetCommanding; }

	int get_efficiency();
	int get_real_efficiency() { return mEfficiency; }

	//int get_siege_planet_level();
	//int get_blockade_level();
	//int get_raid_level();
	//int get_privateer_level();
	//int get_siege_repelling_level();
	//int get_break_blockade_level();
	//int get_prevent_raid_level();
	int get_maneuver_level();
	int get_detection_level();
	//int get_interpretation_level();

	int get_overall_attack();
	int get_overall_defense();

	/*int get_siege_planet_race_level()
	{ return mRaceInitialSkill[mRace-1][SIEGE_PLANET]; }
	int get_blockade_race_level()
	{ return mRaceInitialSkill[mRace-1][BLOCKADE]; }
	int get_raid_race_level() { return mRaceInitialSkill[mRace-1][RAID]; }
	int get_privateer_race_level() { return mRaceInitialSkill[mRace-1][PRIVATEER]; }
	int get_siege_repelling_race_level()
	{ return mRaceInitialSkill[mRace-1][SIEGE_REPELLING]; }
	int get_break_blockade_race_level()
	{ return mRaceInitialSkill[mRace-1][BREAK_BLOCKADE]; }
	int get_prevent_raid_race_level()
	{ return mRaceInitialSkill[mRace-1][PREVENT_RAID]; }*/
	int get_offense_race_level()
	{ return mRaceInitialSkill[mRace-1][OFFENSE]; }
	int get_defense_race_level()
	{ return mRaceInitialSkill[mRace-1][DEFENSE]; }
	int get_maneuver_race_level() { return mRaceInitialSkill[mRace-1][MANEUVER]; }

	int get_detection_race_level() { return mRaceInitialSkill[mRace-1][DETECTION]; }

	//int get_interpretation_race_level()
	//{ return mRaceInitialSkill[mRace-1][INTERPRETATION]; }

	int get_special_ability() { return mSpecialAbility; }
	char *get_special_ability_name();
	char *get_special_ability_acronym();
	int get_racial_ability() { return mRacialAbility; }
	char *get_racial_ability_name();
	char *get_racial_ability_acronym();

	char *get_ability_name();

	inline bool is_changed();
public: // set
	inline void set_owner(int aOwner);
	void gain_exp(int aExp);
	void set_fleet_number(int aFleetNumber);

	void set_level_by_effect( int aAbility, int aLevel );
	void clear_level_by_effect();

public:
	bool distribute_exp();

public: // etc
	friend CStoreCenter &operator<<(CStoreCenter &aStoreCenter, CAdmiral &aAdmiral);
	RECYCLE(gAdmiralZone);
};

inline void
	CAdmiral::set_owner(int aOwner)
{
	mStoreFlag += STORE_OWNER;
	mOwner = aOwner;
}

inline bool
	CAdmiral::is_changed()
{
	return !mStoreFlag.is_empty();
}

//----------------------------------------------------- CAdmiralList
/**
*/
class CAdmiralList: public CSortedList
{
public:
	CAdmiralList();
	virtual ~CAdmiralList();

	CPlayer *parent;

	bool add_admiral(CAdmiral* aAdmiral);
	bool remove_admiral(int aID);
	bool remove_without_free_admiral(int aID);

	CAdmiral* get_by_id(int aID);

	bool update_DB();

	char *attached_fleet_commander_info_html();
	char *attached_fleet_commander_info_javascript(CPlayer *aPlayer);
	char *pool_fleet_commander_info_html();
	char *pool_fleet_commander_info_javascript(CPlayer *aPlayer);
	char *fleet_commander_list_html();
	char *fleet_commander_list_javascript(CPlayer *aPlayer);

protected:
	virtual bool free_item(TSomething aItem);
	virtual int compare(TSomething aItem1, TSomething aItem2) const;
	virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
	virtual const char *debug_info() const { return "admiral list"; }
};

//--------------------------------------------------- CAdmiralNameList
/**
*/
class CAdmiralNameList: public CStringList
{
public:
	CAdmiralNameList();
	virtual ~CAdmiralNameList();

	bool add_name(char *aName);

		// get
	inline int get_race();
	inline int get_id();
	inline bool is_first();

		// set
	inline void set_race(int aRace);
	inline void first();
	inline void last();

protected:
	int mRace;
	bool mFirst;
	virtual const char *debug_info() const { return "admiral name list"; }
};

inline int
	CAdmiralNameList::get_race()
{
	return mRace;
}

inline bool
	CAdmiralNameList::is_first()
{
	return mFirst;
}

inline int
	CAdmiralNameList::get_id()
{
	return mRace*2 + ((mFirst) ? 0:1);
}

inline void
	CAdmiralNameList::set_race(int aRace)
{
	mRace = aRace;
}

inline void
	CAdmiralNameList::first()
{
	mFirst = true;
}

inline void
	CAdmiralNameList::last()
{
	mFirst = false;
}

//--------------------------------------------------- CAdmiralNameTable
/**
*/
class CAdmiralNameTable: public CSortedList
{
public:
	CAdmiralNameTable();
	virtual ~CAdmiralNameTable();

	bool add_name_list(CAdmiralNameList *aAdmiralNameList);
	const char *get_random_name(int aRace);

protected:
	const char *get_evintos_name();

	virtual bool free_item(TSomething aItem);
	virtual int compare(TSomething aItem1, TSomething aItem2) const;
	virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
	virtual const char *debug_info() const { return "admiral name table"; }
};

#endif
