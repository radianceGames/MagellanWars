#include "common.h"
#include "util.h"
#include "component.h"
#include "define.h"
#include "race.h"
#include <libintl.h>
#include <cstdlib>
#include "archspace.h"
#include "game.h"

TZone gArmorZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CArmor),
	0,
	0,
	NULL,
	"Zone CArmor"
};

TZone gEngineZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CEngine),
	0,
	0,
	NULL,
	"Zone CEngine"
};


TZone gDeviceZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDevice),
	0,
	0,
	NULL,
	"Zone CDevice"
};

TZone gComputerZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CComputer),
	0,
	0,
	NULL,
	"Zone CComputer"
};

TZone gShieldZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CShield),
	0,
	0,
	NULL,
	"Zone CShield"
};

TZone gWeaponZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CWeapon),
	0,
	0,
	NULL,
	"Zone CWeapon"
};



char *CFleetEffect::mEffectName[] = {
	// fleet effect
	// 0 - 9
	"AR",
	"Weapon AR",
	"Computer",
	"DR",
	"Armor DR",
	"Mobility",
	"Speed",
	"HP",
	"Detection Range",
	"Morale",
	// 10 - 19
	"Complete Cloaking",
	"Weak Cloaking",
	"Complete Cloaking Detection",
	"Weak Cloaking Detection",
	"Shield Solidity",
	"Shield Strength",
	"Impenetrable Shield",
	"Impenetrable Armor",
	"Shield Recharge Rate",
	"Beam Damage",
	// 20 - 29
	"Beam AR",
	"DR against Beam",
	"Beam Defense",
	"Missile Damage",
	"Missile AR",
	"DR against Missile",
	"Missile Defense",
	"Projectile Damage",
	"Projectile AR",
	"DR against Projectile",
	// 30 - 39
	"Projectile Defense",
	"PSI Attack",
	"PSI Defense",
	"Genetic Defense",
	"Chain Reaction",
	"Repair",
	"Repair Speed",
	"Non-Repairable",
	"Cooling Time",
	"Beam Cooling Time",
	// 40 - 49
	"Missile Cooling Time",
	"Projectile Cooling Time",
	"Stealth",
	"Panic Modifier",
	"Berserk Modifier",
	"Efficiency",
	"Trained",
	"Damage",
	"PSI Damage",
	"Psionic Wave Damage",
	// 50 - 59
	"Critical Hit",
	"Misinterpret",
	"Commander Survival",
	"Never Berserk",
	"Never Retreat Rout",
	"PSI Defense Panalty",
	"PSI Neutralization Field",
	"Space Mining",
	"Synergy",
	"Shield Integrity",
	// 60 - 67
	"Fast Retreat",
	"Front Armor",
	"Side Armor",
	"Rear Armor",
	"Nul-S-Distortion",
	"Nul-S-Pierce",
	"Nul-A-Pierce",
	"Danger Rating",
	// weapon effect
	"Shield Piercing",
	"Armor Piercing",
	"Additional Damage to Bio Armor",
	"Shield Distortion",
	"Shield Overheat",
	"Corrosivity",
	"PSI",
	"PSI Empower",
	"Death",
	"Complete Armor Piercing",
	"Complete Shield Piercing",
	"Complete Shield Distortion",
	""
};

char *CFleetEffect::mTargetName[] = {
	"Local Effect",
	"Ally Fleet",
	"Enemy Fleet",
	"All",
	""
};

char *CFleetEffect::mTypeName[] = {
	"Absolute",
	"Proportional",
	""
};

char *CComponent::mCategoryName[] = {
	"ARMOR",	//armor
	"COMP",		//computer
	"SHLD",		//shield
	"ENGN",		//engine
	"DEV",		//device
	"WPN"		//weapon
};

char *CComponent::mCategoryNameNormal[] = {
	"Armor",
	"Computer",
	"Shield",
	"Engine",
	"Device",
	"Weapon"
};

char *CComponent::mAttributeName[] = {
	"",
	"PSI Race Only",
	"Bio Armor Only",
	"Non-Bio Armor Only",
	"Reactive Armor Only",
	"Normal Armor Only",
	"Hybrid Armor Only",
	"Customizable Armor Only",
	""
};

CFleetEffect::CFleetEffect()
{
	mType = mApplyType = mPeriod = mAmount = mTarget = mRange = mCharge = 0;
}

