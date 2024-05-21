#include "common.h"
#include "util.h"
#include "admiral.h"
#include "player.h"
#include "archspace.h"
#include "game.h"
#include "race.h"
#include <cstdlib>


TZone gAdmiralZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CAdmiral),
	0,
	0,
	NULL,
	"Zone CAdmiral"
};

int CAdmiral::mMaxID = 0;

int CAdmiral::mExpLevelTable[MAX_LEVEL+1] = {
	0,			// level 0
	1000,		// level 1
	3000,
	6000,
	10000,
	15000,		// level 5
	21000,
	28000,
	36000,
	45000,
	55000,		// level 10
	66000,
	78000,
	91000,
	105000,
	120000,		// level 15
	136000,
	153000,
	171000,
	190000,
	210000		// level 20
};

int
	CAdmiral::mPlusFleetCommanding[MAX_LEVEL+1] =
{
	0,	// level 0
	0,	// level 1
	3,
	3,
	3,
	2,	// level 5
	2,
	2,
	2,
	2,
	1,	// level 10
	1,
	1,
	1,
	1,
	1,	// level 15
	1,
	1,
	1,
	1,
	0	// level 20
};

int
	CAdmiral::mArmadaCommandingData[CAdmiral::AC_MAX][NUMBER_OF_ADMIRAL_SKILL_LEVEL] =
{
	{-2, -2, -2, -1, -1, 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 7, 7, 8},
	{-2, -2, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6},
	{-2, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4},
	{-1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2}
};

char *CAdmiral::mStartingCircumstanceName[] =
{
	"Supercommander",
	"Excellent",
	"Very good",
	"Good",
	"Average",
	"Poor",
	"Bad",
	"Very Bad",
	"Cannon Fodder"
};

char *CAdmiral::mSpecialAbilityName[] =
{
	"Engineering Specialist",
	"Shield System Specialist",
	"Missile Specialist",
	"Ballistic Expert",
	"Energy System Specialist",
	"Veteran Fighter Pilot"
};

char *CAdmiral::mRacialAbilityName[] =
{
	"Irrational Tactics",
	"Intuition",
	"Lone Wolf",
	"DNA Poison Replicater",
	"Breeder Male",
	"Clonal Double",
	"Xenophobic Fanatic",
	"Mental Giant",
	"Artifact Crystal",
	"Psychic Progenitor",
	"Artifact Cooling Engine",
	"Lying Dormant",
	"Missile Craters",
	"Meteor Drones",
	"Cyber Scan Unit",
	"Trajectory Augmentation",
	"Pattern Broadcaster",
	"Famous Privateer",
	"Commerce King",
	"Retreat Shield",
	"Genetic Throwback",
	"Rigid Thinking",
	"Management Protocol",
	"Blitzkreig",
	"Tactical Genius",
	"Shield Disrupter",
	"Defensive Matrix",
	"Consciousness Crystal",
	"Crusader",
	"Impingement Neutralization Field",
	"Armada Synergy Specialist"
};

int CAdmiral::mPossibleRacialSkill[MAX_RACE][MAX_RACIAL_SKILL] = 
{
    {
        RA_IRRATIONAL_TACTICS,
        RA_INTUITION,
        RA_LONE_WOLF
    },
    {
        RA_DNA_POISON_REPLICATER,
        RA_BREEDER_MALE,
        RA_CLONAL_DOUBLE
    },
    {
        RA_SHIELD_DISRUPTER,
        RA_FAMOUS_PRIVATEER,
        RA_COMMERCE_KING
    },
    {
        RA_CYBER_SCAN_UNIT,
        RA_TRAJECTORY_AUGMENTATION,
        RA_PATTERN_BROADCASTER
    },
    {
        RA_CYBER_SCAN_UNIT,
        RA_RIGID_THINKING,
        RA_MANAGEMENT_PROTOCOL
    },
    {
        RA_LYING_DORMANT,
        RA_MISSILE_CRATERS,
        RA_METEOR_DRONES
    },
    {
        RA_MENTAL_GIANT,
        RA_RETREAT_SHIELD,
        RA_GENETIC_THROWBACK
    },
    {
        RA_XENOPHOBIC_FANATIC,
        RA_MENTAL_GIANT,
        RA_CONSCIOUSNESS_CRYSTAL
    },
    {
        RA_ARTIFACT_CRYSTAL,
        RA_TACTICAL_GENIUS,
        RA_BLITZKRIEG
    },
    {
        RA_DEFENSIVE_MATRIX,
        RA_PSYCHIC_PROGENITOR,
        RA_ARTIFACT_COOLING_ENGINE
    }
};


