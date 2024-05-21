#include "common.h"
#include "util.h"
#include "planet.h"
#include <cstdlib>
#include "archspace.h"
#include "game.h"
#include <libintl.h>
#include "race.h"
#include <cmath>
#include "council.h"
#include "admin.h"
#include "define.h"
#include <cmath>

int CPlanet::mRatioUltraPoor = 50;
int CPlanet::mRatioPoor = 75;
int CPlanet::mRatioNormal = 100;
int CPlanet::mRatioRich = 175;
int CPlanet::mRatioUltraRich = 250;
int CPlanet::mCommerceMultiplier = 100;

TZone gPlanetZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPlanet),
	0,
	0,
	NULL,
	"Zone CPlanet"
};

const char*
	get_attribute_description(CPlanet::EPlanetAttribute aAttribute)
{
	switch(aAttribute)
	{
		case CPlanet::PA_ARTIFACT:
			return GETTEXT("Artifact");
		case CPlanet::PA_MASSIVE_ARTIFACT:
			return GETTEXT("Massive Artifact");
		case CPlanet::PA_ASTEROID:
			return GETTEXT("Asteroid");
		case CPlanet::PA_MOON:
			return GETTEXT("Moon");
		case CPlanet::PA_MOON_CLUSTER:
			return GETTEXT("Moon Cluster");
		case CPlanet::PA_RADIATION:
			return GETTEXT("Radiation");
		case CPlanet::PA_SEVERE_RADIATION:
			return GETTEXT("Severe Radiation");
		case CPlanet::PA_HOSTILE_MONSTER:
			return GETTEXT("Hostile Monster");
		case CPlanet::PA_OBSTINATE_MICROBE:
			return GETTEXT("Obstinate Microbe");
		case CPlanet::PA_BEAUTIFUL_LANDSCAPE:
			return GETTEXT("Beautiful Landscape");
		case CPlanet::PA_BLACK_HOLE:
			return GETTEXT("Black Hole");
		case CPlanet::PA_NEBULA:
			return GETTEXT("Nebula");
		case CPlanet::PA_DARK_NEBULA:
			return GETTEXT("Dark Nebula");
		case CPlanet::PA_VOLCANIC_ACTIVITY:
			return GETTEXT("Volcanic Activity");
		case CPlanet::PA_INTENSE_VOLCANIC_ACTIVITY:
			return GETTEXT("Intense Volcanic Activity");
		case CPlanet::PA_OCEAN:
			return GETTEXT("Ocean");
		case CPlanet::PA_IRREGULAR_CLIMATE:
			return GETTEXT("Irregular Climate");
		case CPlanet::PA_MAJOR_SPACE_ROUTE:
			return GETTEXT("Major Space Route");
		case CPlanet::PA_MAJOR_SPACE_CROSSROUTE:
			return GETTEXT("Major Space Crossroutes");
		case CPlanet::PA_FRONTIER_AREA:
			return GETTEXT("Frontier Area");
		case CPlanet::PA_GRAVITY_CONTROLED:
			return GETTEXT("Gravity Controlled");
		case CPlanet::PA_ANCIENT_RUINS:
			return GETTEXT("Ancient Ruins");
		case CPlanet::PA_SHIP_YARD:
			return GETTEXT("Ship Yard");
		case CPlanet::PA_MILITARY_STRONGHOLD:
			return GETTEXT("Military Stronghold");
		case CPlanet::PA_MAINTENANCE_CENTER:
			return GETTEXT("Maintenance Center");
		case CPlanet::PA_COGNITION_AMPLIFIER:
			return GETTEXT("Cognition Amplifier Relic");
		case CPlanet::PA_UNDERGROUND_CAVERNS:
			return GETTEXT("Underground Caverns");
		case CPlanet::PA_RARE_ORE:
			return GETTEXT("Rare Ore");
		case CPlanet::PA_LOST_TRABOTULIN_LIBRARY:
			return GETTEXT("Lost Trabotulin Library");
/*		case CPlanet::PA_STABLE_WORMHOLE:
			return GETTEXT("Stable Wormhole");
		case CPlanet::PA_DIPLOMATIC_OUTPOST:
			return GETTEXT("Diplomatic Outpost");*/
		case CPlanet::PA_END:
			return "";
	}

	return NULL;;
}

const char*
	get_size_description(int aSize)
{
	switch(aSize)
	{
		case CPlanet::SIZE_TINY:
			return GETTEXT("Tiny");
		case CPlanet::SIZE_SMALL:
			return GETTEXT("Small");
		case CPlanet::SIZE_MEDIUM:
			return GETTEXT("Medium");
		case CPlanet::SIZE_LARGE:
			return GETTEXT("Large");
		case CPlanet::SIZE_HUGE:
			return GETTEXT("Huge");
	}
	return NULL;
}


const char*
	get_resource_description(int aResource)
{
	switch(aResource)
	{
		case CPlanet::RESOURCE_ULTRA_POOR:
			return GETTEXT("Ultra Poor");
		case CPlanet::RESOURCE_POOR:
			return GETTEXT("Poor");
		case CPlanet::RESOURCE_NORMAL:
			return GETTEXT("Normal");
		case CPlanet::RESOURCE_RICH:
			return GETTEXT("Rich");
		case CPlanet::RESOURCE_ULTRA_RICH:
			return GETTEXT("Ultra Rich");
	}
	return NULL;
}




int CPlanet::mMaxID = 0;

CPlanet::CPlanet():mDistributeRatio(40,30,30)
{
	mPlanetNewsCenter.set_planet(this);

	mID = 0;
	mOwnerID = 0;
	mClusterID = -1;
	mCluster = NULL;
	mOrder = -1;
	mMaxPopulation = 0;
	mPopulation = 0;
	mTemperature = 300;
	mAtmosphere[GAS_H2] = 0;
	mAtmosphere[GAS_Cl2] = 0;
	mAtmosphere[GAS_CO2] = 0;
	mAtmosphere[GAS_O2] = 0;
	mAtmosphere[GAS_N2] = 0;
	mAtmosphere[GAS_CH4] = 0;
	mAtmosphere[GAS_H2O] = 0;
	mSize = 2;
	mResource = 2;
	mGravity = 1.0;

	mInvestment = 0;
	mInvestRate = 0;
	mWasteRate = 0;

	mTerraforming = false;
	mTerraformingTimer = 0;

	mCommerceWith[0] = 0;
	mCommerceWith[1] = 0;
	mCommerceWith[2] = 0;

	mPrivateerTimer = 0;
	mPrivateerAmount = 0;

	mBlockadeTimer = 0;

	compute_max_population();
}

CPlanet::CPlanet(MYSQL_ROW aRow)
{
	mPlanetNewsCenter.set_planet(this);

	mID = atoi(aRow[STORE_ID]);

	mClusterID = atoi(aRow[STORE_CLUSTER]);
	mCluster = UNIVERSE->get_by_id(mClusterID);

	mOwnerID = atoi(aRow[STORE_OWNER]);

	mOrder = atoi(aRow[STORE_ORDER]);
	mName = (char *)aRow[STORE_NAME];
	mAttribute.set_string(aRow[STORE_ATTRIBUTE]);
	mPopulation = atoi(aRow[STORE_POPULATION]);

	mBuilding.set_all(
		atoi(aRow[STORE_BUILDING_FACTORY]),
		atoi(aRow[STORE_BUILDING_MILITARY_BASE]),
		atoi(aRow[STORE_BUILDING_RESEARCH_LAB]));

	mBuildingProgress.set_all(
		atoi(aRow[STORE_PROGRESS_FACTORY]),
		atoi(aRow[STORE_PROGRESS_MILITARY_BASE]),
		atoi(aRow[STORE_PROGRESS_RESEARCH_LAB]));

	mDistributeRatio.set_all(
		atoi(aRow[STORE_RATIO_FACTORY]),
		atoi(aRow[STORE_RATIO_MILITARY_BASE]),
		atoi(aRow[STORE_RATIO_RESEARCH_LAB]));

	set_atmosphere(aRow[STORE_ATMOSPHERE]);
	set_temperature(atoi(aRow[STORE_TEMPERATURE]));
	set_size(atoi(aRow[STORE_SIZE]));
	set_resource(atoi(aRow[STORE_RESOURCE]));
	set_gravity(atof(aRow[STORE_GRAVITY]));

	set_destroy_timer(atoi(aRow[STORE_TURNS_TILL_DESTRUCTION]));

	if (atoi(aRow[STORE_PLANET_INVEST_POOL])) {
		mPlanetInvestPool = TRUE;
	} else {
		mPlanetInvestPool = FALSE;
	}

	mInvestment = atoi(aRow[STORE_INVESTMENT]);
	mInvestRate = 0;
	mWasteRate = calc_waste_rate();

	mTerraforming = (bool)atoi(aRow[STORE_TERRAFORMING]);
	mTerraformingTimer = atoi(aRow[STORE_TERRAFORMING_TIMER]);

	mCommerceWith[0] = atoi(aRow[STORE_COMMERCE_WITH_1]);
	mCommerceWith[1] = atoi(aRow[STORE_COMMERCE_WITH_2]);
	mCommerceWith[2] = atoi(aRow[STORE_COMMERCE_WITH_3]);

	mPrivateerTimer = atoi(aRow[STORE_PRIVATEER_TIMER]);
	mPrivateerAmount = 0;

	mBlockadeTimer = atoi(aRow[STORE_BLOCKADE_TIMER]);

	mPlanetNewsCenter.set_population(atoi(aRow[STORE_NEWS_POPULATION]));
	mPlanetNewsCenter.set_factory(atoi(aRow[STORE_NEWS_FACTORY]));
	mPlanetNewsCenter.set_military_base(atoi(aRow[STORE_NEWS_MILITARY_BASE]));
	mPlanetNewsCenter.set_research_lab(atoi(aRow[STORE_NEWS_RESEARCH_LAB]));

	if (mID > mMaxID) mMaxID = mID;

	refresh_resource_per_turn();
	compute_max_population();
}

CPlanet::~CPlanet()
{
}

void
	CPlanet::start_terraforming()
{
	mStoreFlag += STORE_TERRAFORMING;
	mTerraforming = true;
	mTerraformingTimer = 0;
}

CPlayer*
CPlanet::get_owner()
{
	CPlayer *
	Player = PLAYER_TABLE->get_by_game_id(mOwnerID);
	return Player;
}

CCluster*
CPlanet::get_cluster()
{
	if (mCluster) return mCluster;
	if (mClusterID <= 0) return NULL;
	mCluster = UNIVERSE->get_by_id(mClusterID);
	return mCluster;
}

int
	CPlanet::get_cluster_id()
{
	if (mClusterID > 0) return mClusterID;
	if (!mCluster)
	{
		if (mClusterID <= 0) return -1;
		mCluster = UNIVERSE->get_by_id(mClusterID);
		if (!mCluster) return -1;
		mClusterID = mCluster->get_id();
	}
	return mCluster->get_id();
}

char *
	CPlanet::get_population_with_unit()
{
	if (mPopulation < 1000)
	{
		return (char *)format("%s k", dec2unit(mPopulation));
	}
	else
	{
		return (char *)format("%s M", dec2unit(mPopulation/1000));
	}
}

const char*
	CPlanet::get_attribute_description(CPlanet::EPlanetAttribute aAttribute)
{
	switch(aAttribute)
	{
		case CPlanet::PA_ARTIFACT:
			return GETTEXT("Artifact");
		case CPlanet::PA_MASSIVE_ARTIFACT:
			return GETTEXT("Massive Artifact");
		case CPlanet::PA_ASTEROID:
			return GETTEXT("Asteroid");
		case CPlanet::PA_MOON:
			return GETTEXT("Moon");
		case CPlanet::PA_MOON_CLUSTER:
			return GETTEXT("Moon Cluster");
		case CPlanet::PA_RADIATION:
			return GETTEXT("Radiation");
		case CPlanet::PA_SEVERE_RADIATION:
			return GETTEXT("Severe Radiation");
		case CPlanet::PA_HOSTILE_MONSTER:
			return GETTEXT("Hostile Monster");
		case CPlanet::PA_OBSTINATE_MICROBE:
			return GETTEXT("Obstinate Microbe");
		case CPlanet::PA_BEAUTIFUL_LANDSCAPE:
			return GETTEXT("Beautiful Landscape");
		case CPlanet::PA_BLACK_HOLE:
			return GETTEXT("Black Hole");
		case CPlanet::PA_NEBULA:
			return GETTEXT("Nebula");
		case CPlanet::PA_DARK_NEBULA:
			return GETTEXT("Dark Nebula");
		case CPlanet::PA_VOLCANIC_ACTIVITY:
			return GETTEXT("Volcanic Activity");
		case CPlanet::PA_INTENSE_VOLCANIC_ACTIVITY:
			return GETTEXT("Intense Volcanic Activity");
		case CPlanet::PA_OCEAN:
			return GETTEXT("Oceanic");
		case CPlanet::PA_IRREGULAR_CLIMATE:
			return GETTEXT("Irregular Climate");
		case CPlanet::PA_MAJOR_SPACE_ROUTE:
			return GETTEXT("Major Space Route");
		case CPlanet::PA_MAJOR_SPACE_CROSSROUTE:
			return GETTEXT("Major Space Crossroutes");
		case CPlanet::PA_FRONTIER_AREA:
			return GETTEXT("Frontier Area");
		case CPlanet::PA_GRAVITY_CONTROLED:
			return GETTEXT("Gravity Controlled");
		case CPlanet::PA_ANCIENT_RUINS:
			return GETTEXT("Ancient Ruins");
		case CPlanet::PA_SHIP_YARD:
			return GETTEXT("Ship Yard");
		case CPlanet::PA_MILITARY_STRONGHOLD:
			return GETTEXT("Military Stronghold");
		case CPlanet::PA_MAINTENANCE_CENTER:
			return GETTEXT("Maintenance Center");
		case CPlanet::PA_COGNITION_AMPLIFIER:
			return GETTEXT("Cognition Amplifier Relic");
		case CPlanet::PA_UNDERGROUND_CAVERNS:
			return GETTEXT("Underground Caverns");
		case CPlanet::PA_RARE_ORE:
			return GETTEXT("Rare Ore");
		case CPlanet::PA_LOST_TRABOTULIN_LIBRARY:
			return GETTEXT("Lost Trabotulin Library");
/*		case CPlanet::PA_STABLE_WORMHOLE:
			return GETTEXT("Stable Wormhole");
		case CPlanet::PA_DIPLOMATIC_OUTPOST:
			return GETTEXT("Diplomatic Outpost");*/
		case CPlanet::PA_END:
			return "";
	}

	return "";
}

