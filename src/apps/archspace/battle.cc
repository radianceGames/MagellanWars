#include <libintl.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include "common.h"
#include "util.h"
#include "battle.h"
#include "fleet.h"
#include "council.h"
#include "player.h"
#include "archspace.h"
#include "game.h"
#include "race.h"


TZone gBattleZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CBattle),
	0,
	0,
	NULL,
	"Zone CBattle"
};

TZone gBattleRecordZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CBattleRecord),
	0,
	0,
	NULL,
	"Zone CBattleRecord"
};

TZone gBattleFleetZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CBattleFleet),
	0,
	0,
	NULL,
	"Zone CBattleFleet"
};

TZone gTargetFleetZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CTargetFleet),
	0,
	0,
	NULL,
	"Zone CTargetFleet"
};

TZone gFormationPointZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CFormationPoint),
	0,
	0,
	NULL,
	"Zone CFormationPoint"
};

char *
	CBattleFleet::mOrderName[] =
{
	"Normal",
	"Formation",
	"Penetrate",
	"Flank",
	"Reserve",
	"Free",
	"Stand Ground",
	"Assault",
	"On Penetrate",
	"On Flank",
	"On Assault"
};

static CVector BattleFieldCenter(5000, 5000);

bool CBattle::mBattleSimulationEnabled = false;

// Returns a CVector with 0 length,
// pointing towards 0 degrees
CVector::CVector() {
    mX = mY = 0;
    mDirection = 0.0;
}

// Returns a CVector with aX x-length, aY y-length
// pointing towards 0 degrees
CVector::CVector(int aX, int aY) {
    mX = aX;
    mY = aY;
    mDirection = 0.0;
}

// CVector deconstructer
CVector::~CVector() {
}

// Sets the x-length to aX, y-length to aY,
// and direction to aDirection
void CVector::set_vector(int aX, int aY, double aDirection) {
    mX = aX;
    mY = aY;
    mDirection = 0;
    turn(aDirection);
}

// Increases a CVector's direction by aDegree (mod 360)
// In other words turn it through aDegree degrees
void CVector::turn(double aDegree) {
    mDirection += aDegree;

    while (mDirection < 0) mDirection += 360.;
    while (mDirection > 360) mDirection -= 360;
}

// Turns the CVector in the direction of aVector, or by aMaxTurn
// whichever involves less turning
void CVector::turn_to(CVector *aVector, double aMaxTurn) {
    double DeltaDirection = (double) delta_direction(aVector);
    if (DeltaDirection > 180) DeltaDirection -= 360;

    if (DeltaDirection > aMaxTurn) DeltaDirection = aMaxTurn;
    if (DeltaDirection < -aMaxTurn) DeltaDirection = -aMaxTurn;

    turn(DeltaDirection);
}

// Returns true if CVector is pointing in the same direction
// as aVector, within +-2 degrees
bool CVector::is_heading_to(CVector *aVector) {
    int Delta = delta_direction(aVector);
    if( Delta < 2 || Delta > 358 ) return true;
    return false;
}

// Returns the difference in direction of
// the CVector and a comparison vector aVector
int CVector::delta_direction(CVector *aVector) {
    int dX = aVector->get_x() - get_x();
    int dY = aVector->get_y() - get_y();

    // Same vectors
    if(dX == 0 && dY == 0) return 0;

    int Direction;
    if (dX == 0){
        if (dY > 0) Direction = 90;
        else Direction = 270;
    } else if (dY == 0) {
        if (dX > 0) Direction = 0;
        else Direction = 180;
    } else {
        double Distance = hypot((double) dX, (double) dY);

        Direction = as_acos(dX/Distance);
        if(dY < 0) Direction = 360-Direction;
    }

    int Delta = Direction - (int) mDirection;
    while (Delta < 0) Delta += 360;
    while (Delta > 360) Delta -= 360;

    return Delta;
}

// Returns true if aVector's tip lies within aDistance of the
// CVector's tip AND within +- aDirection degrees of the CVector's direction
bool CVector::is_in_range(CVector *aVector, int aDistance, int aDirection) {
    if (distance(aVector) > aDistance) return false;

    int DeltaDirection = delta_direction(aVector);

    if (DeltaDirection < aDirection || DeltaDirection > 360 - aDirection) return true;
    return false;
}

// Moves the CVector by aDeltaX in the x-direction and aDeltaY in the y-direction
void CVector::move(int aDeltaX, int aDeltaY) {
    mX += aDeltaX;
    mY += aDeltaY;

    // Ensure that the vector remains within the bounds of the gridlines
    if (mX < 0) mX = 0;
    if (mX > 10000) mX = 10000;
    if (mY < 0) mY = 0;
    if (mY > 10000) mY = 10000;
}

// Moves the vector in its mDirection, by aLength units
void CVector::move(int aLength) {
    double dX = aLength*as_cos((int) mDirection);
    double dY = aLength*as_sin((int) mDirection);

    move((int)dX, (int)dY);
}

// Returns the distance between the tip of the CVector
// and that of aVector
int CVector::distance(CVector *aVector) {
    int dX = get_x() - aVector->get_x();
    int dY = get_y() - aVector->get_y();

    return (int)hypot((double)dX, (double)dY);
}

// Returns true if the CVector lies on any border of the battle grid
bool CVector::touch_border() {
    if (mX <= 0 || mX >= 10000 || mY <= 0 || mY >= 10000) return true;
    return false;
}

// Rotates the vector about its position by aDirection degrees
void CVector::rotate(int aDirection) {
    double NewX, NewY;

    // Rotation transformation matrix
    NewX = as_cos(aDirection) * mX - as_sin(aDirection) * mY;
    NewY = as_sin(aDirection) * mX + as_cos(aDirection) * mY;

    mX = (int) NewX;
    mY = (int) NewY;
}

// Rotates a vector in aDirection degrees about aFixedPoint
void CVector::rotate_about(int aDirection, CVector *aFixedPoint) {
    mX -= aFixedPoint->get_x();
    mY -= aFixedPoint->get_y();

    rotate(aDirection);

    mX += aFixedPoint->get_x();
    mY += aFixedPoint->get_y();
}

CTargetFleet::CTargetFleet()
{
	mTarget = NULL;
	mDistance = mDangerRating = 0;
}

CTargetFleet::CTargetFleet( CBattleFleet *aMe, CBattleFleet *aEnemy )
{
	mTarget = aEnemy;
	mDistance = aMe->distance( (CVector*)aEnemy );

	if( mDistance == 0 ) mDangerRating = aEnemy->get_power();
	else mDangerRating = aEnemy->get_power()/mDistance;

	if( mDangerRating < 0 ) mDangerRating = 1;

	if( aMe->get_enemy() == aEnemy || aEnemy->get_enemy() == aMe )
		mDangerRating *= 2;
		
	if( aEnemy->is_capital_fleet() ) {
        mDangerRating *= 3;
        if ( aMe->is_stealthed() ) {
            mDangerRating *= 2;
        }
    }

    mDangerRating = aEnemy->calc_PA( mDangerRating, CFleetEffect::FE_DANGER_RATING );

	int
		MisinterpretChance = aEnemy->calc_0base_PA( 100, CFleetEffect::FE_MISINTERPRET );
	if( number(100) <= MisinterpretChance ){
		int
			MisVar = 24+number(376);	// 25-400
		mDangerRating = mDangerRating*MisVar/100;
	}
}

CTargetFleet::~CTargetFleet()
{
	mTarget = NULL;
}

CTargetFleetList::CTargetFleetList()
{
}

CTargetFleetList::~CTargetFleetList()
{
	remove_all();
}

bool
	CTargetFleetList::free_item(TSomething aItem)
{
	CTargetFleet
		*Target = (CTargetFleet*)aItem;

	if( Target == NULL ) return false;

	delete Target;

	return true;
}

CTurret::CTurret()
{
	mNumber = mCooling = 0;
	mInited = false;
}

CTurret::~CTurret()
{

}

void
	CTurret::init( CWeapon *aWeapon, int aNumber )
{
	*(CWeapon*)this = *aWeapon;

	mNumber = aNumber;
	mCooling = 0;
	mInited = true;
}

void
	CTurret::recharge( CBattleFleet *aFleet )
{
	int
		Cooling = get_cooling_time();

	Cooling = (int)(Cooling*(100+aFleet->get_morale()/-5+10+aFleet->get_exp()/-5+10)/100);
	mCooling = aFleet->calc_PA( Cooling, CFleetEffect::FE_COOLING_TIME );
	switch( get_weapon_type() ){
		case WT_BEAM :
			mCooling = aFleet->calc_PA( mCooling, CFleetEffect::FE_BEAM_COOLING_TIME );
			break;
		case WT_MISSILE :
			mCooling = aFleet->calc_PA( mCooling, CFleetEffect::FE_MISSILE_COOLING_TIME );
			break;
		case WT_PROJECTILE :
			mCooling = aFleet->calc_PA( mCooling, CFleetEffect::FE_PROJECTILE_COOLING_TIME );
			break;
	}
}

CBattleFleet::CBattleFleet()
{
	mFleet = NULL;
	for( int i = 0; i < 5; i++ )
		mDevice[i] = NULL;
	mJumpWaitTurns = 0;
}

CBattleFleet::~CBattleFleet()
{

}

int
	CBattleFleet::get_real_id()
{
	return mFleet->get_id();
}

int
	CBattleFleet::get_real_owner()
{
	return mFleet->get_owner();
}

const char *
	CBattleFleet::get_class_name()
{
	if( mFleet ) return mFleet->CShipDesign::get_name();
	return "";
}

const char *
	CBattleFleet::get_size_name()
{
	if( mFleet ) return mFleet->get_size_name();
	return "";
}

double CBattleFleet::get_bat_ratio()
{
 return (mBattle->get_ratio());
}

bool
	CBattleFleet::init(CBattle *aBattle, CPlayer *aOwner, CFleet *aFleet, CAdmiralList *aAdmiralList, int aID)
{
    //SLOG("battle.cc : CBattleFleet::init : called!");
	if (aOwner == NULL || aFleet == NULL) return false;

	mID = aID;
	mNick = aFleet->get_nick();
	mOwner = aOwner->get_game_id();
	mFleet = aFleet;

	mSize = aFleet->get_body()-4000;
	mStatus = COMMAND_NORMAL;
	mSubStatus = SUBSTATUS_NONE;
	set_target(0);
	mPreCharge = 0;
	CShipSize
		*Ship = (CShipSize*)SHIP_SIZE_TABLE->get_by_id(aFleet->get_body());

	mArmor = (CArmor*)COMPONENT_TABLE->get_by_id( aFleet->get_armor() );

	if(Ship == NULL)
	{
		return false;
	}

	mMaxHP = (int)(Ship->get_space() * mArmor->get_hp_multiplier());

	reset_exploit();
	reset_enemy();
	reset_engaged();

	if (aOwner->has_ability(ABILITY_HIGH_MORALE))
	{
		mMorale = 125;
	}
	else
	{
		mMorale = 100;
	}
	

	mMoraleStatus = MORALE_NORMAL;
	mStatusLastingTurn = 0;
	mMaxShip = aFleet->get_current_ship();

    //SLOG("battle.cc : CBattleFleet::init : fleet parameters set");

	if (aOwner->get_game_id() == EMPIRE_GAME_ID)
	{
	    //SLOG("battle.cc : CBattleFleet::init : fleet belongs to the empire");
		mMoraleModifier = 5 - aFleet->get_exp()/10;
		mBerserkModifier = 0;
	}
	else
	{

	    //SLOG("battle.cc : CBattleFleet::init : fleet belongs to a player");
		CRace *
		Race = aOwner->get_race_data();

        //SLOG("battle.cc : CBattleFleet::init : player race data retrieved");
		// - is better than +
		mMoraleModifier = Race->get_morale_modifier() - aFleet->get_exp()/10 +5;
		mBerserkModifier = Race->get_berserk_modifier();

		
		// morale penalty from honor
		CCouncil *Council = aOwner->get_council();
		int AvgHonor = (int) ( aOwner->get_honor() + Council->get_honor() ) / 2;
		mMoraleModifier -= (AvgHonor - 50) / 3;
		mBerserkModifier += (AvgHonor - 50) / 10;

		
		// prevents from disorder on first turn
		// if (mMoraleModifier > 25) mMoraleModifier = 25;

		if (Race->has_ability(ABILITY_PSI)) mAttribute += PSI_RACE;
		if (aOwner->has_ability(ABILITY_ENHANCED_PSI)) mAttribute += ENHANCED_PSI;
		if (Race->has_ability(ABILITY_FAST_MANEUVER)) mAttribute += ENHANCED_MOBILITY;
		
	}

	mCommander = aAdmiralList->get_by_id(aFleet->get_admiral_id());
	if (!mCommander) return false;

	if (mCommander->get_special_ability() == CAdmiral::RA_TACTICAL_GENIUS)
	{
		mMorale += 25;
	}

    //SLOG("battle.cc : CBattleFleet::init : fetching components");

    //SLOG("battle.cc : CBattleFleet::init : fetching engine");
	mEngine = (CEngine *)COMPONENT_TABLE->get_by_id(aFleet->get_engine());
	//SLOG("battle.cc : CBattleFleet::init : engine fetched");

    //SLOG("battle.cc : CBattleFleet::init : fetching computer");
	mComputer = (CComputer *)COMPONENT_TABLE->get_by_id(aFleet->get_computer());
	//SLOG("battle.cc : CBattleFleet::init : computer fetched");

    //SLOG("battle.cc : CBattleFleet::init : fetching shield");
	mShield = (CShield *)COMPONENT_TABLE->get_by_id(aFleet->get_shield());
	//SLOG("battle.cc : CBattleFleet::init : shield fetched");

    //SLOG("battle.cc : CBattleFleet::init : fetching devices");
	for (int i=0 ; i<DEVICE_MAX_NUMBER ; i++)
	{
		if (aFleet->get_device(i) == 0) continue;
		mDevice[i] = (CDevice*)COMPONENT_TABLE->get_by_id(aFleet->get_device(i));
		if (mDevice[i])
		{
			mStaticEffectList += mDevice[i]->get_effect_list();
		}
	}
	//SLOG("battle.cc : CBattleFleet::init : devices fetched");

    //SLOG("battle.cc : CBattleFleet::init : fetching weapons");
	for (int i=0 ; i<WEAPON_MAX_NUMBER ; i++)
	{
		if (aFleet->get_weapon(i) == 0) continue;
		CWeapon *
		Weapon = (CWeapon *)COMPONENT_TABLE->get_by_id(aFleet->get_weapon(i));
        //SLOG("battle.cc : CBattleFleet::init : initializing turret %d", i);
		mTurret[i].init(Weapon, aFleet->get_weapon_number(i));
		//SLOG("battle.cc : CBattleFleet::init : turret %d initialized", i);
		/*if (mTurret[i].get_range() > mRedZoneRadius)
		{
			mRedZoneRadius = mTurret[i].get_range();
		}*/
	}
	//SLOG("battle.cc : CBattleFleet::init : weapons fetched");
	
	//SLOG("battle.cc : CBattleFleet::init : components fetched");

	mRedZoneRadius = 0; // Not used currently...

	mIsCapital = false;

    //SLOG("battle.cc : CBattleFleet::init : calling init_common");
	init_common();
	//SLOG("battle.cc : CBattleFleet::init : finished calling init_common");

	if (mFleet && has_effect(CFleetEffect::FE_TRAINED))
		mFleet->set_exp(100);
	mExp = mFleet->get_exp();
	mOriginalPower = aFleet->get_power();

    //SLOG("battle.cc : CBattleFleet::init : returned true");

	return true;
}