static int
	GeniusRatio[CAdmiral::SC_MAX][21] =
{
	{ 0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 11, 12, 13, 14, 15, 16, 17},
	{ 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
	{ 5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25},
	{15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
	{25, 27, 28, 29, 30, 30, 30, 29, 28, 27, 25, 23, 21, 19, 16, 16, 16, 16, 16, 15, 15},
	{25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,  8, 8, 7, 7, 6, 6},
	{15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 0, 0, 0, 0, 0},
	{ 9,  8,  7,  6,  5,  4,  3,  2,  1,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0 ,0 ,0},
	{ 6,  5,  4,  3,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0 ,0 ,0 ,0 ,0}
};

int
	CAdmiral::mRaceInitialSkill[MAX_RACE][MAX_SKILL] =
{
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0},
	{ 0,  0,  0,  0}
};

CAdmiral::CAdmiral(CPlayer *aPlayer)
{
	extern void scramble_10( int skill[] );

	static int
		InitialSkill[MAX_ADMIRAL_TYPE][CAdmiral::MAX_SKILL] =
	{
		{  1,  2,  3,  4},
		{  0,  1,  2,  3},
		{ -1,  0,  1,  2},
		{ -1,  0,  0,  1},
		{ -1, -1,  0,  1},
		{ -2, -1, -1,  0},
		{ -3, -2, -1,  0},
		{ -4, -3, -2, -1},
		{ -5, -4, -3, -2}
	};


	mRace = aPlayer->get_race();
	CControlModel *
	ControlModel = aPlayer->get_control_model();
	mStartingCircumstance = get_starting_circumstance_number(ControlModel->get_genius());
	mName = ADMIRAL_NAME_TABLE->get_random_name(mRace);
	mOwner = aPlayer->get_game_id();
	mExp = 0;
	mLevel = 1;
	mID = ++mMaxID;
	mFleetNumber = 0;

	// command abilities
	// default : 0 to 3. It's never changed.
	mArmadaCommanding = number(4) - 1;
	// default : 6 to 11
	mFleetCommanding = number(6) + 5;
	// default : 25 to 55%, min : 0%, max : 100%
	mEfficiency = number(31) + 24;

	static int
		SkillIndex[MAX_SKILL] = {  1,  3,  5,  7 };
	scramble_10( SkillIndex );

	for( int i = 0; i < MAX_SKILL; i++ ){
		mSkill[SkillIndex[i]][LEVEL] = InitialSkill[mStartingCircumstance][i] + mRaceInitialSkill[mRace-1][i];
	}

	static int
		IncreaseRate[] = { 1,  2, 3, 4 };
	scramble_10( SkillIndex );

	for (int i=0 ; i<MAX_SKILL ; i++)
	{
		mSkill[SkillIndex[i]][SKILL_UP_LEVEL] = IncreaseRate[i];
	}

	clear_level_by_effect();

	mSpecialAbility = number(SA_MAX)-1;
	mRacialAbility = mPossibleRacialSkill[mRace-1][number(3)-1];
}

CAdmiral::CAdmiral(MYSQL_ROW aRow)
{

	mID = atoi(aRow[STORE_ID]);
	mOwner = atoi(aRow[STORE_OWNER]);
	mRace = atoi(aRow[STORE_RACE]);
	mStartingCircumstance = atoi(aRow[STORE_TYPE]);
	mName = (char *)aRow[STORE_NAME];
	mExp = atoi(aRow[STORE_EXP]);
	mLevel = atoi(aRow[STORE_LEVEL]);
	mFleetNumber = atoi(aRow[STORE_FLEET_NUMBER]);

	// command abilities
	mArmadaCommanding = atoi(aRow[STORE_ARMADA_COMMANDING]);
	mFleetCommanding = atoi(aRow[STORE_FLEET_COMMANDING]);
	mEfficiency = atoi(aRow[STORE_EFFICIENCY]);

	// attack abilities
	//mSkill[SIEGE_PLANET][LEVEL]
	//= atoi(aRow[STORE_SIEGE_PLANET]);

	//mSkill[SIEGE_PLANET][SKILL_UP_LEVEL]
	//= atoi(aRow[STORE_SIEGE_PLANET_UP_LEVEL]);

	//mSkill[BLOCKADE][LEVEL]
	//= atoi(aRow[STORE_BLOCKADE]);

	//mSkill[BLOCKADE][SKILL_UP_LEVEL]
	//= atoi(aRow[STORE_BLOCKADE_UP_LEVEL]);

	//mSkill[RAID][LEVEL]
	//= atoi(aRow[STORE_RAID]);

	//mSkill[RAID][SKILL_UP_LEVEL]
	//= atoi(aRow[STORE_RAID_UP_LEVEL]);

	//mSkill[PRIVATEER][LEVEL]
	//= atoi(aRow[STORE_PRIVATEER]);

	//mSkill[PRIVATEER][SKILL_UP_LEVEL]
	//= atoi(aRow[STORE_PRIVATEER_UP_LEVEL]);

	// defense abilities
	//mSkill[SIEGE_REPELLING][LEVEL]
	//= atoi(aRow[STORE_SIEGE_REPELLING]);

	//mSkill[SIEGE_REPELLING][SKILL_UP_LEVEL]
	//= atoi(aRow[STORE_SIEGE_REPELLING_UP_LEVEL]);

	//mSkill[BREAK_BLOCKADE][LEVEL]
	//= atoi(aRow[STORE_BREAK_BLOCKADE]);

	//mSkill[BREAK_BLOCKADE][SKILL_UP_LEVEL]
	//= atoi(aRow[STORE_BREAK_BLOCKADE_UP_LEVEL]);

	//mSkill[PREVENT_RAID][LEVEL]
	//= atoi(aRow[STORE_PREVENT_RAID]);

	//mSkill[PREVENT_RAID][SKILL_UP_LEVEL]
	//= atoi(aRow[STORE_PREVENT_RAID_UP_LEVEL]);

	//combat abilities
	mSkill[DEFENSE][LEVEL]
	= atoi(aRow[STORE_DEFENSE]);

	mSkill[DEFENSE][SKILL_UP_LEVEL]
	= atoi(aRow[STORE_DEFENSE_UP_LEVEL]);

	mSkill[OFFENSE][LEVEL]
	= atoi(aRow[STORE_OFFENSE]);

	mSkill[OFFENSE][SKILL_UP_LEVEL]
	= atoi(aRow[STORE_OFFENSE_UP_LEVEL]);

	// effectiveness abilities
	mSkill[MANEUVER][LEVEL]
	= atoi(aRow[STORE_MANEUVER]);

	mSkill[MANEUVER][SKILL_UP_LEVEL]
	= atoi(aRow[STORE_MANEUVER_UP_LEVEL]);

	mSkill[DETECTION][LEVEL]
	= atoi(aRow[STORE_DETECTION]);

	mSkill[DETECTION][SKILL_UP_LEVEL]
	= atoi(aRow[STORE_DETECTION_UP_LEVEL]);

	//mSkill[INTERPRETATION][LEVEL]
	//= atoi(aRow[STORE_INTERPRETATION]);

	//mSkill[INTERPRETATION][SKILL_UP_LEVEL]
	//= atoi(aRow[STORE_INTERPRETATION_UP_LEVEL]);

	clear_level_by_effect();

	mSpecialAbility = atoi(aRow[STORE_COMMON_ABILITY]);
	mRacialAbility = atoi(aRow[STORE_RACE_ABILITY]);

	if (mID > mMaxID) mMaxID = mID;

	type(QUERY_NONE);
}

CAdmiral::CAdmiral(int aLevel, int geniusLevel, int aFleetCommandingBonus, int aRace)
{
	if (aLevel < 1) aLevel = 1;
	if (aLevel > 20) aLevel = 20;

	extern void scramble_10( int skill[] );
	static int
		InitialSkill[MAX_ADMIRAL_TYPE][CAdmiral::MAX_SKILL] =
	{
		{  1,  2,  3,  4},
		{  0,  1,  2,  3},
		{ -1,  0,  1,  2},
		{ -1,  0,  0,  1},
		{ -1, -1,  0,  1},
		{ -2, -1, -1,  0},
		{ -3, -2, -1,  0},
		{ -4, -3, -2, -1},
		{ -5, -4, -3, -2}
	};

	mOwner = 0;
	mID = ++mMaxID;
	if( aRace < 0 ) {
		mRace = number( MAX_RACE );
		if (mRace == CRace::RACE_TRABOTULIN) {
            mRace = number( MAX_RACE );
        }
    } else  {
		mRace = aRace;
    }

	mName = ADMIRAL_NAME_TABLE->get_random_name( mRace );
	if (!strcmp(mName, "BoneyMan") || !strcmp(mName, "hxc")) {
        geniusLevel = 15;
    }
	mStartingCircumstance = get_starting_circumstance_number( geniusLevel );
	mExp = 0;
	mLevel = 1;
	mFleetNumber = 0;

	// command abilities
	// default : 0 to 3. It's never changed.
	mArmadaCommanding = number(4) - 1;
	// default : 6 to 11
	mFleetCommanding = number(6) + 5 + aFleetCommandingBonus;
	// default : 25 to 55%, min : 0%, max : 100%
	mEfficiency = number(31) + 24;

	static int
		SkillIndex[MAX_SKILL] = { 1, 3, 5, 7};
	scramble_10(SkillIndex);

	for (int i = 0 ; i<MAX_SKILL ; i++)
	{
		mSkill[SkillIndex[i]][LEVEL] = InitialSkill[mStartingCircumstance][i] + mRaceInitialSkill[mRace-1][i];
	}

	static int
		IncreaseRate[] = {1, 2, 3, 4 };
	scramble_10(SkillIndex);

	for (int i=0 ; i<MAX_SKILL ; i++)
	{
		mSkill[SkillIndex[i]][SKILL_UP_LEVEL] = IncreaseRate[i];
	}

	clear_level_by_effect();

	mSpecialAbility = number(SA_MAX)-1;
	mRacialAbility = mPossibleRacialSkill[mRace-1][number(3)-1];

	give_level(aLevel);
}

char *
	CAdmiral::get_nick()
{
	static CString
		Nick;
	Nick.clear();

	Nick.format("%s(#%d)", get_name(), get_id());

	return (char *)Nick;
}

#if CLASSIC_MODE /* CLASSIC_MODE */

// Selects a commander starting circumstance based on a table
int CAdmiral::get_starting_circumstance_number(int aGenius) {
    int Random = number(100);
    int Total = 0;

    if( aGenius < -5 ) aGenius = -5;
    if( aGenius > 15 ) aGenius = 15;

    for(int i=0; i<MAX_ADMIRAL_TYPE; i++) {
        Total += GeniusRatio[i][aGenius+5];
        if (Total >= Random)
            return i;
    }
    return 4;
}

#else /* NOT CLASSIC_MODE */

// Selects a commander starting circumstance based on a formula
int CAdmiral::get_starting_circumstance_number(int aGenius) {
    int Random = number(100);

    if( aGenius < 0 ) aGenius = 0;

    int finalType = (int) Random/(aGenius + 5);
    if (finalType < SC_SUPERCOMMANDER) {
        finalType = SC_SUPERCOMMANDER;
    } else if (finalType >= SC_MAX) {
        finalType = SC_MAX - 1;
    }

    return finalType;
}

#endif /* NOT CLASSIC_MODE */

int
	CAdmiral::get_fleet_commanding()
{
	int
		Skill = mFleetCommanding;

	/*if( mRacialAbility == RA_JURY_RIGGER ){
		if( mLevel <= 5 )
			Skill--;
		else if( mLevel <= 12 )
			Skill -= 2;
		else
			Skill -= 3;
	}*/

	return Skill;
}

int
	CAdmiral::get_efficiency()
{
	int
		Skill = mEfficiency;

	if( mRacialAbility == RA_CLONAL_DOUBLE ){
		if( mLevel <= 7 )
			Skill -= 10;
		else if( mLevel <= 13 )
			Skill -= 5;
	}
	if( mRacialAbility == RA_PSYCHIC_PROGENITOR ){
		if( mLevel <= 12 )
			Skill -= 10;
		else if( mLevel <= 17 )
			Skill -= 5;
	}
	if( mRacialAbility == RA_TRAJECTORY_AUGMENTATION ){
		Skill += 10+mLevel;
	}
	if( mRacialAbility == RA_RETREAT_SHIELD ){
		Skill -= mLevel*2;
	}
	if( mRacialAbility == RA_BREEDER_MALE ){
		Skill += mLevel/2;

	}
	Skill += mSkillByEffect[10];

	if (Skill > 100) Skill = 100;

	return Skill;
}

int
	CAdmiral::get_overall_attack()
{
	int
		Skill = mSkill[OFFENSE][LEVEL];

	CPlayer *
	Owner = PLAYER_TABLE->get_by_game_id(get_owner());

	if( Owner->has_ability(ABILITY_PACIFIST) )
		Skill -= 3;

	if( Owner->has_ability(ABILITY_TACTICAL_MASTERY) )
		Skill += 3;

	if( mRacialAbility == RA_GENETIC_THROWBACK ){
		if( mLevel <= 10 )
			Skill++;
		else if( mLevel <= 15 )
			Skill += 2;
		else if( mLevel <= 19 )
			Skill += 3;
		else
			Skill += 5;
	}
	if( mRacialAbility == RA_XENOPHOBIC_FANATIC ){
		if( mLevel <= 12 )
		;
		else if( mLevel <= 19 )
			Skill++;
		else
			Skill += 2;
	}

	return Skill;
}

int
	CAdmiral::get_overall_defense()
{
	int
		Skill = mSkill[DEFENSE][LEVEL];


	CPlayer *
	Owner = PLAYER_TABLE->get_by_game_id(get_owner());

	if( Owner->has_ability(ABILITY_TACTICAL_MASTERY) )
		Skill += 3;

	if( mRacialAbility == RA_GENETIC_THROWBACK ){
		if( mLevel <= 19 )
			Skill--;
	}
	if( mRacialAbility == RA_BLITZKRIEG ){
		if( mLevel <= 19 )
			Skill--;
	}

	return Skill;
}

/****************int
	CAdmiral::get_siege_planet_level()
{
	int
		Skill = mSkill[SIEGE_PLANET][LEVEL];

	if( mRacialAbility == RA_XENOPHOBIC_FANATIC ){
		if( mLevel <= 4 )
			Skill++;
		else if( mLevel <= 9 )
			Skill += 2;
		else if( mLevel <= 14 )
			Skill += 3;
		else if( mLevel <= 18 )
			Skill += 4;
		else
			Skill += 5;
	}

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[SIEGE_PLANET];

	return Skill;
}

int
	CAdmiral::get_blockade_level()
{
	int
		Skill = mSkill[BLOCKADE][LEVEL];

	if( mRacialAbility == RA_XENOPHOBIC_FANATIC ){
		if( mLevel <= 4 )
			Skill++;
		else if( mLevel <= 9 )
			Skill += 2;
		else if( mLevel <= 14 )
			Skill += 3;
		else if( mLevel <= 18 )
			Skill += 4;
		else
			Skill += 5;
	}

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[BLOCKADE];

	return Skill;
}

int
	CAdmiral::get_raid_level()
{
	int
		Skill = mSkill[RAID][LEVEL];

	if( mRacialAbility == RA_XENOPHOBIC_FANATIC ){
		if( mLevel <= 4 )
			Skill++;
		else if( mLevel <= 9 )
			Skill += 2;
		else if( mLevel <= 14 )
			Skill += 3;
		else if( mLevel <= 18 )
			Skill += 4;
		else
			Skill += 5;
	}

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[RAID];

	return Skill;
}

int
	CAdmiral::get_privateer_level()
{
	int
		Skill = mSkill[PRIVATEER][LEVEL];

	if( mRacialAbility == RA_LONE_WOLF ) {
		if( mLevel <= 4 )
			Skill++;
		else if( mLevel <=8 )
			Skill += 2;
		else if( mLevel <= 12 )
			Skill += 3;
		else if( mLevel <= 17 )
			Skill += 4;
		else if( mLevel <= 19 )
			Skill += 5;
		else
			Skill += 7;
	}
	if( mRacialAbility == RA_XENOPHOBIC_FANATIC ){
		if( mLevel <= 4 )
			Skill++;
		else if( mLevel <= 9 )
			Skill += 2;
		else if( mLevel <= 14 )
			Skill += 3;
		else if( mLevel <= 18 )
			Skill += 4;
		else
			Skill += 5;
	}
	if( mRacialAbility == RA_FAMOUS_PRIVATEER ){
		if( mLevel <= 6 )
			Skill += 3;
		else if( mLevel <= 13 )
			Skill += 4;
		else if( mLevel <= 19 )
			Skill += 5;
		else
			Skill += 7;
	}

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[PRIVATEER];

	return Skill;
}

int
	CAdmiral::get_siege_repelling_level()
{
	int
		Skill = mSkill[SIEGE_REPELLING][LEVEL];

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[SIEGE_REPELLING];

	return Skill;
}

int
	CAdmiral::get_break_blockade_level()
{
	int
		Skill = mSkill[BREAK_BLOCKADE][LEVEL];

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[BREAK_BLOCKADE];

	return Skill;
}

int
	CAdmiral::get_prevent_raid_level()
{
	int
		Skill = mSkill[PREVENT_RAID][LEVEL];

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[PREVENT_RAID];

	return Skill;
}*/

int
	CAdmiral::get_maneuver_level()
{
	int
		Skill = mSkill[MANEUVER][LEVEL];

	if( mRacialAbility == RA_IRRATIONAL_TACTICS ){
		if( mLevel <= 7 )
			Skill -= 2;
		else if( mLevel <= 19 )
			Skill--;
	}
	if( mRacialAbility == RA_METEOR_DRONES ){
		if( mLevel <= 7 )
			Skill--;
		else if( mLevel <= 13 )
			Skill -= 2;
		else if( mLevel <= 19 )
			Skill -= 3;
		else
			Skill -= 4;
	}
	if( mRacialAbility == RA_BLITZKRIEG ){
		if( mLevel <= 5 )
			Skill += 2;
		else if( mLevel <= 9 )
			Skill += 3;
		else if( mLevel <= 12 )
			Skill += 4;
		else if( mLevel <= 19 )
			Skill += 5;
		else
			Skill += 7;
	}

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[MANEUVER];

	return Skill;
}

int
	CAdmiral::get_detection_level()
{
	int
		Skill = mSkill[DETECTION][LEVEL];

	if( mRacialAbility == RA_MENTAL_GIANT ){
		if( mLevel <= 6 )
			Skill += 3;
		else if( mLevel <= 9 )
			Skill += 4;
		else if( mLevel <= 12 )
			Skill += 5;
		else if( mLevel <= 15 )
			Skill += 6;
		else if( mLevel <= 17 )
			Skill += 8;
		else if( mLevel <= 18 )
			Skill += 10;
		else if( mLevel <= 19 )
			Skill += 11;
		else
			Skill += 12;
	}
	if( mRacialAbility == RA_ARTIFACT_COOLING_ENGINE ){
		if( mLevel <= 5 )
			Skill -= 1;
		else if( mLevel <= 8 )
			Skill -= 2;
		else if( mLevel <= 13 )
			Skill -= 4;
		else if( mLevel <= 17 )
			Skill -= 6;
		else
			Skill -= 8;
	}
	if( mRacialAbility == RA_CYBER_SCAN_UNIT)
	{
		if( mLevel <= 3 )
			Skill += 1;
		else if( mLevel <= 5 )
			Skill += 2;
		else if( mLevel <= 8 )
			Skill += 3;
		else if( mLevel <= 12 )
			Skill += 4;
		else if( mLevel <= 15 )
			Skill += 5;
		else if( mLevel <= 17 )
			Skill += 6;
		else if( mLevel <= 19 )
			Skill += 7;
		else
			Skill += 9;
	}
	if( mRacialAbility == RA_INTUITION)
	{
		if( mLevel <= 4 )
			Skill += 1;
		else if( mLevel <= 7 )
			Skill += 2;
		else if( mLevel <= 11 )
			Skill += 3;
		else if( mLevel <= 14 )
			Skill += 4;
		else if( mLevel <= 17 )
			Skill += 5;
		else
			Skill += 6;
	}

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}
	
	// ---Change--- //
	if( mRacialAbility == RA_CRUSADER ){
		Skill += (int) mLevel/2;
	}
	// ---End Change--- //

	Skill += mSkillByEffect[DETECTION];

	return Skill;
}
/*
int
	CAdmiral::get_interpretation_level()
{
	int
		Skill = mSkill[INTERPRETATION][LEVEL];

	if (mRacialAbility == RA_INTUITION)
	{
		if( mLevel <=4 )
			Skill += 3;
		else if( mLevel <=10 )
			Skill += 4;
		else if( mLevel <=14 )
			Skill += 5;
		else if( mLevel <=19 )
			Skill += 6;
		else		// no additional bonus, but they cannot corret. this is temporary implementation.
			Skill += 9;
	}
	if (mRacialAbility == RA_ARTIFACT_COOLING_ENGINE)
	{
		if( mLevel <= 6 )
			Skill --;
		else if( mLevel <= 12 )
			Skill -= 2;
		else if( mLevel <= 18 )
			Skill -= 3;
		else if( mLevel == 19 )
			Skill -= 4;
		else
			Skill -= 5;
	}
	if( mRacialAbility == RA_CYBER_SCAN_UNIT)
	{
		if( mLevel <= 4 )
			Skill++;
		else if( mLevel <= 8 )
			Skill += 2;
		else if( mLevel <= 12 )
			Skill += 3;
		else if( mLevel <= 15 )
			Skill += 4;
		else if( mLevel <= 17 )
			Skill += 5;
		else if( mLevel <= 19 )
			Skill += 6;
		else
			Skill += 7;
	}

	if( mRacialAbility == RA_BREEDER_MALE ){
		if( mLevel <= 12 )
			Skill++;
		else
			Skill += 2;
	}

	Skill += mSkillByEffect[INTERPRETATION];

	return Skill;
}*/