CFleetEffect::CFleetEffect( int aType, int aAmount, int aApply )
{
	mType = aType;
	mApplyType = aApply;
	mAmount = aAmount;

	mPeriod = mTarget = mRange = mCharge = 0;
}

CFleetEffect::~CFleetEffect()
{
}

void
CFleetEffect::set_type( char *aTypeName )
{
	int
		i;
	for( i = 0; mEffectName[i][0] && strcasecmp( mEffectName[i], aTypeName ); i++ );

	if( i == FE_MAX )
		SLOG( "Wrong Fleet Effect Name : %s", aTypeName );
	else
		mType = i;
}

void
CFleetEffect::set_apply_type( char *aTypeName )
{
	int
		i;
	for( i = 0; mTypeName[i][0] && strcasecmp( mTypeName[i], aTypeName ); i++ );

	if( i == AT_MAX )
		SLOG( "Wrong Apply Time Name : %s", aTypeName );
	else
		mApplyType = i;
}

void
CFleetEffect::set_target( char *aTargetName )
{
	int
		i;
	for( i = 0; mTargetName[i][0] && strcasecmp( mTargetName[i], aTargetName ); i++ );

	if( i == TARGET_MAX )
		SLOG( "Wrong Target Name : %s", aTargetName );
	else
		mTarget = i;
}

const char *
CFleetEffect::print_html()
{
	static
		CString Buf;

	Buf.clear();

	if( mType < 0 || mType >= FE_MAX )
		Buf.format( "ILLEGAL EFFECT[%d] ", mType );
	else
		Buf.format( "%s", mEffectName[mType] );

	if( mAmount < 0 )
		Buf.format( " %d", mAmount );
	else if( mAmount > 0 )
		Buf.format( " +%d", mAmount );

	if( mApplyType == AT_PROPORTIONAL )
		Buf.format( "%%" );

	if( mPeriod != 0 )
		Buf.format( " for each %d turn", mPeriod );

	if( mTarget != LOCAL_EFFECT )
		Buf.format( " to %s in range of %d", mTargetName[mTarget], mRange );

	Buf += "<BR>\n";

	return (char*)Buf;
}

CFleetEffectListStatic::CFleetEffectListStatic()
{
}

CFleetEffectListStatic::~CFleetEffectListStatic()
{
}

bool
CFleetEffectListStatic::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

CFleetEffectListStatic &
CFleetEffectListStatic::operator=(CFleetEffectListStatic &aList)
{
	remove_all();

	for( int i = 0; i < aList.length(); i++ )
		push( aList.get(i) );

	return *this;
}

CFleetEffectListDynamic::CFleetEffectListDynamic()
{
}

CFleetEffectListDynamic::~CFleetEffectListDynamic()
{
	remove_all();
}

bool
CFleetEffectListDynamic::free_item(TSomething aItem)
{
	CFleetEffect
		*Effect = (CFleetEffect*)aItem;

	if( Effect == NULL ) return false;

	delete Effect;

	return true;
}

CFleetEffectListDynamic &
CFleetEffectListDynamic::operator+=(CFleetEffectListDynamic &aList)
{
	for( int i = 0; i < aList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)aList.get(i);
		CFleetEffect
			*NewEffect = new CFleetEffect();

		*NewEffect = *Effect;
		push( NewEffect );
	}

	return *this;
}

CFleetEffectListDynamic &
CFleetEffectListDynamic::operator+=(CFleetEffectListStatic &aList)
{
	for( int i = 0; i < aList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)aList.get(i);
		CFleetEffect
			*NewEffect = new CFleetEffect();

		*NewEffect = *Effect;
		push( NewEffect );
	}

	return *this;
}

CComponent::CComponent()
{
	mID = 0;
	mLevel = 0;
}

char *
CComponent::get_category_name_normal()
{
	return mCategoryNameNormal[mCategory];
}

char *
CComponent::get_category_name_normal(int aCategory)
{
	return mCategoryNameNormal[aCategory];
}

void
CComponent::set_category( char *aCategoryName )
{
	if (!aCategoryName) return;

	for( int i = 0; i< CC_MAX; i++){
		if( strcmp( mCategoryName[i], aCategoryName ) == 0 )
			mCategory = i;
	}
}

const char *
CComponent::get_category_name( int aCategory )
{
	return mCategoryName[ aCategory ];
}