char *
	CPlanet::get_max_population_with_unit()
{
	if (mMaxPopulation < 1000)
	{
		return (char *)format("%s k", dec2unit(mMaxPopulation));
	} else
	{
		return (char *)format("%s M", dec2unit(mMaxPopulation/1000));
	}
}

bool
	CPlanet::set_max_population()
{
	compute_max_population();
	return true;
}

int
	CPlanet::get_power()
{
	return 200 + ((1 + mBuilding.get(BUILDING_FACTORY) +
			mBuilding.get(BUILDING_MILITARY_BASE) +
			mBuilding.get(BUILDING_RESEARCH_LAB)));
	//return (500 + mPopulation/100);
}

void
	CPlanet::set_owner(CPlayer* aPlayer)
{
	if (aPlayer == NULL) return;

	mOwnerID = aPlayer->get_game_id();
	mStoreFlag += STORE_OWNER;
}

void
	CPlanet::set_cluster(CCluster* aCluster)
{
	mCluster = aCluster;
	mClusterID = aCluster->get_id();
}

bool
	CPlanet::set_commerce_between(int aPlanetID1, int aPlanetID2)
{
	CPlanet
		*Planet1 = PLANET_TABLE->get_by_id(aPlanetID1),
	*Planet2 = PLANET_TABLE->get_by_id(aPlanetID2);
	if (Planet1 == NULL || Planet2 == NULL) return false;

	int
		Index1 = -1,
	Index2 = -1;

	for (int i=0 ; i<=2 ; i++)
	{
		if (Planet1->get_commerce_with(i) == 0)
		{
			Index1 = i;
			break;
		}
	}

	for (int i=0 ; i<=2 ; i++)
	{
		if (Planet2->get_commerce_with(i) == 0)
		{
			Index2 = i;
			break;
		}
	}

	if (Index1 > -1 && Index2 > -1)
	{
		Planet1->set_commerce_with(Index1, aPlanetID2);
		Planet2->set_commerce_with(Index2, aPlanetID1);
	}

	return true;
}

bool
	CPlanet::clear_commerce_between(int aPlanetID1, int aPlanetID2)
{
	CPlanet
		*Planet1 = PLANET_TABLE->get_by_id(aPlanetID1),
	*Planet2 = PLANET_TABLE->get_by_id(aPlanetID2);
	if (Planet1 == NULL || Planet2 == NULL) return false;

	Planet1->clear_commerce_with(aPlanetID2);
	Planet2->clear_commerce_with(aPlanetID1);

	return true;
}

bool
	CPlanet::set_attribute(EPlanetAttribute aPlanetAttribute)
{
	if (aPlanetAttribute < PA_BEGIN ||
		aPlanetAttribute >= PA_END) return false;

	if (mAttribute.has(aPlanetAttribute)) return false;

	mStoreFlag += STORE_ATTRIBUTE;
	mAttribute += aPlanetAttribute;

	return true;
}

bool
	CPlanet::remove_attribute(EPlanetAttribute aPlanetAttribute)
{
	if (aPlanetAttribute < PA_BEGIN ||
		aPlanetAttribute >= PA_END) return false;

	if (!mAttribute.has(aPlanetAttribute)) return false;

	mStoreFlag += STORE_ATTRIBUTE;
	mAttribute -= aPlanetAttribute;

	if (aPlanetAttribute == PA_GRAVITY_CONTROLED)
		start_terraforming();

	return true;
}

bool
	CPlanet::has_attribute(EPlanetAttribute aPlanetAttribute)
{
	if (aPlanetAttribute < PA_BEGIN ||
		aPlanetAttribute >= PA_END) return false;

	return mAttribute.has(aPlanetAttribute);
}

bool CPlanet::has_attribute(int aPlanetAttribute) {
	if ((EPlanetAttribute) aPlanetAttribute < PA_BEGIN ||
		(EPlanetAttribute) aPlanetAttribute >= PA_END) return false;

	return mAttribute.has((EPlanetAttribute) aPlanetAttribute);
}

CString&
CPlanet::query()
{
	static CString
		Query;

	Query.clear();

	switch( type() ){
		case QUERY_INSERT :
			Query = "INSERT INTO planet ( "
				"id, "
				"cluster, "
				"owner, "
				"order_, "
				"name, "
				"attribute, "
				"population, "

				"building_factory, "
				"building_military_base, "
				"building_research_lab, "

				"progress_factory, "
				"progress_military_base, "
				"progress_research_lab, "

				"ratio_factory, "
				"ratio_military_base, "
				"ratio_research_lab, "

				"atmosphere, "
				"temperature, "
				"size, "
				"resource, "
				"gravity, "

				"terraforming, "
				"terraforming_timer, "

				"commerce_with_1, "
				"commerce_with_2, "
				"commerce_with_3, "

				"privateer_timer, "
				"blockade_timer, "

				"news_population, "
				"news_factory, "
				"news_military_base,"
				"news_research_lab "
				") VALUES ( ";

			Query.format("%d, %d, %d, %d, '%s', '%s', %d",
				mID,
				get_cluster_id(),
				get_owner_id(),
				mOrder,
				(mName.length()) ?
				(char*)add_slashes((char *)mName):"",
				mAttribute.get_string(PA_END),
				mPopulation );

			Query.format(", %d, %d, %d",
				mBuilding.get(BUILDING_FACTORY),
				mBuilding.get(BUILDING_MILITARY_BASE),
				mBuilding.get(BUILDING_RESEARCH_LAB));

			Query.format(", %d, %d, %d",
				mBuildingProgress.get(BUILDING_FACTORY),
				mBuildingProgress.get(BUILDING_MILITARY_BASE),
				mBuildingProgress.get(BUILDING_RESEARCH_LAB) );

			Query.format(", %d, %d, %d",
				mDistributeRatio.get(BUILDING_FACTORY),
				mDistributeRatio.get(BUILDING_MILITARY_BASE),
				mDistributeRatio.get(BUILDING_RESEARCH_LAB) );

			Query.format(", '%s', %d, %d, %d, %f",
				get_atmosphere(),
				mTemperature,
				mSize,
				mResource,
				mGravity);


			Query.format(", %d, %d",
				(mTerraforming) ? 1:0,
				mTerraformingTimer);

			Query.format(", %d, %d, %d",
				mCommerceWith[0], mCommerceWith[1], mCommerceWith[2]);

			Query.format(", %d, %d",
				mPrivateerTimer, mBlockadeTimer);

			Query.format(", %d, %d, %d, %d)",
				mPlanetNewsCenter.get_population(),
				mPlanetNewsCenter.get_factory(),
				mPlanetNewsCenter.get_military_base(),
				mPlanetNewsCenter.get_research_lab());

			break;

		case QUERY_UPDATE :
			Query.format("UPDATE planet SET population = %d", get_population());
			if (mTerraforming)
				Query.format(", terraforming_timer = %d",
				mTerraformingTimer);

#define STORE(x, y, z) if (mStoreFlag.has(x)) Query.format(y, z)

			STORE(STORE_ID, ", id = %d", mID);
			STORE(STORE_CLUSTER, ", cluster = %d", mClusterID);
			STORE(STORE_OWNER, ", owner = %d", mOwnerID);
			STORE(STORE_ORDER, ", order_ = %d", mOrder);
			STORE(STORE_NAME, ", name = '%s'",
				(mName.length()) ? (char*)add_slashes((char *)mName):"");
			STORE(STORE_ATTRIBUTE, ", attribute = '%s'",
				mAttribute.get_string(PA_END));
			STORE(STORE_POPULATION, ", population = %d", mPopulation);

			Query += mBuilding.get_query("building_");
			Query += mBuildingProgress.get_query("progress_");
			Query += mDistributeRatio.get_query("ratio_");

			STORE(STORE_ATMOSPHERE, ", atmosphere = '%s'", get_atmosphere());
			STORE(STORE_TEMPERATURE, ", temperature = %d", mTemperature);
			STORE(STORE_SIZE, ", size = %d", mSize);
			STORE(STORE_RESOURCE, ", resource = %d", mResource);
			STORE(STORE_GRAVITY, ", gravity = %f", mGravity);

			STORE(STORE_INVESTMENT, ", investment = %d", mInvestment);

			STORE(STORE_TERRAFORMING, ", terraforming = %d",
				(mTerraforming) ? 1:0);
			STORE(STORE_TERRAFORMING_TIMER,
				", terraforming_timer = %d", mTerraformingTimer);

			STORE(STORE_COMMERCE_WITH_1, ", commerce_with_1=%d", mCommerceWith[0]);
			STORE(STORE_COMMERCE_WITH_2, ", commerce_with_2=%d", mCommerceWith[1]);
			STORE(STORE_COMMERCE_WITH_3, ", commerce_with_3=%d", mCommerceWith[2]);

			STORE(STORE_PRIVATEER_TIMER, ", privateer_timer = %d", mPrivateerTimer);
			STORE(STORE_BLOCKADE_TIMER, ", blockade_timer = %d", mBlockadeTimer);

			STORE(STORE_TURNS_TILL_DESTRUCTION, ", turns_till_destruction = %d", mDestroyTurns);
			STORE(STORE_PLANET_INVEST_POOL, ", planet_invest_pool = %d", mPlanetInvestPool);

			Query += mPlanetNewsCenter.get_query();

			Query.format(" WHERE id = %d", mID);

			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM planet WHERE id = %d", mID);

			break;
	}

	mStoreFlag.clear();

	return Query;
}

const char*
	CPlanet::get_size_description()
{
	switch(mSize)
	{
		case SIZE_TINY:
			return GETTEXT("Tiny");
		case SIZE_SMALL:
			return GETTEXT("Small");
		case SIZE_MEDIUM:
			return GETTEXT("Medium");
		case SIZE_LARGE:
			return GETTEXT("Large");
		case SIZE_HUGE:
			return GETTEXT("Huge");
	}
	return NULL;
}

const char*
	CPlanet::get_resource_description()
{
	switch(mResource)
	{
		case RESOURCE_ULTRA_POOR:
			return GETTEXT("Ultra Poor");
		case RESOURCE_POOR:
			return GETTEXT("Poor");
		case RESOURCE_NORMAL:
			return GETTEXT("Normal");
		case RESOURCE_RICH:
			return GETTEXT("Rich");
		case RESOURCE_ULTRA_RICH:
			return GETTEXT("Ultra Rich");
	}
	return NULL;
}
bool
	CPlanet::set_atmosphere(EGasType aGasType, int aGas)
{
	if (aGasType < GAS_BEGIN || aGasType>=GAS_MAX) return false;

	mStoreFlag += STORE_ATMOSPHERE;

	mAtmosphere[aGasType] = aGas;

	return true;
}

bool
	CPlanet::set_temperature(int aTemperature)
{
	if (aTemperature < MIN_TEMPERATURE ||
		aTemperature > MAX_TEMPERATURE) return false;

	mStoreFlag += STORE_TEMPERATURE;

	mTemperature = aTemperature;

	return true;
}

bool
	CPlanet::set_gravity(double aGravity)
{
	if (aGravity < MIN_GRAVITY || aGravity > MAX_GRAVITY)
		return false;

	mStoreFlag += STORE_GRAVITY;

	mGravity = aGravity;

	return true;
}