void
	CAdmiral::gain_exp(int aExp)
{
	mStoreFlag += STORE_EXP;
	mExp += aExp;
	level_up();
}

void
	CAdmiral::set_fleet_number(int aFleetNumber)
{
	mFleetNumber = aFleetNumber;
	mStoreFlag += STORE_FLEET_NUMBER;
}

char *
	CAdmiral::get_starting_circumstance_name()
{
	return mStartingCircumstanceName[mStartingCircumstance];
}

int
	CAdmiral::get_armada_commanding()
{
	int
		Skill = mArmadaCommanding; //+mSkillByEffect[11]
	if( Skill < AC_A ) Skill = AC_A;
	if( Skill > AC_D ) Skill = AC_D;

	return Skill;
}

char *
	CAdmiral::get_armada_commanding_name()
{
	switch (get_armada_commanding())
	{
		case AC_A :
			return "A";
		case AC_B :
			return "B";
		case AC_C :
			return "C";
		case AC_D :
			return "D";
		default :
			return "";
	}
}

int
	CAdmiral::get_armada_commanding_effect(int aSkillType)
{
	int
		Level = mSkill[aSkillType][LEVEL];
	if( Level < -5 ) Level = -5;
	if( Level > 20 ) Level = 20;

	if (mRacialAbility != RA_TACTICAL_GENIUS)
		return mArmadaCommandingData[get_armada_commanding()][Level+5];
	else
		return (int)(3*mArmadaCommandingData[get_armada_commanding()][Level+5]/2);
}

