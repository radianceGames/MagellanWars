#include "common.h"
#include "util.h"
#include "effect.h"
#include <cstdlib>
#include "player.h"
#include "archspace.h"

TZone gPlayerEffectZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPlayerEffect),
	0,
	0,
	NULL,
	"Zone CPlayerEffect"
};


char * CPlayerEffect::mTypeName[] =
{
	"Change Planet Resource",
	"Switch Planet Order",
	"Set Fleet Mission",
	"Lose Planet",
	"Destroy All Docked Ship",
	"Win and Gain Planet",
	"Lose Tech",
	"Lose Project",
	"Planet Lost Building",
	"Skip Turn",
	"Change Control Model",
	"Change Production",
	"Change All Commander Ability",
	"Gain Tech",
	"Gain Fleet",
	"Gain Project",
	"Gain Secret Project",
	"Guarded by Imperial Fleet",
	"Change Empire Relation",
	"Commander Level Up",
	"Grant Boon",
	"Lose Commander",
	"Change Planet Control Model",
	"Change Planet Population",
	"Lose Planet Gravity Control",
	"Gain Planet Gravity Control",
	"Change Ship Ability on Planet",
	"Fleet Return Time",
	"Damage Fleet",
	"Change Honor",
	"Show Player",
	"Council Declare Total War",
	"Win or Lose Planet",
	"Change Concentration Mode",
	"Recruit Enemy Commander",
	"Kill Commander and Disband Fleet",
	"Invasion from Empire",
	"Gain Commander",
	"Paralyze Planet",
	"Produce MP Per Turn",
	"Produce RP Per Turn",
	"Consume PP Per Turn",
	"Imperial Retribution",
	"Enemy Panic Modifier",
	"Gain Ability",
	"Lose Ability",
    "Consume PP by RP From Effect Per Turn",
	"Commander Survival",
	""
};

char *CPlayerEffect::mApplyTypeName[] = {
	"Absolute",
	"Proportional",
	""
};

CPlayerEffect::CPlayerEffect()
{
	mID = 0;
	mLife = -1;
	mType = -1;
	mTarget = 0;
	mApply = -1;
	mArg1 = 0;
	mArg2 = 0;
	mSourceType = mSource = 0;
	reset_savable();
}

CPlayerEffect::CPlayerEffect(MYSQL_ROW aRow)
{
	mID = atoi(aRow[FIELD_ID]);
	mOwner = atoi(aRow[FIELD_OWNER]);
	mLife = atoi(aRow[FIELD_LIFE]);
	mType = atoi(aRow[FIELD_TYPE]);
	mTarget = atoi(aRow[FIELD_TARGET]);
	mApply = atoi(aRow[FIELD_APPLY]);
	mArg1 = atoi(aRow[FIELD_ARG1]);
	mArg2 = atoi(aRow[FIELD_ARG2]);
	mSourceType = atoi(aRow[FIELD_SOURCE_TYPE]);
	mSource = atoi(aRow[FIELD_SOURCE]);

	set_savable();
}

CPlayerEffect::CPlayerEffect(CPlayer *aOwner, CPlayerEffect *aEffect)
{
	CPlayerEffectList *
		EffectList = aOwner->get_effect_list();

	mID = EffectList->get_new_id();
	mOwner = aOwner->get_game_id();
	mLife = aEffect->get_life();
	mType = aEffect->get_type();
	mTarget = aEffect->get_target();
	mApply = aEffect->get_apply();
	mArg1 = aEffect->get_argument1();
	mArg2 = aEffect->get_argument2();
	mSourceType = aEffect->get_source_type();
	mSource = aEffect->get_source();
	reset_savable();
}

CPlayerEffect::~CPlayerEffect()
{
}

bool
CPlayerEffect::set_type( const char *aType )
{
	for( int i = 0; i < PA_MAX; i++ ){
		if( strcasecmp( aType, mTypeName[i] ) == 0 ){
			mType = i;
			return true;
		}
	}

	return false;
}