bool
	CPlanet::set_size(int aSize)
{
	if (aSize < SIZE_TINY || aSize > SIZE_HUGE)
		return false;

	mSize = aSize;

	return true;
}

bool
	CPlanet::set_resource(int aResource)
{
	if (aResource < RESOURCE_ULTRA_POOR ||
		aResource > RESOURCE_ULTRA_RICH) return false;

	mStoreFlag += STORE_RESOURCE;

	mResource = aResource;

	return true;
}

bool
	CPlanet::change_population(int aPopulation)
{
	if (aPopulation == 0) return true;

	if (aPopulation > 0)
	{
		if ((long long int)aPopulation >
			(long long int)MAX_PLANET_POPULATION - (long long int)mPopulation)
		{
			mPopulation = MAX_PLANET_POPULATION;
		}
		else
		{
			mPopulation += aPopulation;
		}
	}
	else
	{
		if (mPopulation + aPopulation > MAX_COUNCIL_PP)
		{
			mPopulation = MAX_COUNCIL_PP;
		}
		else if (mPopulation + aPopulation < 0)
		{
			mPopulation = 0;
		}
		else
		{
			mPopulation += aPopulation;
		}
	}

	mStoreFlag += STORE_POPULATION;

	CPlayer *
	Owner = PLAYER_TABLE->get_by_game_id(mOwnerID);
	if (Owner != NULL)
	{
		if (Owner->get_game_id() != EMPIRE_GAME_ID)
		{
			Owner->get_planet_list()->refresh_power();
		}
	}

	return true;
}

bool
	CPlanet::change_atmosphere(EGasType aDecreasing, EGasType aIncreasing)
{
	if (aDecreasing < GAS_BEGIN	|| aDecreasing >= GAS_MAX) return false;
	if (aIncreasing < GAS_BEGIN	|| aIncreasing >= GAS_MAX) return false;

	if (mAtmosphere[aDecreasing] <= MIN_ATMOSPHERE) return false;
	if (mAtmosphere[aIncreasing] >= MAX_ATMOSPHERE) return false;

	mStoreFlag += STORE_ATMOSPHERE;

	mAtmosphere[aDecreasing]--;
	mAtmosphere[aIncreasing]++;

	return true;
}

void
	CPlanet::change_temperature(int aTemperature)
{
	mTemperature += aTemperature;

	if (mTemperature < MIN_TEMPERATURE)
		mTemperature = MIN_TEMPERATURE;
	if (mTemperature > MAX_TEMPERATURE)
		mTemperature = MAX_TEMPERATURE;

	mStoreFlag += STORE_TEMPERATURE;
}

void
	CPlanet::change_gravity(double aGravity)
{
	mGravity += aGravity;

	if (mGravity < MIN_GRAVITY) mGravity = MIN_GRAVITY;
	if (mGravity > MAX_GRAVITY) mGravity = MAX_GRAVITY;

	mStoreFlag += STORE_GRAVITY;
}

void
	CPlanet::change_size(int aSize)
{
	mSize += aSize;

	if (mSize < SIZE_TINY) mSize = SIZE_TINY;
	if (mSize > SIZE_HUGE) mSize = SIZE_HUGE;
}

void
	CPlanet::change_resource(int aResource)
{
	mResource += aResource;

	if (mResource < RESOURCE_ULTRA_POOR)
		mResource = RESOURCE_ULTRA_POOR;

	if (mResource > RESOURCE_ULTRA_RICH)
		mResource = RESOURCE_ULTRA_RICH;

	mStoreFlag += STORE_RESOURCE;
}

bool
	CPlanet::change_investment(int aAmount)
{
	if (aAmount == 0) return true;

	if (aAmount > 0)
	{
		if ((long long int)aAmount >
			(long long int)MAX_PLANET_INVESTMENT_PP - (long long int)mInvestment)
		{
			mInvestment = MAX_PLANET_INVESTMENT_PP;
		}
		else
		{
			mInvestment += aAmount;
		}
	}
	else
	{
		if (mInvestment + aAmount > MAX_PLANET_INVESTMENT_PP)
		{
			mInvestment = MAX_PLANET_INVESTMENT_PP;
		}
		else if (mInvestment < -aAmount)
		{
			mInvestment = 0;
		}
		else
		{
			mInvestment += aAmount;
		}
	}

	mStoreFlag += STORE_INVESTMENT;

	return true;
}

const char *
	CPlanet::get_atmosphere()
{
	static char Atmosphere[GAS_MAX+1];

	Atmosphere[GAS_H2] = mAtmosphere[GAS_H2] + '0';
	Atmosphere[GAS_Cl2] = mAtmosphere[GAS_Cl2] + '0';
	Atmosphere[GAS_CO2] = mAtmosphere[GAS_CO2] + '0';
	Atmosphere[GAS_O2] = mAtmosphere[GAS_O2] + '0';
	Atmosphere[GAS_N2] = mAtmosphere[GAS_N2] + '0';
	Atmosphere[GAS_CH4] = mAtmosphere[GAS_CH4] + '0';
	Atmosphere[GAS_H2O] = mAtmosphere[GAS_H2O] + '0';
	Atmosphere[GAS_MAX] = 0;

	return Atmosphere;
}

bool
	CPlanet::set_atmosphere(const char* aAtmosphere)
{
	if (strlen(aAtmosphere) < GAS_MAX) return false;

	if (aAtmosphere[GAS_H2] < '0') return false;
	if (aAtmosphere[GAS_Cl2] < '0') return false;
	if (aAtmosphere[GAS_CO2] < '0') return false;
	if (aAtmosphere[GAS_O2] < '0') return false;
	if (aAtmosphere[GAS_N2] < '0') return false;
	if (aAtmosphere[GAS_CH4] < '0') return false;
	if (aAtmosphere[GAS_H2O] < '0') return false;

	if (aAtmosphere[GAS_H2] > '5') return false;
	if (aAtmosphere[GAS_Cl2] > '5') return false;
	if (aAtmosphere[GAS_CO2] > '5') return false;
	if (aAtmosphere[GAS_O2] > '5') return false;
	if (aAtmosphere[GAS_N2] > '5') return false;
	if (aAtmosphere[GAS_CH4] > '5') return false;
	if (aAtmosphere[GAS_H2O] > '5') return false;

	mAtmosphere[GAS_H2] = aAtmosphere[GAS_H2] - '0';
	mAtmosphere[GAS_Cl2] = aAtmosphere[GAS_Cl2] - '0';
	mAtmosphere[GAS_CO2] = aAtmosphere[GAS_CO2] - '0';
	mAtmosphere[GAS_O2] = aAtmosphere[GAS_O2] - '0';
	mAtmosphere[GAS_N2] = aAtmosphere[GAS_N2] - '0';
	mAtmosphere[GAS_CH4] = aAtmosphere[GAS_CH4] - '0';
	mAtmosphere[GAS_H2O] = aAtmosphere[GAS_H2O] - '0';

	mStoreFlag += STORE_ATMOSPHERE;

	return true;
}

int
	CPlanet::compute_nogada_point()
{
	int
		NogadaPointPerMillien,
		NogadaPoint;

	// 노가다 Control Model에 의한 노가다 포인트 계산
	NogadaPointPerMillien = 5;

	// 생산되는 Nogada Point 계산
	NogadaPoint = (get_population() / 1000) * NogadaPointPerMillien;

	// 환경 Control Model에 의한 노가다 포인트 계산
	if (mControlModel.get_environment() <= -10)
		NogadaPoint /= 10;
	else if (mControlModel.get_environment() < -1)
		NogadaPoint = (NogadaPoint/10)
			*(11+mControlModel.get_environment());

	NogadaPoint -= NogadaPoint * mWasteRate / 100;

	// investment effect
	int BonusRatio = ((int)(mInvestRate/20))*10;
	NogadaPoint = NogadaPoint+(int)(NogadaPoint*BonusRatio/100);

	return NogadaPoint;
}

int
	CPlanet::compute_upkeep_and_output(int aNogadaPoint, CResource *aNewResource, CResource *aUpkeep)
{
	int
		UpkeepRatio,
		ActiveFactory,
		FactoryNogadaPoint,
		ProductPointPerFactory,
		ProductPoint,
		ActiveResearchLab,
		ResearchLabNogadaPoint,
		ResearchPointPerResearchLab,
		ResearchPoint,
		ActiveMilitaryBase,
		MilitaryBaseNogadaPoint,
		MilitaryPointPerMilitaryBase,
		MilitaryPoint;

	// Planet Resource Multiplier table
	static const int ResourceMultiplierTable[] = {
		CPlanet::mRatioUltraPoor,
		CPlanet::mRatioPoor,
		CPlanet::mRatioNormal,
		CPlanet::mRatioRich,
		CPlanet::mRatioUltraRich };

	UpkeepRatio = -mControlModel.get_facility_cost();
	if( UpkeepRatio < -5 ) UpkeepRatio = -5;

	// reset NewResource and Upkeep
	if (aNewResource != NULL) aNewResource->clear();
	if (aUpkeep != NULL) aUpkeep->clear();

	// calculate nogada point upkeep
	ActiveFactory = FactoryNogadaPoint = mBuilding.get(BUILDING_FACTORY);
	FactoryNogadaPoint = FactoryNogadaPoint * (10+UpkeepRatio) / 10;

	if (FactoryNogadaPoint > aNogadaPoint)
	{
		ActiveFactory = ActiveFactory * aNogadaPoint / FactoryNogadaPoint;
		FactoryNogadaPoint = aNogadaPoint;
	}

	// pay nogada point for factory
	aNogadaPoint -= FactoryNogadaPoint;

	// get active factory
	ProductPointPerFactory = 60 + (mControlModel.get_production()*10);

	ProductPointPerFactory *= ResourceMultiplierTable[get_resource()];

	ProductPointPerFactory /= 1000;

	if( ProductPointPerFactory < 2 )
		ProductPointPerFactory = 2;

	// calculate product point
	ProductPoint = ActiveFactory * ProductPointPerFactory;

	ProductPoint -= ProductPoint * mWasteRate / 100;

	// product point output
	if (!get_blockade())
		if (aNewResource != NULL) aNewResource->set(RESOURCE_PRODUCTION, ProductPoint);

	if(mOwnerID == 5797)
	{
		system_log("Production Point in compute_upkeep = %d", ProductPoint);
	}

	// calculate nogada point upkeep
	if (aUpkeep != NULL) aUpkeep->set(BUILDING_FACTORY, ActiveFactory*(10+UpkeepRatio));

	// calculate nogada point for research lab
	ActiveResearchLab = ResearchLabNogadaPoint =
		mBuilding.get(BUILDING_RESEARCH_LAB);
	ResearchLabNogadaPoint = ResearchLabNogadaPoint * (10+UpkeepRatio) / 10;

	if (ResearchLabNogadaPoint > aNogadaPoint)
	{
		ActiveResearchLab = ActiveResearchLab *
			aNogadaPoint / ResearchLabNogadaPoint;
		ResearchLabNogadaPoint = aNogadaPoint;
	}

	// pay nogada point for research lib
	aNogadaPoint -= ResearchLabNogadaPoint;

	//get active research lab
	ResearchPointPerResearchLab = 10 + mControlModel.get_research();
	if( ResearchPointPerResearchLab < 1 )
		ResearchPointPerResearchLab = 1;

	// calculate upkeep and research point
	ResearchPoint = ActiveResearchLab *
		ResearchPointPerResearchLab;
	ResearchPoint -= ResearchPoint * mWasteRate / 100;

	if (!get_blockade())
		if (aNewResource != NULL) aNewResource->set(RESOURCE_RESEARCH, ResearchPoint);
	if (aUpkeep != NULL) aUpkeep->set(BUILDING_RESEARCH_LAB, ActiveResearchLab*(10+UpkeepRatio));

	// calculate nogada point for military base
	ActiveMilitaryBase = MilitaryBaseNogadaPoint =
		mBuilding.get(BUILDING_MILITARY_BASE);
	MilitaryBaseNogadaPoint = MilitaryBaseNogadaPoint *
		(10+UpkeepRatio) / 10;

	// pay nogada point for military
	aNogadaPoint -= MilitaryBaseNogadaPoint;

	//get active military base
	MilitaryPointPerMilitaryBase = 10 + mControlModel.get_uncapped_military();

	// calculate upkeep and military point
	MilitaryPoint = ActiveMilitaryBase *
		MilitaryPointPerMilitaryBase;
	MilitaryPoint -= MilitaryPoint * mWasteRate / 100;

	if (!get_blockade())
		if (aNewResource != NULL) aNewResource->set(RESOURCE_MILITARY, MilitaryPoint);
	if (aUpkeep != NULL) aUpkeep->set(BUILDING_MILITARY_BASE, ActiveMilitaryBase*(10+UpkeepRatio));

	// return used nogada point
	return
	FactoryNogadaPoint+
	ResearchLabNogadaPoint+
	MilitaryBaseNogadaPoint;
}