int
	CAdmiral::get_armada_commanding_effect_to_efficiency()
{
	if (mRacialAbility != RA_TACTICAL_GENIUS)
	{
		switch (get_armada_commanding())
		{
			case AC_A :
				return 10;
			case AC_B :
				return 5;
			case AC_C :
				return 0;
			case AC_D :
				return -5;
			default :
				return -999;
		}
	}
	else
	{
		switch (get_armada_commanding())
		{
			case AC_A :
				return 15;
			case AC_B :
				return 10;
			case AC_C :
				return 5;
			case AC_D :
				return 0;
			default :
				return -999;
		}
	}
}

char *
	CAdmiral::get_special_ability_name()
{
	return mSpecialAbilityName[mSpecialAbility];
}

char *
	CAdmiral::get_racial_ability_name()
{
	return mRacialAbilityName[mRacialAbility];
}

char *CAdmiral::mSpecialAbilityAcronym[] =
{
	"ES",
	"SSS",
	"MS",
	"BE",
	"ESS",
	"VFP"
};

char *CAdmiral::mRacialAbilityAcronym[] =
{
	"IT",
	"INT",
	"LW",
	"DPR",
	"BM",
	"CD",
	"XF",
	"MG",
	"AC",
	"PP",
	"ACE",
	"LD",
	"MC",
	"MD",
	"CSU",
	"TA",
	"PB",
	"FP",
	"CK",
	"RS",
	"GT",
	"RT",
	"MP",
	"BKG",
	"TG",
	"SD",
	"DM",
	"CC",
	"CRU",
	"INF",
	"ASS"
};

