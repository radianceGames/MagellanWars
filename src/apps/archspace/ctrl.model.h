#if !defined(__ARCHSPACE_CTRL_MODEL_H__)
#define __ARCHSPACE_CTRL_MODEL_H__

#include <cstring>
#include "common.h"

extern TZone gControlModelZone;

// 아직 제작 중. 창연씨의 ControlModel 작업에 따라 변수 추가 예정
// Player와 Planet의 ControlModel
// Production과 Upkeep 계산에 사용

enum EAbility {
	ABILITY_BEGIN = 1,
	ABILITY_NO_BREATH = 1,
	ABILITY_TERRAFORM_GRAVITY,
	ABILITY_NO_SPY,
	ABILITY_DEAD,
	ABILITY_COMPLETE_DEAD,
	ABILITY_PSI,
	ABILITY_ENHANCED_PSI,
	ABILITY_DUKE,
	ABILITY_MARQUIS,
	ABILITY_EARL,
	ABILITY_VISCOUNT,
	ABILITY_BARON,
	ABILITY_ROGUE_DUKE,
	ABILITY_ROGUE_MARQUIS,
	ABILITY_ROGUE_EARL,
	ABILITY_ROGUE_VISCOUNT,
	ABILITY_ROGUE_BARON,
	ABILITY_GENETIC_ENGINEERING_SPECIALIST,
	ABILITY_FRAGILE_MIND_STRUCTURE,
	ABILITY_GREAT_SPAWNING_POOL,
	ABILITY_FAST_MANEUVER,
	ABILITY_STEALTH,
	ABILITY_SCAVENGER,
	ABILITY_INFORMATION_NETWORK_SPECIALIST,
	ABILITY_EFFICIENT_INVESTMENT,
	ABILITY_DOWNLOADABLE_COMMANDER_EXPERIENCE,
	ABILITY_ASTEROID_MANAGEMENT,
	ABILITY_DIPLOMAT,
	ABILITY_TRAINED_MIND,
	ABILITY_PACIFIST,
	ABILITY_FANATIC_FLEET,
	ABILITY_HIGH_MORALE,
	ABILITY_STEALTH_PIRATE,
	ABILITY_TACTICAL_MASTERY,
	ABILITY_FANATICAL_RECRUITING,
	ABILITY_MILITARISTIC_DOMINANCE,
	ABILITY_ADVANCED_BATTLE_PROCESSING,
	ABILITY_HIVE_SHIP_YARD,
	ABILITY_ENHANCED_PSI_WEAPONRY,
	ABILITY_NATURAL_STEALTH,
	ABILITY_STEALTHED_AMBUSH,
	ABILITY_TEMPORAL_MASTERY,
	ABILITY_CATALYSM,
	ABILITY_MAX
};

extern char *AbilityName[];

inline int
ability_name_to_index( char *aName )
{
	for( int i = ABILITY_BEGIN; i < ABILITY_MAX; i++ ){
		if( strcasecmp( aName, AbilityName[i] ) == 0 ) return i;
	}

	return -1;
}

inline char *
index_to_ability_name(int aAbility)
{
	if (aAbility < ABILITY_BEGIN || aAbility >= ABILITY_MAX) return NULL;

	return AbilityName[aAbility];
}

// CControlModel won't be saved.
// so there is no change.
/**
*/
class CControlModel
{
	public:
		CControlModel();
		CControlModel(CControlModel& aControlModel);
		~CControlModel();

		enum EControlModel {
			CM_ENVIRONMENT = 0,
			CM_GROWTH,
			CM_RESEARCH,
			CM_PRODUCTION,
			CM_MILITARY,
			CM_SPY,
			CM_COMMERCE,
			CM_EFFICIENCY,
			CM_GENIUS,
			CM_DIPLOMACY,
			CM_FACILITY_COST,
			CM_MAX
		};

		void clear();

		int get_environment();
		int get_growth();
		int get_research();
		int get_production();
		int get_military();
		int get_spy();
		int get_commerce();
		int get_efficiency();
		int get_genius();
		int get_diplomacy();
		int get_facility_cost();
		int get_uncapped_military();