void
	CPlanet::build_new_building(int aNogadaPoint)
{
	int
		TotalDist,
		MilitaryBaseNogadaPoint,
		ResearchLabNogadaPoint,
		FactoryNogadaPoint;
	long long int
		BuildingCost;

	// clear NewBuilding
	mNewBuilding.clear();

	// check mDistributeRatio validity
	TotalDist = mDistributeRatio.total();

	if (!TotalDist)
	{
		mDistributeRatio.set(BUILDING_FACTORY, 40);
		mDistributeRatio.set(BUILDING_MILITARY_BASE, 30);
		mDistributeRatio.set(BUILDING_RESEARCH_LAB, 30);
	}
	else if (TotalDist != 100)
	{
		int Temp, Rest = 10;

		Temp = mDistributeRatio.get(BUILDING_FACTORY)*10/TotalDist;
		mDistributeRatio.set(BUILDING_FACTORY, Temp*10);
		Rest -= Temp;

		Temp = mDistributeRatio.get(BUILDING_MILITARY_BASE)*10
			/TotalDist;
		mDistributeRatio.set(BUILDING_MILITARY_BASE, Temp*10);
		Rest -= Temp;

		mDistributeRatio.set(BUILDING_RESEARCH_LAB, Rest*10);
	}

	aNogadaPoint += aNogadaPoint * mWasteRate/100;
	// calc nogada point for each building type
	MilitaryBaseNogadaPoint = aNogadaPoint
		* mDistributeRatio.get(BUILDING_MILITARY_BASE)/100;
	ResearchLabNogadaPoint = aNogadaPoint
		* mDistributeRatio.get(BUILDING_RESEARCH_LAB)/100;
	FactoryNogadaPoint = aNogadaPoint
		- ResearchLabNogadaPoint - MilitaryBaseNogadaPoint;
	// do not change above line to below.
	// this is for handling very small amount of NP
	// add nogada point to each building progress
	mBuildingProgress.change(BUILDING_FACTORY, FactoryNogadaPoint);
	mBuildingProgress.change(BUILDING_MILITARY_BASE, MilitaryBaseNogadaPoint);
	mBuildingProgress.change(BUILDING_RESEARCH_LAB, ResearchLabNogadaPoint);

	// calc building cost by # of building and control model
	// min 50%
	BuildingCost = mBuilding.total() + 5;
	BuildingCost = BuildingCost *
		(10-mControlModel.get_facility_cost()) / 10;
	if (BuildingCost < (mBuilding.total()+5)/2)
		BuildingCost = (mBuilding.total()+5)/2;

	// change nogada point to building

	while (mBuildingProgress.get(BUILDING_FACTORY) > BuildingCost)
	{
		mBuildingProgress.change(BUILDING_FACTORY, -BuildingCost);
		mBuilding.change(BUILDING_FACTORY, 1);
		mNewBuilding.change(BUILDING_FACTORY, 1);
		BuildingCost = (double)BuildingCost * 1.25;
	}

	while (mBuildingProgress.get(BUILDING_MILITARY_BASE) > BuildingCost)
	{
		mBuildingProgress.change(BUILDING_MILITARY_BASE, -BuildingCost);
		mBuilding.change(BUILDING_MILITARY_BASE, 1);
		mNewBuilding.change(BUILDING_MILITARY_BASE, 1);
		BuildingCost = (double)BuildingCost * 1.25;
	}

	while (mBuildingProgress.get(BUILDING_RESEARCH_LAB) > BuildingCost)
	{
		mBuildingProgress.change(BUILDING_RESEARCH_LAB, -BuildingCost);
		mBuilding.change(BUILDING_RESEARCH_LAB, 1);
		mNewBuilding.change(BUILDING_RESEARCH_LAB, 1);
		BuildingCost = (double)BuildingCost * 1.25;
	}

}

void CPlanet::process_population_growth()
{
	int
		GrowthRatio;

	// calc pop max
	compute_max_population();

	float Growth = mControlModel.get_growth();

	// groath ratio = (max pop - current pop) / current pop * 5%
	// max 5%, min -5%
	if (!get_population())
	{
		change_population(10000);
		GrowthRatio = 0;
	}
	else
		GrowthRatio = (float)(mMaxPopulation - get_population()) * (.025f + (Growth * .005f));

	// pop growth = current pop * growth ratio + base growth
	// new pop = current pop + pop growth
	change_population(GrowthRatio);
}

void
	CPlanet::update_turn()
{
	// privateer
	// build planet control model
	// calc nogada point
	// pay nogada upkeep and calc active buildings
	// calc upkeep and output
	// calc income of commerce
	// dist nogada by ratio
	// build new building
	// calc pop max and pop growth
	// terraforming

	if (is_paralyzed() == true) return;

	time_t
	NowGameTime = CGame::get_game_time();

	int
		NogadaPoint,
		UsedNogadaPoint,
		RemainNogadaPoint;

	process_privateer(NowGameTime);

	if (get_blockade())
	{
		if (mBlockadeTimer < NowGameTime)
		{
			mBlockadeTimer = 0;

			ADMIN_TOOL->add_blockade_log(
				(char *)format("The blockade has expired on the planet %s whose owner is %s.",
					(char *)mName, get_owner()->get_nick()));
		}
	}

	CPlayer *
	Owner = get_owner();
	if (Owner == NULL)
	{
		SLOG("ERROR : Owner is NULL in CPlanet::update_turn(), mID = %d", mID);
		return;
	}

	build_control_model(Owner);

	NogadaPoint = compute_nogada_point();

	UsedNogadaPoint = compute_upkeep_and_output(NogadaPoint, &mNewResource, &mUpkeep);

	if (!get_blockade())
		mNewResource.change(RESOURCE_PRODUCTION, calc_commerce());

	if (mPrivateerAmount > 0)
	{
		mNewResource.change(RESOURCE_PRODUCTION, -mPrivateerAmount);
		mPrivateerAmount = 0;
	}

	RemainNogadaPoint = NogadaPoint - UsedNogadaPoint;

	build_new_building(RemainNogadaPoint);

	process_population_growth();

	process_terraforming();

	if (mPlanetInvestPool) {
		long long int InvestMaxPP;
		if (Owner->has_ability(ABILITY_EFFICIENT_INVESTMENT))
		{
			InvestMaxPP = get_invest_max_production() / 2;
		}
		else
		{
			InvestMaxPP = get_invest_max_production();
		}
		Owner->add_planet_invest_pool_usage(InvestMaxPP);
	}
}

void
	CPlanet::process_privateer(time_t aNewGameTime)
{
	if (mPrivateerTimer == 0) return;

	if (mPrivateerTimer < aNewGameTime)
	{
		mPrivateerTimer = 0;

		ADMIN_TOOL->add_privateer_log(
			(char *)format("The planet %s, whose owner is the player %s, isn't under privateer anymore.",
				(char *)mName, get_owner()->get_nick()));
	}
}

void
	CPlanet::process_terraforming()
{
	if (!mTerraforming) return;

	mTerraformingTimer++;
	int NeedTurn = TURNS_PER_24HOURS
		- (int)(TURNS_PER_24HOURS*(mInvestRate/25)/10);

	if (mTerraformingTimer >= NeedTurn)
	{
//		SLOG("terraforming %s", get_name());
		mTerraformingTimer = 0;

		if (terraforming_atmosphere()) return;
		if (terraforming_temperature()) return;
		if (terraforming_gravity()) return;
		if (change_planet_attribute()) return;
		mTerraforming = false;
		mStoreFlag += STORE_TERRAFORMING;
//		SLOG("Terraforming is complete.");
		get_owner()->time_news(
			(char*)format(GETTEXT("%1$s, terraforming is completed."),
				get_name()));
	}
}

bool
	CPlanet::terraforming_atmosphere()
{
	CPlayer *
	Owner = get_owner();
	CRace *
	Race = Owner->get_race_data();
	CCommandSet &
	Ability = (CCommandSet &)Race->get_ability();

	if (Ability.has(ABILITY_NO_BREATH)) return false;

	char* Atmosphere =
		(char*)get_owner()->get_race_data()->get_atmosphere();

	Atmosphere[GAS_H2] -= '0';
	Atmosphere[GAS_Cl2] -= '0';
	Atmosphere[GAS_CO2] -= '0';
	Atmosphere[GAS_O2] -= '0';
	Atmosphere[GAS_N2] -= '0';
	Atmosphere[GAS_CH4] -= '0';
	Atmosphere[GAS_H2O] -= '0';

/*	SLOG("Race H2:%d, Cl2:%d, CO2:%d, O2:%d, N2:%d, CH4:%d, H2O:%d",
			Atmosphere[GAS_H2], Atmosphere[GAS_Cl2],
			Atmosphere[GAS_CO2], Atmosphere[GAS_O2],
			Atmosphere[GAS_N2], Atmosphere[GAS_CH4],
			Atmosphere[GAS_H2O]);

	SLOG("Planet H2:%d, Cl2:%d, CO2:%d, O2:%d, N2:%d, CH4:%d, H2O:%d",
			mAtmosphere[GAS_H2], mAtmosphere[GAS_Cl2],
			mAtmosphere[GAS_CO2], mAtmosphere[GAS_O2],
			mAtmosphere[GAS_N2], mAtmosphere[GAS_CH4],
			mAtmosphere[GAS_H2O]);*/


	if ((Atmosphere[GAS_H2] == mAtmosphere[GAS_H2]) &&
		(Atmosphere[GAS_Cl2] == mAtmosphere[GAS_Cl2]) &&
		(Atmosphere[GAS_CO2] == mAtmosphere[GAS_CO2]) &&
		(Atmosphere[GAS_O2] == mAtmosphere[GAS_O2]) &&
		(Atmosphere[GAS_N2] == mAtmosphere[GAS_N2]) &&
		(Atmosphere[GAS_CH4] == mAtmosphere[GAS_CH4]) &&
		(Atmosphere[GAS_H2O] == mAtmosphere[GAS_H2O])) return false;

	// step one
	EGasType Gas1=GAS_MAX, Gas2=GAS_MAX;
	for(int i=GAS_BEGIN; i<GAS_MAX; i++)
	{
		if (mAtmosphere[i] > Atmosphere[i])
		{
			Gas1 = (EGasType)i;
			break;
		}
	}

	if (Gas1 != GAS_MAX)
	{
		for(int i=GAS_BEGIN; i<GAS_MAX; i++)
		{
			if (mAtmosphere[i] < Atmosphere[i])
			{
				Gas2 = (EGasType)i;
				break;
			}
		}
	}

//	SLOG("Gas2 %s", get_gas_description(Gas2));

	if (Gas2 != GAS_MAX)
	{
		change_atmosphere(Gas1, Gas2);
//		SLOG("Change Atmosphere %s decreasing, %s increasing",
//				get_gas_description(Gas1), get_gas_description(Gas2));
		get_owner()->time_news(
			(char*)format(GETTEXT("Terraforming : %1$s has been decreased and %2$s has been increased on %3$s."),
				get_gas_description(Gas1),
				get_gas_description(Gas2),
				get_name()));

		return true;
	}

	// step two
	int GasCount = 0;
	for(int i=GAS_BEGIN; i<GAS_MAX; i++)
		GasCount += mAtmosphere[i];

//	SLOG("Total Gas : %d", GasCount);
	if (GasCount < 5) // need more air
	{
		EGasType Gas = (EGasType)(number(GAS_MAX)-1);
		mAtmosphere[Gas]++;
		mStoreFlag += STORE_ATMOSPHERE;
//		SLOG("Make Atmosphere %s ", get_gas_description(Gas));

		get_owner()->time_news(
			(char*)format(GETTEXT("Terraforming : %1$s has been generated on %2$s."),
				get_gas_description(Gas),
				get_name()));

		return true;
	}

	return true;
}

bool
	CPlanet::terraforming_temperature()
{
	int Temperature =
		get_owner()->get_race_data()->get_temperature();

	if (abs(mTemperature - Temperature) < 50) return false;

	if (mTemperature > Temperature)
	{
		mTemperature -= 25 + number(50);
		mStoreFlag += STORE_TEMPERATURE;
//		SLOG("Temperature is going down %d", mTemperature);
		get_owner()->time_news(
			(char *)format(GETTEXT("Terraforming : The temperature has gone down to %1$s K on %2$s."),
				dec2unit(mTemperature),
				get_name()));
		return true;
	}

	if (mTemperature < Temperature)
	{
		mTemperature += 25 + number(50);
		mStoreFlag += STORE_TEMPERATURE;
//		SLOG("Temperature is going up %d", mTemperature);
		get_owner()->time_news(
			(char*)format(GETTEXT("Terraforming : The temperature has gone up to %1$s K in %2$s."),
				dec2unit(mTemperature),
				get_name()));
		return true;
	}

	return false;
}