char *
	CAdmiral::get_special_ability_acronym()
{
	return mSpecialAbilityAcronym[mSpecialAbility];
}

char *
	CAdmiral::get_racial_ability_acronym()
{
	return mRacialAbilityAcronym[mRacialAbility];
}

char *
	CAdmiral::get_ability_name()
{
	CString
		CommonAbility = get_special_ability_name(),
	RaceAbility = get_racial_ability_name();

	return (char *)format("%s, %s", (char *)CommonAbility, (char *)RaceAbility);
}

bool
	CAdmiral::level_up()
{
	if (mLevel >= MAX_LEVEL) return false;
	if (mExp < mExpLevelTable[mLevel]) return false;

	do
	{
		mLevel++;
		mStoreFlag += STORE_LEVEL;


		mFleetCommanding += mPlusFleetCommanding[mLevel];
		if(mRacialAbility == RA_BREEDER_MALE)
		{
			switch(mLevel)
			{
				case 2:
					{
						mFleetCommanding += 1;
						break;
					}
				case 6:
					{
						mFleetCommanding += 1;
						break;
					}
				case 10:
					{
						mFleetCommanding += 1;
						break;
					}
				case 13:
					{
						mFleetCommanding += 1;
						break;
					}
				case 16:
					{
						mFleetCommanding += 1;
						break;
					}
				case 19:
					{
						mFleetCommanding += 1;
						break;
					}
			}
		}
		if(mRacialAbility == RA_MANAGEMENT_PROTOCOL)
		{
			switch(mLevel)
			{
				case 2:
					{
						mFleetCommanding += 1;
						break;
					}
				case 7:
					{
						mFleetCommanding += 1;
						break;
					}
				case 12:
					{
						mFleetCommanding += 1;
						break;
					}
				case 17:
					{
						mFleetCommanding += 1;
						break;
					}
			}
		}
		// ---Change--- //
		if(mRacialAbility == RA_ARMADA_SYNERGY_SPECIALIST)
		{
			mFleetCommanding += 1;
		}
		// ---End Change--- //
		
		if(mFleetCommanding > 100)
			mFleetCommanding = 100;

		mStoreFlag += STORE_FLEET_COMMANDING;
		/*if (mLevel%mSkill[SIEGE_PLANET][SKILL_UP_LEVEL] == 0)
		{
			mSkill[SIEGE_PLANET][LEVEL]++;
			mStoreFlag += STORE_SIEGE_PLANET;
		}
		if (mLevel%mSkill[BLOCKADE][SKILL_UP_LEVEL] == 0)
		{
			mSkill[BLOCKADE][LEVEL]++;
			mStoreFlag += STORE_BLOCKADE;
		}
		if (mLevel%mSkill[RAID][SKILL_UP_LEVEL] == 0)
		{
			mSkill[RAID][LEVEL]++;
			mStoreFlag += STORE_RAID;
		}
		if (mLevel%mSkill[PRIVATEER][SKILL_UP_LEVEL] == 0)
		{
			mSkill[PRIVATEER][LEVEL]++;
			mStoreFlag += STORE_PRIVATEER;
		}
		if (mLevel%mSkill[SIEGE_REPELLING][SKILL_UP_LEVEL] == 0)
		{
			mSkill[SIEGE_REPELLING][LEVEL]++;
			mStoreFlag += STORE_SIEGE_REPELLING;
		}
		if (mLevel%mSkill[BREAK_BLOCKADE][SKILL_UP_LEVEL] == 0)
		{
			mSkill[BREAK_BLOCKADE][LEVEL]++;
			mStoreFlag += STORE_BREAK_BLOCKADE;
		}
		if (mLevel%mSkill[PREVENT_RAID][SKILL_UP_LEVEL] == 0)
		{
			mSkill[PREVENT_RAID][LEVEL]++;
			mStoreFlag += STORE_PREVENT_RAID;
		}*/
		if (mLevel%mSkill[OFFENSE][SKILL_UP_LEVEL] == 0)
		{
			mSkill[OFFENSE][LEVEL]++;
			mStoreFlag += STORE_OFFENSE;
		}
		if (mLevel%mSkill[DEFENSE][SKILL_UP_LEVEL] == 0)
		{
			mSkill[DEFENSE][LEVEL]++;
			mStoreFlag += STORE_DEFENSE;
		}

		if (mLevel%mSkill[MANEUVER][SKILL_UP_LEVEL] == 0)
		{
			mSkill[MANEUVER][LEVEL]++;
			mStoreFlag += STORE_MANEUVER;
		}
		if (mLevel%mSkill[DETECTION][SKILL_UP_LEVEL] == 0)
		{
			mSkill[DETECTION][LEVEL]++;
			mStoreFlag += STORE_DETECTION;
		}
		/*if (mLevel%mSkill[INTERPRETATION][SKILL_UP_LEVEL] == 0)
		{
			mSkill[INTERPRETATION][LEVEL]++;
			mStoreFlag += STORE_INTERPRETATION;
		}*/
		switch (mStartingCircumstance)
		{
			case SC_SUPERCOMMANDER :
				mEfficiency += dice(2, 4);
				break;
			case SC_EXCELLENT :
				mEfficiency += number(8);
				break;
			case SC_VERY_GOOD :
				mEfficiency += dice(2, 3);
				break;
			case SC_GOOD :
				mEfficiency += number(6);
				break;
			case SC_AVERAGE :
				mEfficiency += dice(2, 2);
				break;
			case SC_POOR :
				mEfficiency += number(4);
				break;
			case SC_BAD :
				mEfficiency += number(3);
				break;
			case SC_VERY_BAD :
				mEfficiency += number(2);
				break;
			case SC_CANNON_FODDER :
			default :
				mEfficiency += 1;
				break;
		}

		mStoreFlag += STORE_EFFICIENCY;
	}
	while (mExp >= mExpLevelTable[mLevel] && mLevel < MAX_LEVEL);

	return true;
}

