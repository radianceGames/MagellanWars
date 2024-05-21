#if !defined(__ARCHSPACE_EFFECT_H__)
#define __ARCHSPACE_EFFECT_H__

#include "common.h"
#include "store.h"

class CPlayer;

extern TZone gPlayerEffectZone;

class CPlayerEffect : public CStore
{
	public:
		enum EEffectType
		{
			PA_CHANGE_PLANET_RESOURCE,
			PA_SWITCH_PLANET_ORDER,
			PA_SET_FLEET_MISSION,
			PA_LOSE_PLANET,
			PA_DESTROY_ALL_DOCKED_SHIP,
			PA_WIN_AND_GAIN_PLANET,
			PA_LOSE_TECH,
			PA_LOSE_PROJECT,
			PA_PLANET_LOST_BUILDING,
			PA_SKIP_TURN,
			PA_CHANGE_CONTROL_MODEL,
			PA_CHANGE_PRODUCTION,
			PA_CHANGE_ALL_COMMANDER_ABILITY,
			PA_GAIN_TECH,
			PA_GAIN_FLEET,
			PA_GAIN_PROJECT,
			PA_GAIN_SECRET_PROJECT,
			PA_GUARDED_BY_IMPERIAL_FLEET,
			PA_CHANGE_EMPIRE_RELATION,
			PA_COMMANDER_LEVEL_UP,
			PA_GRANT_BOON,
			PA_LOSE_COMMANDER,
			PA_CHANGE_PLANET_CONTROL_MODEL,
			PA_CHANGE_PLANET_POPULATION,
			PA_LOSE_PLANET_GRAVITY_CONTROL,
			PA_GAIN_PLANET_GRAVITY_CONTROL,
			PA_CHANGE_SHIP_ABILITY_ON_PLANET,
			PA_CHANGE_YOUR_FLEET_RETURN_TIME,
			PA_DAMAGE_FLEET,
			PA_CHANGE_HONOR,
			PA_SHOW_PLAYER,
			PA_COUNCIL_DECLARE_TOTAL_WAR,
			PA_WIN_OR_LOSE_PLANET,
			PA_CHANGE_CONCENTRATION_MODE,
			PA_RECRUIT_ENEMY_COMMANDER,
			PA_KILL_COMMANDER_AND_DISBAND_FLEET,
			PA_INVASION_FROM_EMPIRE,
			PA_GAIN_COMMANDER,
			PA_PARALYZE_PLANET,
			PA_PRODUCE_MP_PER_TURN,
			PA_PRODUCE_RP_PER_TURN,
			PA_CONSUME_PP_PER_TURN,
			PA_IMPERIAL_RETRIBUTION,
			PA_ENEMY_MORALE_MODIFIER,
			PA_GAIN_ABILITY,
			PA_LOSE_ABILITY,
			PA_CONSUME_PP_BY_RP_FROM_EFFECT_PER_TURN,
			PA_COMMANDER_SURVIVAL,
			PA_MAX
		};

		enum EApplyMethod
		{
			APPLY_ABSOLUTE = 0,
			APPLY_PROPOTIONAL
		};

		enum {
			ST_SPY,
			ST_EVENT,
			ST_PROJECT,
			ST_FROM_ADMIN
		};

		enum {
			FIELD_ID,
			FIELD_OWNER,
			FIELD_LIFE,
			FIELD_TYPE,
			FIELD_TARGET,
			FIELD_APPLY,
			FIELD_ARG1,
			FIELD_ARG2,
			FIELD_SOURCE_TYPE,
			FIELD_SOURCE
		};

	protected :
		int
			mID,
			mOwner;
		time_t
			mLife,
			mType;
		int
			mTarget;
		int
			mApply;
		int
			mArg1,
			mArg2;
		int
			mSourceType;
		int
			mSource;

		bool
			mSavable;

		static char *mTypeName[];
		static char *mApplyTypeName[];

	public:
		CPlayerEffect();
		CPlayerEffect(MYSQL_ROW aRow);
		CPlayerEffect(CPlayer *aOwner, CPlayerEffect *aEffect);
		virtual ~CPlayerEffect();

		virtual const char *table() { return "player_effect"; }
		virtual CString &query();

		int get_id() { return mID; }
		void set_id(int aID) { mID = aID; }

		int get_owner() { return mOwner; }
		void set_owner(int aOwner) { mOwner = aOwner; }

		time_t get_life() { return mLife; }
		void set_life(time_t aLife) { mLife = aLife; }

		int get_type() { return mType; }
		char *get_type_name();
		void set_type(int aType) { mType = aType; }
		bool set_type( const char *aType );

		int get_target() { return mTarget; }
		void set_target(int aTarget) { mTarget = aTarget; }

		int get_apply() { return mApply; }
		void set_apply(int aApply) { mApply = aApply; }
		bool set_apply(char *aApply);

		int get_argument1() { return mArg1; }
		void set_argument1(int aValue) { mArg1 = aValue; }

		int get_argument2() { return mArg2; }
		void set_argument2(int aValue) { mArg2 = aValue; }

		int get_source_type() { return mSourceType; }
		int get_source() { return mSource; }
		void set_source(int aType, int aSource = 0)
			{ mSourceType = aType; mSource = aSource; }

		bool is_savable() { return mSavable; }
		void set_savable() { mSavable = true; }
		void reset_savable() { mSavable = false; }

		static char *get_type_name(int aType) { return mTypeName[aType]; }

		RECYCLE(gPlayerEffectZone);
};

class CPlayerEffectList: public CSortedList
{
	public:
		CPlayerEffectList();
		virtual ~CPlayerEffectList();

		int add_player_effect(CPlayerEffect *aEffect);
		bool remove_player_effect(CPlayerEffect *aEffect);
		bool remove_player_effect(int aID);

		CPlayerEffect *get_by_type(int aType, int aTarget = 0);
		int get_new_id();
		void update_turn();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;

		virtual const char *debug_info() const { return "player effect list"; }
};


#endif