bool
	CPlanet::terraforming_gravity()
{
//	SLOG("terraforming_gravity()");

	double Gravity =
		get_owner()->get_race_data()->get_gravity();

	if (fabs(mGravity - Gravity) < 0.2) return false;

	if (mAttribute.has(PA_GRAVITY_CONTROLED)) return false;

	#define TECH_GRAVITY_CONTROL 			1326
	if (!get_owner()->has_tech(TECH_GRAVITY_CONTROL)) return true;

//	SLOG("set up gravity control unit");
	get_owner()->time_news(
		(char*)format(GETTEXT("Terraforming : You set up a gravity control unit on %1$s."),
			get_name()));

	set_attribute(PA_GRAVITY_CONTROLED);

	return true;
}

bool
	CPlanet::change_planet_attribute()
{
	#define TECH_SOLAR_MANIPULATION 		1321
	#define TECH_PRIMITIVE_LANGUAGE 		1118
	#define TECH_GENETIC_THERAPY			1408

//	SLOG("change_planet_attribute()");

	if (!has_attribute(PA_RADIATION) &&
		!has_attribute(PA_HOSTILE_MONSTER) &&
		!has_attribute(PA_OBSTINATE_MICROBE)) return false;

	if (has_attribute(PA_RADIATION) &&
		get_owner()->has_tech(TECH_SOLAR_MANIPULATION))
	{
		remove_attribute(PA_RADIATION);
//		SLOG("remove PA_RADIATION");
		get_owner()->time_news(
			(char *)format(
				GETTEXT("You clear the radiation of %1$s's by solar manipulation technology."),
				get_name()));

		return true;
	}

	if (has_attribute(PA_HOSTILE_MONSTER) &&
		get_owner()->has_tech(TECH_PRIMITIVE_LANGUAGE))
	{
		remove_attribute(PA_HOSTILE_MONSTER);
//		SLOG("remove PA_HOSTILE_MONSTER");
		get_owner()->time_news(
			(char *)format(
				GETTEXT("You control the hostile monster of %1$s by primitive language technology."),
				get_name()));
		return true;
	}

	if (has_attribute(PA_OBSTINATE_MICROBE) &&
		get_owner()->has_tech(TECH_GENETIC_THERAPY))
	{
		remove_attribute(PA_OBSTINATE_MICROBE);
//		SLOG("remove PA_OBSTINATE_MICROBE");
		get_owner()->time_news(
			(char *)format(
				GETTEXT("You destroy the obstinate microbe in %1$s by genetic therapy technology."),
				get_name()));
		return true;
	}

	return true;
}

void
	CPlanet::build_control_model(CPlayer *aOwner)
{
	mControlModel.clear();

	// calculate control by planet attribute
	// add player main control
	// investment effect

	mControlModel.change_environment(calc_environment_control());

	if (mAttribute.has(PA_ARTIFACT))
		mControlModel.change_research(10);

	if (mAttribute.has(PA_MASSIVE_ARTIFACT))
		mControlModel.change_research(20);

	if (mAttribute.has(PA_ASTEROID))
		mControlModel.change_production(2);

	if (mAttribute.has(PA_MOON))
	{
		mControlModel.change_military(3);
		mControlModel.change_growth(3);
		mControlModel.change_commerce(2);
	}

	if (mAttribute.has(PA_MOON_CLUSTER))
	{
		mControlModel.change_military(4);
		mControlModel.change_growth(3);
		mControlModel.change_commerce(5);
		mControlModel.change_facility_cost(1);
	}

	if (mAttribute.has(PA_RADIATION))
		mControlModel.change_environment(-1);

	if (mAttribute.has(PA_SEVERE_RADIATION))
		mControlModel.change_environment(-2);

	if (mAttribute.has(PA_HOSTILE_MONSTER))
		mControlModel.change_environment(-1);

	if (mAttribute.has(PA_OBSTINATE_MICROBE))
		mControlModel.change_environment(-2);

	if (mAttribute.has(PA_BEAUTIFUL_LANDSCAPE))
	{
		mControlModel.change_commerce(3);
		mControlModel.change_spy(-1);
	}

	if (mAttribute.has(PA_BLACK_HOLE))
	{
		mControlModel.change_commerce(-3);
		mControlModel.change_environment(-2);
		mControlModel.change_production( -2);
	}

	if (mAttribute.has(PA_DARK_NEBULA))
	{
		mControlModel.change_environment(-2);
		mControlModel.change_commerce(-2);
	}

	if (mAttribute.has(PA_VOLCANIC_ACTIVITY))
	{
		mControlModel.change_environment(-1),
		mControlModel.change_production(4);
	}

	if (mAttribute.has(PA_INTENSE_VOLCANIC_ACTIVITY))
	{
		mControlModel.change_environment(-2),
		mControlModel.change_production(10);
	}

	if (mAttribute.has(PA_OCEAN))
	{
		mControlModel.change_environment(1);
		mControlModel.change_growth(3);
		mControlModel.change_facility_cost(-4);
		mControlModel.change_research(4);
	}

	if (mAttribute.has(PA_IRREGULAR_CLIMATE))
		mControlModel.change_environment(-2);

	if (mAttribute.has(PA_MAJOR_SPACE_ROUTE))
		mControlModel.change_commerce(5);

	if (mAttribute.has(PA_MAJOR_SPACE_CROSSROUTE))
		mControlModel.change_commerce(10);

	if (mAttribute.has(PA_FRONTIER_AREA))
		mControlModel.change_commerce(-2);

	if (mAttribute.has(PA_ANCIENT_RUINS))
	{
		mControlModel.change_research(2);
		mControlModel.change_commerce(2);
	}

	if (mAttribute.has(PA_SHIP_YARD))
		mControlModel.change_military(5);

	if (mAttribute.has(PA_NEBULA))
	{
		mControlModel.change_military(2);
		mControlModel.change_research(2);
		mControlModel.change_commerce(2);
	}

	if (mAttribute.has(PA_MILITARY_STRONGHOLD))
	{
		mControlModel.change_military(15);
		mControlModel.change_commerce(-5);
	}

	if (mAttribute.has(PA_MAINTENANCE_CENTER))
	{
		mControlModel.change_facility_cost(3);
		mControlModel.change_commerce(2);
		mControlModel.change_growth(2);
	}

	if (mAttribute.has(PA_COGNITION_AMPLIFIER))
	{
		mControlModel.change_facility_cost(3);
		mControlModel.change_production(6);
	}

	if (mAttribute.has(PA_UNDERGROUND_CAVERNS))
	{
		mControlModel.change_facility_cost(1);
		mControlModel.change_military(1);
		mControlModel.change_growth(3);
	}

	if (mAttribute.has(PA_RARE_ORE))
	{
		mControlModel.change_production(3);
		mControlModel.change_commerce(2);
	}

	if (mAttribute.has(PA_LOST_TRABOTULIN_LIBRARY))
	{
		mControlModel.change_research(30);
		mControlModel.change_military(5);
	}
	/*
	if (mAttribute.has(PA_STABLE_WORMHOLE))
	{
		mControlModel.change_commerce(4);
	}
	
	if (mAttribute.has(PA_DIPLOMATIC_OUTPOST))
	{
		mControlModel.change_commerce(4);
		mControlModel.change_military(2);
		mControlModel.change_facility_cost(1);
	}
*/
	mControlModel += *(aOwner->get_control_model());

	for( int i = 0; i < aOwner->get_effect_list()->length(); i++ ){
		CPlayerEffect
			*Effect = (CPlayerEffect*)(aOwner->get_effect_list()->get(i));
		if (Effect->get_type() != CPlayerEffect::PA_CHANGE_PLANET_CONTROL_MODEL) continue;
		if (Effect->get_target() != mID) continue;

		switch( Effect->get_argument1() ){
			case CControlModel::CM_ENVIRONMENT :
				mControlModel.change_environment(Effect->get_argument2());
				break;
			case CControlModel::CM_GROWTH :
				mControlModel.change_growth(Effect->get_argument2());
				break;
			case CControlModel::CM_RESEARCH :
				mControlModel.change_research(Effect->get_argument2());
				break;
			case CControlModel::CM_PRODUCTION :
				mControlModel.change_production(Effect->get_argument2());
				break;
			case CControlModel::CM_MILITARY :
				mControlModel.change_military(Effect->get_argument2());
				break;
			case CControlModel::CM_SPY :
				mControlModel.change_spy(Effect->get_argument2());
				break;
			case CControlModel::CM_COMMERCE :
				mControlModel.change_commerce(Effect->get_argument2());
				break;
			case CControlModel::CM_EFFICIENCY :
				mControlModel.change_efficiency(Effect->get_argument2());
				break;
			case CControlModel::CM_GENIUS :
				mControlModel.change_genius(Effect->get_argument2());
				break;
			case CControlModel::CM_DIPLOMACY :
				mControlModel.change_diplomacy(Effect->get_argument2());
				break;
			case CControlModel::CM_FACILITY_COST :
				mControlModel.change_facility_cost(Effect->get_argument2());
				break;
			default :
				SLOG( "Effect Wrong Control Model %d - %d/%d", Effect->get_target(), Effect->get_source_type(), Effect->get_source() );
				break;
		}
	}

	mWasteRate = calc_waste_rate();

	mInvestRate = calc_invest_rate();

	// investment effect
	mControlModel.change_growth((int)(mInvestRate/10));
	mControlModel.change_facility_cost((int)(mInvestRate/20));
}

int
	CPlanet::calc_invest_rate()
{
	int
		Rate;
	long long int
		InvestMaxPP;

	CPlayer *
	Owner = get_owner();
	if (!Owner) return -1;

	if (Owner->has_ability(ABILITY_EFFICIENT_INVESTMENT))
	{
		InvestMaxPP = get_invest_max_production() / 2;
	}
	else
	{
		InvestMaxPP = get_invest_max_production();
	}

	if (InvestMaxPP && ((mPlanetInvestPool )||( mInvestment)))
	{
		int
			Invest;

		if (InvestMaxPP <= mInvestment)
		{
			Invest = InvestMaxPP;
			change_investment(-Invest);
		}
		else
		{
			Invest = mInvestment;
			change_investment(-Invest);
			if (mPlanetInvestPool) {
				int investpool = Owner->get_planet_invest_pool();
				if ( Invest + investpool >= InvestMaxPP )
				{
					Owner->add_planet_invest_pool(Invest-InvestMaxPP);
					Invest = InvestMaxPP;
				}
				else
				{
					Owner->set_planet_invest_pool(0);
					Invest += investpool;
				}
			}
		}

		Rate = (int)(Invest * (100-mWasteRate) / InvestMaxPP);
	} else
		Rate = 0;

	return Rate;
}