const char *
CComponent::get_category_name()
{
	return mCategoryName[ mCategory ];
}

const char *
CComponent::html_print_row()
{
	static CString
		buf;

	buf.clear();

	buf.format( "Name : %s(%d)", get_name(), get_id() );
	buf += "<BR>\n";
	buf.format( "Category : %d", get_category() );
	buf += "<BR>\n";
	buf.format( "Description : %s", get_description() );
	buf += "<BR>\n";

	return buf;
}

int
CComponent::effect_amount( int aEffect )
{
	for( int i = 0; i < mEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mEffectList.get(i);

		if( Effect->get_type() == aEffect )
			return Effect->get_amount();
	}
	return 0;
}

bool
CComponent::has_effect( int aEffect )
{
	for( int i = 0; i < mEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mEffectList.get(i);

		if( Effect->get_type() == aEffect )
			return true;
	}
	return false;
}

void
CComponent::set_attribute( char *aAttrStr )
{
	for( int i = CA_BEGIN; i < CA_MAX; i++ ){
		if( strcasecmp( aAttrStr, mAttributeName[i] ) == 0 ){
			set_attribute( i );
			return;
		}
	}
	SLOG( "Wrong Component Attribute : %s", aAttrStr );
}

char *CWeapon::mWeaponTypeName[] = {
	"BM",	//beam
	"MSL",	//missile
	"PRJ",	//projectile
	"FGT"	//fighter
};

char *CWeapon::mWeaponTypeNameNormal[] = {
	"Beam",
	"Missile",
	"Projectile",
	"Fighter"
};

CWeapon::CWeapon()
{
	mCategory = 5;	//CC_WEAPON;
};

char *
CWeapon::get_weapon_type_name_normal()
{
	return mWeaponTypeNameNormal[mWeaponType];
}

void
CWeapon::set_weapon_type( char *aWeaponType )
{
	if (!aWeaponType) return;

	for( int i =0; i< WT_MAX; i++){
		if( strcmp( mWeaponTypeName[i], aWeaponType ) == 0 )
			mWeaponType = i;
	}
}

const char *
CWeapon::html_print_row()
{
	static CString
		buf;
	buf.clear();

	buf += CComponent::html_print_row();
	buf += "<BR>\n";

	buf.format("%s : %s<BR>\n",
				GETTEXT("Weapon Type"),
				mWeaponTypeName[ get_weapon_type() ]);
	buf.format("%s : %d<BR>\n",
				GETTEXT("Attacking Rate"),
				get_attacking_rate());
	buf.format("%s : %d<BR>\n",
				GETTEXT("Damage Roll"),
				get_damage_roll());
	buf.format("%s : %d<BR>\n",
				GETTEXT("Damage Dice"),
				get_damage_dice());
	buf.format("%s : %d<BR>\n",
				GETTEXT("Space"),
				get_space());
	buf.format("%s : %d<BR>\n",
				GETTEXT("Cooling Time"),
				get_cooling_time());
	buf.format("%s : %d<BR>\n",
				GETTEXT("Range"),
				get_range());
	buf.format("%s : %d<BR>\n",
				GETTEXT("Angle of Fire"),
				get_angle_of_fire());
	buf.format("%s : %d<BR>\n",
				GETTEXT("Speed"),
				get_speed());

	return buf;
}

CArmor::CArmor()
{
	mCategory = CC_ARMOR;
}

char *CArmor::mArmorTypeName[] = {
	"NORM",	//normal
	"BIO",	//bio
	"REACT",	//reactive
	"HYBD", //hybrid
	"CUSTOM" //customizable
};

char *CArmor::mArmorTypeNameNormal[] = {
	"Normal",
	"Bio",
	"React",
	"Hybrid",
	"Customizable"
};

char *
CArmor::get_armor_type_name_normal()
{
	return mArmorTypeNameNormal[mArmorType];
}

void
CArmor::set_armor_type( char *aArmorType )
{
	if (!aArmorType) return;

	for( int i =0; i< AT_MAX; i++){
		if( strcmp( mArmorTypeName[i], aArmorType ) == 0 )
			mArmorType = i;
	}
}

const char *
CArmor::html_print_row()
{
	static CString
		buf;

	buf.clear();
	buf += CComponent::html_print_row();

	buf.format( "Armor Type : %s<BR>\n", mArmorTypeName[ get_armor_type() ] );
	buf.format( "Defense Rate : %d<BR>\n", get_defense_rate() );
	buf.format( "HP Multiplier : %.2f<BR>\n", get_hp_multiplier() );

	return buf;
}