		int get_real_environment() { return mEnvironment; }
		int get_real_growth() { return mGrowth; }
		int get_real_research() { return mResearch; }
		int get_real_production() { return mProduction; }
		int get_real_military() { return mMilitary; }
		int get_real_spy() { return mSpy; }
		int get_real_commerce() { return mCommerce; }
		int get_real_efficiency() { return mEfficiency; }
		int get_real_genius() { return mGenius; }
		int get_real_diplomacy() { return mDiplomacy; }
		int get_real_facility_cost() { return mFacilityCost; }

		int get_value(int aEnvironment);

		inline void set_environment(int aValue);
		inline void set_growth(int aValue);
		inline void set_research(int aValue);
		inline void set_production(int aValue);
		inline void set_military(int aValue);
		inline void set_spy(int aValue);
		inline void set_commerce(int aValue);
		inline void set_efficiency(int aValue);
		inline void set_genius(int aValue);
		inline void set_diplomacy(int aValue);
		inline void set_facility_cost(int aValue);

		inline void change_environment(int aValue);
		inline void change_growth(int aValue);
		inline void change_research(int aValue);
		inline void change_production(int aValue);
		inline void change_military(int aValue);
		inline void change_spy(int aValue);
		inline void change_commerce(int aValue);
		inline void change_efficiency(int aValue);
		inline void change_genius(int aValue);
		inline void change_diplomacy(int aValue);
		inline void change_facility_cost(int aValue);

		bool set(char *aType, int aValue);
		bool get(char *aType, int &aValue);
		bool change(char *aType, int aValue);

		CControlModel &operator += (const CControlModel &aControlModel);
		CControlModel &operator -= (const CControlModel &aControlModel);
		CControlModel &operator = (const CControlModel &aControlModel);

		bool is_empty();

		static const char* get_environment_description(int aEnvironment);
		static char *get_cm_description(int aSection);

		char *get_effects_string();
		char *get_effects_string_for_html();

		const char *debug_info();
		const char *html_in_race();

	private:

		int mEnvironment;
		int mGrowth;
		int mResearch;
		int mProduction;
		int mMilitary;
		int mSpy;
		int mCommerce;
		int mEfficiency;
		int mGenius;
		int mDiplomacy;
		int mFacilityCost;

	RECYCLE(gControlModelZone);
};

void CControlModel::set_environment(int aValue) { mEnvironment = aValue; }
void CControlModel::set_growth(int aValue) { mGrowth = aValue; }
void CControlModel::set_research(int aValue) { mResearch = aValue; }
void CControlModel::set_production(int aValue) { mProduction = aValue; }
void CControlModel::set_military(int aValue) { mMilitary = aValue; }
void CControlModel::set_spy(int aValue) { mSpy = aValue; }
void CControlModel::set_commerce(int aValue) { mCommerce = aValue; }
void CControlModel::set_efficiency(int aValue) { mEfficiency = aValue; }
void CControlModel::set_genius(int aValue) { mGenius = aValue; }
void CControlModel::set_diplomacy(int aValue) { mDiplomacy = aValue; }
void CControlModel::set_facility_cost(int aValue) { mFacilityCost = aValue; }

void CControlModel::change_environment(int aValue)
		{ set_environment(mEnvironment+aValue); }
void CControlModel::change_growth(int aValue)
		{ set_growth(mGrowth+aValue); }
void CControlModel::change_research(int aValue)
		{ set_research(mResearch+aValue); }
void CControlModel::change_production(int aValue)
		{ set_production(mProduction+aValue); }
void CControlModel::change_military(int aValue)
		{ set_military(mMilitary+aValue); }
void CControlModel::change_spy(int aValue)
		{ set_spy(mSpy+aValue); }
void CControlModel::change_commerce(int aValue)
		{ set_commerce(mCommerce+aValue); }
void CControlModel::change_efficiency(int aValue)
		{ set_efficiency(mEfficiency+aValue); }
void CControlModel::change_genius(int aValue)
		{ set_genius(mGenius+aValue); }
void CControlModel::change_diplomacy(int aValue)
		{ set_diplomacy(mDiplomacy+aValue); }
void CControlModel::change_facility_cost(int aValue)
		{ set_facility_cost(mFacilityCost+aValue); }

#endif