// initialize for new planet
void
	CPlanet::initialize(CRace *aRace)
{
	mMaxID++;
	if (mMaxID < 0) mMaxID = 0;

	mID = (mMaxID) ? mMaxID:1;
	mOwnerID = 0;
	mClusterID = -1;
	mMaxPopulation = 0;
	mPopulation = 0;

	mBuilding.clear();
	mBuildingProgress.clear();
	mDistributeRatio.set_all(40, 30, 30);

	if (!aRace)
	{ // atmosphere
		if(number(4) != 4)
		{ // 75%
			for(int i = 0; i < GAS_MAX; i++)
				set_atmosphere((EGasType)i, 0 );

			int Gas1 = number(3) + 2;
			EGasType Type1 = (EGasType)(number(GAS_MAX) - 1);

			set_atmosphere(Type1, Gas1);

			int Gas2 = 5 - Gas1;
			int Type2 = number(GAS_MAX) - 1;
			if ((EGasType)Type2 == Type1)
			{
				if (Type2 < GAS_MAX-1) Type2++;
				else if (Type2 > GAS_BEGIN) Type2--;
			}
			set_atmosphere((EGasType)Type2, Gas2);
		} else 	// 25% no air
			for(int i = 0; i < GAS_MAX; i++)
				set_atmosphere((EGasType)i, 0 );

		// temperature 100-500k
		set_temperature(100+number(400) );

		// size 0 - 4
		set_size( dice(2,4) - 4 );

		// gravity 0.2 - 2.5G
		set_gravity( 0.2+(double)number(15)/10.0+mSize*0.2 );

		// resource 0-4
		set_resource( dice(2,4) - 4 );
	} else {
		set_atmosphere(aRace->get_atmosphere());
		set_temperature(aRace->get_temperature());
		set_gravity(aRace->get_gravity());
		mBuilding.set( BUILDING_FACTORY, 30 );
		mBuilding.set( BUILDING_RESEARCH_LAB, 10 );
		mBuilding.set( BUILDING_MILITARY_BASE, 10 );
	}

	mAttribute.clear();

	// attribute
	if (number(100) <= 5)
	{ // 5%
		if (number(100) <= 10) // 10%
			mAttribute += PA_MASSIVE_ARTIFACT;
		else
			mAttribute += PA_ARTIFACT;
	}

	if (number(100) <= 10) // 10%
		mAttribute += PA_ASTEROID;

	if (number(100) <= 10) // 10%
	{
		if (number(100) <= 15)
			mAttribute += PA_MOON_CLUSTER;
		else
			mAttribute += PA_MOON;
	}

	if (number(100) <= 20)
	{ // 20%
		if (number(100) <= 20) // 20%
			mAttribute += PA_SEVERE_RADIATION;
		else {
			mAttribute += PA_RADIATION;
			start_terraforming();
		}
	}

	if (number(100) <= 10) // 10%
	{
		mAttribute += PA_HOSTILE_MONSTER;
		start_terraforming();
	}

	if (number(100) <= 10) // 10%
	{
		mAttribute += PA_OBSTINATE_MICROBE;
		start_terraforming();
	}

	if (number(100) <= 10) // 10%
		mAttribute += PA_BEAUTIFUL_LANDSCAPE;

	if (number(100) <= 2) // 2%
		mAttribute += PA_BLACK_HOLE;
		
/*	if (number(100) <= 4) // 4%
		mAttribute += PA_STABLE_WORMHOLE;
		
	if (number(100) <= 4) // 4%
		mAttribute += PA_DIPLOMATIC_OUTPOST;
*/
	if (number(100) <= 5)
	{ // 5%
		if (number(100) <= 20) // 20%
			mAttribute += PA_DARK_NEBULA;
		else
			mAttribute += PA_NEBULA;
	}

	if (number(100) <= 10)
	{ // 10%
		if (number(100) <= 10) // 10%
			mAttribute += PA_INTENSE_VOLCANIC_ACTIVITY;
		else
			mAttribute += PA_VOLCANIC_ACTIVITY;
	}

	if (number(100) <= 4) // 4%
		mAttribute += PA_OCEAN;

	if (number(100) <= 10) // 10%
		mAttribute += PA_IRREGULAR_CLIMATE;

	if (number(100) <= 10)
	{ // 10%
		if (number(100) <= 5) // 5%
			mAttribute += PA_MAJOR_SPACE_CROSSROUTE;
		else
			mAttribute += PA_MAJOR_SPACE_ROUTE;
	} else if (number(100) <= 10) // 10%
		mAttribute += PA_FRONTIER_AREA;

	if (number(100) <= 5)
		mAttribute += PA_GRAVITY_CONTROLED;

	if (number(200) <= 1) // 0.5%
		mAttribute += PA_ANCIENT_RUINS;

	if (number(100) <= 4)
		mAttribute += PA_SHIP_YARD;

	if (number(250) <= 1) // 0.4%
		mAttribute += PA_MILITARY_STRONGHOLD;

	if (number(100) <= 3)
		mAttribute += PA_MAINTENANCE_CENTER;

	if (number(250) <= 1) // 0.4%
		mAttribute += PA_COGNITION_AMPLIFIER;

	if (number(100) <= 4)
		mAttribute += PA_UNDERGROUND_CAVERNS;

	if (number(100) <= 4)
		mAttribute += PA_RARE_ORE;

	if (number(1000) <= 1)
		mAttribute += PA_LOST_TRABOTULIN_LIBRARY;

	compute_max_population();

//	SLOG("Planet initalize Attribute:%s", mAttribute.get_string(PA_END));

	mPlanetNewsCenter.init(this);

	type(QUERY_INSERT);
}

void
	CPlanet::init_planet_news_center()
{
	mPlanetNewsCenter.init(this);
}

const char*
	CPlanet::get_order_name()
{
	switch(mOrder)
	{
		case 0:
			return GETTEXT("Home Planet");
		case 1:
			return GETTEXT("The 2nd Planet");
		case 2:
			return GETTEXT("The 3rd Planet");
		default:
			return (char *)format(GETTEXT("The %1$sth Planet"),
				dec2unit(mOrder+1));
	}
}

int
	CPlanet::get_pp_per_turn()
{
	int
		NogadaPoint = compute_nogada_point();
	CResource
		Temp;
	compute_upkeep_and_output(NogadaPoint, &Temp, NULL);

	return Temp.get(RESOURCE_PRODUCTION);
}

int
	CPlanet::get_mp_per_turn()
{
	int
		NogadaPoint = compute_nogada_point();
	CResource
		Temp;
	compute_upkeep_and_output(NogadaPoint, &Temp, NULL);

	return Temp.get(RESOURCE_MILITARY);
}

int
	CPlanet::get_rp_per_turn()
{
	int
		NogadaPoint = compute_nogada_point();
	CResource
		Temp;
	compute_upkeep_and_output(NogadaPoint, &Temp, NULL);

	return Temp.get(RESOURCE_RESEARCH);
}

int
	CPlanet::get_production_per_turn()
{
	int
		ProductionPerTurn = get_pp_per_turn();

	if (get_blockade() <= 0) ProductionPerTurn += calc_commerce();
	if (get_privateer() > 0) ProductionPerTurn -= mPrivateerAmount;

	return ProductionPerTurn;
}
int
	CPlanet::get_pure_production_per_turn()
{
	int
		ProductionPerTurn = get_pp_per_turn();

	if (get_privateer() > 0) ProductionPerTurn -= mPrivateerAmount;

	return ProductionPerTurn;
}
int
	CPlanet::get_pure_commerce_per_turn()
{
	int
		CommercePerTurn = calc_commerce();

	if (get_privateer() > 0) CommercePerTurn -= mPrivateerAmount;

	return CommercePerTurn;
}
int
	CPlanet::get_research_per_turn()
{
	int ActiveResearchLab =
		mBuilding.get(BUILDING_RESEARCH_LAB);
	int ResearchPointPerResearchLab = 10 + mControlModel.get_research();
	if( ResearchPointPerResearchLab < 1 )
		ResearchPointPerResearchLab = 1;


	int
		ResearchPerTurn = ResearchPointPerResearchLab * ActiveResearchLab;

	if (!(get_blockade() <= 0))
		ResearchPerTurn = 0;


	return ResearchPerTurn;
}
int
	CPlanet::get_military_per_turn()
{
	int ActiveMilitaryBases =
		mBuilding.get(BUILDING_MILITARY_BASE);
	int MilitaryPointPerMillitaryBase = 10 + mControlModel.get_uncapped_military();
	if( MilitaryPointPerMillitaryBase < 1 )
		MilitaryPointPerMillitaryBase = 1;

	int
		MPPerTurn = MilitaryPointPerMillitaryBase * ActiveMilitaryBases;
		
	#if !CLASSIC_MODE
    int BonusMilitaryCM = get_owner()->get_control_model()->get_uncapped_military() - 10;
    if (BonusMilitaryCM < 0) {
        BonusMilitaryCM = 0;
    }
    MPPerTurn += ActiveMilitaryBases * 3 * BonusMilitaryCM;

	#endif /* NOT Classis Mode */

	if (!(get_blockade() <= 0))
		MPPerTurn = 0;


	return MPPerTurn;
}

int
	CPlanet::get_upkeep_per_turn(int aResourceType)
{
	int
		NogadaPoint = compute_nogada_point();
	CResource
		Temp;
	compute_upkeep_and_output(NogadaPoint, NULL, &Temp);

	switch (aResourceType)
	{
		case BUILDING_FACTORY :
			return Temp.get(BUILDING_FACTORY);

		case BUILDING_MILITARY_BASE :
			return Temp.get(BUILDING_MILITARY_BASE);

		case BUILDING_RESEARCH_LAB :
			return Temp.get(BUILDING_RESEARCH_LAB);

		case -1 :
			return Temp.total();

		default :
			{
				SLOG("ERROR : Wrong aResourceType in CPlanet::get_upkeep_per_turn(), aResourceType = %d", aResourceType);
				return Temp.total();
			}
	}
}

const char*
	CPlanet::html_management_record()
{
	static CString Info;
	CString
		ImageURL,
		ImageFile;
	ImageURL.clear();
	ImageURL.format("%s/image/as_game/",
		(char *)CGame::mImageServerURL);
	ImageFile.clear();
	ImageFile.format("%s%s%s", (char *)ImageURL, "planets/", (char *)get_planet_image_url());

	Info = "<tr>\n";
	Info += "<td align=center>\n";
	Info += "<table border=1 width=550 cellspacing=0 cellpadding=0 bordercolor=#2a2a2a>\n";
	Info += "<tr>\n";

	Info += "<td class=tabletxt width=137 bgcolor=#171717>";
	Info.format("&nbsp;%s", get_order_name());
	Info += "</td>\n";

	Info += "<td colspan=3 class=tabletxt width=407>\n";
	Info += "<table border=0 cellspacing=0 cellpadding=0>\n";
	Info += "<tr><td>";
	Info.format("<img src=%s align=top>", (char *)ImageFile);
	Info += "</td><td class=tabletxt>";

	if (get_destroy_turns() > 0) {
		Info.format("<a href=planet_detail.as?PLANET_ID=%d>%s</a> (Auto-destroy in %d turns)", get_id(), get_name(), (get_destroy_turns()));
	} else {
		Info.format("<a href=planet_detail.as?PLANET_ID=%d>%s</a>", get_id(), get_name());
	}

	Info += "</td></tr></table>";

	Info += "</td>\n";

	Info += "</tr>\n";

	Info += "<tr align=center>\n";

	Info += "<td class=tabletxt colspan=4>\n";
	Info += "<table width=555 cellspacing=0 cellpadding=0>\n";

	Info += "<tr>\n";
	Info.format("<td class=tabletxt width=114 align=right>%s&nbsp;:</td>\n",
		GETTEXT("Population"));
	Info.format("<td class=tabletxt width=200>&nbsp;%s</td>\n",
		get_population_with_unit());
	Info.format("<TD align=right class=tabletxt width=113>%s&nbsp;:</td>\n",
		GETTEXT("Environment"));
	Info.format("<td class=tabletxt width=126>&nbsp;%s</td>\n",
		CControlModel::get_environment_description(calc_environment_control()));
	Info += "</tr>\n";

	Info += "<tr>\n";
	Info.format("<td class=tabletxt width=114 align=right>%s&nbsp;:</td>\n",
		GETTEXT("Production"));
	Info.format("<td class=tabletxt width=200>&nbsp;%sPP,&nbsp;%dRP,&nbsp;%dMP </td>\n",
		dec2unit(get_production_per_turn()),get_research_per_turn(),get_military_per_turn());

	Info.format("<td align=right class=tabletxt width=113>%s&nbsp;:</td>\n",
		GETTEXT("Upkeep"));
	Info.format("<td class=tabletxt width=126>&nbsp;%s PP</td>\n",
		dec2unit(get_upkeep_per_turn()));
	Info += "</tr>\n";

	Info += "<tr>\n";
	Info.format("<td class=tabletxt width=114 align=right>%s&nbsp;:</td>\n",
		GETTEXT("Resource"));
	Info.format("<td class=tabletxt width=200>&nbsp;%s</td>\n",
		get_resource_description());
	Info.format("<TD align=right class=tabletxt width=113>%s&nbsp;:</td>\n",
		GETTEXT("Waste Rate"));
	Info.format("<td class=tabletxt width=126>&nbsp;%d %%</td>\n", mWasteRate);
/*	Info.format("<TD align=right class=tabletxt width=113>%s&nbsp;:</td>\n",
		GETTEXT("Commerce Modifier"));
	Info.format("<td class=tabletxt width=126>&nbsp;%d %%</td>\n", get_diplomacy_modifier_to_commerce());*/
	Info += "</tr>\n";

	Info += "<tr>\n";
	Info += "<td colspan=4 class=tabletxt>&nbsp;</td>\n";
	Info += "</tr>\n";

	Info += "<tr>\n";

	Info.format("<td class=tabletxt width=114 align=right>%s&nbsp;:</td>\n",
		GETTEXT("Building"));

	Info += "<td colspan=3 class=tabletxt>";
	Info += "&nbsp;&nbsp;";
	Info.format("%s %s",
		GETTEXT("Factory"), dec2unit(mBuilding.get(BUILDING_FACTORY)));
	Info += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	Info.format("%s %s",
		GETTEXT("Laboratory"), dec2unit(mBuilding.get(BUILDING_RESEARCH_LAB)));
	Info += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	Info.format("%s %s",
		GETTEXT("Military Base"), dec2unit(mBuilding.get(BUILDING_MILITARY_BASE)));

	Info += "</td>\n";

	Info += "</tr>\n";
	Info += "<tr>\n";
	Info.format("<td class=tabletxt width=114 align=right>%s&nbsp;:</td>\n",
		GETTEXT("Building Ratio"));
	Info += "<td colspan=3 class=tabletxt>";
	Info += "&nbsp;&nbsp;";
	Info.format("%s %d %%",
		GETTEXT("Factory"), mDistributeRatio.get(BUILDING_FACTORY));
	Info += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	Info.format("%s %d %%",
		GETTEXT("Laboratory"), mDistributeRatio.get(BUILDING_RESEARCH_LAB));
	Info += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	Info.format("%s %d %%",
		GETTEXT("Military Base"), mDistributeRatio.get(BUILDING_MILITARY_BASE));
	Info += "</td>\n";
	Info += "</tr>\n";

	Info += "</table>\n";
	Info += "</td>\n";
	Info += "</tr>\n";

	Info += "</table>\n";
	Info += "</td>\n";
	Info += "</tr>\n";
	Info += "<tr>\n";
	Info += "<TD align=center>&nbsp;</td>\n";
	Info += "</tr>\n";

	return (char *)Info;
}