CDevice::CDevice()
{
	mCategory = CC_DEVICE;
}

const char *
CDevice::html_print_row()
{
	static CString
		buf;

	buf.clear();
	buf += CComponent::html_print_row();

	buf.format( "Min Class : %d<BR>\n", get_min_class() );
	buf.format( "Max Class : %d<BR>\n", get_max_class() );

	return buf;
}

CEngine::CEngine()
{
	mCategory = CC_ENGINE;
}

const char *
CEngine::html_print_row()
{
	static CString
		buf;

	buf.clear();
	buf += CComponent::html_print_row();

	buf.format( "Battle Speed : %d", get_battle_speed( 0 ) );
	for(int i=1 ; i<10 ; i++) {
		buf.format( "/%d", get_battle_speed( i ) );
	}
	buf += "<BR>\n";
	buf.format( "Battle Mobility : %d", get_battle_mobility( 0 ) );
	for(int i=1 ; i<10 ; i++) {
		buf.format( "/%d", get_battle_mobility( i ) );
	}
	buf += "<BR>\n";
	buf.format( "Cruise Speed : %d", get_cruise_speed() );

	return buf;
}

CComputer::CComputer()
{
	mCategory = CC_COMPUTER;
}

const char *
CComputer::html_print_row()
{
	static CString
		buf;

	buf.clear();
	buf += CComponent::html_print_row();

	buf.format( "Attacking Rate : %d<BR>\n", get_attacking_rate() );
	buf.format( "Defense Rate : %d<BR>\n", get_defense_rate() );

	return buf;
}

CShield::CShield()
{
	mCategory = CC_SHIELD;
}

const char *
CShield::html_print_row()
{
	static CString
		buf;

	buf.clear();
	buf += CComponent::html_print_row();

	buf.format( "Deflection Solidity : %d<BR>\n", get_deflection_solidity() );
	buf.format( "Strength : %d", get_strength( 0 ) );
	for (int i=1 ; i<10 ; i++) {
		buf.format( "/%d", get_strength(i) );
	}
	buf += "<BR>\n";
	buf.format( "Recharge Rate : %d", get_recharge_rate( 0 ) );
	for (int i=1 ; i<10 ; i++) {
		buf.format( "/%d", get_recharge_rate(i) );
	}
	buf += "<BR>\n";

	return buf;
}

CComponentList::CComponentList()
{
}

CComponentList::~CComponentList()
{
	remove_all();
}

bool
CComponentList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CComponentList::compare(TSomething aItem1, TSomething aItem2) const
{
	CComponent
		*component1 = (CComponent*)aItem1,
		*component2 = (CComponent*)aItem2;

	if (component1->get_id() > component2->get_id()) return 1;
	if (component1->get_id() < component2->get_id()) return -1;
	return 0;
}