void
	CBattleFleet::init_common()
{
	mStaticEffectList += mArmor->get_effect_list();

	mKilledShip = mKilledFleet = 0;
	mAdmiralExp = CMission::mAdmiralExpBattle;

	CFleetEffect
		*NewEffect;

	CFleet *
	OriginalFleet = this->get_fleet();
	CPlayer *
	OriginalOwner = PLAYER_TABLE->get_by_game_id(OriginalFleet->get_owner());

	if (has_enhanced_mobility()) {
		NewEffect = new CFleetEffect( CFleetEffect::FE_SPEED, 30, CFleetEffect::AT_PROPORTIONAL );
		add_static_effect( NewEffect );
	}

	if (OriginalOwner->has_ability(ABILITY_ENHANCED_PSI_WEAPONRY))
	{
		NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DAMAGE, 30, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
	}

	if (OriginalOwner->has_ability(ABILITY_NATURAL_STEALTH))
	{
		NewEffect = new CFleetEffect( CFleetEffect::FE_STEALTH,  mCommander->get_level() * 5 / 4, CFleetEffect::AT_PROPORTIONAL );
		add_static_effect( NewEffect );
	}

	if (OriginalOwner->has_ability(ABILITY_STEALTHED_AMBUSH))
	{
		NewEffect = new CFleetEffect( CFleetEffect::FE_STEALTH,  mCommander->get_level() * 5 / 4, CFleetEffect::AT_PROPORTIONAL );
		add_static_effect( NewEffect );
	}
	
	switch( mCommander->get_racial_ability() ){
		case CAdmiral::RA_IRRATIONAL_TACTICS :
			if( mCommander->get_level() <= 5 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_DR, 10, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 10 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_DR, 15, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 15 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_DR, 20, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 19 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_DR, 25, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			} else {
				NewEffect = new CFleetEffect( CFleetEffect::FE_DR, 30, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			}
			NewEffect = new CFleetEffect( CFleetEffect::FE_ARMOR_DR, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_INTUITION :
			if( mCommander->get_level() >= 5 )
				mAttribute += WEAK_CLOAKING_DETECTION;
			NewEffect = new CFleetEffect( CFleetEffect::FE_CRITICAL_HIT, mCommander->get_level(), CFleetEffect::AT_ABSOLUTE );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DANGER_RATING, mCommander->get_level() * 50 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_LONE_WOLF:
			mBerserkModifier += 5;
			NewEffect = new CFleetEffect( CFleetEffect::FE_MOBILITY, mCommander->get_level()* 3, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_STEALTH,  mCommander->get_level()* 3, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			if( mCommander->get_level() >= 5 )
				mAttribute += WEAK_CLOAKING;
			break;
		case CAdmiral::RA_DNA_POISON_REPLICATER:
			NewEffect = new CFleetEffect( CFleetEffect::FE_PROJECTILE_DAMAGE, mCommander->get_level() * 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_MISSILE_DAMAGE, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_BREEDER_MALE :
			NewEffect = new CFleetEffect( CFleetEffect::FE_REPAIR_SPEED, mCommander->get_level() * 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_CLONAL_DOUBLE :
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR, mCommander->get_level() * 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_XENOPHOBIC_FANATIC :
			mBerserkModifier += 25;
			mMoraleModifier -= 25;
			if( mCommander->get_level() <= 7 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DEFENSE, -5, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 15 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DEFENSE, -10, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 19 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DEFENSE, -15, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			} else {
				NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DEFENSE, -20, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			}
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_ATTACK, mCommander->get_level()*5, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_MOBILITY, 5 + mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DAMAGE, mCommander->get_level() * 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_MENTAL_GIANT :
			mBerserkModifier -= 5;
			mMoraleModifier -= 5;
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_ATTACK, mCommander->get_level() * 5, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			if( mCommander->get_level() <= 6 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DEFENSE, -10, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 9 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DEFENSE, -5, CFleetEffect::AT_PROPORTIONAL );
				add_static_effect( NewEffect );
				mAttribute += WEAK_CLOAKING_DETECTION;
			} else {
				mAttribute += COMPLETE_CLOAKING_DETECTION;
			}
			NewEffect = new CFleetEffect( CFleetEffect::FE_MISSILE_AR, mCommander->get_level() * 4 / 2, CFleetEffect::AT_PROPORTIONAL );
            add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_ARTIFACT_CRYSTAL :
			NewEffect = new CFleetEffect( CFleetEffect::FE_BEAM_DAMAGE, mCommander->get_level()*2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_RECHARGE_RATE, mCommander->get_level()*5, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_SPEED, mCommander->get_level()*2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_PSYCHIC_PROGENITOR :
			//mBerserkModifier += 5;
			//mMoraleModifier += 10;
			mAttribute += COMPLETE_CLOAKING_DETECTION;
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_ATTACK, 20+mCommander->get_level()*4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			if( mCommander->get_level() >= 10 ){
				mAttribute += COMPLETE_CLOAKING;
			} else if( mCommander->get_level() >= 5 ){
				mAttribute += WEAK_CLOAKING;
			}
			break;
		case CAdmiral::RA_ARTIFACT_COOLING_ENGINE :
			NewEffect = new CFleetEffect( CFleetEffect::FE_COOLING_TIME, -mCommander->get_level() * 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_AR, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DANGER_RATING, mCommander->get_level() * 30 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_LYING_DORMANT :
			mAttribute += WEAK_CLOAKING;
			NewEffect = new CFleetEffect( CFleetEffect::FE_STEALTH, mCommander->get_level() * 5, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_GENERIC_DEFENSE, -mCommander->get_level() / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_MISSILE_CRATERS :
			NewEffect = new CFleetEffect( CFleetEffect::FE_COOLING_TIME, -mCommander->get_level() * 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			if ( mCommander->get_level() <= 5 ) {
			    NewEffect = new CFleetEffect( CFleetEffect::FE_ARMOR_DR, -20, CFleetEffect::AT_PROPORTIONAL );
			    add_static_effect( NewEffect );
			} else if ( mCommander->get_level() <= 10 ) {
                NewEffect = new CFleetEffect( CFleetEffect::FE_ARMOR_DR, -15, CFleetEffect::AT_PROPORTIONAL );
			    add_static_effect( NewEffect );
            } else if ( mCommander->get_level() <= 15 ) {
                NewEffect = new CFleetEffect( CFleetEffect::FE_ARMOR_DR, -10, CFleetEffect::AT_PROPORTIONAL );
			    add_static_effect( NewEffect );
            } else {
                NewEffect = new CFleetEffect( CFleetEffect::FE_ARMOR_DR, -5, CFleetEffect::AT_PROPORTIONAL );
			    add_static_effect( NewEffect );
            }
			NewEffect = new CFleetEffect( CFleetEffect::FE_GENERIC_DEFENSE, -mCommander->get_level() / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_AR, mCommander->get_level() / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_METEOR_DRONES :
			NewEffect = new CFleetEffect( CFleetEffect::FE_GENERIC_DEFENSE, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR, mCommander->get_level() * 1 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DANGER_RATING, mCommander->get_level() * 50 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR_AGAINST_MISSILE, mCommander->get_level() * 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_CYBER_SCAN_UNIT :
			mAttribute += WEAK_CLOAKING_DETECTION;
			NewEffect = new CFleetEffect( CFleetEffect::FE_AR, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_MISSILE_AR, mCommander->get_level() * 5 / 2, CFleetEffect::AT_PROPORTIONAL );
            add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_TRAJECTORY_AUGMENTATION :
			NewEffect = new CFleetEffect( CFleetEffect::FE_PROJECTILE_AR, mCommander->get_level()* 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_PROJECTILE_DAMAGE, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_PATTERN_BROADCASTER :
			/*NewEffect = new CFleetEffect( CFleetEffect::FE_STEALTH, -(10+mCommander->get_level()*2), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_MISINTERPRET, 10+mCommander->get_level()*4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );*/
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR_AGAINST_MISSILE, mCommander->get_level() * 9, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_FAMOUS_PRIVATEER :
			NewEffect = new CFleetEffect( CFleetEffect::FE_SPEED, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );

			NewEffect = new CFleetEffect( CFleetEffect::FE_STEALTH, 20 + mCommander->get_level() * 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			if( mCommander->get_level() >= 7 ){
				mAttribute += WEAK_CLOAKING;
			}
			break;
		case CAdmiral::RA_COMMERCE_KING :
			NewEffect = new CFleetEffect( CFleetEffect::FE_HP, mCommander->get_level() * 5 / 2, CFleetEffect::AT_PROPORTIONAL);
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR_AGAINST_MISSILE, mCommander->get_level() * 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_RETREAT_SHIELD :
			mBerserkModifier -= 5;
			mMoraleModifier -= 10;
			NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_STRENGTH, mCommander->get_level() * 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_IMPENETRABLE_SHIELD, mCommander->get_level()* 5 / 2, CFleetEffect::AT_ABSOLUTE );
			add_static_effect( NewEffect );
			// ---Change--- //
			NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_INTEGRITY, mCommander->get_level() * 5 / 2, CFleetEffect::AT_ABSOLUTE );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_FAST_RETREAT, mCommander->get_level(), CFleetEffect::AT_ABSOLUTE );
			add_static_effect( NewEffect );
			// ---End Change--- //
			if( mCommander->get_level() <= 5 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_SOLIDITY, 1, CFleetEffect::AT_ABSOLUTE);
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 10 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_SOLIDITY, 2, CFleetEffect::AT_ABSOLUTE);
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 15 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_SOLIDITY, 2, CFleetEffect::AT_ABSOLUTE);
				add_static_effect( NewEffect );
			} else if( mCommander->get_level() <= 19 ){
				NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_SOLIDITY, 3, CFleetEffect::AT_ABSOLUTE);
				add_static_effect( NewEffect );
			} else {
				NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_SOLIDITY, 3, CFleetEffect::AT_ABSOLUTE);
				add_static_effect( NewEffect );
			}
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR_AGAINST_MISSILE, mCommander->get_level() * 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_GENETIC_THROWBACK :
			mMoraleModifier += 5;
			if (mCommander->get_level() < 10 )
			{
				mAttribute += WEAK_CLOAKING;
			}
			if (mCommander->get_level() >= 10 )
			{
				mAttribute += COMPLETE_CLOAKING;
			}
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_ATTACK, mCommander->get_level()*5, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DAMAGE, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_AR, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_RIGID_THINKING :
			mBerserkModifier -= 5;
			mMoraleModifier -= 10;
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DEFENSE, mCommander->get_level()*5, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR, mCommander->get_level() * 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_IMPENETRABLE_ARMOR, mCommander->get_level() / 2, CFleetEffect::AT_ABSOLUTE );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR_AGAINST_MISSILE, mCommander->get_level() * 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_MANAGEMENT_PROTOCOL :
			NewEffect = new CFleetEffect( CFleetEffect::FE_SPEED, 10+mCommander->get_level()*2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DEFENSE, mCommander->get_level()*2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_AR, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect(CFleetEffect::FE_EFFICIENCY, mCommander->get_level() * 2, CFleetEffect::AT_ABSOLUTE);
			add_static_effect( NewEffect );
			mMoraleModifier -= 10;
			break;
		case CAdmiral::RA_BLITZKRIEG :
			NewEffect = new CFleetEffect( CFleetEffect::FE_DAMAGE, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_CRITICAL_HIT, mCommander->get_level() * 3 / 4, CFleetEffect::AT_ABSOLUTE );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_TACTICAL_GENIUS :
			mBerserkModifier -= 5;
			mMoraleModifier -= 15;
			NewEffect = new CFleetEffect( CFleetEffect::FE_GENERIC_DEFENSE, mCommander->get_level() * 3 / 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_IMPENETRABLE_ARMOR, mCommander->get_level() * 3 / 2, CFleetEffect::AT_ABSOLUTE );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DANGER_RATING, mCommander->get_level() * 30 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR_AGAINST_MISSILE, mCommander->get_level() * 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_SHIELD_DISRUPTER :
			NewEffect = new CFleetEffect( CFleetEffect::FE_AR, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_DEFENSIVE_MATRIX :
			mMoraleModifier -= 20;
			mBerserkModifier -= 20;
			NewEffect = new CFleetEffect( CFleetEffect::FE_GENERIC_DEFENSE, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_HP, mCommander->get_level() * 1 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR_AGAINST_MISSILE, mCommander->get_level() * 3, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_CONSCIOUSNESS_CRYSTAL :
			NewEffect = new CFleetEffect( CFleetEffect::FE_BEAM_DAMAGE, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_AR, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_DAMAGE, mCommander->get_level(), CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_PSI_ATTACK, mCommander->get_level() * 5 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		// ---Change--- //
		case CAdmiral::RA_CRUSADER :
		      // -20 Morale modifier
		      // +10 berserk modifier
			mMoraleModifier -= 20;
			mBerserkModifier += 10;
			// Complete cloaking detection
			mAttribute += COMPLETE_CLOAKING_DETECTION;
            // Up to 50% damage
			NewEffect = new CFleetEffect( CFleetEffect::FE_DAMAGE, mCommander->get_level() * 6 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			// Up to 30% cooling time reduction
			NewEffect = new CFleetEffect( CFleetEffect::FE_COOLING_TIME, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_IMPINGEMENT_NEUTRALIZATION_FIELD :
		      // -30 Morale modifier
			mMoraleModifier -= 30;
			mBerserkModifier -= 30;
            // Up to 30% damage reduction
			NewEffect = new CFleetEffect( CFleetEffect::FE_GENERIC_DEFENSE, mCommander->get_level() * 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			// Up to 30 IA
			NewEffect = new CFleetEffect( CFleetEffect::FE_IMPENETRABLE_ARMOR, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			// Up to 30 SI
			NewEffect = new CFleetEffect( CFleetEffect::FE_SHIELD_INTEGRITY, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			// Up to 30 IS
			NewEffect = new CFleetEffect( CFleetEffect::FE_IMPENETRABLE_SHIELD, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			NewEffect = new CFleetEffect( CFleetEffect::FE_HP, mCommander->get_level() * 3 / 2, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			// Up to 80% DR against missiles
			NewEffect = new CFleetEffect( CFleetEffect::FE_DR_AGAINST_MISSILE, mCommander->get_level() * 4, CFleetEffect::AT_PROPORTIONAL );
			add_static_effect( NewEffect );
			break;
		case CAdmiral::RA_ARMADA_SYNERGY_SPECIALIST :
		      // To add new effects
		      // Synergy point -> gives 200 at level 20
		      // Each 100 synergy points increases armada AR/DR by 1%
		      // Double Synergy points if capital commander
		      // Currently just gives efficiency and lots of FC
		      // -40 Morale modifier
			mMoraleModifier -= 40;
			NewEffect = new CFleetEffect(CFleetEffect::FE_EFFICIENCY, mCommander->get_level() * 2, CFleetEffect::AT_ABSOLUTE);
			add_static_effect( NewEffect );
		// ---End Change--- //
	}

	// under effect things
	mShieldSolidity = mShield->get_deflection_solidity();
	mShieldMaxStrength = mShield->get_strength(mSize-1);
	mShieldRechargeRate = mShield->get_recharge_rate(mSize-1);

	mShieldSolidity = calc_PA( mShieldSolidity, CFleetEffect::FE_SHIELD_SOLIDITY );
	mShieldMaxStrength = calc_PA( mShieldMaxStrength, CFleetEffect::FE_SHIELD_STRENGTH );

	mMaxHP = calc_PA( mMaxHP, CFleetEffect::FE_HP );
	for( int i = 0; i < mMaxShip; i++ ){
		mHP[i] = mMaxHP;
		mShieldStrength[i] = mShieldMaxStrength;
	}
	mTotalHP = mMaxHP * mMaxShip;

	mMorale = calc_PA_float( mMorale, CFleetEffect::FE_MORALE );

	if( has_effect( CFleetEffect::FE_COMPLETE_CLOAKING ) )
		mAttribute += COMPLETE_CLOAKING;
	if( has_effect( CFleetEffect::FE_WEAK_CLOAKING ) )
		mAttribute += WEAK_CLOAKING;
	if( has_effect( CFleetEffect::FE_COMPLETE_CLOAKING_DETECTION ) )
		mAttribute += COMPLETE_CLOAKING_DETECTION;
	if( has_effect( CFleetEffect::FE_WEAK_CLOAKING_DETECTION ) )
		mAttribute += WEAK_CLOAKING_DETECTION;

	mMoraleModifier = calc_PA( mMoraleModifier, CFleetEffect::FE_MORALE_MODIFIER );
	mBerserkModifier = calc_PA( mBerserkModifier, CFleetEffect::FE_BERSERK_MODIFIER );

}

void
	CBattleFleet::set_command(int aCommand)
{
	mStatus = aCommand;

	switch (mStatus)
	{
		case COMMAND_PENETRATE :
		case COMMAND_ASSAULT :
			set_substatus(SUBSTATUS_PENETRATION);
			break;
		case COMMAND_FLANK :
			set_substatus(SUBSTATUS_MOVE_STRAIGHT);
			break;
		default :
			set_substatus(SUBSTATUS_NONE);
			break;
	}
}

void
	CBattleFleet::set_status(int aStatus, int aLastingTurn)
{
	mStatus = aStatus;
	mStatusLastingTurn = aLastingTurn;
	switch (mStatus)
	{
		case STATUS_ROUT :
			set_substatus(SUBSTATUS_TURN_TO_NEAREST_BORDER);
			break;
		case STATUS_RETREAT :
			set_substatus(SUBSTATUS_TURN_TO_BACKWARD);
			break;
		default :
			set_substatus(SUBSTATUS_NONE);
			break;
	}
}

bool
	CBattleFleet::recover_status()
{
	mStatusLastingTurn--;

	if( mStatusLastingTurn <= 0 ) return true;
	return false;
}

void
	CBattleFleet::trace( CVector *aVector )
{
// calc_effective_reach_time


	CVector
		Tmp = *((CVector*)this);

	// turning only
	turn_to( aVector );
	int
		ERT1 = calc_effective_reach_time( aVector );
	// turn + move
	move();
	int
		ERT2 = calc_effective_reach_time( aVector );
	// move
	*(CVector*)this = Tmp;
	move();
	int
		ERT3 = calc_effective_reach_time( aVector );

	*(CVector*)this = Tmp;
	// turning only
	if( ERT1 < ERT2 && ERT1 < ERT3 ){
		turn_to( aVector );
	// move
	} else if( ERT3 < ERT1 && ERT3 < ERT2 ){
		move();
	// turn + move
	} else {
		turn_to( aVector );
		move();
	}
}

int
	CBattleFleet::get_speed()
{
	int
		Speed = mEngine->get_battle_speed(mSize-1) * (100+(int)mMorale/5-20+mExp*3/20)/100;
	Speed = calc_PA( Speed, CFleetEffect::FE_SPEED );

	return Speed;
}

double
	CBattleFleet::get_mobility()
{
	int
		Mobility = mEngine->get_battle_mobility(mSize-1)*(100+(int)mMorale/10+mExp*3/10-15);
	Mobility = calc_PA( Mobility, CFleetEffect::FE_MOBILITY );

	return (double)Mobility/5000.0;
}

int
	CBattleFleet::get_detection_range()
{
	int
		Mult = 15+mComputer->get_level()*2+(int)((double)mCommander->get_detection_level()*1.5);

	Mult = calc_PA( Mult, CFleetEffect::FE_COMPUTER );
	Mult = calc_PA( Mult, CFleetEffect::FE_DETECTION_RANGE );

	return 20*Mult;
}

int CBattleFleet::get_active_ratio() {
	int
		Count = 0;

	for( int i = 0; i < mMaxShip; i++ )
		Count += mHP[i];
	Count = Count*100/mTotalHP;

	return Count;
}

int
	CBattleFleet::count_active_ship()
{
	int
		Count = 0;

	for( int i = 0; i < mMaxShip; i++ )
		if( mHP[i] > 0 ) Count++;

	return Count;
}

// Returns the total power of the ships left in the fleet
int CBattleFleet::get_power() {
    return mOriginalPower*get_active_ratio()/100;
}

void
	CBattleFleet::get_path( int *aLeftX, int *aRightX, int *aTopY, int *aBottomY )
{
	*aLeftX = 0;
	switch (mStatus)
	{
		case COMMAND_NORMAL :
			*aTopY = 750;
			*aBottomY = -750;
			*aRightX = 1500;
			break;
		case COMMAND_FORMATION :
			*aTopY = 750;
			*aBottomY = -750;
			*aRightX = 1500;
			break;
		case COMMAND_PENETRATE :
			*aTopY = 250;
			*aBottomY = -250;
			*aRightX = 1000;
			break;
		case COMMAND_FLANK :
			*aTopY = 500;
			*aBottomY = -500;
			break;
		case COMMAND_RESERVE :
			*aTopY = 750;
			*aBottomY = -750;
			break;
		case COMMAND_STAND_GROUND :
			*aTopY = 750;
			*aBottomY = -750;
			*aRightX = 1500;
			break;
		case COMMAND_ASSAULT :
			*aTopY = 200;
			*aBottomY = -200;
			break;
		case COMMAND_FREE :
			*aTopY = 750;
			*aBottomY = -750;
			*aRightX = 1500;
			break;
		default :
			*aTopY = 750;
			*aBottomY = -750;
			*aRightX = 1500;
			break;
	}
}

bool
	CBattleFleet::on_path( CVector *aPoint )
{
	int
		dX = aPoint->get_x()-get_x(),
	dY = aPoint->get_y()-get_y();
	int
		newX = (int)(dX*as_cos(-get_direction())-dY*as_sin(-get_direction())),
	newY = (int)(dX*as_sin(-get_direction())+dY*as_cos(-get_direction()));

	int
		TopY, BottomY, LeftX, RightX;
	get_path( &LeftX, &RightX, &TopY, &BottomY );

	if( newX > LeftX && newX < RightX && newY > BottomY && newY < TopY )
		return true;
	return false;
}

bool
	CBattleFleet::path_meet_border()
{
	int
		TopY, BottomY, LeftX, RightX;
	get_path( &LeftX, &RightX, &TopY, &BottomY );

	CVector
		TopLeft( get_x()+LeftX, get_y()+TopY ),
		TopRight( get_x()+RightX, get_y()+TopY ),
		BottomLeft( get_x()+LeftX, get_y()+BottomY ),
		BottomRight( get_x()+RightX, get_y()+BottomY );

	TopLeft.rotate_about( get_direction(), (CVector*)this );
	TopRight.rotate_about( get_direction(), (CVector*)this );
	BottomLeft.rotate_about( get_direction(), (CVector*)this );
	BottomRight.rotate_about( get_direction(), (CVector*)this );

	if( TopLeft.touch_border() || TopRight.touch_border() ||
		BottomLeft.touch_border() || BottomRight.touch_border() )
		return true;
	return false;
}

bool
	CBattleFleet::path_meet_vertical_border()
{
	int
		TopY, BottomY, LeftX, RightX;
	get_path( &LeftX, &RightX, &TopY, &BottomY );

	CVector
		TopLeft( get_x()+LeftX, get_y()+TopY ),
		TopRight( get_x()+RightX, get_y()+TopY ),
		BottomLeft( get_x()+LeftX, get_y()+BottomY ),
		BottomRight( get_x()+RightX, get_y()+BottomY );

	TopLeft.rotate_about( get_direction(), (CVector*)this );
	TopRight.rotate_about( get_direction(), (CVector*)this );
	BottomLeft.rotate_about( get_direction(), (CVector*)this );
	BottomRight.rotate_about( get_direction(), (CVector*)this );

	if( TopLeft.get_x() <= 0 || TopLeft.get_x() >= 10000 ||
		TopRight.get_x() <= 0 || TopRight.get_x() >= 10000 ||
		BottomLeft.get_x() <= 0 || BottomLeft.get_x() >= 10000 ||
		BottomRight.get_x() <= 0 || BottomRight.get_x() >= 10000 )
		return true;
	return false;
}

bool
	CBattleFleet::on_fire_range( CBattleFleet *aEnemy )
{
	//if (aEnemy->is_detected() == false) return false;
	if (can_see(aEnemy) == false) return false;
	if (aEnemy->is_disabled() == true) return false;

	for (int i=0 ; i<WEAPON_MAX_NUMBER ; i++)
	{
		CTurret *
		Turret = get_turret(i);

		if (Turret->is_empty() == true) continue;
		if (Turret->ready() == false) continue;
		if (is_in_range( (CVector*)aEnemy, Turret->get_range(), Turret->get_angle_of_fire()/2 ) )
			return true;
	}

	return false;
}

bool
	CBattleFleet::on_complete_fire_range( CBattleFleet *aEnemy )
{
	//if( aEnemy->is_detected() == false ) return false;
	if (can_see(aEnemy) == false) return false;
	if( aEnemy->is_disabled() ) return false;

	for( int i = 0; i < WEAPON_MAX_NUMBER; i++ ){
		CTurret
			*Turret = get_turret(i);

		if( Turret->is_empty() ) continue;
		if( is_in_range( (CVector*)aEnemy, Turret->get_range(), Turret->get_angle_of_fire()/2 ) == false )
			return false;
	}

	return true;
}

int
	CBattleFleet::calc_effective_reach_time( CVector *aVector )
{
	int
		ERT = 0;

	int
		DeltaDirection = delta_direction( aVector );
	if( DeltaDirection > 180 ) DeltaDirection = 360 - DeltaDirection;

	if( get_mobility() == 0.0 ) ERT += 1800;
	else ERT += (int)(DeltaDirection/get_mobility());

	if( get_speed() == 0 ) ERT += 1800;
	else ERT += distance( aVector ) / get_speed();

	return ERT;
}

// ---Change--- //
// Returns true if a fleet is able to jump out of the battle
// at the current instant
bool CBattleFleet::tick_jump_timer() { 
    int mTurnsToJumpOut = JUMP_OUT_TICKS;
    
    // Code for faster retreat to be added here

    return (++mJumpWaitTurns > mTurnsToJumpOut);
}
// ---End Change--- //

void
	CBattleFleet::attack( CBattleRecord *aRecord, CBattleFleet *aEnemy )
{
	remove_cloaking();
	aEnemy->remove_cloaking();

	for (int i=0 ; i<WEAPON_MAX_NUMBER ; i++)
	{
		CTurret *
		Turret = get_turret(i);

		if (Turret->is_empty() == true)
		{
			continue;
		}
		if (Turret->ready() == true)
		{
			fire( aRecord, Turret, aEnemy );
		}
		if( aEnemy->get_status() == STATUS_ANNIHILATED_THIS_TURN )
			break;
	}

	set_target( aEnemy->get_id() );
}

void
	CBattleFleet::siege(CPlanet *aPlanet, CString &aReport)
{
	int
		MB = aPlanet->get_building().get(BUILDING_MILITARY_BASE);

	int
		Damage = (24+get_size()*10) * MB;

	CPlayer *
	Owner = aPlanet->get_owner();
	CComponentList *
	ComponentList = Owner->get_component_list();

	// 1.2 times by weapon level
	int
		WL = ComponentList->get_weapon_level();
	if (WL > 1) Damage = (int)(Damage*pow(1.2, (double)WL));

	Damage -= MB * get_shield_solidity();

	if (Damage > 0)
	{
		aReport.format(GETTEXT("%1$s unit(s) of military base open(s) fire at %2$s on the orbit."),
			dec2unit(MB), mFleet->get_nick());
		aReport += "<BR>\n";
	}

	damage_first_active(Damage);
}

void
	CBattleFleet::raid_attack(CBattleFleet *aEnemy )
{
	if (aEnemy->is_disabled()) return;

	for(int i=0; i<WEAPON_MAX_NUMBER; i++)
	{
		CTurret
			*Turret = get_turret(i);

		if (Turret->is_empty()) continue;
		raid_fire(Turret, aEnemy);

		if (aEnemy->get_status() == STATUS_ANNIHILATED_THIS_TURN) break;
	}
}

void
	CBattleFleet::fire( CBattleRecord *aRecord, CTurret *aTurret, CBattleFleet *aEnemy )
{
	if( aTurret->ready() == false ) return;
	CFleet *
	OriginalFleet = this->get_fleet();
	CPlayer *
	OriginalOwner = PLAYER_TABLE->get_by_game_id(OriginalFleet->get_owner());
	if (is_stealthed() == true) mRemainingStealthTurns = number(20);
	if (OriginalOwner->has_ability(ABILITY_NATURAL_STEALTH)) mRemainingStealthTurns += 20;

	set_engaged();
	aEnemy->set_engaged();


	int
		AR,
		DR,
		HitChance;

	AR = calc_AR( aTurret );
	DR = aEnemy->calc_DR( aTurret );

	if( DR > AR )
		HitChance = AR*100/DR/2;
	else
		HitChance = 100 - (DR*100/AR/2);

	if( HitChance < 5 ) HitChance = 5;
	if( HitChance > 95 ) HitChance = 95;

	aRecord->add_fire( this, aEnemy, aTurret, HitChance );

	int
		CritChance = 0;
	int
		SideDir = aEnemy->delta_direction( (CVector*)this );
	if( SideDir > 180 ) SideDir = 360 - SideDir;
	
	// 0 = front 1 = side 2 = rear
	int hit_direction = 0;

	// showing front
	if( SideDir >= 0 && SideDir <= 45 ){
		CritChance = 5;
	// showing side
	} else if( SideDir > 45 && SideDir < 135 ){
		CritChance = 10;
		hit_direction = 1;
	// showing rear
	} else {
		CritChance = 20;
		hit_direction = 2;
	}
	CritChance += aTurret->effect_amount( CFleetEffect::WE_ARMOR_PIERCING );
	CritChance = calc_PA( CritChance, CFleetEffect::FE_CRITICAL_HIT );
	CritChance = aEnemy->calc_minus_PA( CritChance, CFleetEffect::FE_IMPENETRABLE_ARMOR );
	
	if (hit_direction == 0) {
	    CritChance = aEnemy->calc_minus_PA( CritChance, CFleetEffect::FE_FRONT_ARMOR );
	} else if (hit_direction == 1) {
        CritChance = aEnemy->calc_minus_PA( CritChance, CFleetEffect::FE_SIDE_ARMOR );
	} else {
        CritChance = aEnemy->calc_minus_PA( CritChance, CFleetEffect::FE_REAR_ARMOR );
	}

	int
		HitCount = 0,
	MissCount = 0,
	SunkenCount = 0,
	TotalDamage = 0;

	damage( aTurret, aEnemy, HitChance, CritChance, &TotalDamage, &HitCount, &MissCount, &SunkenCount );

	mKilledShip += SunkenCount;
	if( aEnemy->is_disabled() ) mKilledFleet++;

	aRecord->add_hit( this, aEnemy, HitCount, MissCount, TotalDamage, SunkenCount );

	aTurret->recharge( this );
}

// Changes the morale of CBattleFleet to aMorale
// Also updates 
float CBattleFleet::change_morale(float aMorale) {
    mMorale += aMorale;
    if (mMorale < 0) mMorale = 0;
    if (mMorale > 200) mMorale = 200;

    return mMorale;
}

void
	CBattleFleet::raid_fire(CTurret *aTurret, CBattleFleet *aEnemy )
{
	int
		AR,
		DR,
		HitChance;

	AR = calc_AR( aTurret );
	DR = aEnemy->calc_DR( aTurret );

	if( DR > AR )
		HitChance = AR*100/DR/2;
	else
		HitChance = 100 - (DR*100/AR/2);

	if( HitChance < 5 ) HitChance = 5;
	if( HitChance > 95 ) HitChance = 95;

	int
		HitCount = 0,
	MissCount = 0,
	SunkenCount = 0,
	TotalDamage = 0;

	damage( aTurret, aEnemy, HitChance, 0, &TotalDamage, &HitCount, &MissCount, &SunkenCount );
	aTurret->set_cooling(0);
}

bool
	CBattleFleet::damage_first_active( int aDam, bool aPSI )
{
	int
		Index = -1;
	for( int i = 0; i < mMaxShip; i++ )
	{
		if( mHP[i] > 0 )
		{
			Index = i;
			break;
		}
	}

	if( Index == -1 ) return false;

	if (aPSI)
	{
		aDam = calc_PA(aDam, CFleetEffect::FE_PSI_DEFENSE_PANALTY);
	}

	if( mShieldStrength[Index] < aDam ){
		aDam -= mShieldStrength[Index];
		mShieldStrength[Index] = 0;
		mHP[Index] -= aDam;
		if( mHP[Index] < 0 ){
			aDam += mHP[Index];
			mHP[Index] = 0;
		}
	} else {
		mShieldStrength[Index] -= aDam;
	}

	float
		MoraleDrop = aDam*2*100.0/mTotalHP,
	PSIMoraleDrop = MoraleDrop;

	if( aPSI ){
		PSIMoraleDrop = calc_minus_PA_float( PSIMoraleDrop, CFleetEffect::FE_PSI_DEFENSE );
		MoraleDrop += PSIMoraleDrop;
	}

	change_morale( -MoraleDrop );

	if( mHP[Index] <= 0 )
	{
		mHP[Index] = 0;
		if( count_active_ship() <= 0 )
			set_status( STATUS_ANNIHILATED_THIS_TURN );
		return true;
	}
	return false;
}

bool
	CBattleFleet::damage_random_active( int aDam, bool aPSI )
{
	int
		Index = -1,
	Nth = number( count_active_ship() );

	for( int i = 0; i < mMaxShip; i++ ){
		if( mHP[i] > 0 ){
			Nth--;
			if( Nth == 0 ){
				Index = i;
				break;
			}
		}
	}

	if( Index == -1 ) return false;

	if (aPSI == true)
	{
		aDam = calc_PA(aDam, CFleetEffect::FE_PSI_DEFENSE_PANALTY);
	}

	if( mShieldStrength[Index] < aDam ){
		aDam -= mShieldStrength[Index];
		mShieldStrength[Index] = 0;
		mHP[Index] -= aDam;
		if( mHP[Index] < 0 ) {
			aDam += mHP[Index];
			mHP[Index] = 0;
		}
	} else {
		mShieldStrength[Index] -= aDam;
	}

	float
		MoraleDrop = aDam*2*100.0/mTotalHP,
	PSIMoraleDrop = MoraleDrop;

	if( aPSI ){
		PSIMoraleDrop = calc_minus_PA_float( PSIMoraleDrop, CFleetEffect::FE_PSI_DEFENSE );
		MoraleDrop += PSIMoraleDrop;
	}

	change_morale( -MoraleDrop );

	if( mHP[Index] <= 0 )
	{
		mHP[Index] = 0;
		if( count_active_ship() <= 0 )
			set_status( STATUS_ANNIHILATED_THIS_TURN );
		return true;
	}
	return false;
}

int
	CBattleFleet::calc_AR( CTurret *aTurret )
{
	int
		ComputerAR = mComputer->get_attacking_rate(),
	TurretAR = aTurret->get_attacking_rate();
	ComputerAR = calc_PA( ComputerAR, CFleetEffect::FE_COMPUTER );
	TurretAR = calc_PA( TurretAR, CFleetEffect::FE_WEAPON_AR );

	switch( aTurret->get_weapon_type() ){
		case WT_BEAM :
			TurretAR = calc_PA( TurretAR, CFleetEffect::FE_BEAM_AR );
			break;
		case WT_MISSILE :
			TurretAR = calc_PA( TurretAR, CFleetEffect::FE_MISSILE_AR );
			break;
		case WT_PROJECTILE :
			TurretAR = calc_PA( TurretAR, CFleetEffect::FE_PROJECTILE_AR );
			break;
	}

	int
		AR = TurretAR*(100+ComputerAR+mExp)/100;

	AR = calc_PA( AR, CFleetEffect::FE_AR );
	return AR;
}

int
	CBattleFleet::calc_DR( CTurret *aTurret )
{
	int
		ArmorDR = mArmor->get_defense_rate(),
	ComputerDR = mComputer->get_defense_rate(),
	MobilityBase = (int)(50.0*get_mobility());
	ComputerDR = calc_PA( ComputerDR, CFleetEffect::FE_COMPUTER );
	ArmorDR = calc_PA( ArmorDR, CFleetEffect::FE_ARMOR_DR );

	int
		DR = (MobilityBase+ArmorDR)*(100+ComputerDR+mExp)/100;

	DR = calc_PA( DR, CFleetEffect::FE_DR );

	if( aTurret ){
		switch( aTurret->get_weapon_type() ){
			case WT_BEAM :
				DR = calc_PA( DR, CFleetEffect::FE_DR_AGAINST_BEAM );
				break;
			case WT_MISSILE :
				DR = calc_PA( DR, CFleetEffect::FE_DR_AGAINST_MISSILE );
				break;
			case WT_PROJECTILE :
				DR = calc_PA( DR, CFleetEffect::FE_DR_AGAINST_PROJECTILE );
				break;
		}
	}
	return DR;
}

char *
	CBattleFleet::get_order_name(int aOrder)
{
	if (aOrder<1 || aOrder>NUMBER_OF_ORDER) return NULL;
	return mOrderName[aOrder-1];
}

void
	CBattleFleet::repair( int aPercent )
{
	bool
		first = true;
	for( int i = 0; i < mMaxShip; i++ ){
		if( mHP[i] > 0 && mHP[i] < mMaxHP ){
			mHP[i] += mMaxHP*aPercent/100;
			if( first == true ){
				mHP[i] = mMaxHP;	// capital ship will be always healed to 100%
				first = false;
			}
		}
		if( mHP[i] > mMaxHP ) mHP[i] = mMaxHP;
	}
}

bool
	CBattleFleet::is_disabled()
{
	if( get_status() == STATUS_RETREATED
		|| get_status() == STATUS_RETREATED_THIS_TURN
		|| get_status() == STATUS_ANNIHILATED_THIS_TURN
		|| get_status() == STATUS_ANNIHILATED )
		return true;
	return false;
}

int
	CBattleFleet::calc_danger_rating( CBattleFleet *aEnemy )
{
	int
		Danger;
	int
		Distance = distance( (CVector*)aEnemy );
	if( Distance == 0 )
		return 99999999;

	Danger = aEnemy->get_power()*aEnemy->get_exploit()*aEnemy->get_penetration()/Distance;

	if( aEnemy->is_disabled() ) Danger = 0;
	if( aEnemy->get_status() == STATUS_RETREAT ||
		aEnemy->get_status() == STATUS_ROUT ) Danger /= 2;
	if( aEnemy->get_target() == get_id() ) Danger *= 2;

	return Danger;
}

void
	CBattleFleet::weapon_cooling()
{
	for( int i = 0; i < WEAPON_MAX_NUMBER; i++ ){
		if( mTurret[i].is_empty() == false )
			mTurret[i].cooling();
	}
}

void
	CBattleFleet::drop_ship_under_25()
{
	int
		CountAlive = 0;
	for( int i = 0; i < mMaxShip; i++ ){
		if( mHP[i] < mMaxHP/4 )
			mHP[i] = 0;
		else
			CountAlive++;
	}

	if( CountAlive == 0 )
		set_status( STATUS_ANNIHILATED_THIS_TURN );
}

void
	CBattleFleet::reorganize_after_battle( CPlayer *aOwner )
{
	CRepairBay *
	RepairBay = aOwner->get_repair_bay();
	CDock *
	Dock = aOwner->get_dock();

	int
		RepairCount = 0,
	DestCount = 0;

	for (int i=0 ; i<mMaxShip ; i++)
	{
		if (mHP[i] == 0)
		{
			DestCount++;
		}
		else if (mHP[i] < mFleet->get_max_hp_without_effect())
		{
			CDamagedShip *
			DamagedShip = new CDamagedShip((CShipDesign *)mFleet, mHP[i]);
			RepairBay->add_damaged_ship(DamagedShip);
			RepairCount++;

			DamagedShip->type(QUERY_INSERT);
			STORE_CENTER->store(*DamagedShip);
		}
	}

	CString
		Buf;
	int
		ShipCount = Dock->count_ship(mFleet->get_design_id());
	if (ShipCount >= RepairCount+DestCount)
	{
		Dock->change_ship((CShipDesign *)mFleet, -(RepairCount+DestCount));
		Buf.format(GETTEXT("Your fleet %1$s lost %2$d ship(s), send %3$d ship(s) to repair bay, and %4$d ships are replenished to your fleet."),
			mFleet->get_name(),
			DestCount,
			RepairCount,
			RepairCount+DestCount);
	}
	else
	{
		Dock->change_ship((CShipDesign *)mFleet, -ShipCount);
		mFleet->set_current_ship(mFleet->get_current_ship()-RepairCount-DestCount+ShipCount );
		Buf.format(GETTEXT("Your fleet %1$s lost %2$d ship(s), send %3$d ship(s) to repair bay, and %4$d ships are replenished to your fleet."),
			mFleet->get_name(),
			DestCount,
			RepairCount,
			ShipCount);
	}

	aOwner->time_news((char *)Buf);
}

void
	CBattleFleet::add_to_encounter_list( CBattleFleet *aEnemy )
{
	CTargetFleet
		*Target = new CTargetFleet( this, aEnemy );

	mEncounterList.push( Target );
	set_encountered();
}

CBattleFleet*
CBattleFleet::find_target()
{
	if (mEncounterList.length() > 0)
	{
		return get_highest_DAR_encounter_fleet();
	}

	/*for (int i=0 ; i<mEncounterList.length() ; i++)
	{
		CTargetFleet *
		Target = (CTargetFleet *)mEncounterList.get(i);

		if (on_fire_range(Target->get_fleet()) )
			return Target->get_fleet();
	}*/
	return NULL;
}

CBattleFleet*
CBattleFleet::get_highest_DAR_encounter_fleet()
{
	CBattleFleet
		*ReturnFleet = NULL;
	int
		HighestDAR = -1;

	for( int i = 0; i < mEncounterList.length(); i++ )
	{
		CTargetFleet
			*Target = (CTargetFleet*)mEncounterList.get(i);
		if ((Target->get_danger_rating() > HighestDAR) && (((CBattleFleet *)Target->get_fleet())->is_disabled() == false))
		{
			HighestDAR = Target->get_danger_rating();
			ReturnFleet = Target->get_fleet();
		}
	}

	return ReturnFleet;
}

CBattleFleet *
CBattleFleet::get_least_ERT_engaged_fleet(CBattleFleetList *aEnemyList, int aRange)
{
	CBattleFleet *
	ReturnFleet = NULL;
	int
		LeastERT = 10000;

	for (int i=0 ; i<aEnemyList->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aEnemyList->get(i);
		if (Fleet->is_disabled() == true) continue;
		if (Fleet->is_engaged() == false) continue;

		CBattleFleet *
		Enemy = Fleet->get_enemy();
		if (Enemy != NULL)
		{
			if (distance((CVector *)Enemy) > aRange)
			{
				continue;
			}
		}
		else
		{
			if (distance((CVector *)Fleet) > aRange)
				continue;
		}

		int
			ERT = calc_effective_reach_time((CVector *)Fleet);
		if (ERT < LeastERT)
		{
			LeastERT = ERT;
			ReturnFleet = Fleet;
		}
	}

	return ReturnFleet;
}

CBattleFleet *
CBattleFleet::get_least_ERT_fleet(CBattleFleetList *aEnemyList)
{
	CBattleFleet *
	ReturnFleet = NULL;
	int
		LeastERT = 10000;

	for (int i=0 ; i<aEnemyList->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aEnemyList->get(i);
		if (Fleet->is_disabled() == true) continue;

		int
			ERT = calc_effective_reach_time((CVector *)Fleet);
		if (ERT < LeastERT)
		{
			LeastERT = ERT;
			ReturnFleet = Fleet;
		}
	}

	return ReturnFleet;
}

float
	CBattleFleet::calc_PA_float( float aValue, int aEffect )
{
	int
		P = 0,
	A = 0;

	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_target() != CFleetEffect::LOCAL_EFFECT) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}
	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}

	return aValue*(100+P)/100+A;
}

float
	CBattleFleet::calc_minus_PA_float( float aValue, int aEffect )
{
	int
		P = 0,
	A = 0;

	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_target() != CFleetEffect::LOCAL_EFFECT) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}
	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}

	return aValue*(100-P)/100-A;
}

int
	CBattleFleet::calc_PA( int aValue, int aEffect )
{
	int
		P = 0,
	A = 0;

	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_target() != CFleetEffect::LOCAL_EFFECT) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}
	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}

	return aValue*(100+P)/100+A;
}

int
	CBattleFleet::calc_minus_PA( int aValue, int aEffect )
{
	int
		P = 0,
	A = 0;

	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_target() != CFleetEffect::LOCAL_EFFECT) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}
	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}

	return aValue*(100-P)/100-A;
}

int
	CBattleFleet::calc_0base_PA( int aValue, int aEffect )
{
	int
		P = 0,
	A = 0;

	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_target() != CFleetEffect::LOCAL_EFFECT) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}
	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);

		if( Effect->get_type() != aEffect ) continue;
		if( Effect->ready() == false ) continue;
		if( Effect->get_apply_type() == CFleetEffect::AT_ABSOLUTE )
			A += Effect->get_amount();
		else
			P += Effect->get_amount();
		Effect->use();
	}

	return aValue*P/100+A;
}