void
	CPlanet::compute_max_population()
{
	mMaxPopulation = (60 + get_size() * 20) * 1000;

	int
		MaxRatio = 0;
	if( mControlModel.get_growth() > 10 )
		MaxRatio = 8;
	else if( mControlModel.get_growth() < -5 )
		MaxRatio = -3;
	else if( mControlModel.get_growth() > 2 &&
		mControlModel.get_growth() <= 10 )
		MaxRatio = mControlModel.get_growth() - 2;
	else if( mControlModel.get_growth() < -2 &&
		mControlModel.get_growth() >= -5 )
		MaxRatio = mControlModel.get_growth() + 2;
	else
		MaxRatio = 0;

	mMaxPopulation = mMaxPopulation * ( 10 + MaxRatio ) / 10;

	if( mControlModel.get_environment() < -1 )
		mMaxPopulation = mMaxPopulation *
			(21 + mControlModel.get_environment()) / 20;
	if( mMaxPopulation < 10000 ) mMaxPopulation = 10000;
}

int
	CPlanet::calc_environment_control()
{
	CRace* Race = get_owner()->get_race_data();
	const char *Atmosphere = Race->get_atmosphere();

	int Control = 0;

	if (!get_owner()->has_ability(ABILITY_NO_BREATH))
	{
		int Diff = 0;

		Diff += abs(mAtmosphere[GAS_H2] - (Atmosphere[GAS_H2]-'0'));
		Diff += abs(mAtmosphere[GAS_Cl2] - (Atmosphere[GAS_Cl2]-'0'));
		Diff += abs(mAtmosphere[GAS_CO2] - (Atmosphere[GAS_CO2]-'0'));
		Diff += abs(mAtmosphere[GAS_O2] - (Atmosphere[GAS_O2]-'0'));
		Diff += abs(mAtmosphere[GAS_N2] - (Atmosphere[GAS_N2]-'0'));
		Diff += abs(mAtmosphere[GAS_CH4] - (Atmosphere[GAS_CH4]-'0'));
		Diff += abs(mAtmosphere[GAS_H2O] - (Atmosphere[GAS_H2O]-'0'));
		Diff /= 2;
		Control += -Diff;
	}

	Control += (-abs(mTemperature - Race->get_temperature()))/50;

	if (!has_attribute(PA_GRAVITY_CONTROLED))
		Control += (int)(-fabs(mGravity-Race->get_gravity())/0.2);

	return Control;
}
// start telecard 2001/03/08
int
	CPlanet::calc_environment_control(CPlayer *aPlayer)
{
	CRace* race = aPlayer->get_race_data();
	const char *Atmosphere = race->get_atmosphere();

	int control = 0;

	if (!aPlayer->has_ability(ABILITY_NO_BREATH))
	{
		int diff = 0;

		diff += abs(mAtmosphere[GAS_H2] - (Atmosphere[GAS_H2]-'0'));
		diff += abs(mAtmosphere[GAS_Cl2] - (Atmosphere[GAS_Cl2]-'0'));
		diff += abs(mAtmosphere[GAS_CO2] - (Atmosphere[GAS_CO2]-'0'));
		diff += abs(mAtmosphere[GAS_O2] - (Atmosphere[GAS_O2]-'0'));
		diff += abs(mAtmosphere[GAS_N2] - (Atmosphere[GAS_N2]-'0'));
		diff += abs(mAtmosphere[GAS_CH4] - (Atmosphere[GAS_CH4]-'0'));
		diff += abs(mAtmosphere[GAS_H2O] - (Atmosphere[GAS_H2O]-'0'));
		diff /= 2;
		control += -diff;
	}

	control += (-abs(mTemperature - race->get_temperature()))/50;

	if (!has_attribute(PA_GRAVITY_CONTROLED))
	{
		control += (int)(-fabs(mGravity-race->get_gravity())/0.2);
	}

	return control;
}
// end telecard 2001/03/08

const char*
	CPlanet::news()
{
	static CString Buffer;

	const char *News = mPlanetNewsCenter.generate();

	if (!News) return NULL;

	Buffer.clear();

	Buffer = "<tr>\n"
		"<td class=\"speakernews\" COLSPAN=2>\n"
		"<CENTER>\n";
	Buffer.format(GETTEXT("%1$s Planet Report"), get_name());

	Buffer += "</CENTER>\n"
		"</td>\n"
		"</tr>\n";

	Buffer += News;

	return (char*)Buffer;
}

const char*
	CPlanet::get_gas_description(CPlanet::EGasType aGasType)
{
	switch(aGasType)
	{
		case GAS_H2: return GETTEXT("Hydrogen");
		case GAS_Cl2: return GETTEXT("Chlorine");
		case GAS_CO2: return GETTEXT("Carbon dioxide");
		case GAS_O2: return GETTEXT("Oxygen");
		case GAS_N2: return GETTEXT("Nitrogen");
		case GAS_CH4: return GETTEXT("Methane");
		case GAS_H2O: return GETTEXT("Water");
		case GAS_MAX: return "";
	}
	return "";
}

char *
	CPlanet::get_attribute_description()
{
	static CString
		Attribute;
	Attribute.clear();

	bool
		AnyAttribute = false;

	for (int i=PA_BEGIN ; i<PA_END ; i++)
	{
		if (mAttribute.has(i))
		{
			if (AnyAttribute) Attribute += ", ";
			Attribute += get_attribute_description((EPlanetAttribute)i);
			AnyAttribute = true;
		}
	}

	if (AnyAttribute)
	{
		return (char *)Attribute;
	} else
	{
		return GETTEXT("None");
	}
}
const char *
	CPlanet::get_atmosphere_html()
{
	static CString Atmosphere;

	Atmosphere.clear();

	for (int i=GAS_BEGIN; i<GAS_MAX ; i++)
		if (mAtmosphere[i])
			Atmosphere.format("%s %s : %d ", (Atmosphere.length()) ? "<BR>\n":"",
			get_gas_description((EGasType)i), mAtmosphere[i]);

	if (!Atmosphere.length()) return " ";
	return (char *)Atmosphere;
}

const char *
	CPlanet::get_attribute_html()
{
	static CString Attribute;
	CString Temp;
	Attribute.clear();

	for (int i=PA_BEGIN ; i<PA_END ; i++)
	{
		if (mAttribute.has(i))
		{
			if (Attribute.length()) Attribute += "<BR>\n";
			Temp.format("%d", i);
			Attribute += "<span id='"+ Temp + "'>" + get_attribute_description((EPlanetAttribute)i) + "</span>";
			Attribute += "<script> var tooltip"+ Temp + " = new YAHOO.widget.Tooltip('tt"+Temp+"', { context:'"+Temp+"',autodismissdelay:10000, text:paGenerateTooltipText(paGenerateIDfromName('"+get_attribute_description((EPlanetAttribute)i)+"'))} );</script>";
		}
	}

	if (!Attribute.length()) return " ";

	return (char *)Attribute;
}

// Calculates and returns the waste rate of a planet
int CPlanet::calc_waste_rate() {
    
    // Classic Mode waste rate uses hardcoded numbers
//    #if CLASSIC_MODE /* CLASSIC MODE */
	static int MaxCleanOrder[] = {
		3,	// -5
		3,	// -4
		4,	// -3
		4, 	// -2
		5, 	// -1
		6,	// 0
		7,	// 1
		7,	// 2
		8,	// 3
		9,	// 4
		10,	// 5
		12,	// 6
		13,	// 7
		15,	// 8
		17,	// 9
		20,	// 10
	};
	static double WasteRatioPerOrder[] = {
		25	,	// -5
		12.5,	// -4
		10,		// -3
		8.3,	// -2
		8.3,	// -1
		8.3,	// 0
		8.3,	// 1
		7.1,	// 2
		7.1,	// 3
		5.5,	// 4
		4.1,	// 5
		3.8,	// 6
		3.3,	// 7
		3.1,	// 8
		2.7,	// 9
		2.5,	// 10
	};         

	int Efficiency = mControlModel.get_efficiency();

	if( mOrder < MaxCleanOrder[Efficiency] )
	return 0;	// clean planet

	int
		Waste;
	//Waste = (int)(WasteRatioPerOrder[Efficiency] * mOrder); <- HH's stuff.
	Waste = (int)((1+mOrder-MaxCleanOrder[Efficiency])*WasteRatioPerOrder[Efficiency]);
	if( Waste > 60 ) Waste = 60;	// max waste 60
	CPlayer* Player = ((CArchspace*)gApplication)->game()->player_table()->get_by_game_id(mOwnerID);
	if(Player)
	{
		if( mClusterID == Player->get_home_cluster_id()) Waste -= 5;
		if (Waste < 0) Waste = 0;

	}
	return Waste;
	
	// New waste rate uses
/*	#else /* NOT CLASSIC MODE 

    int Efficiency = mControlModel.get_efficiency();
    int Waste;
    int PlanetOrder = get_order();

    // Polynomial to use for Base Planet without waste
    // Efficiency*2 + 2
    
    if (PlanetOrder <= Efficiency*2 + 4) {
        return 0;
    }

    if (Efficiency > MAX_EFFICIENCY) {
        Efficiency = MAX_EFFICIENCY;
    }

    // Polynomail to use to get waste rate per planet after that
    // 0.5 + 2*e^(1 - efficiency/MAXEFF)

    double WasteRatePerOrder = (double) 0.5 + ((double) 1 * exp((double) ((double) 1 - ((double) Efficiency/(double) 25))));


    if ((int) ((double) (PlanetOrder - ((Efficiency * 2) + 4)) * WasteRatePerOrder) > MAX_WASTE_RATE) {
        return MAX_WASTE_RATE;
    } else {
        return (int) ((double) (PlanetOrder - ((Efficiency * 2) + 4)) * WasteRatePerOrder);
    }

	#endif  NOT CLASSIC MODE */
}

int
	CPlanet::get_commerce_with(int aIndex)
{
	return mCommerceWith[aIndex % MAX_COMMERCE_PLANET];
}

void
	CPlanet::set_commerce_with(int aIndex, int aPlanetID)
{
	CPlanet *
	TargetPlanet = PLANET_TABLE->get_by_id(aPlanetID);
	if (TargetPlanet == NULL)
	{
		mCommerceWith[aIndex % MAX_COMMERCE_PLANET] = 0;
	}
	else
	{
		mCommerceWith[aIndex % MAX_COMMERCE_PLANET] = aPlanetID;
	}

	if (aIndex % MAX_COMMERCE_PLANET == 0)
	{
		mStoreFlag += STORE_COMMERCE_WITH_1;
	}
	else if (aIndex % MAX_COMMERCE_PLANET == 1)
	{
		mStoreFlag += STORE_COMMERCE_WITH_2;
	}
	else if (aIndex % MAX_COMMERCE_PLANET == 2)
	{
		mStoreFlag += STORE_COMMERCE_WITH_3;
	}
}

void
	CPlanet::clear_commerce_with(int aPlanetID)
{
	if (mCommerceWith[0] == aPlanetID)
	{
		mCommerceWith[0] = 0;
		mStoreFlag += STORE_COMMERCE_WITH_1;
	}
	if (mCommerceWith[1] == aPlanetID)
	{
		mCommerceWith[1] = 0;
		mStoreFlag += STORE_COMMERCE_WITH_2;
	}
	if (mCommerceWith[2] == aPlanetID)
	{
		mCommerceWith[2] = 0;
		mStoreFlag += STORE_COMMERCE_WITH_3;
	}
}

void
	CPlanet::clear_commerce_all()
{
	if (mCommerceWith[0] > 0)
	{
		CPlanet::clear_commerce_between(mID, mCommerceWith[0]);
	}

	if (mCommerceWith[1] > 0)
	{
		CPlanet::clear_commerce_between(mID, mCommerceWith[1]);
	}

	if (mCommerceWith[2] > 0)
	{
		CPlanet::clear_commerce_between(mID, mCommerceWith[2]);
	}
}

char *
	CPlanet::all_commercial_planet_name()
{
	static CString
		PlanetName;
	PlanetName.clear();

	bool
		Comma = false;
	for (int i=0 ; i<MAX_COMMERCE_PLANET ; i++)
	{
		if( mCommerceWith[i] == 0 ) continue;

		CPlanet *
		Planet = (CPlanet *)PLANET_TABLE->get_by_id(mCommerceWith[i]);
		if (!Planet) continue;

		if (Comma) PlanetName += ", ";
		PlanetName += Planet->get_name();
		PlanetName += " of Player ";
		PlanetName += Planet->get_owner()->get_nick();
		Comma = true;
	}

	return (char *)PlanetName;
}

