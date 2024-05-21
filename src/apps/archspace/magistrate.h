#if !defined(__ARCHSPACE_MAGISTRATE_H__)
#define __ARCHSPACE_MAGISTRATE_H__

#include "common.h"
#include "util.h"
#include "admiral.h"
#include "fleet.h"
#include "planet.h"

class CAdmiral;
class CAdmiralList;
class CFleet;
class CFleetList;
class CPlanet;
class CSortedPlanetList;

#define MAX_MAGISTRATE_PLANETS		50

extern TZone gMagistrateZone;

class CMagistrate
{
	public:
		CMagistrate();
		~CMagistrate();

	public:
		int get_cluster_id() { return mClusterID; }
		bool set_cluster_id(int aClusterID);

		char *get_name() { return (char *)mName; }

		bool add_admiral(CAdmiral *aAdmiral);
		bool remove_admiral(int aAdmiralID);
		CAdmiralList *get_admiral_list() { return &mAdmiralList; }

		bool add_fleet(CFleet *aFleet);
		bool remove_fleet(int aFleetID);
		CFleetList *get_fleet_list() { return &mFleetList; }

		bool add_planet(CPlanet *aPlanet);
		bool remove_planet(int aPlanetID);
		CSortedPlanetList *get_planet_list() { return &mPlanetList; }
		int get_number_of_planets();

		CSortedPlanetList *get_lost_planet_list();
		int get_number_of_lost_planets();

		bool is_dead();
		CPlanet *get_target_planet();

		bool regen_empire_fleets();

		bool update_DB();

		bool initialize(int aClusterID);

	protected :
		void refresh_name();

	private :
		int
			mClusterID;
		CString
			mName;
		CAdmiralList
			mAdmiralList;
		CFleetList
			mFleetList;

		CSortedPlanetList
			mPlanetList;
};

class CMagistrateList: public CSortedList
{
	public:
		CMagistrateList();
		virtual ~CMagistrateList();

		bool add_magistrate(CMagistrate *aMagistrate);
		bool remove_magistrate(int aClusterID);

		CMagistrate *get_by_cluster_id(int aClusterID);

		bool update_DB();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

#endif