bool
	CBattleFleet::has_effect( int aEffect )
{
	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);

		if( Effect->get_type() == aEffect ) return true;
	}

	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);

		if( Effect->get_type() == aEffect ) return true;
	}

	return false;
}

bool
	CBattleFleet::under_area_effect()
{
	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);

		if( Effect->get_target() == CFleetEffect::AE_LOCAL_EFFECT )
			return true;
	}

	return false;
}

int
	CBattleFleet::get_effect_amount(int aEffect)
{
	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);

		if( Effect->get_type() == aEffect ) return Effect->get_amount();
	}

	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);

		if( Effect->get_type() == aEffect ) return Effect->get_amount();
	}
	return 0;
}

bool CBattleFleet::is_stealthed() {
    return has_effect(CFleetEffect::FE_STEALTH);
}

bool
	CBattleFleet::can_see(CBattleFleet *aEnemy )
{
	if (aEnemy->is_full_cloaked() && !mAttribute.has(COMPLETE_CLOAKING_DETECTION)) return false;

	if (aEnemy->is_weak_cloaked() && !mAttribute.has(WEAK_CLOAKING_DETECTION) && !mAttribute.has(COMPLETE_CLOAKING_DETECTION) ) return false;

	int
		Distance = this->distance((CVector *)aEnemy);
	int
		EffectiveDistance = Distance * (100 + (5 - aEnemy->get_size())*5) / 100;
	if (aEnemy->is_stealthed())
	{
		EffectiveDistance = aEnemy->calc_PA(EffectiveDistance, CFleetEffect::FE_STEALTH);
	}
	if (EffectiveDistance > get_detection_range())
	{
		return false;
	}
	return true;
}

void
	CBattleFleet::recharge_shield()
{
	int
		Recharge = mShieldRechargeRate;
	Recharge = calc_PA( Recharge, CFleetEffect::FE_SHIELD_RECHARGE_RATE );

	for( int i = 0; i < mMaxShip; i++ ){
		if( mHP[i] <= 0 ) continue;
		mShieldStrength[i] += Recharge;
		if( mShieldStrength[i] > mShieldMaxStrength )
			mShieldStrength[i] = mShieldMaxStrength;
	}
}

void
	CBattleFleet::repair_in_battle()
{
	if( has_effect( CFleetEffect::FE_NON_REPAIRABLE ) ) return;

	int
		Repair = calc_0base_PA( mMaxHP, CFleetEffect::FE_REPAIR );
	Repair = calc_PA( Repair, CFleetEffect::FE_REPAIR_SPEED );

	for( int i = 0; i < mMaxShip; i++ ){
		if( mHP[i] <= 0 ) continue;
		mHP[i] += Repair;
		if( mHP[i] > mMaxHP )
			mHP[i] = mMaxHP;
	}
}

int
	CBattleFleet::get_efficiency()
{
	int
		Efficiency = mCommander->get_efficiency();

	Efficiency = calc_PA( Efficiency, CFleetEffect::FE_EFFICIENCY );

	return Efficiency;
}

void
	CBattleFleet::charge_effect()
{
	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);
		Effect->charge();
	}
	for( int i = 0; i < mDynamicEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mDynamicEffectList.get(i);
		Effect->charge();
	}
}

CFleetEffect*
CBattleFleet::get_area_effect()
{
	for( int i = 0; i < mStaticEffectList.length(); i++ ){
		CFleetEffect
			*Effect = (CFleetEffect*)mStaticEffectList.get(i);
		if( Effect->get_target() != CFleetEffect::LOCAL_EFFECT )
			return Effect;
	}

	return NULL;
}

void
	CBattleFleet::change_hp( int i, int aHP )
{
	mHP[i] += aHP;
	if( mHP[i] > mMaxHP ) mHP[i] = mMaxHP;
	if( mHP[i] < 0 ) mHP[i] = 0;
}

void
	CBattleFleet::change_shield_strength( int i, int aShield )
{
	mShieldStrength[i] += aShield;
	if( mShieldStrength[i] > mShieldMaxStrength )
		mShieldStrength[i] = mShieldMaxStrength;
	if( mShieldStrength[i] < 0 )
		mShieldStrength[i] = 0;
}

void
	CBattleFleet::damage( CTurret *aTurret, CBattleFleet *aEnemy, int aHitChance, int aCritChance, int *aTotalDamage, int *aHitCount, int *aMissCount, int *aSunkenCount )
{
	int
		ActiveShip = count_active_ship();

	CFleet *
	OriginalFleet = this->get_fleet();
	CPlayer *
	OriginalOwner = PLAYER_TABLE->get_by_game_id(OriginalFleet->get_owner());

	for( int i = 0; i < ActiveShip; i++ )
	{
		if( aEnemy->is_disabled() ) break;

		bool
			PSIIgnore = false;
		if ((aEnemy->has_effect(CFleetEffect::FE_PSI_NEUTRALIZATION_FIELD)) && (aTurret->has_effect(CFleetEffect::WE_PSI)))
		{
			if (number(100) <= aEnemy->calc_PA(0, CFleetEffect::FE_PSI_NEUTRALIZATION_FIELD))
				PSIIgnore = true;
		}
		
		//SLOG("Pure hit chance is %d.", aHitChance);
		// Missile type commander bonus to hit chance
		if (is_full_cloaked()) {
            aHitChance = (int) ((double) 10 * (double) pow((double) aHitChance, (double) 0.5));
            //SLOG("Stealth hit chance is %d.", aHitChance);
		}

		if (number(100) <= aHitChance && PSIIgnore == false)
		{	// hit
			int
				Dam = aTurret->get_number() * dice( aTurret->get_damage_roll(), aTurret->get_damage_dice() );
			switch( aTurret->get_weapon_type() )
			{
				case WT_BEAM :
					Dam = calc_PA( Dam, CFleetEffect::FE_BEAM_DAMAGE );
					Dam = aEnemy->calc_minus_PA( Dam, CFleetEffect::FE_BEAM_DEFENSE );
					break;
				case WT_MISSILE :
					Dam = calc_PA( Dam, CFleetEffect::FE_MISSILE_DAMAGE );
					Dam = aEnemy->calc_minus_PA( Dam, CFleetEffect::FE_MISSILE_DEFENSE );
					break;
				case WT_PROJECTILE :
					Dam = calc_PA( Dam, CFleetEffect::FE_PROJECTILE_DAMAGE );
					Dam = aEnemy->calc_minus_PA( Dam, CFleetEffect::FE_PROJECTILE_DEFENSE );
					break;
			}
			Dam = aEnemy->calc_minus_PA( Dam, CFleetEffect::FE_GENERIC_DEFENSE );
			Dam = aEnemy->calc_PA( Dam, CFleetEffect::FE_CHAIN_REACTION );
			
			if (is_stealthed())
			{
				Dam = (int)((double)Dam * 1.30);
			}

			if (OriginalOwner->has_ability(ABILITY_STEALTHED_AMBUSH) && is_stealthed())
			{
				Dam = (int)((double)Dam * 1.30);
			}
			
			if (is_weak_cloaked() || is_full_cloaked()) {
                Dam = (int)((double)Dam * 1.30);
            }

/*			if( is_psi_race() && aTurret->has_effect( CFleetEffect::WE_PSI_EMPOWER ) )
				Dam *= 2; */

			if ((has_enhanced_psi()) && (aTurret->has_effect( CFleetEffect::WE_PSI )) )
				Dam = (int)((double)Dam * 1.30);

			if (aTurret->has_effect(CFleetEffect::WE_PSI) && OriginalOwner->has_ability(ABILITY_PSI))
			{
				//SLOG ("Raising Psi damage: %d", Dam);
				if (aEnemy->get_effect_amount(CFleetEffect::FE_PSI_DEFENSE) > get_effect_amount(CFleetEffect::FE_PSI_ATTACK)) Dam = calc_PA( Dam, CFleetEffect::FE_PSI_DAMAGE / 2);
				else Dam = calc_PA( Dam, CFleetEffect::FE_PSI_DAMAGE);
				//SLOG ("Raising Psi new damage: %d", Dam);
			}
			else if (aTurret->has_effect(CFleetEffect::WE_PSI) && !OriginalOwner->has_ability(ABILITY_PSI))
			{
				//SLOG ("Raising Psi non-psi race damage: %d", Dam);
				if (aEnemy->get_effect_amount(CFleetEffect::FE_PSI_DEFENSE) > get_effect_amount(CFleetEffect::FE_PSI_ATTACK)) Dam = calc_PA( Dam, CFleetEffect::FE_PSI_DAMAGE / 4);
				else Dam = calc_PA( Dam, CFleetEffect::FE_PSI_DAMAGE / 2);
				//SLOG ("Raising Psi non-psi race new damage: %d", Dam);
			}

			int
				ShieldPierceChance,
				// ---Change--- //
				ShieldDistortionChance;
				// ---End Change--- //
			bool
				ShieldDistorted,
				ShieldPierced;

			ShieldDistorted = false;

			ShieldPierceChance = aTurret->effect_amount( CFleetEffect::WE_SHIELD_PIERCING );
			ShieldDistortionChance = aTurret->effect_amount( CFleetEffect::WE_SHIELD_DISTORTION );

			
            if (is_stealthed()) {
                ShieldPierceChance += 30;
				ShieldDistortionChance += 15;
			}
            /*
			if (OriginalOwner->has_ability(ABILITY_STEALTHED_AMBUSH) && is_stealthed() && (is_weak_cloaked() || is_full_cloaked())) {
                ShieldPierceChance += 50;
				ShieldDistortionChance += 25;
			} */

			if (is_weak_cloaked() || is_full_cloaked()) {
                ShieldPierceChance += 30;
                ShieldDistortionChance += 15;
            }

            ShieldDistortionChance = aEnemy->calc_minus_PA( ShieldDistortionChance, CFleetEffect::FE_SHIELD_INTEGRITY );
            ShieldPierceChance = aEnemy->calc_minus_PA( ShieldPierceChance, CFleetEffect::FE_IMPENETRABLE_SHIELD );

			if( number(100) < ShieldPierceChance )
				ShieldPierced = true;
			else
				ShieldPierced = false;

			if( number(100) < ShieldDistortionChance )
			{
				ShieldDistorted = true;

			}
			else if ( (ShieldDistorted == false) && (mCommander->get_racial_ability() == CAdmiral::RA_SHIELD_DISRUPTER) )
			{
				ShieldDistortionChance = (mCommander->get_level() * 19 / 4);
				ShieldDistortionChance = aEnemy->calc_minus_PA( ShieldDistortionChance, CFleetEffect::FE_SHIELD_INTEGRITY );
				if ( number(100) < ShieldDistortionChance ) {
					ShieldDistorted = true;
				}
			}

			else
				ShieldDistorted = false;

            // Complete Armor Piercing
            if (aTurret->has_effect(CFleetEffect::WE_COMPLETE_ARMOR_PIERCING)) {
                aCritChance = 100;
            } 
            
            if (aTurret->has_effect(CFleetEffect::WE_COMPLETE_SHIELD_PIERCING)) {
                ShieldPierced = true;
            }
            
            if (aTurret->has_effect(CFleetEffect::WE_COMPLETE_SHIELD_DISTORTION)) {
                ShieldDistorted = true;
            }

			// Nullify Shield Distortion
			if (aEnemy->has_effect(CFleetEffect::FE_NUL_S_DISTORTION)) {
                ShieldDistorted = false;
            }
            
            // Nullify Shield Pierce
            if (aEnemy->has_effect(CFleetEffect::FE_NUL_S_PIERCE)) {
                ShieldPierced = false;
            }

            // Nullify Armor Pierce
            if (aEnemy->has_effect(CFleetEffect::FE_NUL_A_PIERCE)) {
                aCritChance = 0;
            }
            
            // Death ignores shields
            if (aTurret->has_effect(CFleetEffect::WE_DEATH)) {
                ShieldDistorted = true;
            }

			int
				ShieldSolidity = aEnemy->get_shield_solidity();
			if( ShieldPierced ) ShieldSolidity /= 2;


			if( Dam <= 0 )
			{
				(*aMissCount)++;
				continue;
			}

			int
				Index = -1;
			if( number(100) <= (get_efficiency() * .5) )
			{
				for( int i = 0; i < aEnemy->get_max_ship(); i++ )
				{
					if( aEnemy->get_hp(i) > 0 )
					{
						Index = i;
						break;
					}
				}
			}
			else
			{
				int
					Nth = number( aEnemy->count_active_ship() );
				for( int i = 0; i < aEnemy->get_max_ship(); i++ )
				{
					if( aEnemy->get_hp(i) > 0 )
					{
						Nth--;
						if( Nth == 0 )
						{
							Index = i;
							break;
						}
					}
				}
			}

			int
				RealDam = 0;

			if( ShieldDistorted == true ){
				int
					BioBonus = aTurret->effect_amount( CFleetEffect::WE_ADDITIONAL_DAMAGE_TO_BIO_ARMOR );

				if( aEnemy->get_armor()->get_armor_type() == AT_BIO )
					Dam = (100+BioBonus)*Dam/100;

				if( number(100) < aCritChance )
					Dam = (int)(2.0*aEnemy->get_armor()->get_hp_multiplier()*Dam);
					
				if (aTurret->has_effect(CFleetEffect::WE_DEATH)) {
                    Dam = aEnemy->get_hp(Index);
                }

				if( Dam < aEnemy->get_hp(Index) )
				{
					*aTotalDamage += Dam;
					RealDam = Dam;
				}
				else
				{
					RealDam = aEnemy->get_hp(Index);
					*aTotalDamage += RealDam;
				}

				aEnemy->change_hp( Index, -Dam );
			} else {
				if( ShieldPierced == true )
					Dam *= 2;
				if( aTurret->has_effect( CFleetEffect::WE_SHIELD_OVERHEAT ) )
					Dam *= 3;

				if ((float)aEnemy->get_shield_strength(Index) / (float)100 > .10f)
					Dam -= (int)(Dam * 0.03f * aEnemy->get_shield_solidity());

				if( aEnemy->get_shield_strength(Index) < Dam ){
					Dam -= aEnemy->get_shield_strength(Index);
					*aTotalDamage += aEnemy->get_shield_strength(Index);
					aEnemy->change_shield_strength( Index, -aEnemy->get_shield_strength(Index) );

					if( ShieldPierced == true ) Dam /= 2;
					if( aTurret->has_effect( CFleetEffect::WE_SHIELD_OVERHEAT ) )
						Dam /= 3;

					int
						BioBonus = aTurret->effect_amount( CFleetEffect::WE_ADDITIONAL_DAMAGE_TO_BIO_ARMOR );

					if( aEnemy->get_armor()->get_armor_type() == AT_BIO )
						Dam = (100+BioBonus)*Dam/100;
					if( aTurret->has_effect(CFleetEffect::WE_CORROSIVITY) )
						Dam = (int)((double)Dam * (1.0 + (double)aTurret->effect_amount(CFleetEffect::WE_CORROSIVITY) / (double)100));
					if( number(100) < aCritChance )
						Dam = (int)(2.0*aEnemy->get_armor()->get_hp_multiplier()*Dam);

					if( Dam < aEnemy->get_hp(Index) )
					{
						*aTotalDamage += Dam;
						RealDam = Dam;
					}
					else
					{
						RealDam = aEnemy->get_hp(Index);
						*aTotalDamage += RealDam;
					}
					aEnemy->change_hp( Index, -Dam );
				} else {
					*aTotalDamage += Dam;
					aEnemy->change_shield_strength( Index, -Dam );
				}
			}

			float
				MoraleDrop = (*aTotalDamage*100.0)/aEnemy->get_total_hp();

			// SLOG ("MORALE TEST: Fleet: %d, aTotalDamage: %d , Enemy_hp: %d , MoraleDrop: %f", aEnemy ->get_real_id(), *aTotalDamage, aEnemy->get_total_hp(), MoraleDrop);

			if( aTurret->has_effect( CFleetEffect::WE_PSI ) ){
				float
					PSIMoraleDrop = MoraleDrop;

				PSIMoraleDrop = calc_PA_float( PSIMoraleDrop, CFleetEffect::FE_PSI_ATTACK );
				PSIMoraleDrop = aEnemy->calc_minus_PA_float( PSIMoraleDrop, CFleetEffect::FE_PSI_DEFENSE );
				if (OriginalOwner->has_ability(ABILITY_PSI)) MoraleDrop += (PSIMoraleDrop * 2);
				else MoraleDrop += (PSIMoraleDrop);
				// SLOG ("PSI MORALE TEST: Fleet: %d, aTotalDamage: %d , Enemy_hp: %d , MoraleDrop: %f", aEnemy ->get_real_id(), *aTotalDamage, aEnemy->get_total_hp(), MoraleDrop);


			}

			aEnemy->change_morale( -MoraleDrop );

			change_morale( (*aTotalDamage*100/get_total_hp())/3 );

			if( aEnemy->get_hp(Index) == 0 ){
				int
					Exp = 0;
				change_morale( 1 );
				aEnemy->change_morale( -2 );
				Exp += aEnemy->get_size()*aEnemy->get_size()*aEnemy->get_size()*3;
				if( aEnemy->count_active_ship() <= 0 ){
					Exp += aEnemy->get_original_power();
					aEnemy->set_status( STATUS_ANNIHILATED_THIS_TURN );
				}
				if( aEnemy->is_capital_fleet() )
					Exp *= 3;
				if (OriginalOwner->has_ability(ABILITY_ADVANCED_BATTLE_PROCESSING))
				{
					Exp = (int)(3 * (double)Exp);
				}
				mAdmiralExp += Exp;
				(*aSunkenCount)++;
			}
			(*aHitCount)++;
		} else {	// fail
			(*aMissCount)++;
		}
	}
}

CFormationPoint::CFormationPoint()
{
	mFleetID = 0;
}

CFormationPoint::CFormationPoint( CBattleFleet *aFleet )
{
	mFleetID = aFleet->get_id();
	*(CVector*)this = *(CVector*)aFleet;
}

CFormationPoint::~CFormationPoint()
{
}

CFormationInfo::CFormationInfo()
{
	mSide = 0;
}

CFormationInfo::CFormationInfo(int aSide)
{
	mSide = aSide;
}

CFormationInfo::~CFormationInfo()
{
	remove_all();
}

bool
	CFormationInfo::free_item( TSomething aItem )
{
	CFormationPoint
		*Fleet = (CFormationPoint*)aItem;
	if( !Fleet ) return false;

	delete Fleet;

	return true;
}

int
	CFormationInfo::compare( TSomething aItem1, TSomething aItem2) const
{
	CFormationPoint
		*fleet1 = (CFormationPoint*)aItem1,
	*fleet2 = (CFormationPoint*)aItem2;

	if ( fleet1->get_id() > fleet2->get_id() ) return 1;
	if ( fleet1->get_id() < fleet2->get_id() ) return -1;
	return 0;
}

int
	CFormationInfo::compare_key( TSomething aItem, TConstSomething aKey) const
{
	CFormationPoint *
	fleet = (CFormationPoint*)aItem;

	if ( fleet->get_id() > (int)aKey ) return 1;
	if ( fleet->get_id() < (int)aKey ) return -1;
	return 0;
}

int
	CFormationInfo::add_formation_point(CFormationPoint *aPoint)
{
	if (!aPoint ) return -1;

	if (find_sorted_key( (TConstSomething)aPoint->get_id() ) >= 0) return -1;
	insert_sorted( aPoint );

	return aPoint->get_id();
}