int
CComponentList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CComponent
		*component = (CComponent*)aItem;

	if (component->get_id() > (int)aKey) return 1;
	if (component->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CComponentList::remove_component(int aComponentID)
{
	int
		index;

	index = find_sorted_key( (void*)aComponentID );
	if (index < 0) return false;

	return CSortedList::remove(index);
}

int
CComponentList::add_component(CComponent *aComponent)
{
	if (!aComponent) return -1;

	if (find_sorted_key((TConstSomething)aComponent->get_id()) >= 0)
		return -1;
	insert_sorted(aComponent);

	return aComponent->get_id();
}

CComponent*
CComponentList::get_by_id(int aComponentID)
{
	if (!length()) return NULL;

	int
		index;

	index = find_sorted_key( (void*)aComponentID );

	if (index < 0) return NULL;

	return (CComponent*)get(index);
}

/* bug! never use
CComponentList*
CComponentList::get_by_category( int aCategory )
{
	CComponentList
		*tmpComponentList;
	tmpComponentList = new CComponentList;

	for(int i=0 ; i<length() ; i++)
	{
		CComponent
			*tmpComponent = (CComponent*)get(i);
		if( tmpComponent->get_category() == aCategory ) {
			tmpComponentList->add_component( tmpComponent );
		}
	}

	return tmpComponentList;
}
*/

CComponent*
CComponentList::get_best_component( int aCategory )
{
	CComponent
		*bestComponent;
	bestComponent = NULL;

	//system_log( "CComponentList::get_best_component() length %d", length() );
	for(int i=0; i<length(); i++)
	{
		CComponent *
			tmpComponent = (CComponent*)get(i);

		if( tmpComponent->get_category() == aCategory )
		{
			if( bestComponent )
			{
				if( bestComponent->get_level() < tmpComponent->get_level() )
				{
					bestComponent = tmpComponent;
				}
			} else
			{
				bestComponent = tmpComponent;
			}
		}
	}

	return bestComponent;
}

char *
CComponentList::armor_list_html()
{
	static CString
		ComponentList;
	ComponentList = "<SELECT NAME=\"ARMOR\">\n";

	for(int i=0 ; i<length() ; i++)
	{
		CComponent *
			Component = (CComponent *)get(i);
		if (Component->get_category() != CComponent::CC_ARMOR) continue;
		ComponentList.format("<OPTION VALUE=%d>%s</OPTION>\n",
				Component->get_id(), Component->get_name());
	}

	return (char *)ComponentList;
}

char *
CComponentList::weapon_list_html(int aNumber, int aSpacePerSlot)
{
	static CString
		WeaponList;
	WeaponList.clear();

	WeaponList.format("<SELECT NAME=\"WEAPON%d\">\n", aNumber);

	for(int i=0 ; i<length() ; i++)
	{
		CComponent *
			Component = (CComponent *)get(i);
		if (Component->get_category() != CComponent::CC_WEAPON) continue;
		CWeapon *
			Weapon = (CWeapon *)Component;
		WeaponList.format("<OPTION VALUE=%d>%s x %d</OPTION>\n",
				Weapon->get_id(), Weapon->get_name(),
				aSpacePerSlot/Weapon->get_space());
	}

	return (char *)WeaponList;
}

int
CComponentList::get_weapon_level()
{
	int Level = 0;

	for(int i=0 ; i<length() ; i++)
	{
		CComponent *
			Component = (CComponent *)get(i);
		if (Component->get_category() != CComponent::CC_WEAPON) continue;
		CWeapon * Weapon = (CWeapon *)Component;
		if (Weapon->get_level() > Level) Level = Weapon->get_level();
	}

	return Level;
}

char *
CComponentList::device_list_html(int aNumber)
{
	static CString
		DeviceList;
	DeviceList.clear();

	DeviceList.format("<SELECT NAME=\"DEVICE%d\">\n", aNumber);
	DeviceList.format("<OPTION VALUE=0>%s</OPTION>\n", GETTEXT("None"));

	for(int i=0 ; i<length() ; i++)
	{
		CComponent *
			Component = (CDevice *)get(i);
		if (Component->get_category() != CComponent::CC_DEVICE) continue;
		DeviceList.format("<OPTION VALUE=%d>%s</OPTION>\n",
				Component->get_id(), Component->get_name());
	}

	return (char *)DeviceList;
}

CComponentTable::CComponentTable()
{
}

CComponentTable::~CComponentTable()
{
	remove_all();
}

bool
CComponentTable::free_item(TSomething aItem)
{
	CComponent
		*component = (CComponent*)aItem;

	if (!component) return false;

	delete component;

	return true;
}

int
CComponentTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CComponent
		*component1 = (CComponent*)aItem1,
		*component2 = (CComponent*)aItem2;

	if (component1->get_id() > component2->get_id()) return 1;
	if (component1->get_id() < component2->get_id()) return -1;
	return 0;
}

int
CComponentTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CComponent
		*component = (CComponent*)aItem;

	if (component->get_id() > (int)aKey) return 1;
	if (component->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CComponentTable::remove_component(int aComponentID)
{
	int
		index;
	index = find_sorted_key((void*)aComponentID);
	if (index < 0) return false;

	return CSortedList::remove(index);
}

int
CComponentTable::add_component(CComponent* aComponent)
{
	if (!aComponent) return -1;

	if(find_sorted_key((TConstSomething)aComponent->get_id()) >= 0)
		return -1;
	insert_sorted(aComponent);

	return aComponent->get_id();
}

CComponent*
CComponentTable::get_by_id(int aComponentID)
{
	if (!length()) return NULL;

	int
		index;

	index = find_sorted_key((void*)aComponentID);

	if (index < 0) return NULL;

	return (CComponent*)get(index);
}

/* end of file component.cc */