void
	CAdmiral::give_level( int aLevel )
{
	//if( aLevel <= 0 || mLevel+aLevel > 20 ) return;
	int StartingLevel = mLevel;
	for( int i = StartingLevel+1; i <= StartingLevel + aLevel && i < 21; i++ )
	{
		mFleetCommanding += mPlusFleetCommanding[i];
		if(mRacialAbility == RA_BREEDER_MALE)
		{
			switch(mLevel)
			{
				case 2:
					{
						mFleetCommanding += 1;
						break;
					}
				case 6:
					{
						mFleetCommanding += 1;
						break;
					}
				case 10:
					{
						mFleetCommanding += 1;
						break;
					}
				case 13:
					{
						mFleetCommanding += 1;
						break;
					}
				case 16:
					{
						mFleetCommanding += 1;
						break;
					}
				case 19:
					{
						mFleetCommanding += 1;
						break;
					}
			}
		}
		if(mRacialAbility == RA_MANAGEMENT_PROTOCOL)
		{
			switch(mLevel)
			{
				case 2:
					{
						mFleetCommanding += 1;
						break;
					}
				case 7:
					{
						mFleetCommanding += 1;
						break;
					}
				case 12:
					{
						mFleetCommanding += 1;
						break;
					}
				case 17:
					{
						mFleetCommanding += 1;
						break;
					}
			}
		}
		if(mRacialAbility == RA_ARMADA_SYNERGY_SPECIALIST) {
            mFleetCommanding += 1;
        }
		if(mFleetCommanding > 100)
			mFleetCommanding = 100;

		/*if (mLevel%mSkill[SIEGE_PLANET][SKILL_UP_LEVEL] == 0)
		{
			mSkill[SIEGE_PLANET][LEVEL]++;
		}
		if (mLevel%mSkill[BLOCKADE][SKILL_UP_LEVEL] == 0)
		{
			mSkill[BLOCKADE][LEVEL]++;
		}
		if (mLevel%mSkill[RAID][SKILL_UP_LEVEL] == 0)
		{
			mSkill[RAID][LEVEL]++;
		}
		if (mLevel%mSkill[PRIVATEER][SKILL_UP_LEVEL] == 0)
		{
			mSkill[PRIVATEER][LEVEL]++;
		}
		if (mLevel%mSkill[SIEGE_REPELLING][SKILL_UP_LEVEL] == 0)
		{
			mSkill[SIEGE_REPELLING][LEVEL]++;
		}
		if (mLevel%mSkill[BREAK_BLOCKADE][SKILL_UP_LEVEL] == 0)
		{
			mSkill[BREAK_BLOCKADE][LEVEL]++;
		}
		if (mLevel%mSkill[PREVENT_RAID][SKILL_UP_LEVEL] == 0)
		{
			mSkill[PREVENT_RAID][LEVEL]++;
		}*/
		if (mLevel%mSkill[OFFENSE][SKILL_UP_LEVEL] == 0)
		{
			mSkill[OFFENSE][LEVEL]++;
		}
		if (mLevel%mSkill[DEFENSE][SKILL_UP_LEVEL] == 0)
		{
			mSkill[DEFENSE][LEVEL]++;
		}

		if (mLevel%mSkill[MANEUVER][SKILL_UP_LEVEL] == 0)
		{
			mSkill[MANEUVER][LEVEL]++;
		}
		if (mLevel%mSkill[DETECTION][SKILL_UP_LEVEL] == 0)
		{
			mSkill[DETECTION][LEVEL]++;
		}
		/*if (mLevel%mSkill[INTERPRETATION][SKILL_UP_LEVEL] == 0)
		{
			mSkill[INTERPRETATION][LEVEL]++;
		}*/
		switch (mStartingCircumstance)
		{
			case SC_SUPERCOMMANDER :
				mEfficiency += dice(2, 4);
				break;
			case SC_EXCELLENT :
				mEfficiency += number(8);
				break;
			case SC_VERY_GOOD :
				mEfficiency += dice(2, 3);
				break;
			case SC_GOOD :
				mEfficiency += number(6);
				break;
			case SC_AVERAGE :
				mEfficiency += dice(2, 2);
				break;
			case SC_POOR :
				mEfficiency += number(4);
				break;
			case SC_BAD :
				mEfficiency += number(3);
				break;
			case SC_VERY_BAD :
				mEfficiency += number(2);
				break;
			case SC_CANNON_FODDER :
			default :
				mEfficiency += 1;
				break;
		}

		mExp = mExpLevelTable[mLevel];
		mLevel++;
		if (mLevel - aLevel >= StartingLevel || mLevel >= 20)
		{
			mStoreFlag += STORE_FLEET_COMMANDING;
			/*mStoreFlag += STORE_SIEGE_PLANET;
			mStoreFlag += STORE_BLOCKADE;
			mStoreFlag += STORE_RAID;
			mStoreFlag += STORE_PRIVATEER;
			mStoreFlag += STORE_SIEGE_REPELLING;
			mStoreFlag += STORE_BREAK_BLOCKADE;
			mStoreFlag += STORE_PREVENT_RAID;*/
			mStoreFlag += STORE_OFFENSE;
			mStoreFlag += STORE_DEFENSE;
			mStoreFlag += STORE_MANEUVER;
			mStoreFlag += STORE_DETECTION;
			//mStoreFlag += STORE_INTERPRETATION;
			mStoreFlag += STORE_EFFICIENCY;
			mStoreFlag += STORE_LEVEL;
		}
	}
}