CBattleFleetList::CBattleFleetList() {
    mOwner = NULL;
    mSide = 0;
    mFormationStatus = 0;
    mFormationSpeed = 0;
    (void)mTotalPower;
    mCapitalFleet = NULL;
}

void CBattleFleetList::init(CPlayer *aOwner, int aSide) {
    mOwner = aOwner;
    mSide = aSide;
    mFormationInfo.set_side(aSide);
}

CBattleFleetList::~CBattleFleetList()
{
	mOwner = NULL;
	remove_all();
}

bool
	CBattleFleetList::free_item( TSomething aItem )
{
	CBattleFleet
		*Fleet = (CBattleFleet*)aItem;
	if( !Fleet ) return false;

	delete Fleet;

	return true;
}

int
	CBattleFleetList::compare( TSomething aItem1, TSomething aItem2) const
{
	CBattleFleet
		*fleet1 = (CBattleFleet*)aItem1,
	*fleet2 = (CBattleFleet*)aItem2;

	if ( fleet1->get_id() > fleet2->get_id() ) return 1;
	if ( fleet1->get_id() < fleet2->get_id() ) return -1;
	return 0;
}

int
	CBattleFleetList::compare_key( TSomething aItem, TConstSomething aKey) const
{
	CBattleFleet *
	fleet = (CBattleFleet*)aItem;

	if (fleet->get_id() > (int)aKey) return 1;
	if (fleet->get_id() < (int)aKey) return -1;
	return 0;
}

bool
	CBattleFleetList::remove_battle_fleet(int aBattleFleetID)
{
	int
		index;
	index = find_sorted_key( (void*)aBattleFleetID );
	if (index < 0) return false;

	return CSortedList::remove(index);
}

int
	CBattleFleetList::add_battle_fleet(CBattleFleet *aBattleFleet)
{
	if (!aBattleFleet) return -1;

	if (find_sorted_key( (TConstSomething)aBattleFleet->get_id() ) >= 0) return -1;
	insert_sorted( aBattleFleet );

	return aBattleFleet->get_id();
}

void
	CBattleFleetList::add_formation_info( CBattleFleet *aBattleFleet )
{
	CFormationPoint
		*FP = new CFormationPoint(aBattleFleet);

	mFormationInfo.add_formation_point( FP );
}

CBattleFleet*
CBattleFleetList::get_by_id(int aBattleFleetID)
{
	if ( !length() ) return NULL;

	int
		index;

	index = find_sorted_key( (void*)aBattleFleetID );

	if (index < 0) return NULL;

	return (CBattleFleet*)get(index);
}

int
	CBattleFleetList::get_new_id()
{
	int
		NewID = 1;

	for (int i=0 ; i<length() ; i++)
	{
		CBattleFleet *
		BattleFleet = (CBattleFleet *)get(i);

		if (BattleFleet->get_id() == NewID)
		{
			NewID++;
			continue;
		}

		return NewID;
	}

	return NewID;
}

void
	CBattleFleetList::auto_deployment(CBattle *aBattle, CFleetList *aFleetList, CAdmiralList *aAdmiralList)
{
    SLOG("battle.cc : auto_deployment : called!");
	mCapitalFleet = NULL;
	int
		CapitalFleetShips = 0;

	CBattleRecord *
	BattleRecord = aBattle->get_record();
	int
		BattleFleetNumber = 0;

	for (int i=0 ; i<aFleetList->length(); i++ )
	{
		if (BattleFleetNumber >= 20) break;

		CFleet *
		Fleet = (CFleet *)aFleetList->get(i);

		if (mSide == CBattle::SIDE_DEFENSE)
		{
			if ((Fleet->get_status() != CFleet::FLEET_STAND_BY)) continue;
			//if ((Fleet->get_status() != CFleet::FLEET_STAND_BY) && (Mission.get_mission() != CMission::MISSION_RETURNING)) continue;
			if (Fleet->get_current_ship() == 0) continue;
		}

		CBattleFleet *
		BattleFleet = new CBattleFleet();

		BattleFleet->init(aBattle, mOwner, Fleet, aAdmiralList, get_new_id());

		if (mCapitalFleet == NULL)
		{
			mCapitalFleet = BattleFleet;
			CapitalFleetShips = Fleet->get_current_ship();
		}
		else
		{
			if (mCapitalFleet->get_fleet()->get_size() < Fleet->get_size())
			{
				mCapitalFleet = BattleFleet;
				CapitalFleetShips = Fleet->get_current_ship();
			}
			else if (mCapitalFleet->get_fleet()->get_size() == Fleet->get_size())
			{
				if (mCapitalFleet->get_fleet()->get_current_ship() < Fleet->get_current_ship())
				{
					mCapitalFleet = BattleFleet;
					CapitalFleetShips = Fleet->get_current_ship();
				}
			}
		}

		add_battle_fleet(BattleFleet);
		BattleFleetNumber++;
	}

	if (length() == 0) return;

	mCapitalFleet->set_capital_fleet();

	int
		Interval = 6000 / length(),
	DeployedFleetNumber = 0;

	if (get_side() == CBattle::SIDE_OFFENSE)
	{
		for (int i=0 ; i<length() ; i++)
		{
			CBattleFleet *
			BattleFleet = (CBattleFleet *)get(i);
			if (BattleFleet->is_capital_fleet())
			{
				BattleFleet->set_vector(1000, 5000, 0);
			} else
			{
				if (2000+Interval*(DeployedFleetNumber + 1) == 5000)
				{
					DeployedFleetNumber++;
				}
				BattleFleet->set_vector(2500, 2000+Interval*(DeployedFleetNumber + 1), 0);
				DeployedFleetNumber++;
			}
			BattleRecord->add_fleet(BattleFleet);
		}
	} else
	{
		for (int i=0 ; i<length() ; i++)
		{
			CBattleFleet *
			BattleFleet = (CBattleFleet *)get(i);
			if (BattleFleet->is_capital_fleet())
			{
				BattleFleet->set_vector(9000, 5000, 180);
			} else
			{
				if (2000+Interval*(DeployedFleetNumber + 1) == 5000)
				{
					DeployedFleetNumber++;
				}
				BattleFleet->set_vector(7500, 2000+Interval*(DeployedFleetNumber + 1), 180);
				DeployedFleetNumber++;
			}

			BattleRecord->add_fleet(BattleFleet);
		}
	}
	
	SLOG("battle.cc : auto_deployment : returned!");
}

bool
	CBattleFleetList::lost()
{
	bool
		is_lost = true;

	for( int i = 0; i < length(); i++ ){
		CBattleFleet
			*Fleet = (CBattleFleet*)get(i);

		if( Fleet->is_disabled() == false )
			is_lost = false;
	}

	return is_lost;
}

int
	CBattleFleetList::get_detected_fleet()
{
	// Not used.

	for( int i = 0; i < length(); i++ ){
		/*CBattleFleet
			*Fleet = (CBattleFleet*)get(i);

		if( Fleet->is_detected() ) return i;*/
	}

	return -1;
}

void
	CBattleFleetList::set_formation_speed()
{
	mFormationSpeed = 10000;

	for (int i=0 ; i<length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)get(i);

		if (Fleet->get_command() != CBattleFleet::COMMAND_FORMATION) continue;
		if (Fleet->get_speed() < mFormationSpeed)
		{
			mFormationSpeed = Fleet->get_speed();
		}
	}
}

void
	CBattleFleetList::update_fleet_after_battle(CPlayer *aEnemy, int aWarType, bool aWin)
{
	CAllyFleetList *
	AllyFleetList = mOwner->get_ally_fleet_list();

	int
		ScrapPP = 0;

	for (int i=0 ; i<length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)get(i);
		CFleet *
		OriginalFleet = Fleet->get_fleet();
		CPlayer *
		OriginalOwner = PLAYER_TABLE->get_by_game_id(OriginalFleet->get_owner());
		CAdmiral *
		Commander = Fleet->get_admiral();

		if (OriginalOwner->get_game_id() != EMPIRE_GAME_ID)
		{
			Fleet->repair(20);

			if( Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN ||
				Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED )
			{
				// query : delete admiral and fleet
				int
					Chance = 0;
				bool
					Dead = true;

				if (mOwner->get_game_id() == EMPIRE_GAME_ID)
				{
					Chance = Commander->get_level()*1 + 5;
				}
				else
				{
					Chance = Commander->get_level()*1 +
						mOwner->get_race_data()->get_survival_modifier();
				}
				if (aWin) Chance += 40;
				else Chance += 0;
				Chance = Fleet->calc_PA(Chance, CFleetEffect::FE_COMMANDER_SURVIVAL);

				if (Chance >= number(100)) // survive
					Dead = false;

				if( Dead && Commander->get_racial_ability() ==
					CAdmiral::RA_CLONAL_DOUBLE )
				{
					if( Commander->get_level() <= 7 )
						Chance = 50;
					else if( Commander->get_level() <= 13 )
						Chance = 75;
					else if( Commander->get_level() <= 19 )
						Chance = 85;
					else if( Commander->get_level() == 20 )
						Chance = 90;
					if( Chance >= number(100) ) // survive
						Dead = false;
				} /*
                if (OriginalOwner->get_tech_list()->get_by_id(1536) != NULL && OriginalOwner->get_tech_list()->get_by_id(1537) != NULL) {
                    Dead = false;
                }*/
				// send news
				if (OriginalOwner == mOwner && Dead == true)
				{
					mOwner->time_news(
						(char *)format(GETTEXT("You lost the fleet %1$s and commander %2$s."),
							OriginalFleet->get_nick(), Commander->get_nick()));
				}
				else if(OriginalOwner->is_dead() == false && Dead == true && OriginalOwner != mOwner)
				{
					mOwner->time_news(
						(char *)format(GETTEXT("You lost the fleet %1$s and the commander %2$s whose owner is %3$s."),
							OriginalFleet->get_nick(),
							Commander->get_name(),
							OriginalOwner->get_nick()));

					OriginalOwner->time_news(
						(char *)format(GETTEXT("You have lost fleet %1$s and commander %2$s in %3$s's battle."),
							OriginalFleet->get_nick(),
							Commander->get_name(),
							mOwner->get_nick()));
				}
				else if(OriginalOwner == mOwner && Dead == false)
				{
					mOwner->time_news(
						(char *)format(GETTEXT("You lost the fleet %1$s whose owner is %2$s."),
							OriginalFleet->get_nick(),
							OriginalOwner->get_nick()));
				}
				else if(OriginalOwner->is_dead() == false && Dead == false && OriginalOwner != mOwner)
				{
					mOwner->time_news(
						(char *)format(GETTEXT("You lost the fleet %1$s whose owner is %2$s."),
							OriginalFleet->get_nick(),
							OriginalOwner->get_nick()));

					OriginalOwner->time_news(
						(char *)format(GETTEXT("You have lost fleet %1$s in %2$s's battle."),
							OriginalFleet->get_nick(),
							mOwner->get_nick()));
				}

				CFleetList *
				OriginalOwnerFleetList = OriginalOwner->get_fleet_list();

				if (Dead)
				{
					if (OriginalOwner->has_ability(ABILITY_DOWNLOADABLE_COMMANDER_EXPERIENCE))
						Commander->distribute_exp();

					Commander->type(QUERY_DELETE);
					*STORE_CENTER << *Commander;
					OriginalOwner->get_admiral_list()->remove_admiral( Commander->get_id() );
					OriginalFleet->type(QUERY_DELETE);
					*STORE_CENTER << *OriginalFleet;

					if (OriginalOwner->get_game_id() != mOwner->get_game_id())
					{
						AllyFleetList->remove_fleet(OriginalOwner, OriginalFleet->get_id());
					}
					OriginalOwnerFleetList->remove_fleet(OriginalFleet->get_id());
				}
				else
				{
					int
						DesignID = OriginalFleet->get_design_id();
					CDock *
					OriginalOwnerDock = OriginalOwner->get_dock();
					int
						ShipTypeCount = OriginalOwnerDock->count_ship(DesignID);

					int OriginalFleetMaxShip = OriginalFleet->get_max_ship();

					if (ShipTypeCount >= OriginalFleetMaxShip)
					{
						OriginalFleet->end_mission();
						OriginalFleet->set_current_ship(OriginalFleetMaxShip);
						OriginalOwnerDock->change_ship((CShipDesign *)OriginalFleet, -OriginalFleetMaxShip);

						Commander->type(QUERY_UPDATE);
						*STORE_CENTER << *Commander;
						OriginalFleet->type(QUERY_UPDATE);
						*STORE_CENTER << *OriginalFleet;
					}
					else
					{
						//add
						Commander->set_fleet_number(0);
						OriginalOwner->get_admiral_list()->remove_without_free_admiral( Commander->get_id() );
						OriginalOwner->get_admiral_pool()->add_admiral(Commander);
						Commander->type(QUERY_UPDATE);
						*STORE_CENTER << *Commander;
						OriginalFleet->type(QUERY_DELETE);
						*STORE_CENTER << *OriginalFleet;

						if (OriginalOwner != mOwner)
						{
							AllyFleetList->remove_fleet(OriginalOwner, OriginalFleet->get_id());
						}
						OriginalOwnerFleetList->remove_fleet(OriginalFleet->get_id());
					}
				}
			}
			else
			{
				// move all damaged ship to repair bay
				//Fleet->reorganize_after_battle( mOwner );
				Fleet->reorganize_after_battle( OriginalOwner );
				// update exp of admiral and fleet
				int
					FleetExp = 10 + Fleet->get_killed_ship() +
					Fleet->get_killed_fleet()*5;
				if (aWarType != CBattle::WAR_PRIVATEER)
				{
					if (aWin)
					{
						OriginalFleet->change_exp( FleetExp );
					}
					else
					{
						OriginalFleet->change_exp( FleetExp/2 );
					}
				}

				OriginalFleet->change_killed_ship( Fleet->get_killed_ship() );
				OriginalFleet->change_killed_fleet( Fleet->get_killed_fleet() );

				if (aWarType != CBattle::WAR_PRIVATEER)
				{
					if (aWin)
					{
						Fleet->get_admiral()->gain_exp( Fleet->get_admiral_exp() );
					}
					else
					{
						Fleet->get_admiral()->gain_exp( Fleet->get_admiral_exp()/4 );
					}
				}
			}
		}

		/* For Tecanoid Races */
		if (aEnemy->get_game_id() != EMPIRE_GAME_ID &&
			aEnemy->has_ability(ABILITY_SCAVENGER))
		{
			int
				DestroyedShip = Fleet->get_max_ship() - Fleet->count_active_ship();

			for (int i=0 ; i<DestroyedShip ; i++)
			{
				int
					Chance = number(100);
				if (Chance <= 20)
				{
					CShipSize *
					Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(Fleet->get_body());
					if (OriginalOwner->get_game_id() == EMPIRE_GAME_ID)
					{
						ScrapPP += Body->get_cost() * 3 / 20;
					}
					else
					{
						ScrapPP += Body->get_cost();
					}
				}
			}

			if (ScrapPP > MAX_PLAYER_PP || ScrapPP < 0)
			{
				ScrapPP = MAX_PLAYER_PP;
			}
		}
	}

	if (ScrapPP > 0)
	{
		aEnemy->change_reserved_production(ScrapPP);
		aEnemy->time_news(
			(char *)format(GETTEXT("You have earned %1$s PP salvaging enemy ships."),
				dec2unit(ScrapPP)));
		if (aEnemy->has_ability(ABILITY_TEMPORAL_MASTERY)) {
		    aEnemy->change_ship_production(ScrapPP/5);
		    aEnemy->time_news(
			(char *)format(GETTEXT("%1$s PP has been put into the ship production pool."),
				dec2unit(ScrapPP/10)));
		}

	}
}

void CBattleFleetList::update_empire_fleet_after_battle(CPlayer *aEnemy, int aWarType, bool aWin)
{
//	CPlayer *OriginalOwner = EMPIRE;
	CEmpireFleetInfoList *EmpireFleetInfoList =  EMPIRE->get_empire_fleet_info_list();
	CEmpireAdmiralInfoList *EmpireAdmiralInfoList = EMPIRE->get_empire_admiral_info_list();

	for (int i=0 ; i<length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)get(i);
		CFleet *
		OriginalFleet = Fleet->get_fleet();
		CAdmiral *
		Commander = Fleet->get_admiral();
		CEmpireFleetInfo *EmpireFleetInfo = EmpireFleetInfoList->get_by_fleet_id(OriginalFleet->get_id());
		CEmpireAdmiralInfo *EmpireAdmiralInfo = EmpireAdmiralInfoList->get_by_admiral_id(Commander->get_id());

		if (EmpireFleetInfo == NULL || EmpireAdmiralInfo == NULL)
		{
			SLOG("Error: Missing Empire Fleet/Admiral Info in CBattleFleetList::update_empire_fleet_after_battle() -- doing nothing!");
			continue;
		}
		if( Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN ||
			Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED )
		{
			switch (EmpireFleetInfo->get_fleet_type())
			{
				case CEmpire::LAYER_MAGISTRATE :
                                                     // Not handled yet
					break;
				case CEmpire::LAYER_FORTRESS :
                                                     // Not handled yet
					break;
				case CEmpire::LAYER_EMPIRE_CAPITAL_PLANET :
					Commander->type(QUERY_DELETE);
					OriginalFleet->type(QUERY_DELETE);
					EmpireFleetInfo->type(QUERY_DELETE);
					EmpireAdmiralInfo->type(QUERY_DELETE);

					STORE_CENTER->store(*EmpireFleetInfo);
					STORE_CENTER->store(*EmpireAdmiralInfo);
					STORE_CENTER->store(*Commander);
					STORE_CENTER->store(*OriginalFleet);

					EMPIRE_CAPITAL_PLANET->remove_admiral(Commander->get_id());
					EMPIRE_CAPITAL_PLANET->remove_fleet(OriginalFleet->get_id());
					EmpireFleetInfoList->remove_empire_fleet_info(EmpireFleetInfo->get_fleet_id());
					EmpireAdmiralInfoList->remove_empire_admiral_info(EmpireAdmiralInfo->get_admiral_id());

					break;
			}
		}
		else
		{
			Fleet->drop_ship_under_25();

			if (OriginalFleet->get_max_ship() != Fleet->count_active_ship())
			{
				OriginalFleet->set_current_ship(Fleet->count_active_ship());
				Commander->type(QUERY_UPDATE);
//					   *STORE_CENTER << *Commander;
				OriginalFleet->type(QUERY_UPDATE);
//					   *STORE_CENTER << *OriginalFleet;

			}
		}
	}
}

// Calculates the original total power of the armada
// and stores it in mTotalPower
void CBattleFleetList::calc_total_power() {
    mTotalPower = 1;

    for( int i = 0; i < length(); i++ ){
        CBattleFleet *Fleet = (CBattleFleet*)get(i);

        //SLOG("Fleet power %d", Fleet->get_power());
        mTotalPower += Fleet->get_power();
    }
}

// Returns the total power of active ships in the armada
int CBattleFleetList::get_current_power() {
    int mCurrentPower = 1;

    for( int i = 0; i < length(); i++ ){
        CBattleFleet *Fleet = (CBattleFleet*)get(i);

        mCurrentPower += Fleet->get_power();
    }
    
    return mCurrentPower;
}

// Counts the total number of ships in an armada
int CBattleFleetList::count_total_ships() {
    int total = 0;

    for( int i = 0; i < length(); i++ ){
        CBattleFleet *Fleet = (CBattleFleet*)get(i);

        total += Fleet->get_max_ship();
    }

    return total;
}

// Returns the total ship class of an armada 
// by summing up the ship classes for each individual ship
int CBattleFleetList::count_total_ship_class() {
    int total = 0;
    
    for( int i = 0; i < length(); i++ ){
        CBattleFleet *Fleet = (CBattleFleet*)get(i);

        total += Fleet->get_max_ship() * Fleet->get_size();
    }
    
    return total;
}

// Updates an entire armada's morale and status effects
void CBattleFleetList::update_morale(float aMoraleUp, float aCapitalMorale, float aFleetMorale) {
    bool itIsAGoodDayToRun = false;
    if (get_capital_fleet()->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN) {
        itIsAGoodDayToRun = true;
        //SLOG("IT IS A GOOD DAY TO RUN!");
    }
    bool capitalIsDisabled = get_capital_fleet()->is_disabled();

    for (int i=0 ; i<length() ; i++) {
        CBattleFleet *Fleet = (CBattleFleet *)get(i);

        // If fleet is disabled, get next fleet
        if (Fleet->is_disabled() == true) continue;

        CAdmiral *Admiral = Fleet->get_admiral();

        // racial ability : lone wolf handling
        float Morale = aMoraleUp;
        if (Admiral->get_racial_ability() == CAdmiral::RA_LONE_WOLF) {
            if (Admiral->get_level() <= 12) {
                Morale += (aCapitalMorale / 2) + aFleetMorale;
            } else if (Admiral->get_level() <= 17) {
                Morale += aFleetMorale;
            }
        } else {
            Morale += aCapitalMorale+aFleetMorale;
        }
        
        //SLOG("Current Power: %d Total Power: %d Player %d", get_current_power(), get_total_power(), Fleet->get_owner());

        // ---Change--- //
        int oldMorale = (int) Fleet->get_morale();
        Fleet->change_morale(Morale);
        int newMorale = (int) Fleet->get_morale();

		
        int OldMoraleStatus = Fleet->get_morale_status();
        
        // If your allies are under attack don't just stand there!
        if ((get_current_power() < (19*(get_total_power()/20)) && Fleet->count_active_ship() > (int) 4*(Fleet->get_max_ship())/5) &&
            (Fleet->get_command() == CBattleFleet::COMMAND_FORMATION || Fleet->get_command() == CBattleFleet::COMMAND_STAND_GROUND)) {
            Fleet->set_command(CBattleFleet::COMMAND_FREE);
        }
        
        // If morale went up this turn and normal status, continue
        if (OldMoraleStatus == CBattleFleet::MORALE_NORMAL && oldMorale < newMorale) { continue; }

        // Flankers don't have the same checks in place
        if (!itIsAGoodDayToRun && capitalIsDisabled != true && Fleet->get_command() != CBattleFleet::COMMAND_FLANK) {
            // If 3/4 of the armada is still alive, no bad status effects may occur
            if (get_current_power() > (3*(get_total_power()/4))) { continue; }
        
            // If 3/5 of the fleet is still alive, fleet is unaffected by status effects
            if (Fleet->count_active_ship() > (int) 3*(Fleet->get_max_ship())/5) { continue; }
        }

        CPlayer *Player = PLAYER_TABLE->get_by_game_id(Fleet->get_owner());
        // If capital died this turn, decide whether to stay or run
        if (itIsAGoodDayToRun) {
            int honor_modifier = ((Player->get_honor() + Player->get_council()->get_honor())/2) - 40;
            if (honor_modifier > 0) {
                honor_modifier = 0;
            }

            // 10% Retreat 10% Rout
            // 1% extra chance per player honor point below 50
            if (number(100) < (10 + (honor_modifier/2))) {
                if (Fleet->has_effect(CFleetEffect::FE_NEVER_RETREAT_ROUT)) break;
                Fleet->set_morale_status(CBattleFleet::MORALE_COMPLETE_BREAK);
                Fleet->set_status(CBattleFleet::STATUS_RETREAT);
                continue;
            }
            if (number(100) < (10 + (honor_modifier/2))) {
                if (Fleet->has_effect(CFleetEffect::FE_NEVER_RETREAT_ROUT)) break;
                Fleet->set_morale_status(CBattleFleet::MORALE_COMPLETE_BREAK);
                Fleet->set_status(CBattleFleet::STATUS_ROUT);
                continue;
            }
        }
        
        //Hippies Change to Moral, Might do some good..... or it might just suck
        // I think we all agree that fleets run too much now
        int WeakMoraleBreak = 70 + Fleet->get_morale_modifier(),
		NormalMoraleBreak = 35 + Fleet->get_morale_modifier(),
		CompleteMoraleBreak = 0;

		/*SLOG ("MORALE TEST: Fleet: %d , Morale: %d, Weak: %d, Normal: %d, Complete: %d",
			Fleet->get_real_id(), (int)Fleet->get_morale(), WeakMoraleBreak,
			NormalMoraleBreak, CompleteMoraleBreak);*/

        if (newMorale <= CompleteMoraleBreak) {
            // If a fleet has 0 morale, 40% retreat, then 50% panic for 30 turns, if not, rout
            // If morale modifier is non-zero, slight chance of berserk for 30 turns instead
            Fleet->set_morale_status(CBattleFleet::MORALE_COMPLETE_BREAK);
            if (number(100) <= 40 - Fleet->get_morale_modifier()) {
                if (Fleet->has_effect(CFleetEffect::FE_NEVER_RETREAT_ROUT)) break;
                Fleet->set_status(CBattleFleet::STATUS_RETREAT);
            } else if (number(100) <= 50 - Fleet->get_morale_modifier()) {
                Fleet->set_status(CBattleFleet::STATUS_PANIC, 30);
            } else if (number(100) <= 50 - Fleet->get_morale_modifier()) {
                if (Fleet->has_effect(CFleetEffect::FE_NEVER_RETREAT_ROUT)) break;
                Fleet->set_status(CBattleFleet::STATUS_ROUT);
            } else {
                if (Fleet->has_effect(CFleetEffect::FE_NEVER_BERSERK)) break;
                Fleet->set_status(CBattleFleet::STATUS_BERSERK, 30);
            }
        } else if (newMorale < NormalMoraleBreak) {
            // The lower the morale, the more likely the outcome
            Fleet->set_morale_status(CBattleFleet::MORALE_NORMAL_BREAK);
            if (OldMoraleStatus == CBattleFleet::MORALE_NORMAL) {
                // Status effects are more likely with huge decrease in morale
                if (newMorale <= 30 + number(15)) {
                    if (Fleet->has_effect(CFleetEffect::FE_NEVER_RETREAT_ROUT)) break;
                    Fleet->set_status(CBattleFleet::STATUS_RETREAT);
                } else {
                    if (newMorale <= 20 + Fleet->get_berserk_modifier()) {
                        if (Fleet->has_effect(CFleetEffect::FE_NEVER_BERSERK)) break;
                            Fleet->set_status(CBattleFleet::STATUS_BERSERK, 60 - newMorale);
                    }
                }
            } else if (OldMoraleStatus == CBattleFleet::MORALE_WEAK_BREAK) {
                // Status effects are less likely from weak -> normal break levels
                if (newMorale <= 30 + number(20)) {
                    if (Fleet->has_effect(CFleetEffect::FE_NEVER_RETREAT_ROUT)) break;
                    Fleet->set_status(CBattleFleet::STATUS_RETREAT);
                } else {
                    if (newMorale <= 25 + Fleet->get_berserk_modifier()) {
                        if (Fleet->has_effect(CFleetEffect::FE_NEVER_BERSERK)) break;
                        Fleet->set_status(CBattleFleet::STATUS_BERSERK, 60 - newMorale);
                    }
                }
            }
        } else if (newMorale < WeakMoraleBreak) {
            // If fleet has below weak break point amount of morale, chance of breaking formation
            Fleet->set_morale_status(CBattleFleet::MORALE_WEAK_BREAK);
            if (OldMoraleStatus == CBattleFleet::MORALE_NORMAL) {
                if (number(100) <= 50) {
                    Fleet->set_command(CBattleFleet::COMMAND_FREE);
                }
            }
        } else {
            Fleet->set_morale_status(CBattleFleet::MORALE_NORMAL);
        }

        // if morale status goes up, recover
        if (Fleet->get_morale_status() < OldMoraleStatus) {
            Fleet->set_command(CBattleFleet::COMMAND_FREE);
        }
        
    }
}