bool
	CPlanet::is_paralyzed()
{
	CPlayer *
	Owner = get_owner();
	if (Owner == NULL)
	{
		SLOG("ERROR : Owner is NULL in CPlanet::is_paralyzed(), mID = %d", mID);
		return false;
	}

	CPlayerEffectList *
	OwnerEffectList = Owner->get_effect_list();

	for (int i=0 ; i<OwnerEffectList->length() ; i++)
	{
		CPlayerEffect *
		Effect = (CPlayerEffect *)OwnerEffectList->get(i);
		if (Effect->get_type() == CPlayerEffect::PA_PARALYZE_PLANET)
		{
			if (Effect->get_target() == mID) return true;
		}
	}
	return false;
}

int
	CPlanet::calc_commerce()
{
	int ProductPoint = 0;

	for(int i=0; i<MAX_COMMERCE_PLANET; i++)
	{
		if (mCommerceWith[i] == 0) continue;

		CPlanet *
		Planet = PLANET_TABLE->get_by_id(mCommerceWith[i]);
		if (Planet == NULL)
		{
			clear_commerce_with(mCommerceWith[i]);

			type(QUERY_UPDATE);
			STORE_CENTER->store(*this);

			continue;
		}

		CPlayer *
		TargetCommerceOwner = Planet->get_owner();
		if (TargetCommerceOwner == NULL)
		{
			clear_commerce_with(mCommerceWith[i]);

			type(QUERY_UPDATE);
			STORE_CENTER->store(*this);

			continue;
		}
		if (TargetCommerceOwner->get_game_id() == EMPIRE_GAME_ID)
		{
			clear_commerce_with(mCommerceWith[i]);

			type(QUERY_UPDATE);
			STORE_CENTER->store(*this);

			continue;
		}

		if (TargetCommerceOwner->is_dead() == true)
		{
			clear_commerce_with(mCommerceWith[i]);

			type(QUERY_UPDATE);
			STORE_CENTER->store(*this);

			continue;
		}

		int
			Point = Planet->get_pp_per_turn(),
		Commerce = mControlModel.get_commerce();

		CCouncil *
		TargetCommerceCouncil = TargetCommerceOwner->get_council();

		if (get_owner()->get_council() == TargetCommerceCouncil->get_supremacy_council())
			Commerce++;

		if (get_owner()->get_council()->get_supremacy_council() == TargetCommerceCouncil)
			Commerce++;

		CPlayerRelation *
		Relation = get_owner()->get_relation(Planet->get_owner());

		if (Relation && Relation->get_relation() == CRelation::RELATION_ALLY)
		{
			Commerce++;
		}

		CCouncilRelation *
		CouncilRelation = get_owner()->get_council()->get_relation(TargetCommerceCouncil);
		if (CouncilRelation)
		{
			if (CouncilRelation->get_relation() == CRelation::RELATION_ALLY) Commerce++;
		}

		Point /= 10;
		Point = (int) ((double) (Point*(100+Commerce*10)/100) *  ((double) mCommerceMultiplier / (double) 100));
		if (Point > 0) ProductPoint += Point;
	}

	int MaxPoint = mPopulation*(6+mControlModel.get_commerce());

	if (ProductPoint > MaxPoint) ProductPoint = MaxPoint;
	if (ProductPoint < 0) return 0;



/*    #if !CLASSIC_MODE  NOT CLASSIC_MODE
    ProductPoint = (int) ((double) (Point*(100+Commerce*10)/100) *  ((double) mCommerceMultiplier / (double) 100));
    #endif */

	return ProductPoint;
}

// Calculates the diplomacy-based "waste rate" for commerce
/*int CPlanet::get_diplomacy_modifier_to_commerce() {

    int Diplomacy = mControlModel.get_diplomacy();
	int DiplomacyMultiplier = 2 * get_order();
	DiplomacyMultiplier = DiplomacyMultiplier - (2 * Diplomacy);
	if (DiplomacyMultiplier < 0) {
        DiplomacyMultiplier = 0;
    } else if (DiplomacyMultiplier > MAX_COMMERCE_WASTE) {
        DiplomacyMultiplier = MAX_COMMERCE_WASTE;
    }
    
    return DiplomacyMultiplier;
}*/

void
	CPlanet::refresh_resource_per_turn()
{
	int
		UpkeepRatio,
		ActiveFactory,
		FactoryNogadaPoint,
		ProductPointPerFactory,
		ProductPoint,
		ActiveResearchLab,
		ResearchLabNogadaPoint,
		ResearchPointPerResearchLab,
		ResearchPoint,
		ActiveMilitaryBase,
		MilitaryBaseNogadaPoint,
		MilitaryPointPerMilitaryBase,
		MilitaryPoint;

	int
		NogadaPoint = compute_nogada_point();

	// Planet Resource Multiplier table
	static const int ResourceMultiplierTable[] = { 25, 50, 100, 200, 400 };

	UpkeepRatio = -mControlModel.get_facility_cost();
	if( UpkeepRatio < -5 ) UpkeepRatio = -5;

	// calculate nogada point upkeep
	ActiveFactory = FactoryNogadaPoint = mBuilding.get(BUILDING_FACTORY);
	FactoryNogadaPoint = FactoryNogadaPoint * (10+UpkeepRatio) / 10;

	if (FactoryNogadaPoint > NogadaPoint)
	{
		ActiveFactory = ActiveFactory * NogadaPoint / FactoryNogadaPoint;
		FactoryNogadaPoint = NogadaPoint;
	}

	// pay nogada point for factory
	NogadaPoint -= FactoryNogadaPoint;

	// get active factory
	ProductPointPerFactory = 60 + (mControlModel.get_production()*10);

	ProductPointPerFactory *=
		ResourceMultiplierTable[get_resource()]/100;
	if( ProductPointPerFactory < 20 )
		ProductPointPerFactory = 20;

	// calculate product point
	ProductPoint = ActiveFactory * ProductPointPerFactory;

	ProductPoint -= ProductPoint * mWasteRate / 100;

	// calculate nogada point upkeep
	mUpkeep.set(BUILDING_FACTORY, ActiveFactory*(10+UpkeepRatio));

	// calculate nogada point for research lab
	ActiveResearchLab = ResearchLabNogadaPoint =
		mBuilding.get(BUILDING_RESEARCH_LAB);
	ResearchLabNogadaPoint = ResearchLabNogadaPoint * (10+UpkeepRatio) / 10;

	if (ResearchLabNogadaPoint > NogadaPoint)
	{
		ActiveResearchLab = ActiveResearchLab *
			NogadaPoint / ResearchLabNogadaPoint;
		ResearchLabNogadaPoint = NogadaPoint;
	}

	// pay nogada point for research lib
	NogadaPoint -= ResearchLabNogadaPoint;

	//get active research lab
	ResearchPointPerResearchLab = 10 + mControlModel.get_research();
	if( ResearchPointPerResearchLab < 1 )
		ResearchPointPerResearchLab = 1;

	// calculate upkeep and research point
	ResearchPoint = ActiveResearchLab *
		ResearchPointPerResearchLab;
	ResearchPoint -= ResearchPoint * mWasteRate / 100;

	mUpkeep.set(BUILDING_RESEARCH_LAB, ActiveResearchLab*(10+UpkeepRatio));

	// calculate nogada point for military base
	ActiveMilitaryBase = MilitaryBaseNogadaPoint =
		mBuilding.get(BUILDING_MILITARY_BASE);
	MilitaryBaseNogadaPoint = MilitaryBaseNogadaPoint *
		(10+UpkeepRatio) / 10;

	if (MilitaryBaseNogadaPoint > NogadaPoint)
	{
		ActiveMilitaryBase = ActiveMilitaryBase *
			NogadaPoint / MilitaryBaseNogadaPoint;
		MilitaryBaseNogadaPoint = NogadaPoint;
	}

	// pay nogada point for military
	NogadaPoint -= MilitaryBaseNogadaPoint;

	//get active military base
	MilitaryPointPerMilitaryBase = 10+
		mControlModel.get_military()*2;

	// calculate upkeep and military point
	MilitaryPoint = ActiveMilitaryBase *
		MilitaryPointPerMilitaryBase;
	MilitaryPoint -= MilitaryPoint * mWasteRate / 200;

	mUpkeep.set(BUILDING_MILITARY_BASE, ActiveMilitaryBase*(10+UpkeepRatio));
}


bool
	CPlanet::get_privateer()
{
	if (mPrivateerTimer > 0) return true;
	return false;
}

time_t
	CPlanet::get_privateer_timer()
{
	return mPrivateerTimer;
}

void
	CPlanet::set_privateer_timer(time_t aTimer)
{
	mStoreFlag += STORE_PRIVATEER_TIMER;
	mPrivateerTimer = aTimer;
}

void
	CPlanet::set_privateer_amount(int aMount)
{
	mPrivateerAmount = aMount;
}

void
	CPlanet::start_privateer()
{
	set_privateer_timer(CGame::get_game_time()
		+ TURNS_PER_6HOURS * CGame::mSecondPerTurn);
}

bool
	CPlanet::get_blockade()
{
	if (mBlockadeTimer > 0) return true;
	return false;
}

time_t
	CPlanet::get_blockade_timer()
{
	return mBlockadeTimer;
}

void
	CPlanet::start_blockade()
{
	mStoreFlag += STORE_BLOCKADE_TIMER;
	mBlockadeTimer = CGame::get_game_time() +
		TURNS_PER_24HOURS * CGame::mSecondPerTurn;
}

void
	CPlanet::set_destroy_timer(int turns)
{
	mDestroyTurns = turns;
	mStoreFlag += STORE_TURNS_TILL_DESTRUCTION;
}

int
	CPlanet::get_destroy_turns()
{
	return mDestroyTurns;
}

bool
	CPlanet::update_destroy_timer()
{
	if (mDestroyTurns > 1) {
		mDestroyTurns = mDestroyTurns - 1;
		mStoreFlag += STORE_TURNS_TILL_DESTRUCTION;
		return FALSE;
	} else if (mDestroyTurns == 1 ) {
		return TRUE;
	} else return FALSE;
}

char *
	CPlanet::get_destroy_time()
{
	return "\0";
}

void
	CPlanet::normalize()
{
	change_size(0);
	change_resource(0);
	mAttribute.clear();
	mStoreFlag += STORE_ATTRIBUTE;
}

const char*
	CPlanet::get_planet_image_url()
{
	static CString url;
	url.clear();

	int GasCount = 0;
	for(int i=GAS_BEGIN; i<GAS_MAX; i++)
		GasCount += mAtmosphere[i];

	if ((mAtmosphere[GAS_CH4] > 0) && (mAtmosphere[GAS_CH4] == GasCount)) url += "Methane";
	else if ((mAtmosphere[GAS_Cl2] > 0) && (mAtmosphere[GAS_Cl2] == GasCount)) url += "Chlorine";
	else if ((mAtmosphere[GAS_H2O] > 0) && (mAtmosphere[GAS_H2O] == GasCount)) url += "Ocean";
	else if (has_attribute(PA_OCEAN)) url += "Ocean";
	else if (has_attribute(PA_INTENSE_VOLCANIC_ACTIVITY)) url += "Volcanic";
	else if (mTemperature < 160) url += "Cold";
	else if (mTemperature > 440) url += "Dune";
	else if ((mAtmosphere[GAS_O2] >= 1) && (mAtmosphere[GAS_N2] >= 1)) url += "Terran";
	else if ((mAtmosphere[GAS_H2] >= 1) && (mAtmosphere[GAS_N2] >= 1)) url += "Composite";
	else if ((mAtmosphere[GAS_CO2] >= 1) && (mAtmosphere[GAS_N2] >= 1)) url += "Composite";
	else if ((mAtmosphere[GAS_H2] > 0) && (mAtmosphere[GAS_H2] >= GasCount/2)) url += "Hydrogen";
	else if ((mAtmosphere[GAS_N2] > 0) && (mAtmosphere[GAS_N2] >= GasCount/2)) url += "Nitrogen";
	else if ((mAtmosphere[GAS_CH4] > 0) && (mAtmosphere[GAS_CH4] >= GasCount/2)) url += "Methane";
	else if ((mAtmosphere[GAS_Cl2] > 0) && (mAtmosphere[GAS_Cl2] >= GasCount/2)) url += "Chlorine";
	else if ((mAtmosphere[GAS_H2O] > 0) && (mAtmosphere[GAS_H2O] >= GasCount/2)) url += "Ocean";
	else if (has_attribute(PA_VOLCANIC_ACTIVITY)) url += "Volcanic";
	else if (GasCount <= 1) url += "Barren";
	else if (mTemperature < 220) url += "Cold";
	else if (mTemperature > 380) url += "Dune";
	else url += "Barren";

	url += "_";

	switch(mSize)
	{
		case SIZE_TINY:
			url += "Tiny"; break;
		case SIZE_SMALL:
			url += "Small"; break;
		case SIZE_MEDIUM:
			url += "Medium"; break;
		case SIZE_LARGE:
			url += "Large"; break;
		case SIZE_HUGE:
			url += "Huge"; break;
	}

	url += ".gif";

	return (char*)url;

}