CString &
CAdmiral::query()
{
	static CString
		Query;

	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO admiral "
				"(id, owner, race, type, "
				"name, exp, level, fleet_number, "
				"armada_commanding, fleet_commanding, efficiency, "
				"offense, offense_up_level, "
				"defense, defense_up_level, "
				"maneuver, maneuver_up_level, "
				"detection, detection_up_level, "
				"commonability, raceability) "
				"VALUES (%d, %d, %d, %d, "
				"'%s', %d, %d, %d, "
				"%d, %d, %d, "
				"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
				mID, mOwner, mRace, mStartingCircumstance,
				(char *)add_slashes((char *)mName), mExp, mLevel, mFleetNumber,
				mArmadaCommanding, mFleetCommanding, mEfficiency,
				mSkill[ OFFENSE ][ LEVEL ],
				mSkill[ OFFENSE ][ SKILL_UP_LEVEL ],
				mSkill[ DEFENSE ][ LEVEL ],
				mSkill[ DEFENSE ][ SKILL_UP_LEVEL ],
				/*mSkill[ RAID ][ LEVEL ],
				mSkill[ RAID ][ SKILL_UP_LEVEL ],
				mSkill[ PRIVATEER ][ LEVEL ],
				mSkill[ PRIVATEER ][ SKILL_UP_LEVEL ],
				mSkill[ SIEGE_REPELLING ][ LEVEL ],
				mSkill[ SIEGE_REPELLING ][ SKILL_UP_LEVEL ],
				mSkill[ BREAK_BLOCKADE ][ LEVEL ],
				mSkill[ BREAK_BLOCKADE ][ SKILL_UP_LEVEL ],
				mSkill[ PREVENT_RAID ][ LEVEL ],
				mSkill[ PREVENT_RAID ][ SKILL_UP_LEVEL ],*/
				mSkill[ MANEUVER ][ LEVEL ],
				mSkill[ MANEUVER ][ SKILL_UP_LEVEL ],
				mSkill[ DETECTION ][ LEVEL ],
				mSkill[ DETECTION ][ SKILL_UP_LEVEL ],
				//mSkill[ INTERPRETATION ][ LEVEL ],
				//mSkill[ INTERPRETATION ][ SKILL_UP_LEVEL ],
				mSpecialAbility, mRacialAbility);
			break;
		case QUERY_UPDATE :
			Query.format("UPDATE admiral SET exp = %d, level = %d", mExp, mLevel);