int
	CBattleFleetList::get_capital_fleet_id()
{
	for (int i=0 ; i<length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)get(i);
		if (Fleet->is_capital_fleet() == true)
		{
			return Fleet->get_real_id();
		}
	}
	return -1;
}

CBattleFleet *
CBattleFleetList::search_capital_fleet()
{
	for (int i=0 ; i<length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)get(i);
		if (Fleet->is_capital_fleet())
		{
			return Fleet;
		}
	}
	return NULL;
}

bool
	CBattleFleetList::deploy_extra_fleet(CBattle *aBattle, CPlanet *aPlanet)
{
	if (get_side() != CBattle::SIDE_DEFENSE) return false;

	CBattleRecord *
	BattleRecord = aBattle->get_record();
	CAllyFleetList *
	AllyFleetList = mOwner->get_ally_fleet_list();

	if (AllyFleetList->length() > 0)
	{
		int
			Interval = 5000 / AllyFleetList->length(),
		DeployedFleetNumber = 0;

		for (int i=0 ; i<AllyFleetList->length() ; i++)
		{
			CFleet *
			Fleet = (CFleet *)AllyFleetList->get(i);
			CPlayer *
			RealOwner = PLAYER_TABLE->get_by_game_id(Fleet->get_owner());
			if (RealOwner == NULL)
			{
				SLOG("ERROR : No RealOwner in CBattleFleetList::deploy_extra_fleet(), Fleet->get_owner() = %d", Fleet->get_owner());
				return false;
			}

			CAdmiralList *
			AdmiralList = RealOwner->get_admiral_list();

			CBattleFleet *
			BattleFleet = new CBattleFleet();
			BattleFleet->init(aBattle, mOwner, Fleet, AdmiralList, get_new_id());
			BattleFleet->set_vector(9200, 2500+Interval*(DeployedFleetNumber + 1), 180);
			add_battle_fleet(BattleFleet);

			DeployedFleetNumber++;

			BattleRecord->add_fleet(BattleFleet);
		}
	}

	CFleetList *
	FleetList = mOwner->get_fleet_list();
	CAllyFleetList
		StationFleetList;
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)FleetList->get(i);
		if (Fleet->get_status() != CFleet::FLEET_UNDER_MISSION) continue;

		CMission &
		Mission = Fleet->get_mission();
		if (Mission.get_mission() != CMission::MISSION_STATION_ON_PLANET) continue;
		if (Mission.get_target() != aPlanet->get_id()) continue;

		StationFleetList.add_fleet(Fleet);
	}

	if (StationFleetList.length() > 0)
	{
		int
			Interval = 5000 / StationFleetList.length(),
		DeployedFleetNumber = 0;

		for (int i=0 ; i<StationFleetList.length() ; i++)
		{
			CFleet *
			Fleet = (CFleet *)StationFleetList.get(i);
			CAdmiralList *
			AdmiralList = mOwner->get_admiral_list();

			CBattleFleet *
			BattleFleet = new CBattleFleet();
			BattleFleet->init(aBattle, mOwner, Fleet, AdmiralList, get_new_id());
			BattleFleet->set_vector(9400, 2500+Interval*(DeployedFleetNumber + 1), 180);
			add_battle_fleet(BattleFleet);

			DeployedFleetNumber++;

			BattleRecord->add_fleet(BattleFleet);
		}
	}

	return true;
}

// Defines a battle
CBattle::CBattle() {
	mTurn = 0;
	mWarType = 0;
}

CBattle::CBattle(int aWarType, CPlayer *aAttacker, CPlayer *aDefender, void *aBattleField)
{
	mTurn = 0;
	mWarType = aWarType;

	mAttacker = aAttacker;
	mDefender = aDefender;

	mBattleField = aBattleField;

	mOffenseFleetList.init(mAttacker, SIDE_OFFENSE);
	mDefenseFleetList.init(mDefender, SIDE_DEFENSE);

	mRecord = new CBattleRecord(mAttacker, mDefender, aWarType, aBattleField);
}

CBattle::~CBattle()
{
}

int
	CBattle::get_attacker_id()
{
	return mAttacker->get_game_id();
}

double
	CBattle::get_ratio()
{
	//SLOG('IceCool Get Ratio')
	return ((mDefender->get_power())/(mAttacker->get_power()));
}

void
	CBattle::finish_report_after_battle()
{
	// set winner
	if ((mDefenseFleetList.lost() == false) && (mOffenseFleetList.lost() == false))
	{
		static bool
			AttackerHasStandGroundFleet;

		AttackerHasStandGroundFleet = false;

		for (int i = 0; i < mOffenseFleetList.length(); i++)
		{
			CBattleFleet
				*Fleet = (CBattleFleet*)mOffenseFleetList.get(i);
			if (Fleet->get_status() == CBattleFleet::COMMAND_STAND_GROUND)
			{
				AttackerHasStandGroundFleet = true;
				i = mOffenseFleetList.length();
			}
		}

		if (AttackerHasStandGroundFleet == false)
		{
			for (int i = 0; i < mDefenseFleetList.length(); i++)
			{
				CBattleFleet
					*Fleet = (CBattleFleet*)mDefenseFleetList.get(i);
				if (Fleet->get_status() == CBattleFleet::COMMAND_STAND_GROUND)
				{
					mRecord->set_winner(mAttacker->get_game_id());
				}
			}
		}
	}
	if (mOffenseFleetList.lost() == true && mDefenseFleetList.lost() == false)
	{
		mRecord->set_winner(mDefender->get_game_id());
	}
	else if (mOffenseFleetList.lost() == false && mDefenseFleetList.lost() == true)
	{
		mRecord->set_winner(mAttacker->get_game_id());
		mRecord->set_attacker_gain(mRecord->get_battle_field_name());
	}
	else
	{
		mRecord->set_draw();
	}

	CString
		LostFleet,
		LostAdmiral;
	bool
		IsFirst = true;

	for (int i=0 ; i<mOffenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mOffenseFleetList.get(i);
		if (Fleet->is_disabled() == true)
		{
			if (IsFirst == false)
			{	LostFleet += ", ";
				LostAdmiral += ", ";
			}
			LostFleet.format("%s(%d)", Fleet->get_nick(), Fleet->get_id());
			LostAdmiral.format("%s(%d)", Fleet->get_admiral()->get_name(), Fleet->get_admiral()->get_id());
			IsFirst = false;
		}
	}
	mRecord->set_attacker_lose_fleet((char *)LostFleet);
	mRecord->set_attacker_lose_admiral((char *)LostAdmiral);

	LostFleet.clear();
	LostAdmiral.clear();
	IsFirst = true;

	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mDefenseFleetList.get(i);
		if (Fleet->is_disabled() == true)
		{
			if (IsFirst == false)
			{	LostFleet += ", ";
				LostAdmiral += ", ";
			}
			LostFleet.format("%s", Fleet->get_nick(), Fleet->get_id());
			LostAdmiral.format("%s (#%d)", Fleet->get_admiral()->get_name(), Fleet->get_admiral()->get_id());
			IsFirst = false;
		}
	}
	mRecord->set_defender_lose_fleet((char *)LostFleet);
	mRecord->set_defender_lose_admiral((char *)LostAdmiral);
}

void
	CBattle::save()
{
	BATTLE_RECORD_TABLE->add_battle_record(mRecord);
	mRecord->save();
}

void
	CBattle::set_attacker( CPlayer *aPlayer )
{
	mAttacker = aPlayer;
}

void
	CBattle::set_defender( CPlayer *aPlayer )
{
	mDefender = aPlayer;
}

char *
	CBattle::get_battle_field_name()
{
	if (mWarType == CBattle::WAR_SIEGE ||
		mWarType == CBattle::WAR_PRIVATEER ||
		mWarType == CBattle::WAR_RAID ||
		mWarType == CBattle::WAR_BLOCKADE ||
        mWarType == CBattle::WAR_RATED_SIEGE ||
        mWarType == CBattle::WAR_RATED_BLOCKADE)
	{
		CPlanet *
		Planet = (CPlanet *)mBattleField;
		return Planet->get_name();
	}
	else if (mWarType == CBattle::WAR_MAGISTRATE)
	{
		CPlanet *
		Planet = (CPlanet *)mBattleField;
		return Planet->get_name();
	}
	else if (mWarType == CBattle::WAR_EMPIRE_PLANET)
	{
		CPlanet *
		Planet = (CPlanet *)mBattleField;
		return Planet->get_name();
	}
	else if (mWarType == CBattle::WAR_FORTRESS)
	{
		CFortress *
		Fortress = (CFortress *)mBattleField;
		return Fortress->get_name();
	}
	else if (mWarType == CBattle::WAR_EMPIRE_CAPITAL_PLANET)
	{
		CEmpireCapitalPlanet *
		EmpireCapitalPlanet = (CEmpireCapitalPlanet *)mBattleField;
		return EmpireCapitalPlanet->get_name();
	}
	else
	{
		SLOG("ERROR : Wrong mWarType in CBattle::get_battle_field_name(), mWarType = %d", mWarType);
		return NULL;
	}
}

bool
	CBattle::init_battle_fleet(CDefensePlan *aOffensePlan, CFleetList *aOffenseFleetList, CAdmiralList *aOffenseAdmiralList, CDefensePlan *aDefensePlan, CFleetList *aDefenseFleetList, CAdmiralList *aDefenseAdmiralList)
{
    //SLOG("battle.cc : init_battle_fleet called!");
	/* Recording defender's ally information */
	if (mDefender->get_game_id() != 0)
	{
		CAllyFleetList *
		AllyFleetList = mDefender->get_ally_fleet_list();
		CPlayerList
			AllyList;

		int max = AllyFleetList->length();
		if (max > 20) max = 20;
		for (int i=0 ; i<max ; i++)
		{
			CFleet *
			Fleet = (CFleet *)AllyFleetList->get(i);
			CPlayer *
			Ally = PLAYER_TABLE->get_by_game_id(Fleet->get_owner());
			if (Ally == NULL)
			{
				//SLOG("ERROR : There is no ally player with ID %d in CBattle::init_battle_fleet(), Fleet->get_owner() = %d", Fleet->get_owner(), Fleet->get_owner());
				continue;
			}

			if (!AllyList.get_by_game_id(Ally->get_game_id()))
			{
				AllyList.add_player(Ally);

				mRecord->add_buf((char *)format("ALLIANCE/%d/%s/%d/%d\n",
						mDefender->get_game_id(),
						Ally->get_name(),
						Ally->get_game_id(),
						Ally->get_race()));
			}
		}
	}

    //SLOG("battle.cc : init_battle_fleet : Defender and allied fleets fetched!");

	/* Deploy offense fleets and defense fleets */
	if (aOffensePlan == NULL)
	{
		mOffenseFleetList.auto_deployment(this, aOffenseFleetList, aOffenseAdmiralList);
		//SLOG("battle.cc : init_battle_fleet : Offense fleets by auto deployment!");
	}
	else if (deploy_by_plan(mAttacker, aOffensePlan, aOffenseFleetList, aOffenseAdmiralList, &mOffenseFleetList) == false)
	{
		mOffenseFleetList.auto_deployment(this, aOffenseFleetList, aOffenseAdmiralList);
		//SLOG("battle.cc : init_battle_fleet : Offense fleets by auto deployment!");
	}

    //SLOG("battle.cc : init_battle_fleet : Offense fleets deployed!");

	if (aDefensePlan == NULL)
	{
		mDefenseFleetList.auto_deployment(this, aDefenseFleetList, aDefenseAdmiralList);
	}
	else if (deploy_by_plan(mDefender, aDefensePlan, aDefenseFleetList, aDefenseAdmiralList, &mDefenseFleetList) == false)
	{
		mDefenseFleetList.auto_deployment(this, aDefenseFleetList, aDefenseAdmiralList);
	}

	if (mOffenseFleetList.length() == 0 || mDefenseFleetList.length() == 0) return false;

	/* Deploy ally fleets for defense fleets */
	if (mDefender->get_game_id() != 0)
	{
		mDefenseFleetList.deploy_extra_fleet(this, (CPlanet *)mBattleField);
	}

    //SLOG("battle.cc : init_battle_fleet : Attacker fleets deployed!");

	mTurn = 0;

	mOffenseFleetList.calc_total_power();
	mDefenseFleetList.calc_total_power();

	/* Record capital fleets' information on the battle record */
	CString
		RecordString;
	RecordString.clear();
	RecordString.format("CAPITAL/%d/%d\n", mOffenseFleetList.get_capital_fleet_id(), mDefenseFleetList.get_capital_fleet_id());
	mRecord->add_buf((char *)RecordString);

    //SLOG("battle.cc : init_battle_fleet : Capital fleets recorded!");

	/* Advantage for defense fleets */
	int DefenderBonus = 0;
	if (mWarType == WAR_SIEGE)
	{
		DefenderBonus = 10;
	}
	else if (mWarType == WAR_BLOCKADE)
	{
		DefenderBonus = 5;
	}

	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		BattleFleet = (CBattleFleet *)mDefenseFleetList.get(i);

		CFleetEffect *
		NewEffect = new CFleetEffect(CFleetEffect::FE_AR, DefenderBonus, CFleetEffect::AT_PROPORTIONAL);
		BattleFleet->add_static_effect(NewEffect);
		NewEffect = new CFleetEffect(CFleetEffect::FE_DR, DefenderBonus, CFleetEffect::AT_PROPORTIONAL);
		BattleFleet->add_static_effect(NewEffect);
	}

	/* Offense Fleet Initialize */
	CAdmiral *
	OffenseCapitalCommander = mOffenseFleetList.get_capital_commander();
	int
		CCSkill = 0,
	CCEff = OffenseCapitalCommander->get_armada_commanding_effect_to_efficiency();
	if (mWarType == WAR_SIEGE)
	{
		CCSkill = OffenseCapitalCommander->get_armada_commanding_effect(CAdmiral::OFFENSE);
	}
	else if (mWarType == WAR_BLOCKADE)
	{
		CCSkill = OffenseCapitalCommander->get_armada_commanding_effect(CAdmiral::OFFENSE);
	}

	for (int i=0 ; i<mOffenseFleetList.length() ; i++)
	{
		CBattleFleet *
		BattleFleet = (CBattleFleet *)mOffenseFleetList.get(i);
		CAdmiral *
		Admiral = BattleFleet->get_admiral();

		int
			ActualCCEff,
			ActualCCSkill,
			Skill;

		if (Admiral->get_racial_ability() == CAdmiral::RA_LYING_DORMANT)
		{
			ActualCCEff = CCEff/2;
			ActualCCSkill = CCSkill/2;
		}
		else if (Admiral->get_racial_ability() == CAdmiral::RA_LONE_WOLF)
		{
			if (Admiral->get_level() <= 19)
			{
				ActualCCEff = CCEff/2;
				ActualCCSkill = CCSkill/2;
			}
			else
			{
				if (CCEff > 0)
				{
					ActualCCEff = CCEff/2;
				}
				else
				{
					ActualCCEff = 0;
				}
				if (CCSkill > 0)
				{
					ActualCCSkill = CCSkill/2;
				}
				else
				{
					ActualCCSkill = 0;
				}
			}
		}
		else
		{
			ActualCCEff = CCEff;
			ActualCCSkill = CCSkill;
		}

		if (mWarType == WAR_SIEGE)
		{
			Skill = Admiral->get_overall_attack() + ActualCCSkill;
		}
		else if (mWarType == WAR_BLOCKADE)
		{
			Skill = Admiral->get_overall_attack() + ActualCCSkill;
		}
		else
		{
			Skill = 0;
		}

		CFleetEffect *
		NewEffect;

		if (ActualCCEff)
		{
			NewEffect = new CFleetEffect(CFleetEffect::FE_EFFICIENCY, ActualCCEff, CFleetEffect::AT_ABSOLUTE);
			BattleFleet->add_static_effect(NewEffect);
		}
		if (Skill)
		{
			NewEffect = new CFleetEffect(CFleetEffect::FE_AR, Skill*3, CFleetEffect::AT_PROPORTIONAL);
			BattleFleet->add_static_effect(NewEffect);
			NewEffect = new CFleetEffect(CFleetEffect::FE_DR, Skill*3, CFleetEffect::AT_PROPORTIONAL);
			BattleFleet->add_static_effect(NewEffect);
		}
		if (Admiral->get_maneuver_level())
		{
			NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, Admiral->get_maneuver_level()*3, CFleetEffect::AT_PROPORTIONAL);
			BattleFleet->add_static_effect(NewEffect);
			NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, Admiral->get_maneuver_level()*3, CFleetEffect::AT_PROPORTIONAL);
			BattleFleet->add_static_effect(NewEffect);
		}
	}

   // SLOG("battle.cc : init_battle_fleet : Attacker fleets initialized!");

	/* Defense Fleet Initialize */
	CAdmiral *
	DefenseCapitalCommander = mDefenseFleetList.get_capital_commander();
	CCEff = DefenseCapitalCommander->get_armada_commanding_effect_to_efficiency();
	if (mWarType == WAR_SIEGE)
	{
		CCSkill = DefenseCapitalCommander->get_armada_commanding_effect(CAdmiral::DEFENSE);
	}
	else if (mWarType == WAR_BLOCKADE)
	{
		CCSkill = DefenseCapitalCommander->get_armada_commanding_effect(CAdmiral::DEFENSE);
	}

	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		BattleFleet = (CBattleFleet *)mDefenseFleetList.get(i);
		CAdmiral *
		Admiral = BattleFleet->get_admiral();

		int
			ActualCCSkill,
			ActualCCEff,
			Skill;

		if (Admiral->get_racial_ability() == CAdmiral::RA_LYING_DORMANT)
		{
			ActualCCEff = CCEff/2;
			ActualCCSkill = CCSkill/2;
		}
		else if (Admiral->get_racial_ability() == CAdmiral::RA_LONE_WOLF)
		{
			if (Admiral->get_level() <= 19)
			{
				ActualCCEff = CCEff/2;
				ActualCCSkill = CCSkill/2;
			}
			else
			{
				if (CCEff > 0)
				{
					ActualCCEff = CCEff/2;
				}
				else
				{
					ActualCCEff = 0;
				}
				if (CCSkill > 0)
				{
					ActualCCSkill = CCSkill/2;
				}
				else
				{
					ActualCCSkill = 0;
				}
			}
		}
		else
		{
			ActualCCEff = CCEff;
			ActualCCSkill = CCSkill;
		}

		if (mWarType == WAR_SIEGE)
		{
			Skill = Admiral->get_overall_defense() +  ActualCCSkill;
		}
		else if (mWarType == WAR_BLOCKADE)
		{
			Skill = Admiral->get_overall_defense() +  ActualCCSkill;
		}
		else
		{
			Skill = 0;
		}

		CFleetEffect *
		NewEffect;

		if (ActualCCEff)
		{
			NewEffect = new CFleetEffect(CFleetEffect::FE_EFFICIENCY, CCEff, CFleetEffect::AT_ABSOLUTE);
			BattleFleet->add_static_effect(NewEffect);
		}
		if (Skill)
		{
			NewEffect = new CFleetEffect(CFleetEffect::FE_AR, Skill*3, CFleetEffect::AT_PROPORTIONAL);
			BattleFleet->add_static_effect(NewEffect);
			NewEffect = new CFleetEffect(CFleetEffect::FE_DR, Skill*3, CFleetEffect::AT_PROPORTIONAL);
			BattleFleet->add_static_effect(NewEffect);
		}
		if (Admiral->get_maneuver_level())
		{
			NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, Admiral->get_maneuver_level()*3, CFleetEffect::AT_PROPORTIONAL);
			BattleFleet->add_static_effect(NewEffect);
			NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, Admiral->get_maneuver_level()*3, CFleetEffect::AT_PROPORTIONAL);
			BattleFleet->add_static_effect(NewEffect);
		}
	}
	
	//SLOG("battle.cc : init_battle_fleet : Defender fleets initialized!");
	
	// Apply other effects
	if (mAttacker->get_game_id() != 0) {
        CPlayerEffectList *
		EffectList = mAttacker->get_effect_list();

		for (int i=0 ; i<EffectList->length() ; i++) {
			CPlayerEffect *
			Effect = (CPlayerEffect *)EffectList->get(i);
			if (Effect->get_type() == CPlayerEffect::PA_COMMANDER_SURVIVAL) {
				for (int i=0 ; i<mOffenseFleetList.length() ; i++)
				{
					CBattleFleet *
					BattleFleet = (CBattleFleet *)mOffenseFleetList.get(i);
					CFleetEffect *
					NewEffect = new CFleetEffect( CFleetEffect::FE_COMMANDER_SURVIVAL, Effect->get_argument1(), CFleetEffect::AT_ABSOLUTE );
					BattleFleet->add_static_effect(NewEffect);
				}
			}
		}
    }

	if (mDefender->get_game_id() != 0)
	{
		CPlanet *
		Planet = (CPlanet *)mBattleField;
		CPlayerEffectList *
		EffectList = mDefender->get_effect_list();

		for (int i=0 ; i<EffectList->length() ; i++)
		{
			CPlayerEffect *
			Effect = (CPlayerEffect *)EffectList->get(i);
			if (Effect->get_type() == CPlayerEffect::PA_ENEMY_MORALE_MODIFIER)
			{
				for (int i=0 ; i<mOffenseFleetList.length() ; i++)
				{
					CBattleFleet *
					BattleFleet = (CBattleFleet *)mOffenseFleetList.get(i);
					CFleetEffect *
					NewEffect = new CFleetEffect( CFleetEffect::FE_MORALE_MODIFIER, Effect->get_argument1(), CFleetEffect::AT_ABSOLUTE );
					BattleFleet->add_static_effect(NewEffect);
				}
			}
			if (Effect->get_type() == CPlayerEffect::PA_COMMANDER_SURVIVAL) {
				for (int i=0 ; i<mOffenseFleetList.length() ; i++)
				{
					CBattleFleet *
					BattleFleet = (CBattleFleet *)mDefenseFleetList.get(i);
					CFleetEffect *
					NewEffect = new CFleetEffect( CFleetEffect::FE_COMMANDER_SURVIVAL, Effect->get_argument1(), CFleetEffect::AT_ABSOLUTE );
					BattleFleet->add_static_effect(NewEffect);
				}
			}
		}
		for (int i=0 ; i<EffectList->length() ; i++)
		{
			CPlayerEffect *
			Effect = (CPlayerEffect *)EffectList->get(i);
			if (Effect->get_type() != CPlayerEffect::PA_CHANGE_SHIP_ABILITY_ON_PLANET) continue;
			if (Effect->get_target() != Planet->get_id()) continue;

			for (int i=0 ; i<mOffenseFleetList.length() ; i++)
			{
				CBattleFleet *
				BattleFleet = (CBattleFleet *)mOffenseFleetList.get(i);
				CFleetEffect *
				NewEffect = new CFleetEffect(Effect->get_argument1(), Effect->get_argument2(), Effect->get_apply());
				BattleFleet->add_static_effect(NewEffect);
			}
			for (int i=0 ; i<mDefenseFleetList.length() ; i++)
			{
				CBattleFleet *
				BattleFleet = (CBattleFleet *)mDefenseFleetList.get(i);
				CFleetEffect *
				NewEffect = new CFleetEffect(Effect->get_argument1(), Effect->get_argument2(), Effect->get_apply());
				BattleFleet->add_static_effect(NewEffect);
			}
		}
	}
	
	//SLOG("init_battle returning true");

	return true;
}