bool
CPlayerEffect::set_apply(char *aApply)
{
	if (!aApply) return false;

	if (!strcmp(aApply, mApplyTypeName[APPLY_ABSOLUTE]))
	{
		mApply = APPLY_ABSOLUTE;
		return true;
	}
	else if (!strcmp(aApply, mApplyTypeName[APPLY_PROPOTIONAL]))
	{
		mApply = APPLY_PROPOTIONAL;
		return true;
	}

	return false;
}

CString &
CPlayerEffect::query()
{
	static CString
		Query;
	Query.clear();

	switch( type() ){
		case QUERY_INSERT :
			Query.format( "INSERT INTO player_effect ( id, owner, life, type, target, apply, arg1, arg2, source_type, source ) VALUES ( %d, %d, %d, %d, %d, %d, %d, %d, %d, %d )",
				mID, mOwner, mLife, mType, mTarget, mApply,
				mArg1, mArg2, mSourceType, mSource );
			break;
		case QUERY_UPDATE :
			break;
		case QUERY_DELETE :
			Query.format( "DELETE FROM player_effect WHERE owner = %d AND id = %d", mOwner, mID );
			break;
	}

	mStoreFlag.clear();

	return Query;
}

CPlayerEffectList::CPlayerEffectList()
{
}

CPlayerEffectList::~CPlayerEffectList()
{
	remove_all();
}

bool
CPlayerEffectList::free_item(TSomething aItem)
{
	CPlayerEffect *
		Effect = (CPlayerEffect *)aItem;
	if (!Effect) return false;

	delete Effect;

	return true;
}

int
CPlayerEffectList::compare(TSomething aItem1, TSomething aItem2) const
{
	CPlayerEffect
		*PlayerEffect1 = (CPlayerEffect *)aItem1,
		*PlayerEffect2 = (CPlayerEffect *)aItem2;

	if (PlayerEffect1->get_id() > PlayerEffect2->get_id()) return 1;
	if (PlayerEffect1->get_id() < PlayerEffect2->get_id()) return -1;
	return 0;
}

int
CPlayerEffectList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPlayerEffect
		*PlayerEffect = (CPlayerEffect *)aItem;

	if (PlayerEffect->get_id() > (int)aKey) return 1;
	if (PlayerEffect->get_id() < (int)aKey) return -1;
	return 0;
}

int
CPlayerEffectList::add_player_effect(CPlayerEffect *aEffect)
{
	if (!aEffect) return 0;

	if (find_sorted_key((TConstSomething)aEffect->get_id()) >= 0)
		return 0;
	insert_sorted(aEffect);

	return aEffect->get_id();
}

bool
CPlayerEffectList::remove_player_effect(CPlayerEffect *aEffect)
{
	if (!aEffect) return false;

	int
		Index = find_sorted_key((void *)(aEffect->get_id()));
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

bool
CPlayerEffectList::remove_player_effect(int aID)
{
	int
		Index = find_sorted_key((void *)aID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CPlayerEffect *
CPlayerEffectList::get_by_type(int aType, int aTarget)
{
	for (int i=0 ; i<length() ; i++)
	{
		CPlayerEffect *
			Effect = (CPlayerEffect *)get(i);

		if (Effect->get_type() == aType)
		{
			if (aTarget == 0) return Effect;
			if (Effect->get_target() == aTarget) return Effect;
		}
	}

	return NULL;
}

int
CPlayerEffectList::get_new_id()
{
	int
		ID = 1;

	while(find_sorted_key((void *)ID) >= 0) ID++;

	return ID;
}

void
CPlayerEffectList::update_turn()
{
	int
		StartIndex = length() - 1;
	time_t
		Now = CGame::get_game_time();

	for (int i=StartIndex ; i>=0 ; i--)
	{
		CPlayerEffect *
			Effect = (CPlayerEffect *)get(i);
	 	if (Effect->get_type() == CPlayerEffect::PA_IMPERIAL_RETRIBUTION)
	 	{
	 	    SLOG("CPlayerEffectList::update_turn() -- PA_IMPERIAL_RETRIBUTION (skipping)");
            continue;
        }
		if (Effect->get_life() == 0 || Effect->get_life() < Now)
		{
			if (Effect->is_savable())
			{
				Effect->type(QUERY_DELETE);
				STORE_CENTER->store(*Effect);
			}
			remove_player_effect(Effect->get_id());
		}
	}
}