#define STORE(x, y, z) \
		if (mStoreFlag.has(x)) Query.format(y, z)

			STORE(STORE_ID, ", id = %d", mID);
			STORE(STORE_OWNER, ", owner = %d", mOwner);
			STORE(STORE_RACE, ", race = %d", mRace);
			STORE(STORE_NAME, ", name = '%s'", (char *)add_slashes((char *)mName));
			STORE(STORE_TYPE, ", type = %d", mStartingCircumstance);
			STORE(STORE_LEVEL, ", level = %d", mLevel);
			STORE(STORE_FLEET_NUMBER, ", fleet_number = %d", mFleetNumber);

			STORE(STORE_ARMADA_COMMANDING,
				", armada_commanding = %d", mArmadaCommanding);
			STORE(STORE_FLEET_COMMANDING,
				", fleet_commanding = %d", mFleetCommanding);
			STORE(STORE_EFFICIENCY,
				", efficiency = %d", mEfficiency);

			STORE(STORE_OFFENSE,
				", offense = %d",
				mSkill[ OFFENSE ][ LEVEL ]);
			STORE(STORE_OFFENSE_UP_LEVEL,
				", offense_up_level = %d",
				mSkill[ OFFENSE ][ SKILL_UP_LEVEL ]);

			STORE(STORE_DEFENSE,
				", defense = %d",
				mSkill[ DEFENSE ][ LEVEL ]);
			STORE(STORE_DEFENSE_UP_LEVEL,
				", defense_up_level = %d",
				mSkill[ DEFENSE ][ SKILL_UP_LEVEL ]);

			/*STORE(STORE_RAID,
				", raid = %d",
				mSkill[ RAID ][ LEVEL ]);
			STORE(STORE_RAID_UP_LEVEL,
				", raid_up_level = %d",
				mSkill[ RAID ][ SKILL_UP_LEVEL ]);
			STORE(STORE_PRIVATEER,
				", privateer = %d",
				mSkill[ PRIVATEER ][ LEVEL ]);
			STORE(STORE_PRIVATEER_UP_LEVEL,
				", privateer_up_level = %d",
				mSkill[ PRIVATEER ][ SKILL_UP_LEVEL ]);

			STORE(STORE_SIEGE_REPELLING,
				", siege_repelling = %d",
				mSkill[ SIEGE_REPELLING ][ LEVEL ]);
			STORE(STORE_SIEGE_REPELLING_UP_LEVEL,
				", siege_repelling_up_level = %d",
				mSkill[ SIEGE_REPELLING ][ SKILL_UP_LEVEL ]);
			STORE(STORE_BREAK_BLOCKADE,
				", break_blockade = %d",
				mSkill[ BREAK_BLOCKADE ][ LEVEL ]);
			STORE(STORE_BREAK_BLOCKADE_UP_LEVEL,
				", break_blockade_up_level = %d",
				mSkill[ BREAK_BLOCKADE ][ SKILL_UP_LEVEL ]);
			STORE(STORE_PREVENT_RAID,
				", prevent_raid = %d",
				mSkill[ PREVENT_RAID ][ LEVEL ]);
			STORE(STORE_PREVENT_RAID_UP_LEVEL,
				", prevent_raid_up_level = %d",
				mSkill[ PREVENT_RAID ][ SKILL_UP_LEVEL ]);*/

			STORE(STORE_MANEUVER,
				", maneuver = %d",
				mSkill[ MANEUVER ][ LEVEL ]);
			STORE(STORE_MANEUVER_UP_LEVEL,
				", maneuver_up_level = %d",
				mSkill[ MANEUVER ][ SKILL_UP_LEVEL ]);
			STORE(STORE_DETECTION,
				", detection = %d",
				mSkill[ DETECTION ][ LEVEL ]);
			STORE(STORE_DETECTION_UP_LEVEL,
				", detection_up_level = %d",
				mSkill[ DETECTION ][ SKILL_UP_LEVEL ]);
			/*STORE(STORE_INTERPRETATION,
				", interpretation = %d",
				mSkill[ INTERPRETATION ][ LEVEL ]);
			STORE(STORE_INTERPRETATION_UP_LEVEL,
				", interpretation_up_level = %d",
				mSkill[ INTERPRETATION ][ SKILL_UP_LEVEL ]);*/
			Query.format( " WHERE id = %d", mID );


			mStoreFlag.clear();

			break;
		case QUERY_DELETE :
			Query.format( "DELETE FROM admiral WHERE id = %d AND owner = %d",
				mID, mOwner );
			break;
	}

	return Query;
}

bool
	CAdmiral::distribute_exp()
{
	CPlayer *
	Owner = PLAYER_TABLE->get_by_game_id(mOwner);
	if (!Owner) return false;

	CAdmiralList *
	AdmiralList = Owner->get_admiral_list();
	for (int i=0 ; i<AdmiralList->length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)AdmiralList->get(i);
		if (Admiral->get_id() == mID) continue;

		int
			Exp = (mLevel - Admiral->get_level())*50;
		if (Exp > 0) Admiral->gain_exp(Exp);
	}

	CAdmiralList *
	AdmiralPool = Owner->get_admiral_pool();
	for (int i=0 ; i<AdmiralPool->length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)AdmiralPool->get(i);
		if (Admiral->get_id() == mID) continue;

		int
			Exp = (mLevel - Admiral->get_level())*50;
		if (Exp > 0) Admiral->gain_exp(Exp);
	}

	return true;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CAdmiral &aAdmiral)
{
	aStoreCenter.store(aAdmiral);
	return aStoreCenter;
}

void scramble_10( int aSkill[] )
{
	// make aSkill to 0-4 array
	for( int i = 0; i < CAdmiral::MAX_SKILL; i++ )
		aSkill[i] = i;

	// random swapping for 20 times
	for( int i = 0; i < CAdmiral::MAX_SKILL*2; i++ ){
		int j = number(4)-1, k = number(4)-1, t;

		t = aSkill[j]; aSkill[j] = aSkill[k]; aSkill[k] = t;
	}
}

void
	CAdmiral::clear_level_by_effect()
{
	for( int i = 0; i < MAX_SKILL+2; i++ )
		mSkillByEffect[i] = 0;
}

void
	CAdmiral::set_level_by_effect( int aAbility, int aLevel )
{
	if( aAbility < 0 || aAbility >= MAX_SKILL+2 ) return;
	mSkillByEffect[aAbility] += aLevel;
}