bool
	CBattle::deploy_by_plan(CPlayer *aPlanOwner, CDefensePlan *aPlan, CFleetList *aFleetList, CAdmiralList *aAdmiralList, CBattleFleetList *aBattleFleetList)
{
	// is fleet available?
	// if not, return false
	// else deploy by plan
	//	add & set x,y,dir by plan to fleet list
	// return true

    //SLOG("battle.cc : deploy_by_plan : called!");

	CDefenseFleetList *
	DefenseFleetList = aPlan->get_fleet_list();
	if (DefenseFleetList == NULL || DefenseFleetList->length() == 0) return false;

	if (aBattleFleetList->get_side() == CBattle::SIDE_DEFENSE)
	{
		int
			NumberOfMissingFleet = 0;
		for (int i=0 ; i<DefenseFleetList->length() ; i++)
		{
			CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)DefenseFleetList->get(i);
			CFleet *
			Fleet = aFleetList->get_by_id(DefenseFleet->get_fleet_id());
			CMission &
			Mission = Fleet->get_mission();
			if (Fleet == NULL)
			{
				NumberOfMissingFleet++;
				if (DefenseFleet->get_fleet_id() == aPlan->get_capital()) return false;
			}
			else if ((Fleet->get_status() != CFleet::FLEET_STAND_BY) && (Mission.get_mission() != CMission::MISSION_RETURNING))
			{
				NumberOfMissingFleet++;
				if (Fleet->get_id() == aPlan->get_capital()) return false;
			}
		}

		if ((double)DefenseFleetList->length()/2.0 <= (double)NumberOfMissingFleet) return false;
	}

    SLOG("battle.cc : deploy_by_plan : defense plan not missing 0.5 of the ships!");

	for (int i=0 ; i<DefenseFleetList->length() ; i++)
	{
		CDefenseFleet *
		DefenseFleet = (CDefenseFleet*)DefenseFleetList->get(i);

		CFleet *
		Fleet = aFleetList->get_by_id(DefenseFleet->get_fleet_id());
		CMission &
		Mission = Fleet->get_mission();
		if (aBattleFleetList->get_side() == CBattle::SIDE_OFFENSE)
		{
			SLOG("battle.cc : deploy_by_plan : It was SIDE_OFFENSE");
            if (Fleet == NULL) continue;
		}
		else if (aBattleFleetList->get_side() == CBattle::SIDE_DEFENSE)
		{
			SLOG("battle.cc : deploy_by_plan : It was SIDE_DEFENSE");
            if (Fleet == NULL) continue;
			if ((Fleet->get_status() != CFleet::FLEET_STAND_BY) && (Mission.get_mission() != CMission::MISSION_RETURNING)) continue;
			if (Fleet->get_current_ship() == 0) continue;
		}

		CBattleFleet *
		BattleFleet = new CBattleFleet();
//		SLOG("FLEET ID: %d", DefenseFleet->get_fleet_id());

        SLOG("battle.cc : deploy_by_plan : Initializing fleet");
		BattleFleet->init(this, aPlanOwner, Fleet, aAdmiralList, aBattleFleetList->get_new_id());
		SLOG("battle.cc : deploy_by_plan : Fleet initialized");

//		SLOG("DEFENSE FLEET COMMAND: %d", DefenseFleet->get_command());
		BattleFleet->set_command(DefenseFleet->get_command());
//		SLOG("BATTLE FLEET COMMAND: %d", BattleFleet->get_command());
		if (aPlan->get_capital() == DefenseFleet->get_fleet_id())
		{
			BattleFleet->set_capital_fleet();
			aBattleFleetList->set_capital_fleet(BattleFleet);
		}
		if (aBattleFleetList->get_side() == CBattle::SIDE_DEFENSE)
		{
			BattleFleet->set_vector(DefenseFleet->get_x(), DefenseFleet->get_y(), 180);
			if (BattleFleet->get_command() == CBattleFleet::COMMAND_FLANK)
			{
				if (BattleFleet->get_y() > 5000)
				{
					BattleFleet->turn(-45);
					//BattleFleet->turn(0);
				}
				else
				{
					BattleFleet->turn(45);
					//BattleFleet->turn(0);
				}
			}
		}
		else
		{
			BattleFleet->set_vector(DefenseFleet->get_x(), DefenseFleet->get_y(), 0);
			if (BattleFleet->get_command() == CBattleFleet::COMMAND_FLANK)
			{
				if (BattleFleet->get_y() > 5000)
				{
					BattleFleet->turn(45);
					//BattleFleet->turn(0);
				}
				else
				{
					BattleFleet->turn(-45);
					//BattleFleet->turn(0);
				}
			}
		}
		
		SLOG("battle.cc : deploy_by_plan : Deployment plan initialized");

		// add formation info
		mRecord->add_fleet(BattleFleet);
		
		SLOG("battle.cc : deploy_by_plan : Formation info recorded!");

		aBattleFleetList->add_battle_fleet(BattleFleet);
		if (BattleFleet->get_command() == CBattleFleet::COMMAND_FORMATION)
		{
			aBattleFleetList->add_formation_info(BattleFleet);
		}
	}

	if (aBattleFleetList->length() == 0) return false;

	if (aBattleFleetList->get_capital_fleet() == NULL)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aBattleFleetList->get(0);
		Fleet->set_capital_fleet();
		aBattleFleetList->set_capital_fleet(Fleet);
	}

    SLOG("battle.cc : deploy_by_plan : returned true!");

	return true;
}

bool
	CBattle::attacker_win()
{
	if ((mDefenseFleetList.lost() == false) && (mOffenseFleetList.lost() == false))
	{
		static bool
			AttackerHasStandGroundFleet;

		AttackerHasStandGroundFleet = false;

		for (int i = 0; i < mOffenseFleetList.length(); i++)
		{
			CBattleFleet
				*Fleet = (CBattleFleet*)mOffenseFleetList.get(i);
			if (Fleet->get_status() == CBattleFleet::COMMAND_STAND_GROUND)
			{
				AttackerHasStandGroundFleet = true;
				i = mOffenseFleetList.length();
			}
		}

		if (AttackerHasStandGroundFleet == false)
		{
			for (int i = 0; i < mDefenseFleetList.length(); i++)
			{
				CBattleFleet
					*Fleet = (CBattleFleet*)mDefenseFleetList.get(i);
				if (Fleet->get_status() == CBattleFleet::COMMAND_STAND_GROUND)
				{
					return true;
				}
			}
		}
	}
	if( mOffenseFleetList.lost() ) return false;
	if( mDefenseFleetList.lost() ) return true;
	return false;
}

// Returns the weighted average ship class of the battle
double CBattle::get_weighted_average_ship_class() {
    CBattleFleetList *AttackerFleetList, *DefenderFleetList;
    AttackerFleetList = &get_offense_battle_fleet_list();
    DefenderFleetList = &get_defense_battle_fleet_list();

    int ship_class_points = AttackerFleetList->count_total_ship_class();
    //SLOG("Ship Class Points attacker: %d", ship_class_points);
    ship_class_points += DefenderFleetList->count_total_ship_class();
    //SLOG("Ship Class Points final: %d", ship_class_points);
    
    return (double) ((double) ship_class_points/(double) count_total_ships());
}

// Returns the total number of ships used in the battle
int CBattle::count_total_ships() {
    CBattleFleetList *AttackerFleetList, *DefenderFleetList;
    AttackerFleetList = &get_offense_battle_fleet_list();
    DefenderFleetList = &get_defense_battle_fleet_list();

    return ((AttackerFleetList->count_total_ships()) + (DefenderFleetList->count_total_ships()));
}

bool
	CBattle::run_step()
{
	if (mTurn>1800 || mOffenseFleetList.lost() == true || mDefenseFleetList.lost() == true)
	{
		CString
			Buf;
		Buf.format("ENDTURN/%d\n", mTurn);
		mRecord->add_buf((char *)Buf);
		return false;
	}

	mRecord->set_there_was_battle();

	mRecord->set_turn(mTurn);

	for (int i=0 ; i<mOffenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mOffenseFleetList.get(i);
		Fleet->clear_dynamic_effect();
	}
	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mDefenseFleetList.get(i);
		Fleet->clear_dynamic_effect();
	}

	update_penetration();

	mOffenseFleetList.set_formation_speed();
	mDefenseFleetList.set_formation_speed();

	prepare_turn(&mOffenseFleetList, &mDefenseFleetList);
	prepare_turn(&mDefenseFleetList, &mOffenseFleetList);

	for (int i=0 ; i<mOffenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mOffenseFleetList.get(i);

		int
			Dam = Fleet->calc_PA(0, CFleetEffect::FE_DAMAGE),
		PSI_Dam = Fleet->calc_PA(0, CFleetEffect::FE_PSI_STATIC_DAMAGE);
		if (Dam > 0)
			Fleet->damage_random_active(Dam);
		if (PSI_Dam > 0 )
			Fleet->damage_random_active(PSI_Dam, true);
	}
	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mDefenseFleetList.get(i);

		int
			Dam = Fleet->calc_PA(0, CFleetEffect::FE_DAMAGE),
		PSI_Dam = Fleet->calc_PA(0, CFleetEffect::FE_PSI_STATIC_DAMAGE);
		if (Dam > 0)
			Fleet->damage_random_active(Dam);
		if (PSI_Dam > 0)
			Fleet->damage_random_active(PSI_Dam, true);
	}

	one_side_run(&mOffenseFleetList, &mDefenseFleetList);
	one_side_run(&mDefenseFleetList, &mOffenseFleetList);

	// morale code
	// calc morale change for fleet disabled and capital disabled
	// change status

	int
		AttackerMoraleUp = 0,
	DefenderMoraleUp = 0,
	AttackerMoraleCapitalDown = 0,
	DefenderMoraleCapitalDown = 0,
	AttackerMoraleFleetDown = 0,
	DefenderMoraleFleetDown = 0;

	for (int i=0 ; i<mOffenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mOffenseFleetList.get(i);
		int
			MoraleDown = 0,
		MoraleUp = 0;

		if (Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN ||
			Fleet->get_status() == CBattleFleet::STATUS_RETREATED_THIS_TURN)
		{
			if (Fleet->is_capital_fleet() == true)
			{
				AttackerMoraleCapitalDown = -40;
				MoraleUp = 15;
			}
			else
			{
				int
					FleetPower = Fleet->get_original_power(),
				TotalPower = mOffenseFleetList.get_total_power();
				MoraleDown = -((FleetPower / TotalPower) * 100);
				MoraleUp = -(MoraleDown / 2);

				if (MoraleDown < -15) MoraleDown = -15;
				if (MoraleUp > 10) MoraleUp = 10;
			}
                    /*
			if (Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN)
			{
				Fleet->set_status(CBattleFleet::STATUS_ANNIHILATED);
			}
			else
			{
				Fleet->set_status(CBattleFleet::STATUS_RETREATED);
			}                                      */
			mRecord->disable_fleet(Fleet);

			DefenderMoraleUp += MoraleUp;
			//SLOG('IceCool Battle Code')
//		   double bRatio = get_ratio(); 
//		   float powerMoraleDown = 0;
//			if (bRatio < 1.0)
//			{;
//				if (bRatio < 0.55)
//				{powerMoraleDown -= 100;}
//				else if (bRatio < 0.6)
//				{powerMoraleDown -= 75;}
//				else if (bRatio < 0.7)
//				{powerMoraleDown -= 40;}
//				else if (bRatio < 0.8)
//				{powerMoraleDown -= 20;}
//				else if (bRatio < 0.9)
//				{powerMoraleDown -= 10;}	
//			}
			
			AttackerMoraleFleetDown += MoraleDown;//+ powerMoraleDown;

		}
	}
	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mDefenseFleetList.get(i);
		int
			MoraleDown = 0,
		MoraleUp = 0;

		if (Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN ||
			Fleet->get_status() == CBattleFleet::STATUS_RETREATED_THIS_TURN)
		{
			if (Fleet->is_capital_fleet() == true)
			{
				DefenderMoraleCapitalDown = -30;
				MoraleUp = 15;
			}
			else
			{
				int
					FleetPower = Fleet->get_original_power(),
				TotalPower = mDefenseFleetList.get_total_power();
				MoraleDown = -((FleetPower / TotalPower) * 100);
				MoraleUp = -(MoraleDown / 2);

				if (MoraleDown < -15) MoraleDown = -15;
				if (MoraleUp > 10) MoraleUp = 10;
			}
             /*
			if (Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN)
			{
				Fleet->set_status(CBattleFleet::STATUS_ANNIHILATED);
			}
			else
			{
				Fleet->set_status(CBattleFleet::STATUS_RETREATED);
			}                    */
			mRecord->disable_fleet(Fleet);

			AttackerMoraleUp += MoraleUp;
			DefenderMoraleFleetDown += MoraleDown;
			
		}
	}
	//SLOG("MORALE TEST 1A -- Attacker MoralUp: %d, MoralCapDown: %d, MoraleFleetDown: %d", AttackerMoraleUp, AttackerMoraleCapitalDown, AttackerMoraleFleetDown);
	//SLOG("MORALE TEST 1B -- Defender MoralUp: %d, MoralCapDown: %d, MoraleFleetDown: %d", DefenderMoraleUp, DefenderMoraleCapitalDown, DefenderMoraleFleetDown);
	
	mOffenseFleetList.update_morale(AttackerMoraleUp, AttackerMoraleCapitalDown, AttackerMoraleFleetDown);
	mDefenseFleetList.update_morale(DefenderMoraleUp, DefenderMoraleCapitalDown, DefenderMoraleFleetDown);

    // If STATUS_SOMETHING_THIS_TURN, update to STATUS_SOMETHING
    for (int i=0 ; i<mOffenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mOffenseFleetList.get(i);

		if (Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN ||
			Fleet->get_status() == CBattleFleet::STATUS_RETREATED_THIS_TURN)
		{

			if (Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN)
			{
				Fleet->set_status(CBattleFleet::STATUS_ANNIHILATED);
			}
			else
			{
				Fleet->set_status(CBattleFleet::STATUS_RETREATED);
			}
		}
	}

	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mDefenseFleetList.get(i);


		if (Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN ||
			Fleet->get_status() == CBattleFleet::STATUS_RETREATED_THIS_TURN)
		{
			if (Fleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN)
			{
				Fleet->set_status(CBattleFleet::STATUS_ANNIHILATED);
			}
			else
			{
				Fleet->set_status(CBattleFleet::STATUS_RETREATED);
			}
		}
	}

	mTurn++;

	return true;
}

void
	CBattle::one_side_run(CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// formation global rule :
	// if any fleet path meet vertical border, disband
	// if any fleet is encountered, encountered
	// if no fleet is encountered, and fleet was encountered, reformation

	int
		NewFormationStatus = CBattleFleetList::FORMATION_NONE;

	for (int i=0 ; i<aActive->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aActive->get(i);

		if (Fleet->is_disabled() == true) continue;
		if (Fleet->get_command() != CBattleFleet::COMMAND_FORMATION) continue;

		if (Fleet->path_meet_vertical_border() == true)
		{
			NewFormationStatus = CBattleFleetList::FORMATION_DISBAND;
			break;
		}
		if (Fleet->is_encountered() == true)
		{
			NewFormationStatus = CBattleFleetList::FORMATION_ENCOUNTER;
			break;
		}
	}

	aActive->set_formation_status(NewFormationStatus);

	for (int i=0 ; i<aActive->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aActive->get(i);
		if (Fleet->is_disabled() == true) continue;

		Fleet->weapon_cooling();


		switch (Fleet->get_status())
		{
			case CBattleFleet::STATUS_BERSERK :
				{
					fleet_run_berserk(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::STATUS_DISORDER :
				{
					fleet_run_disorder(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::STATUS_PANIC :
				{
					fleet_run_panic(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::STATUS_ROUT :
				{
					fleet_run_rout(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::STATUS_RETREAT :
				{
					fleet_run_retreat(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::COMMAND_FORMATION :
				{
					fleet_run_formation(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::COMMAND_FREE :
				{
					fleet_run_free(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::COMMAND_STAND_GROUND :
				{
					fleet_run_stand_ground(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::COMMAND_ASSAULT :
				{
					fleet_run_assault(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::COMMAND_PENETRATE :
				{
					fleet_run_penetrate(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::COMMAND_FLANK:
				{
					fleet_run_flank(Fleet, aActive, aPassive);
				}
				break;

			case CBattleFleet::COMMAND_NORMAL :
			default :
				{
					fleet_run_normal(Fleet, aActive, aPassive);
				}
				break;
		}

		if (mTurn%10 == 0)
		{
			mRecord->add_location(Fleet);
		}

	}
}

void
	CBattle::fleet_run_berserk(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	if (aI->touch_border() == true)
	{
		aI->set_status(CBattleFleet::STATUS_RETREATED_THIS_TURN);
		return;
	}
	if (aI->recover_status() == true)
	{
		aI->remove_abnormal_status();
		return;
	}

	// attack the most front fleet in efr
	// turn to fleet attacking self
	// move forward

	CBattleFleet *
	MostFrontFleet = NULL;
	int
		Delta,
		LowestDelta = 360;

	for (int i=0 ; i<aActive->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aActive->get(i);

		if (Fleet == aI || Fleet->is_disabled() == true) continue;
		if (aI->on_fire_range(Fleet) == false) continue;

		Delta = aI->delta_direction((CVector *)Fleet);
		if (Delta > 180)
		{
			Delta = 360-Delta;
		}

		if (Delta < LowestDelta)
		{
			LowestDelta = Delta;
			MostFrontFleet = Fleet;
		}
	}
	for (int i=0 ; i<aPassive->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aPassive->get(i);

		if (Fleet->is_disabled() == true) continue;
		if (aI->on_fire_range(Fleet) == false) continue;

		Delta = aI->delta_direction((CVector *)Fleet);
		if (Delta > 180)
		{
			Delta = 360-Delta;
		}

		if (Delta < LowestDelta)
		{
			LowestDelta = Delta;
			MostFrontFleet = Fleet;
		}
	}

	if (MostFrontFleet)
	{
		aI->attack(mRecord, MostFrontFleet);
	}

	LowestDelta = 360;
	MostFrontFleet = NULL;

	for (int i=0 ; i<aPassive->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aPassive->get(i);

		if (Fleet->is_disabled() == true) continue;
		if (Fleet->get_enemy() != aI) continue;

		Delta = aI->delta_direction((CVector *)Fleet);
		if (Delta > 180)
		{
			Delta = 360-Delta;
		}

		if (Delta < LowestDelta)
		{
			LowestDelta = Delta;
			MostFrontFleet = Fleet;
		}
	}
	if (MostFrontFleet)
	{
		aI->turn_to((CVector *)MostFrontFleet);
	}

	if (MostFrontFleet == NULL || aI->on_complete_fire_range(MostFrontFleet) == false)
	{
		aI->move();
	}
}

void
	CBattle::fleet_run_disorder(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// while you can attack, attack
	// if there is encountered fleet
	//	turn to highest dar fleet by 70%
	//	turn to random by 30%
	// else
	//	move forward by 70%
	//	turn to random by 30%

	if (aI->recover_status() == true)
	{
		aI->remove_abnormal_status();
		return;
	}

	CBattleFleet *
	Target;

	Target = aI->find_target();
	if (Target != NULL)
	{
		aI->attack(mRecord, Target);
	}

	if (aI->is_encountered() == true)
	{
		if (number(10) <= 7)
		{
			Target = aI->get_highest_DAR_encounter_fleet();
			if (Target != NULL)
			{
				if (aI->on_complete_fire_range(Target) == false)
				{
					aI->trace((CVector *)Target);
				}
				else
				{
					aI->turn_to((CVector *)Target);
				}
			}
		}
		else
		{
			aI->turn(aI->get_mobility() * (number(3) - 2));
		}
	}
	else
	{
		if (number(10) <= 7)
		{
			aI->move();
		}
		else
		{
			aI->turn(aI->get_mobility() * (number(3) - 2));
		}
	}
}

void
	CBattle::fleet_run_retreat(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{

	// if jump timer expired, retreat
	//else wait


	if (aI->tick_jump_timer())
	{
		aI->set_status(CBattleFleet::STATUS_RETREATED_THIS_TURN);
		return;
	}

}

void
	CBattle::fleet_run_rout(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// if touch border, set status - retreated
	// if substatus == turn to nearest border
	//	turn to nearest border
	// else move
	// all ships hp below 25% are dead

	if (aI->touch_border() == true)
	{
		aI->set_status(CBattleFleet::STATUS_RETREATED_THIS_TURN);
		return;
	}

	if (aI->get_substatus() == CBattleFleet::SUBSTATUS_TURN_TO_NEAREST_BORDER)
	{
		int
			Bottom = aI->get_y(),
		Top = 10000-aI->get_y(),
		Left = aI->get_x(),
		Right = 10000-aI->get_x();
		CVector
			EscapeTo;
		if (Bottom < Top && Bottom < Left && Bottom < Right)
		{
			EscapeTo.set_vector(aI->get_x(), 0, 0);
		}
		else if (Top < Bottom && Top < Left && Top < Right)
		{
			EscapeTo.set_vector(aI->get_x(), 10000, 0);
		}
		else if (Left < Bottom && Left < Top && Left < Right)
		{
			EscapeTo.set_vector(0, aI->get_y(), 0);
		}
		else
		{
			EscapeTo.set_vector(10000, aI->get_y(), 0);
		}

		aI->turn_to(&EscapeTo);
		if (aI->is_heading_to(&EscapeTo) == true)
		{
			aI->set_substatus( CBattleFleet::SUBSTATUS_NONE );
		}
	}
	else
	{
		aI->move();
	}

	aI->drop_ship_under_25();
}

void
	CBattle::fleet_run_panic(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	if (aI->recover_status() == true)
	{
		aI->remove_abnormal_status();
	}
}

void
	CBattle::fleet_run_formation(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// if formation status = disband
	//	set order - free
	// if formation status = none
	//	move forward
	// if formation status = encounter
	//	fight with encountering fleet
	//	or support fighting fleet in 2000
	//	or stand-by

	CBattleFleet *
	Target;

	if (aActive->get_formation_status() == CBattleFleetList::FORMATION_NONE)
	{
		aI->CVector::move(aActive->get_formation_speed());
	}
	else if (aActive->get_formation_status() == CBattleFleetList::FORMATION_DISBAND)
	{
		aI->set_command(CBattleFleet::COMMAND_FREE);
	}
	else
	{
		Target = aI->find_target();
		if (Target != NULL)
		{
			aI->attack(mRecord, Target);
		}

		if (aI->is_encountered() == true)
		{
			Target = aI->get_highest_DAR_encounter_fleet();
			if (Target != NULL)
			{
				if (aI->on_complete_fire_range(Target) == false)
				{
					aI->trace((CVector *)Target);
				}
				else
				{
					aI->turn_to((CVector *)Target);
				}
			}
		}
		else
		{
			Target = aI->get_least_ERT_engaged_fleet(aPassive, 2000);
			if (Target != NULL)
			{
				aI->trace((CVector *)Target);
			}
		}
	}
}

void
	CBattle::fleet_run_penetrate(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// while penetrating
	//	until pr 80% move forward
	//	if attack-able, attack
	//	after pr 80%, change to charge
	// while charging
	//	attack if possible
	//	trace to least ert fleet

	CBattleFleet *
	Target;

	if (aI->get_substatus() == CBattleFleet::SUBSTATUS_PENETRATION)
	{
		Target = aI->find_target();
		if (Target != NULL && aI->is_cloaked() == false)
		{
			aI->attack(mRecord, Target);
		}

		if (aI->get_penetration() < 80)
		{
			aI->move();
		}
		else
		{
			aI->set_substatus(CBattleFleet::SUBSTATUS_CHARGE);
		}
	}
	else if (aI->get_substatus() == CBattleFleet::SUBSTATUS_CHARGE)
	{
		Target = aI->find_target();
		if (Target != NULL)
		{
			aI->attack(mRecord, Target);
		}

		Target = aI->get_least_ERT_fleet(aPassive);
		if (Target != NULL)
		{
			if (aI->on_complete_fire_range(Target) == false)
			{
				aI->trace((CVector *)Target);
			}
			else
			{
				aI->turn_to((CVector *)Target);
			}
		}
		else
		{
			aI->move();
		}

		if (aI->path_meet_border() == true)
		{
			aI->set_command(CBattleFleet::COMMAND_FREE);
		}
	}
}

void
	CBattle::fleet_run_flank(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// while moving forward,
	//	move forward until flight path meet edge
	//	if flight path meet edge, change substatus to turn to forward
	// while turn to forward
	//	turn to forward
	//	if turn to forward is done, change to penetration
	// while penetration
	//	go until penetration 80%
	//	if penetration > 80% change to chrage
	// while charge
	//	attack the least ert enemy

	CBattleFleet *
	Target;

	if (aI->get_substatus() == CBattleFleet::SUBSTATUS_MOVE_STRAIGHT)
	{
		aI->move();
		if (aI->path_meet_border() == true)
		{
			aI->set_substatus(CBattleFleet::SUBSTATUS_TURN_TO_FORWARD);
		}
	}
	else if (aI->get_substatus() == CBattleFleet::SUBSTATUS_TURN_TO_FORWARD)
	{
		CVector
			Forward;
		if (aI->get_direction() > 90 && aI->get_direction() < 270)
		{
			Forward.set_vector(0, aI->get_y(), 0);
		}
		else
		{
			Forward.set_vector(10000, aI->get_y(), 0);
		}
		aI->turn_to(&Forward);
		aI->move();
		if (aI->is_heading_to(&Forward) == true)
		{
			aI->set_substatus(CBattleFleet::SUBSTATUS_PENETRATION);
		}
	}
	else if (aI->get_substatus() == CBattleFleet::SUBSTATUS_PENETRATION)
	{
		Target = aI->find_target();
		if (Target && aI->is_cloaked() == false)
		{
			aI->attack(mRecord, Target);
		}

		if (aI->get_penetration() < 80)
		{
			aI->move();
		}
		else
		{
			aI->set_substatus(CBattleFleet::SUBSTATUS_CHARGE);
		}
	}
	else if (aI->get_substatus() == CBattleFleet::SUBSTATUS_CHARGE)
	{
		Target = aI->find_target();
		if (Target != NULL)
		{
			aI->attack(mRecord, Target);
		}

		Target = aI->get_least_ERT_fleet(aPassive);
		if (Target != NULL)
		{
			if (aI->on_complete_fire_range(Target) == false)
			{
				aI->trace((CVector *)Target);
			}
			else
			{
				aI->turn_to((CVector *)Target);
			}
		}
		else
		{
			aI->move();
		}

		if (aI->path_meet_border() == true)
		{
			aI->set_command(CBattleFleet::COMMAND_FREE);
		}
	}
}

void
	CBattle::fleet_run_assault(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// while penetrating
	//	until pr 80% move forward
	//	if attack-able, attack
	//	after pr 80%, change to charge
	// while charging
	//	attack if possible
	//	trace to least ert fleet

	CBattleFleet *
	Target;

	if (aI->get_substatus() == CBattleFleet::SUBSTATUS_PENETRATION)
	{
		Target = aI->find_target();
		if (Target != NULL)
		{
			aI->attack(mRecord, Target);
		}

		if (aI->get_penetration() < 40)
		{
			aI->move();
		}
		else
		{
			aI->set_substatus(CBattleFleet::SUBSTATUS_CHARGE);
		}
	}
	else if (aI->get_substatus() == CBattleFleet::SUBSTATUS_CHARGE)
	{
		Target = aI->find_target();
		if (Target != NULL)
		{
			aI->attack(mRecord, Target);
		}

		Target = aI->get_least_ERT_fleet(aPassive);
		if (Target != NULL)
		{
			if (aI->on_complete_fire_range(Target) == false)
			{
				aI->trace((CVector *)Target);
			}
			else
			{
				aI->turn_to((CVector *)Target);
			}
		}
		else
		{
			aI->move();
		}

		if (aI->path_meet_border() == true)
		{
			aI->set_command(CBattleFleet::COMMAND_FREE);
		}
	}
}

void
	CBattle::fleet_run_stand_ground(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// if encountered, turn to the highest DAR fleet
	// fire if you can
	// if got engagement signal from 500 range,
	// try to support

	CBattleFleet *
	Target;

	if (aI->is_encountered() == true)
	{
		Target = aI->find_target();

		system_log("SHIP ECOUNTERED: stand_ground");

		if (Target != NULL)
		{
			system_log("SHIP ECOUNTERED: stand_ground->attack!!!");
			aI->attack(mRecord, Target);
		}

		Target = aI->get_highest_DAR_encounter_fleet();
		if (Target != NULL)
		{
			aI->turn_to((CVector *)Target);
		}
	}
	else
	{
		Target = aI->get_least_ERT_engaged_fleet(aPassive, 500);
		if (Target != NULL)
		{
			aI->trace((CVector *)Target);
			aI->set_command(CBattleFleet::COMMAND_FREE);
		}
	}
}

void
	CBattle::fleet_run_normal(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// if encountered, set order free
	// if pr = 100, set order free
	// if fp meet border, set order free
	// if engage signal received in range 1000, turn to least ert
	// else move forward

	if (aI->is_encountered() || aI->get_penetration() == 100 || aI->path_meet_border())
	{
		aI->set_command(CBattleFleet::COMMAND_FREE);
	}
	else
	{
		CBattleFleet *
		Target = aI->get_least_ERT_engaged_fleet(aPassive, 1000);
		if (Target == NULL)
		{
			aI->move();
		}
		else
		{
			aI->trace((CVector *)Target);
		}
	}
}

void
	CBattle::fleet_run_free(CBattleFleet *aI, CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	// if encountered
	//	if there is attack-able fleet, attack
	//	turn to highest dar fleet
	// else if there is engaged friendly fleet, try to help
	// else if turn around is set, turn around to 5000,5000
	// else if fp meets border, set turn around
	// else move forward

	CBattleFleet *
	Target;

	if (aI->is_encountered() == true)
	{
		Target = aI->find_target();
		if (Target != NULL)
		{
			aI->attack(mRecord, Target);
		}

		Target = aI->get_highest_DAR_encounter_fleet();
		if (Target != NULL)
		{
			if (aI->on_complete_fire_range(Target) == false)
			{
				aI->trace((CVector *)Target);
			}
			else
			{
				aI->turn_to((CVector *)Target);
			}
		}

		aI->reset_substatus();
	}
	else
	{
		Target = aI->get_least_ERT_engaged_fleet(aPassive);
		if (Target != NULL)
		{
			aI->trace((CVector *)Target);
			aI->reset_substatus();
		}
		else if (aI->get_substatus() == CBattleFleet::SUBSTATUS_TURNING_TO_CENTER)
		{
			aI->turn_to(&BattleFieldCenter);
			if (aI->is_heading_to(&BattleFieldCenter))
			{
				aI->reset_substatus();
#if defined(LINUX)
				aI->move();
#endif
			}
		}
		else if (aI->path_meet_border())
		{
			aI->set_substatus(CBattleFleet::SUBSTATUS_TURNING_TO_CENTER);
			aI->turn_to(&BattleFieldCenter);
		}
		else
		{
			aI->move();
		}
	}
}

void
	CBattle::prepare_turn(CBattleFleetList *aActive, CBattleFleetList *aPassive)
{
	for (int i=0 ; i<aActive->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aActive->get(i);
		if (Fleet->is_disabled() == true) continue;

		CFleetEffect *
		NewEffect;
		CPlayer *
		Owner = aActive->get_owner();

		if (Owner->has_ability(ABILITY_TRAINED_MIND))
		{
			NewEffect = new CFleetEffect();
			NewEffect->set_type(CFleetEffect::FE_NEVER_BERSERK);
			Fleet->add_dynamic_effect(NewEffect);
		}
		if (Owner->has_ability(ABILITY_FANATIC_FLEET))
		{
			NewEffect = new CFleetEffect();
			NewEffect->set_type(CFleetEffect::FE_NEVER_RETREAT_ROUT);
			Fleet->add_dynamic_effect(NewEffect);
		}
		if (Owner->has_ability(ABILITY_FRAGILE_MIND_STRUCTURE))
		{
			CBattleFleet *
			CapitalFleet = aActive->get_capital_fleet();
			if (CapitalFleet == NULL)
			{
				SLOG("ERROR : There is no capital fleet in aActive FleetList");
				return;
			}

			NewEffect = new CFleetEffect();
			NewEffect->set_type(CFleetEffect::FE_PSI_DEFENSE_PANALTY);
			NewEffect->set_apply_type(CFleetEffect::AT_PROPORTIONAL);

			if (CapitalFleet->is_disabled())
			{
				NewEffect->set_amount(50);
			}
			else
			{
				NewEffect->set_amount(25);
			}

			Fleet->add_dynamic_effect(NewEffect);

		}

		switch (Fleet->get_status())
		{
			case CBattleFleet::STATUS_BERSERK :
				NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 20, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, -10, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_AR, -10, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -10, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_COOLING_TIME, -40, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				break;

			case CBattleFleet::STATUS_DISORDER :
				NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, -20, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, -20, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_AR, -10, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -10, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_COOLING_TIME, 20, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				break;

			case CBattleFleet::STATUS_PANIC :
				fleet_run_panic(Fleet, aActive, aPassive);
				break;

			case CBattleFleet::STATUS_ROUT :
				NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 25, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, 25, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -25, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				break;

			case CBattleFleet::STATUS_RETREAT :
				NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 10, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				break;

			case CBattleFleet::COMMAND_FORMATION :
				NewEffect = new CFleetEffect(CFleetEffect::FE_DR, 10, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				break;

			case CBattleFleet::COMMAND_FREE :
				NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 25, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, 15, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				break;

			case CBattleFleet::COMMAND_STAND_GROUND :
				NewEffect = new CFleetEffect(CFleetEffect::FE_AR, 10, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_DR, 20, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				NewEffect = new CFleetEffect(CFleetEffect::FE_COMMANDER_SURVIVAL, -10, CFleetEffect::AT_ABSOLUTE);
				Fleet->add_dynamic_effect(NewEffect);
				break;

			case CBattleFleet::COMMAND_ASSAULT :
				if (Fleet->get_substatus() == CBattleFleet::SUBSTATUS_PENETRATION)
				{
					NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 20, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, 20, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_AR, -15, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_COOLING_TIME, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
				}
				if (Fleet->get_substatus() == CBattleFleet::SUBSTATUS_CHARGE)
				{
					NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 5, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, 5, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_AR, -25, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
				}
				break;

			case CBattleFleet::COMMAND_PENETRATE :
				if (Fleet->get_substatus() == CBattleFleet::SUBSTATUS_PENETRATION)
				{
					NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 50, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, 30, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_AR, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
				}
				if (Fleet->get_substatus() == CBattleFleet::SUBSTATUS_CHARGE)
				{
					NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 5, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, 5, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_AR, 5, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
				}
				break;

			case CBattleFleet::COMMAND_FLANK:
				if (Fleet->get_substatus() == CBattleFleet::SUBSTATUS_CHARGE)
				{
					NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 20, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, 20, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_AR, 5, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
				}
				else
				{
					NewEffect = new CFleetEffect(CFleetEffect::FE_SPEED, 60, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, 60, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_AR, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
					NewEffect = new CFleetEffect(CFleetEffect::FE_DR, -10, CFleetEffect::AT_PROPORTIONAL);
					Fleet->add_dynamic_effect(NewEffect);
				}
				break;

			case CBattleFleet::COMMAND_NORMAL :
			default :
				NewEffect = new CFleetEffect(CFleetEffect::FE_AR, 5, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);
				/*NewEffect = new CFleetEffect(CFleetEffect::FE_MOBILITY, -5, CFleetEffect::AT_PROPORTIONAL);
				Fleet->add_dynamic_effect(NewEffect);*/
				break;
		}
	}

	/*for (int i=0 ; i<aPassive->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aPassive->get(i);

		if( Fleet->is_engaged() == false)
		{
			Fleet->reset_detected();
		}
	}*/

	CFleetEffectListDynamic* aEffectList;
	for (int i=0 ; i<aActive->length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)aActive->get(i);

		if (Fleet->is_disabled() == true) continue;

		Fleet->reset_encountered();
		Fleet->clear_encounter_list();

		CBattleFleet *
		Enemy = Fleet->get_enemy();
		if (Enemy != NULL)
		{
			if (Enemy->is_disabled() == true) Fleet->reset_enemy();
		}

		Fleet->loose_engaged();

		Fleet->charge_effect();
		Fleet->recharge_shield();
		Fleet->repair_in_battle();

		CFleetEffect * Effect;
		aEffectList = Fleet->get_static_effect_list();

		for( int i = 0; i < aEffectList->length(); i++ )
		{
			Effect = (CFleetEffect*)aEffectList->get(i);
			if( Effect->get_target() == CFleetEffect::LOCAL_EFFECT ) continue;

			if (Effect->ready() == true)
			{
				if (Effect->get_target() == CFleetEffect::AREA_EFFECT_TARGET_ENEMY ||
					Effect->get_target() == CFleetEffect::AREA_EFFECT_TARGET_ALL)
				{
					for (int j=0 ; j<aPassive->length() ; j++)
					{
						CBattleFleet *
						SubjectFleet = (CBattleFleet *)aPassive->get(j);

						if (SubjectFleet->is_disabled() == true) continue;
						if (SubjectFleet->under_area_effect() == true) continue;

						if (Fleet->distance((CVector *)SubjectFleet) < Effect->get_range())
						{
							CFleetEffect *
							NewEffect = new CFleetEffect();
							NewEffect->set_type(Effect->get_type());
							NewEffect->set_amount(Effect->get_amount());
							NewEffect->set_apply_type(Effect->get_apply_type());
							NewEffect->set_target(CFleetEffect::AE_LOCAL_EFFECT);

							SubjectFleet->add_dynamic_effect(NewEffect);
						}
					}
				}

				if (Effect->get_target() == CFleetEffect::AREA_EFFECT_TARGET_ALLY ||
					Effect->get_target() == CFleetEffect::AREA_EFFECT_TARGET_ALL)
				{
					for (int j=0 ; j<aActive->length() ; j++)
					{
						CBattleFleet *
						SubjectFleet = (CBattleFleet*)aActive->get(j);

						if (SubjectFleet->is_disabled() == true) continue;
						if (SubjectFleet->under_area_effect() == true) continue;

						if (Fleet->distance((CVector *)SubjectFleet) < Effect->get_range())
						{
							CFleetEffect *
							NewEffect = new CFleetEffect();
							NewEffect->set_type(Effect->get_type());
							NewEffect->set_amount(Effect->get_amount());
							NewEffect->set_apply_type(Effect->get_apply_type());
							NewEffect->set_target(CFleetEffect::AE_LOCAL_EFFECT);

							SubjectFleet->add_dynamic_effect(NewEffect);
						}
					}
				}
			}
		}

		int
			RedZoneRadius = 300; // Radii within which a fleet is encountered.
		for (int j=0 ; j<aPassive->length() ; j++)
		{
			CBattleFleet *
			SubjectFleet = (CBattleFleet *)aPassive->get(j);

			if (SubjectFleet->is_disabled() == true) continue;

			if (Fleet->can_see(SubjectFleet) == false) continue;

			int
				Distance = Fleet->distance((CVector *)SubjectFleet);
			/*
			int
				EffectiveDistance = Distance * (100 + (5 - SubjectFleet->get_size())*5) / 100;
			EffectiveDistance = SubjectFleet->calc_PA(EffectiveDistance, CFleetEffect::FE_STEALTH);

			if (EffectiveDistance < DetectionRange)
			{
				SubjectFleet->set_detected();
			}*/

			if (Distance < RedZoneRadius || Fleet->on_path((CVector *)SubjectFleet))
			{
				Fleet->add_to_encounter_list(SubjectFleet);
			}
		}
	}
}

void
	CBattle::update_penetration()
{
	for (int i=0 ; i<mOffenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mOffenseFleetList.get(i);

		int
			PenetratedPower = 0,
		UnpenetratedPower = 0;

		for (int j=0 ; j<mDefenseFleetList.length() ; j++)
		{
			CBattleFleet *
			SubjectFleet = (CBattleFleet *)mDefenseFleetList.get(j);
			if (SubjectFleet->is_disabled() == true) continue;

			if (Fleet->get_x() > SubjectFleet->get_x())
			{
				PenetratedPower += SubjectFleet->get_power();
			}
			else
			{
				UnpenetratedPower += SubjectFleet->get_power();
			}
		}

		if ((PenetratedPower + UnpenetratedPower) == 0)
		{
			Fleet->set_penetration(100);
		}
		else
		{
			Fleet->set_penetration(PenetratedPower/(PenetratedPower + UnpenetratedPower) * 100);
		}
		if (Fleet->get_penetration() == 0)
		{
			Fleet->set_penetration(1);
		}
	}

	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mDefenseFleetList.get(i);

		int
			PenetratedPower = 0,
		UnpenetratedPower = 0;

		for (int j=0 ; j<mOffenseFleetList.length() ; j++)
		{
			CBattleFleet *
			SubjectFleet = (CBattleFleet *)mOffenseFleetList.get(j);
			if (SubjectFleet->is_disabled() == true) continue;

			if (Fleet->get_x() < SubjectFleet->get_x())
			{
				PenetratedPower += SubjectFleet->get_power();
			}
			else
			{
				UnpenetratedPower += SubjectFleet->get_power();
			}
		}

		if ((PenetratedPower + UnpenetratedPower) == 0)
		{
			Fleet->set_penetration(100);
		}
		else
		{
			Fleet->set_penetration(PenetratedPower/(PenetratedPower + UnpenetratedPower) * 100);
		}
		if (Fleet->get_penetration() == 0)
		{
			Fleet->set_penetration(1);
		}
	}
}

void
	CBattle::update_fleet_after_battle()
{
	if (attacker_win() == true)
	{
		mOffenseFleetList.update_fleet_after_battle(mDefender, mWarType, true);
		mDefenseFleetList.update_fleet_after_battle(mAttacker, mWarType, false);
	}
	else
	{
		mOffenseFleetList.update_fleet_after_battle(mDefender, mWarType, false);
		mDefenseFleetList.update_fleet_after_battle(mAttacker, mWarType, true);
	}
}

void
	CBattle::update_empire_fleet_after_battle()
{
	if (mAttacker->get_game_id() == EMPIRE_GAME_ID)
	{
		mOffenseFleetList.update_empire_fleet_after_battle(mDefender, mWarType, attacker_win());
	}
	else if (mDefender->get_game_id() == EMPIRE_GAME_ID)
	{
		mDefenseFleetList.update_empire_fleet_after_battle(mAttacker, mWarType, !attacker_win());
	}
}

char *
	CBattle::get_result_report()
{
	static CString
		Buf;
	Buf.clear();

	Buf.format("%s : <BR>\n", GETTEXT("ATTACKER"));
	for (int i=0 ; i<mOffenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mOffenseFleetList.get(i);
		Buf.format("%s (%s*%d) - Killed %d <BR>\n",
			Fleet->get_nick(),
			Fleet->get_class_name(),
			Fleet->get_max_ship(),
			Fleet->get_max_ship() - Fleet->count_active_ship());
	}
	Buf += "<BR>\n";

	Buf.format("%s : <BR>\n", GETTEXT("DEFENDER"));
	for (int i=0 ; i<mDefenseFleetList.length() ; i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet *)mDefenseFleetList.get(i);
		Buf.format("%s (%s*%d) - Killed %d <BR>\n",
			Fleet->get_nick(),
			Fleet->get_class_name(),
			Fleet->get_max_ship(),
			Fleet->get_max_ship() - Fleet->count_active_ship());
	}

	return (char *)Buf;
}

bool
	CBattle::init_raid(CFleet *aFleet)
{
	if (mWarType != WAR_RAID)
	{
		SLOG("ERROR : mWarType is not WAR_RAID in CBattle::init_raid(), mWarType = %d", mWarType);
		return false;
	}

	CPlanet *
	Planet = (CPlanet *)mBattleField;

	mReport.clear();
	mReport.format(GETTEXT("You smuggled %1$s to %2$s for a raid."),
		aFleet->get_nick(),
		Planet->get_name());
	mReport += "<BR>\n";

	CAdmiralList
		*AttackerAdmiralList = mAttacker->get_admiral_list(),
	*DefenderAdmiralList = mDefender->get_admiral_list();

	CBattleFleet *
	BattleFleet = new CBattleFleet();
	BattleFleet->init(this, mAttacker, aFleet, AttackerAdmiralList, mOffenseFleetList.get_new_id());
	mOffenseFleetList.add_battle_fleet(BattleFleet);

	int
		FleetCount = 0;

	CString
		FleetName;
	FleetName.clear();

	for(int i=0; i<mDefender->get_fleet_list()->length(); i++)
	{
		CFleet *
		Fleet = (CFleet*)mDefender->get_fleet_list()->get(i);
		if (Fleet->get_status() != CFleet::FLEET_UNDER_MISSION) continue;
		CMission &
		Mission = Fleet->get_mission();

		if (Mission.get_mission() == CMission::MISSION_PATROL &&
			Mission.get_target() == Planet->get_id())
		{
			CBattleFleet *
			BattleFleet = new CBattleFleet();

			BattleFleet->init(this, mDefender, Fleet, DefenderAdmiralList, mDefenseFleetList.get_new_id());
			mDefenseFleetList.add_battle_fleet(BattleFleet);

			if (FleetCount > 0) FleetName += ", ";
			FleetName.format(GETTEXT("%1$s on a patrol mission"),
				Fleet->get_nick());
			FleetCount++;
		}
		else if (Mission.get_mission() == CMission::MISSION_STATION_ON_PLANET &&
			Mission.get_target() == Planet->get_id())
		{
			CBattleFleet *
			BattleFleet = new CBattleFleet();

			BattleFleet->init(this, mDefender, Fleet, DefenderAdmiralList, mDefenseFleetList.get_new_id());
			mDefenseFleetList.add_battle_fleet(BattleFleet);

			if (FleetCount > 0) FleetName += ", ";
			FleetName.format(GETTEXT("%1$s on a station mission"),
				Fleet->get_nick());
			FleetCount++;
		}
	}

	if (!mDefenseFleetList.length())
	{
		mReport.format(GETTEXT("%1$s has no fleets to protect %2$s."),
			mDefender->get_nick(),
			Planet->get_name());
	}
	else
	{
		mReport.format(GETTEXT("%1$s has the %2$s to protect %3$s."),
			mDefender->get_nick(),
			(char *)FleetName,
			Planet->get_name());
	}
	mReport += "<BR>\n";

	return true;
}

bool
	CBattle::raid_fleet_detection_check()
{
	if (mWarType != WAR_RAID)
	{
		SLOG("ERROR : mWarType is not WAR_RAID in CBattle::raid_fleet_detection_check(), mWarType = %d", mWarType);
		return false;
	}

	CPlanet *
	Planet = (CPlanet *)mBattleField;
	CBattleFleet *
	BattleFleet = (CBattleFleet*)mOffenseFleetList.get(0);

	int StealthScore = 10;

	// by device

	// by ship size
	StealthScore += (BattleFleet->get_size()) - 5;

	// by raid bonus
	CAdmiral *Admiral = BattleFleet->get_admiral();
	StealthScore += Admiral->get_overall_attack()*5;

	if (!mDefenseFleetList.length()) return false;

	for(int i=0; i<mDefenseFleetList.length(); i++)
	{
		CBattleFleet *
		BattleFleet = (CBattleFleet*)mDefenseFleetList.get(i);
		CAdmiral *
		Admiral = BattleFleet->get_admiral();

		int DetectionScore = 10;

		DetectionScore += Admiral->get_detection_level()*2;
		DetectionScore += Admiral->get_overall_defense()*2;

		DetectionScore += Planet->get_building().get(BUILDING_MILITARY_BASE);
		int Chance;
		if (StealthScore != 0) {
			Chance = 50 * (DetectionScore / StealthScore);
		} else {
			SLOG("ERROR : StealthScore = 0 in CBattle::raid_fleet_detection_check(), BattleFleet id = %d", BattleFleet->get_id());
			return true;//if its a hack, don't let the benefit from it.
		}

		if (mAttacker->has_ability(ABILITY_STEALTH))
			Chance -= 25;

		if (mAttacker->has_ability(ABILITY_STEALTH_PIRATE))
			Chance -= 25;

		if (number(100) <= Chance)
		{
			mReport.format(GETTEXT("Your raid was detected by %1$s."),
				BattleFleet->get_nick());
			mReport += "<BR>\n";

			return true;
		}
	}
	return false;
}

bool
	CBattle::raid_fleet_war()
{
	if (mWarType != WAR_RAID)
	{
		SLOG("ERROR : mWarType is not WAR_RAID in CBattle::raid_fleet_war(), mWarType = %d", mWarType);
		return false;
	}

	CPlanet *
	Planet = (CPlanet *)mBattleField;
	CBattleFleet *
	RaidFleet = (CBattleFleet *)mOffenseFleetList.get(0);

	mDefender->time_news((char*)format(
			GETTEXT("%1$s's fleet tried to raid your planet %2$s,"
				" your fleet defended in the orbit."),
			mAttacker->get_nick(), Planet->get_name()));

	for(int i=0; i<mDefenseFleetList.length(); i++)
	{
		CBattleFleet *Fleet = (CBattleFleet*)mDefenseFleetList.get(i);

		Fleet->raid_attack(RaidFleet);
		Fleet->raid_attack(RaidFleet);
		Fleet->raid_attack(RaidFleet);

		// admiral gain exp
		CAdmiral *Admiral = Fleet->get_admiral();
		Admiral->gain_exp( CMission::mAdmiralExpDetect );

		if (RaidFleet->get_status() == CBattleFleet::STATUS_ANNIHILATED_THIS_TURN)
		{
			mReport.format(GETTEXT("%1$s's %2$s attacks and sinks the raid fleet %3$s."),
				mDefender->get_nick(),
				Fleet->get_nick(),
				RaidFleet->get_nick());
			mReport += "<BR>\n";
			return false;
		}
		else
		{
			mReport.format(GETTEXT("%1$s's %2$s attacks the raid fleet %3$s (%4$d/%5$d ship)."),
				mDefender->get_nick(),
				Fleet->get_nick(),
				RaidFleet->get_nick(),
				RaidFleet->count_active_ship(),
				RaidFleet->get_max_ship());
			return true;
		}
	}
	return true;
}

bool
	CBattle::raid_siege_war()
{
	if (mWarType != WAR_RAID)
	{
		SLOG("ERROR : mWarType is not WAR_RAID in CBattle::raid_siege_war(), mWarType = %d", mWarType);
		return false;
	}

	CPlanet *
	Planet = (CPlanet *)mBattleField;
	CBattleFleet *
	RaidFleet = (CBattleFleet *)mOffenseFleetList.get(0);

	mDefender->time_news(
		(char *)format(
			GETTEXT("%1$s's fleet tried to raid your planet %2$s, you ordered all military bases to fire."),
			mAttacker->get_nick(),
			Planet->get_name()));

	RaidFleet->siege(Planet, mReport);

	if (RaidFleet->get_status() ==
		CBattleFleet::STATUS_ANNIHILATED_THIS_TURN)
	{
		mReport.format(GETTEXT("The military base(s) on the %1$s attacked and sank the raid fleet %2$s."),
			Planet->get_name(),
			RaidFleet->get_nick());
		mReport += "<BR>\n";
		return false;
	}
	else
	{
		CString
			ActiveShipNumberString,
			MaxShipNumberString;
		ActiveShipNumberString = dec2unit(RaidFleet->count_active_ship());
		MaxShipNumberString = dec2unit(RaidFleet->get_max_ship());

		mReport.format(GETTEXT("The military base(s) on the %1$s attacked the raid fleet %2$s (%3$s/%4$s)."),
			Planet->get_name(),
			RaidFleet->get_nick(),
			(char *)ActiveShipNumberString,
			(char *)MaxShipNumberString);
		mReport += "<BR>\n";
		return true;
	}
}

bool
	CBattle::siege_war()
{
	if (mWarType != WAR_SIEGE && mWarType != WAR_RAID) return false;

	CPlanet *
	Planet = (CPlanet *)mBattleField;
	CBattleFleet
		*HighestPowerFleet = NULL;
	int
		HighestPower = 0;

	for(int i=0; i<mOffenseFleetList.length(); i++)
	{
		CBattleFleet *
		Fleet = (CBattleFleet*)mOffenseFleetList.get(i);
		if( Fleet->is_disabled() ) continue;
		if( Fleet->get_power() > HighestPower ){
			HighestPowerFleet = Fleet;
			HighestPower = Fleet->get_power();
		}
	}

	if (HighestPowerFleet == NULL) return false;

	HighestPowerFleet->siege(Planet, mReport);
	if (attacker_win() == true) return true;

	return false;
}

bool
	CBattle::raid_bomb()
{
	if (mWarType != WAR_RAID)
	{
		SLOG("ERROR : mWarType is not WAR_RAID in CBattle::raid_bomb(), mWarType = %d", mWarType);
		return false;
	}

	CPlanet *
	Planet = (CPlanet *)mBattleField;
	CBattleFleet *
	RaidFleet = (CBattleFleet *)mOffenseFleetList.get(0);
	CAdmiral *
	Admiral = RaidFleet->get_admiral();

	int Bonus = Admiral->get_overall_attack();

	for(int i=0; i<mDefenseFleetList.length(); i++)
	{
		CBattleFleet *Fleet = (CBattleFleet*)mDefenseFleetList.get(i);
		CAdmiral *Admiral = Fleet->get_admiral();
		Bonus -= Admiral->get_overall_defense();
	}

	int ShipSize = RaidFleet->get_size();
	int Damage = ShipSize * ShipSize * RaidFleet->count_active_ship() / 8;
	Damage += Bonus;
	if (Damage < 1) Damage = 1;
	if (Damage > 50) Damage = 50;

	int RealDamage = 0;
	int MilitaryBase = Planet->get_building().get(BUILDING_MILITARY_BASE);

	// protected by military base
	if (MilitaryBase > 0)
	{
		if (Damage*2 > MilitaryBase)
		{
			Damage -= MilitaryBase/2;
			RealDamage += MilitaryBase;
			MilitaryBase = 0;
			Planet->get_building().set(BUILDING_MILITARY_BASE, 0);

			mReport.format(GETTEXT("%1$s destroyed all military bases on %2$s."),
				RaidFleet->get_nick(),
				Planet->get_name());
			mReport += "<BR>\n";
			mDefender->time_news((char *)format("%s%s.<BR>\n",
					GETTEXT("All military bases have been destroyed on "), Planet->get_name()));
		}
		else
		{
			MilitaryBase -= Damage*2;
			RealDamage += Damage*2;
			Planet->get_building().set(BUILDING_MILITARY_BASE, MilitaryBase);
			mReport.format(GETTEXT("The military bases on the %1$s protected against the raid successfully."),
				Planet->get_name());
			mReport += "<BR>\n";
			mDefender->time_news((char*)format(
					GETTEXT("%1$s unit(s) of military base have been destroyed by enemy raiding on %2$s."),
					dec2unit(Damage*2), Planet->get_name()));
			Damage = 0;
		}
	}

	// destroy building
	if (Damage > 0)
	{
		int
			Factory = Planet->get_building().get(BUILDING_FACTORY);
		if (Factory > 0)
		{
			int
				DamagedFactory = Factory*Damage/100;
			mDefender->time_news((char*)format(
					GETTEXT("%1$s unit(s) of factory are/is destroyed by the raid on %2$s."),
					dec2unit(DamagedFactory), Planet->get_name()));

			Planet->get_building().set(BUILDING_FACTORY, Factory - DamagedFactory);
			RealDamage += DamagedFactory;

			if (DamagedFactory == 0 || DamagedFactory == 1)
			{
				mReport.format(GETTEXT("%1$s has destroyed %2$s factory on the planet %3$s."),
					RaidFleet->get_nick(),
					dec2unit(DamagedFactory),
					Planet->get_name());
			}
			else
			{
				mReport.format(GETTEXT("%1$s has destroyed %2$s factories on the planet %3$s."),
					RaidFleet->get_nick(),
					dec2unit(DamagedFactory),
					Planet->get_name());
			}
			mReport += "<BR>\n";
		}

		int
			ResearchLab = Planet->get_building().get(BUILDING_RESEARCH_LAB);
		if (ResearchLab > 0)
		{
			int
				DamagedResearchLab = ResearchLab*Damage/100;
			mDefender->time_news((char*)format(
					GETTEXT("%1$s has destroyed %2$s research lab(s) on %3$s."),
					RaidFleet->get_nick(),
					dec2unit(ResearchLab - DamagedResearchLab),
					Planet->get_name()));

			Planet->get_building().set(BUILDING_RESEARCH_LAB, ResearchLab - DamagedResearchLab);
			RealDamage += DamagedResearchLab;

			mReport.format(GETTEXT("%1$s has destroyed %2$s research lab(s) on %3$s."),
				RaidFleet->get_nick(),
				dec2unit(DamagedResearchLab),
				Planet->get_name());
			mReport += "<BR>\n";
		}

		int
			Population = Planet->get_population();
		if (Population > 0)
		{
			int
				DamagedPopulation = Population*Damage/100;
			mDefender->time_news(
				(char *)format(GETTEXT("The population was reduced by %1$s on %2$s due to enemy raids."),
					dec2unit(DamagedPopulation), Planet->get_name()));

			Planet->change_population(-DamagedPopulation);

			mReport.format(GETTEXT("%1$s reduced population on the %2$s by %3$s."),
				RaidFleet->get_nick(),
				Planet->get_name(),
				dec2unit(DamagedPopulation));
			mReport += "<BR>\n";
		}
	}

	// Now Raid exp. = total buildings destroyed * 20
	if (CMission::mAdmiralExpRaid > 0)
	{
		Admiral->gain_exp( CMission::mAdmiralExpRaid );
		mAttacker->time_news(format(GETTEXT("%1$s made successful raid, he/she gained %2$s points of exp."),
				Admiral->get_name(),
				CMission::mAdmiralExpRaid));
	}
	else {
		RealDamage *= CMission::mAdmiralExpRaidMultiplier;
		Admiral->gain_exp ( RealDamage );
		mAttacker->time_news((char *)format(GETTEXT("%s made successful raid, he/she gained %d points of exp."),
				Admiral->get_name(),
				RealDamage));
	}


	return true;
}

bool
	CBattle::init_privateer(CFleet *aFleet)
{
	if (mWarType != WAR_PRIVATEER)
	{
		SLOG("ERROR : mWarType is not WAR_PRIVATEER in CBattle::init_privateer(), mWarType = %d", mWarType);
		return false;
	}

	CPlanet *
	Planet = (CPlanet *)mBattleField;
	mReport.clear();
	mReport.format(GETTEXT("You smuggled %1$s to %2$s for a privateer."),
		aFleet->get_nick(), Planet->get_name());
	mReport += "<BR>\n";

	CAdmiralList
		*AttackerAdmiralList = mAttacker->get_admiral_list(),
	*DefenderAdmiralList = mDefender->get_admiral_list();

	CBattleFleet *
	BattleFleet = new CBattleFleet();

	BattleFleet->init(this, mAttacker, aFleet, AttackerAdmiralList, mOffenseFleetList.get_new_id());
	mOffenseFleetList.add_battle_fleet(BattleFleet);

	int
		FleetCount = 0;

	CString
		FleetName;
	FleetName.clear();

	for(int i=0; i<mDefender->get_fleet_list()->length(); i++)
	{
		CFleet *
		Fleet = (CFleet*)mDefender->get_fleet_list()->get(i);
		if (Fleet->get_status() != CFleet::FLEET_UNDER_MISSION) continue;
		CMission &
		Mission = Fleet->get_mission();

		if (Mission.get_mission() == CMission::MISSION_PATROL ||
			Mission.get_target() == Planet->get_id())
		{
			CBattleFleet *
			BattleFleet = new CBattleFleet();

			BattleFleet->init(this, mDefender, Fleet, DefenderAdmiralList, mDefenseFleetList.get_new_id());
			mDefenseFleetList.add_battle_fleet(BattleFleet);

			if (FleetCount > 0) FleetName += ", ";
			FleetName.format(GETTEXT("%1$s on a patrol mission"),
				Fleet->get_nick());
			FleetCount++;
		}
		else if (Mission.get_mission() == CMission::MISSION_STATION_ON_PLANET ||
			Mission.get_target() == Planet->get_id())
		{
			CBattleFleet *
			BattleFleet = new CBattleFleet();

			BattleFleet->init(this, mDefender, Fleet, DefenderAdmiralList, mDefenseFleetList.get_new_id());
			mDefenseFleetList.add_battle_fleet(BattleFleet);

			if (FleetCount > 0) FleetName += ", ";
			FleetName.format(GETTEXT("%1$s on a station mission"),
				Fleet->get_nick());
			FleetCount++;
		}
	}
	if (!mDefenseFleetList.length())
	{
		mReport.format(GETTEXT("%1$s has no fleets to protect %2$s."),
			mDefender->get_nick(),
			Planet->get_name());
	}
	else
	{
		mReport.format(GETTEXT("%1$s has the %2$s to protect %3$s."),
			mDefender->get_nick(),
			(char *)FleetName,
			Planet->get_name());
	}
	mReport += "<BR>\n";

	return true;
}

bool
	CBattle::privateer_fleet_detection_check(bool aBattle)
{
	if (mWarType != WAR_PRIVATEER)
	{
		SLOG("ERROR : mWarType is not WAR_PRIVATEER in CBattle::privateer_fleet_detection_check(), mWarType = %d", mWarType);
		return false;
	}

	CPlanet *
	Planet = (CPlanet *)mBattleField;
	CBattleFleet *
	PrivateerFleet = (CBattleFleet *)mOffenseFleetList.get(0);

	int StealthScore = 100;

	// by device

	// by ship size
	StealthScore += (PrivateerFleet->get_size()) - 5;

	// by privateer bonus
	CAdmiral *Admiral = PrivateerFleet->get_admiral();
	StealthScore += Admiral->get_overall_attack()*5;

	if (!mDefenseFleetList.length()) return false;

	for(int i=0; i<mDefenseFleetList.length(); i++)
	{
		CBattleFleet *
		BattleFleet = (CBattleFleet *)mDefenseFleetList.get(i);
		CAdmiral *
		Admiral = BattleFleet->get_admiral();

		int DetectionScore = 10;
		DetectionScore += Admiral->get_detection_level()*3;
		DetectionScore += Planet->get_building().get(BUILDING_MILITARY_BASE);

		int
			Chance = 50 * (DetectionScore / StealthScore);

		if (mAttacker->has_ability(ABILITY_STEALTH))
			Chance -= 25;

		if (mAttacker->has_project(9009)) // STEALTH_PIRATE (Buckaneer Ending Mk. I)
			Chance -= 25;

		if (number(100) <= Chance)
		{
			mReport.format(GETTEXT("Your privateer is detected by %1$s."),
				BattleFleet->get_nick());
			mReport += "<BR>\n";

			if (aBattle)
			{
				CString News;

				News.format(GETTEXT("%1$s was detected by %2$s in %3$s's orbit, "
						"%4$s attacked %5$s's privateer fleet."),
					PrivateerFleet->get_nick(),
					BattleFleet->get_nick(),
					Planet->get_name(),
					BattleFleet->get_nick(),
					mAttacker->get_nick());
				mDefender->time_news((char *)News);

				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);
				BattleFleet->raid_attack(PrivateerFleet);

				if (PrivateerFleet->get_status() ==
					CBattleFleet::STATUS_ANNIHILATED_THIS_TURN)
				{
					mReport.format(GETTEXT("%1$s's %2$s attacks and sinks the privateer fleet %3$s."),
						mDefender->get_nick(),
						BattleFleet->get_nick(),
						PrivateerFleet->get_nick());
					mReport += "<BR>\n";
				}
				else
				{
					mReport.format(GETTEXT("%1$s's %2$s attacks the raid fleet %3$s (%4$d/%5$d ship)."),
						mDefender->get_nick(),
						BattleFleet->get_nick(),
						PrivateerFleet->get_nick(),
						PrivateerFleet->count_active_ship(),
						PrivateerFleet->get_max_ship());
					mReport += "<BR>\n";
				}
			}
			return true;
		}
	}
	return false;
}

CBattleRecord::CBattleRecord()
{
	mID = 0;
	mFireID = 1;
	mIsDraw = false;
	mThereWasBattle = 0;
}

CBattleRecord::CBattleRecord(CPlayer *aAttacker, CPlayer *aDefender, int aWarType, void *aBattleField)
{
	mID = 0;
	mAttackerID = aAttacker->get_game_id();
	mDefenderID = aDefender->get_game_id();
	mAttackerName = aAttacker->get_name();
	mDefenderName = aDefender->get_name();

	if (aAttacker->get_game_id() != 0)
	{
		mAttackerRace = aAttacker->get_race();
		mAttackerCouncil = aAttacker->get_council_id();
	}
	else
	{
		mAttackerRace = 0;
		mAttackerCouncil = 0;
	}

	if (aDefender->get_game_id() != 0)
	{
		mDefenderRace = aDefender->get_race();
		mDefenderCouncil = aDefender->get_council_id();
	}
	else
	{
		mDefenderRace = 0;
		mDefenderCouncil = 0;
	}

	mTime = time(0);
	mWarType = aWarType;
	mIsDraw = false;
	(void)mWinner;

	switch (mWarType)
	{
		case CBattle::WAR_SIEGE :
		case CBattle::WAR_PRIVATEER :
		case CBattle::WAR_RAID :
		case CBattle::WAR_RATED_SIEGE :
		case CBattle::WAR_RATED_BLOCKADE :
		case CBattle::WAR_BLOCKADE :
			{
				CPlanet *
				Planet = (CPlanet *)aBattleField;
				mPlanetID = Planet->get_id();
				mBattleFieldName = Planet->get_name();
			}
			break;

		case CBattle::WAR_MAGISTRATE :
		case CBattle::WAR_MAGISTRATE_COUNTERATTACK :
		case CBattle::WAR_EMPIRE_PLANET :
		case CBattle::WAR_EMPIRE_PLANET_COUNTERATTACK :
			{
				CPlanet *
				Planet = (CPlanet *)aBattleField;
				mPlanetID = Planet->get_id();
				mBattleFieldName = Planet->get_name();
			}
			break;

		case CBattle::WAR_FORTRESS :
			{
				CFortress *
				Fortress = (CFortress *)aBattleField;
				(void)mPlanetID;
				mBattleFieldName = Fortress->get_name();
			}
			break;

		case CBattle::WAR_EMPIRE_CAPITAL_PLANET :
			{
				CEmpireCapitalPlanet *
				EmpireCapitalPlanet = (CEmpireCapitalPlanet *)aBattleField;
				(void)mPlanetID;
				mBattleFieldName = EmpireCapitalPlanet->get_name();
			}
			break;

		default :
			{
				SLOG("ERROR : Wrong mWarType in CBattleRecord::CBattleRecord(), mWarType = %d", mWarType);
				return;
			}
			break;
	}

	mThereWasBattle = 0;

	mFireID = 1;

	add_buf((char *)format("FIELD/%s\n",
			(char *)mBattleFieldName));
	add_buf((char *)format("ATTACKER/%s/%d/%d\n",
			(char *)mark_forward_slashes(mAttackerName), mAttackerID, mAttackerRace));
	add_buf((char *)format("DEFENDER/%s/%d/%d\n",
			(char *)mark_forward_slashes(mDefenderName), mDefenderID, mDefenderRace));
	add_buf((char *)format("TIME/%d\n",
			mTime));
}

CBattleRecord::CBattleRecord(MYSQL_ROW aRow)
{
	mID = atoi(aRow[STORE_ID]);
	mAttackerID = atoi(aRow[STORE_ATTACKER_ID]);
	mDefenderID = atoi(aRow[STORE_DEFENDER_ID]);
	mAttackerName = aRow[STORE_ATTACKER_NAME];
	mDefenderName = aRow[STORE_DEFENDER_NAME];
	mAttackerRace = atoi(aRow[STORE_ATTACKER_RACE]);
	mDefenderRace = atoi(aRow[STORE_DEFENDER_RACE]);
	mAttackerCouncil = atoi(aRow[STORE_ATTACKER_COUNCIL]);
	mDefenderCouncil = atoi(aRow[STORE_DEFENDER_COUNCIL]);
	mTime = atoi(aRow[STORE_TIME]);
	mWarType = atoi(aRow[STORE_WAR_TYPE]);
	if (strcmp(aRow[STORE_IS_DRAW], "YES") == 0)
	{
		mIsDraw = true;
	}
	else
	{
		mIsDraw = false;
	}

	mWinner = atoi(aRow[STORE_WINNER]);
	mPlanetID = atoi(aRow[STORE_PLANET_ID]);
	mBattleFieldName = aRow[STORE_BATTLE_FIELD_NAME];

	mAttackerGain = aRow[STORE_ATTACKER_GAIN];
	if (mAttackerGain == NULL || strcmp(mAttackerGain, "(null)") == 0)
	{
		mAttackerGain = "";
	}
	mAttackerLoseFleet = aRow[STORE_ATTACKER_LOSE_FLEET];
	if (mAttackerLoseFleet == NULL || strcmp(mAttackerLoseFleet, "(null)") == 0)
	{
		mAttackerLoseFleet = "";
	}
	mAttackerLoseAdmiral = aRow[STORE_ATTACKER_LOSE_ADMIRAL];
	if (mAttackerLoseAdmiral == NULL || strcmp(mAttackerLoseAdmiral, "(null)") == 0)
	{
		mAttackerLoseAdmiral = "";
	}
	mDefenderLoseFleet = aRow[STORE_DEFENDER_LOSE_FLEET];
	if (mDefenderLoseFleet == NULL || strcmp(mDefenderLoseFleet, "(null)") == 0)
	{
		mDefenderLoseFleet = "";
	}
	mDefenderLoseAdmiral = aRow[STORE_DEFENDER_LOSE_ADMIRAL];
	if (mDefenderLoseAdmiral == NULL || strcmp(mDefenderLoseAdmiral, "(null)") == 0)
	{
		mDefenderLoseAdmiral = "";
	}

	mRecordFile = aRow[STORE_RECORD_FILE];
	mThereWasBattle = atoi(aRow[STORE_THERE_WAS_BATTLE]);
	mFireID = 1;
}

CBattleRecord::~CBattleRecord()
{
}

void
	CBattleRecord::set_id(int aID)
{
	mID = aID;
}

CString &
CBattleRecord::query()
{
	static CString
		Query;

	Query.clear();

	switch (type())
	{
		case QUERY_INSERT :
			{
				CString
					IsDrawString;
				if (mIsDraw == true)
				{
					IsDrawString = "YES";
				}
				else
				{
					IsDrawString = "NO";
				}

				Query = "INSERT INTO battle_record (id, attacker_id, defender_id, attacker_name, defender_name, attacker_race, defender_race, attacker_council, defender_council, time, war_type, is_draw, winner, planet_id, battle_field_name, attacker_gain, attacker_lose_fleet, attacker_lose_admiral, defender_lose_fleet, defender_lose_admiral, record_file, there_was_battle) VALUES (";
				Query.format("%d, %d, %d",
					mID,
					mAttackerID,
					mDefenderID);
				Query.format(", '%s'", (char *)add_slashes((char *)mAttackerName));
				Query.format(", '%s'", (char *)add_slashes((char *)mDefenderName));
				Query.format(", %d, %d, %d, %d, %d, %d, '%s', %d, %d, '%s'",
					mAttackerRace,
					mDefenderRace,
					mAttackerCouncil,
					mDefenderCouncil,
					mTime,
					mWarType,
					(char *)IsDrawString,
					mWinner,
					mPlanetID,
					(char *)add_slashes((char *)mBattleFieldName));

				if (add_slashes((char *)mAttackerGain).length() == 0)
				{
					Query += ", ''";
				}
				else
				{
					Query.format(", '%s'", (char *)add_slashes((char *)mAttackerGain));
				}
				if (add_slashes((char *)mAttackerLoseFleet).length() == 0)
				{
					Query += ", ''";
				}
				else
				{
					Query.format(", '%s'", (char *)add_slashes((char *)mAttackerLoseFleet));
				}
				if (mAttackerLoseAdmiral.length() == 0)
				{
					Query += ", ''";
				}
				else
				{
					Query.format(", '%s'", (char *)add_slashes((char *)mAttackerLoseAdmiral));
				}
				if (add_slashes((char *)mDefenderLoseFleet).length() == 0)
				{
					Query += ", ''";
				}
				else
				{
					Query.format(", '%s'", (char *)add_slashes((char *)mDefenderLoseFleet));
				}
				if (mDefenderLoseAdmiral.length() == 0)
				{
					Query += ", ''";
				}
				else
				{
					Query.format(", '%s'", (char *)add_slashes((char *)mDefenderLoseAdmiral));
				}
				Query.format(", '%s', %d)",
					(char *)mRecordFile,
					mThereWasBattle);
			}
			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM battle_record WHERE id = %d", mID);
			break;

		default :
			break;
	}

	mStoreFlag.clear();

	return Query;
}

void
	CBattleRecord::add_buf(char *aStr)
{
	mBuf += aStr;
}

void
	CBattleRecord::add_fleet( CBattleFleet *aFleet )
{
	CString
		Buf;

	CString
		FleetNickString,
		FleetClassNameString;
	FleetNickString = (char *)mark_forward_slashes(aFleet->get_nick());
	FleetClassNameString = (char *)mark_forward_slashes(aFleet->get_class_name());

	Buf.format( "FL/%d/%d/%s/%s/%s/%s/%d/%d/%d/%d/%d\n", aFleet->get_real_owner(), aFleet->get_real_id(), (char *)FleetNickString, aFleet->get_admiral()->get_name(), (char *)FleetClassNameString, /*aFleet->get_size_name()*/ "NONE", aFleet->count_active_ship(), aFleet->get_x(), aFleet->get_y(), (int)aFleet->get_direction(), aFleet->get_command() );
/*
	// start telecard 2001/02/07
	Buf.format( "# %s/%s/%s/%s/%s\n",
					aFleet->get_size_name(),
					aFleet->get_armor()->get_name(),
					aFleet->get_engine()->get_name(),    //BY MINDWARPER [16/3/2004]
					aFleet->get_computer()->get_name(),
					aFleet->get_shield()->get_name() );

	for(int i=0 ; i<WEAPON_MAX_NUMBER ; i++)
	{
		CTurret
			*turret = aFleet->get_turret(i);
		//if( turret != NULL )
		if( turret->is_empty() == false )
		{
			// start telecard 2001/02/07
			Buf.format( "# %s * %d\n",
							turret->get_name(),
							turret->get_number() );
		}
	}
	for(int i=0 ; i<DEVICE_MAX_NUMBER ; i++)
	{
		CDevice
			*device = aFleet->get_device(i);
		if( device != NULL )
		{
			// start telecard 2001/02/07
			//Buf.format( "# %s\n", device->get_name() );
		}
	}
	// end telecard
*/
	add_buf( (char*)Buf );
}

void
	CBattleRecord::add_fire( CBattleFleet *aAttacker, CBattleFleet *aTarget, CTurret *aTurret, int aHitChance )
{
	CString
		Buf;

	Buf.format( "F/%d/%d/%d/%d/%d/%d/%s/%d/%d/%d\n", get_new_fire_id(), mTurn, aAttacker->get_real_owner(), aAttacker->get_real_id(), aTarget->get_real_owner(), aTarget->get_real_id(), aTurret->get_name(), aTurret->get_weapon_type(), aTurret->get_number()*aAttacker->count_active_ship(), aHitChance );
	// end telecard
	add_buf( (char*)Buf );
}

void
	CBattleRecord::add_hit( CBattleFleet *aAttacker, CBattleFleet *aTarget, int aHitCount, int aMissCount, int aTotalDamage, int aSunkCount )
{
	CString
		Buf;
	Buf.format( "H/%d/%d/%d/%d/%d/%d/\n", get_fire_id(), mTurn, aHitCount, aMissCount, aTotalDamage, aSunkCount );
	// end telecard
	add_buf( (char*)Buf );
}

void
	CBattleRecord::add_location( CBattleFleet *aFleet )
{
	CString
		Buf;
	Buf.format( "M/%d/%d/%d/%d/%d/%d/%d/%d/%d\n", mTurn, aFleet->get_real_owner(), aFleet->get_real_id(), aFleet->get_x(), aFleet->get_y(), (int)aFleet->get_direction(), aFleet->get_command(), aFleet->get_substatus(), aFleet->count_active_ship() );
	add_buf( (char*)Buf );
}

void
	CBattleRecord::disable_fleet( CBattleFleet *aFleet )
{
	CString
		Buf;
	Buf.format( "D/%d/%d/%d\n", mTurn, aFleet->get_real_owner(), aFleet->get_real_id() );
	// end telecard
	add_buf( (char*)Buf );
}

void
	CBattleRecord::save()
{
	mRecordFile.clear();
	mRecordFile.format( "%s/%d/%d", ARCHSPACE->configuration().get_string( "Game", "BattleLogDir" ), as_calc_date_index(8), mID );

	FILE
		*fp = fopen( (char *)mRecordFile, "w" );
	if (!fp)
	{
		SLOG("ERROR : Battle log file(%s) open failed.", (char *)mRecordFile);
		return;
	}

	fputs( (char *)mBuf, fp );
	fclose(fp);

	type(QUERY_INSERT);
	STORE_CENTER->store(*this);
}

char *
	CBattleRecord::get_war_type_string()
{
	if (mWarType == CBattle::WAR_SIEGE)
	{
		return GETTEXT("Siege");
	}
	else if (mWarType == CBattle::WAR_PRIVATEER)
	{
		return GETTEXT("Privateer");
	}
	else if (mWarType == CBattle::WAR_RATED_SIEGE)
	{
		return GETTEXT("Rated Siege");
	}
	else if (mWarType == CBattle::WAR_RATED_BLOCKADE)
	{
		return GETTEXT("Rated Blockade");
	}
	else if (mWarType == CBattle::WAR_RAID)
	{
		return GETTEXT("Raid");
	}
	else if (mWarType == CBattle::WAR_BLOCKADE)
	{
		return GETTEXT("Blockade");
	}
	else if (mWarType == CBattle::WAR_MAGISTRATE)
	{
		return GETTEXT("Magistrate");
	}
	else if (mWarType == CBattle::WAR_MAGISTRATE_COUNTERATTACK)
	{
		return GETTEXT("Magistrate Attack");
	}
	else if (mWarType == CBattle::WAR_EMPIRE_PLANET)
	{
		return GETTEXT("Empire Planet");
	}
	else if (mWarType == CBattle::WAR_EMPIRE_PLANET_COUNTERATTACK)
	{
		return GETTEXT("Empire Planet Attack");
	}
	else if (mWarType == CBattle::WAR_FORTRESS)
	{
		return GETTEXT("Fortress");
	}
	else if (mWarType == CBattle::WAR_EMPIRE_CAPITAL_PLANET)
	{
		return GETTEXT("Empire Capital Planet");
	}
	else
	{
		SLOG("ERROR : Wrong mWarType in CBattleRecord::get_war_type_string(), mWarType = %d", mWarType);
		return NULL;
	}
}

void
	CBattleRecord::set_draw()
{
	mIsDraw = true;
}

void
	CBattleRecord::reset_draw()
{
	mIsDraw = false;
}

CBattleRecordTable::CBattleRecordTable()
{

}

CBattleRecordTable::~CBattleRecordTable()
{
	remove_all();
}

bool
	CBattleRecordTable::free_item( TSomething aItem )
{
	CBattleRecord
		*Record = (CBattleRecord*)aItem;
	if( !Record ) return false;

	delete Record;

	return true;
}

int
	CBattleRecordTable::compare( TSomething aItem1, TSomething aItem2 ) const
{
	CBattleRecord
		*fleet1 = (CBattleRecord*)aItem1,
	*fleet2 = (CBattleRecord*)aItem2;

	if ( fleet1->get_id() > fleet2->get_id() ) return 1;
	if ( fleet1->get_id() < fleet2->get_id() ) return -1;
	return 0;
}

int
	CBattleRecordTable::compare_key( TSomething aItem, TConstSomething aKey ) const
{
	CBattleRecord *
	fleet = (CBattleRecord*)aItem;

	if (fleet->get_id() > (int)aKey) return 1;
	if (fleet->get_id() < (int)aKey) return -1;
	return 0;
}

bool
	CBattleRecordTable::remove_battle_record(int aBattleRecordID)
{
	int
		index;
	index = find_sorted_key( (void*)aBattleRecordID );
	if (index < 0) return false;

	return CSortedList::remove(index);
}

int
	CBattleRecordTable::add_battle_record(CBattleRecord *aBattleRecord)
{
	if (!aBattleRecord) return -1;

	if( aBattleRecord->get_id() == 0 ){
		if( length() == 0 )
			aBattleRecord->set_id(1);
		else
			aBattleRecord->set_id( ((CBattleRecord*)get(length()-1))->get_id()+1 );
	}

	if (find_sorted_key( (TConstSomething)aBattleRecord->get_id() ) >= 0) return -1;
	insert_sorted( aBattleRecord );

	return aBattleRecord->get_id();
}

CBattleRecord*
CBattleRecordTable::get_by_id( int aBattleRecordID )
{
	if ( !length() ) return NULL;

	int
		index;

	index = find_sorted_key( (void*)aBattleRecordID );

	if (index < 0) return NULL;

	return (CBattleRecord*)get(index);
}

bool
	CBattleRecordTable::load(CMySQL &aMySQL)
{
	SLOG( "Battle Record Loading" );

	//aMySQL.query("LOCK TABLE battle_record READ");
	aMySQL.query("SELECT * FROM battle_record");

	aMySQL.use_result();

	int
		deleteIndex = as_calc_date_index(8) + 1;
	if (deleteIndex == 8)
	{
		deleteIndex = 0;
	}
	while( aMySQL.fetch_row() )
	{
		CBattleRecord* record = new CBattleRecord( aMySQL.row() );
		if( (record->get_time()/86400)%8 == deleteIndex)
		{
			record->type( QUERY_DELETE );
			STORE_CENTER->store( *record );
			delete record;
			continue;
		}
		else
		{	add_battle_record( record );
		}
	}
	// end telecard 2001/02/12

	aMySQL.free_result();
	//aMySQL.query( "UNLOCK TABLES" );

	SLOG( "%d battle records are loaded", length() );

	return true;
}
