#if !defined(__ARCHSPACE_FORTRESS_H__)
#define __ARCHSPACE_FORTRESS_H__

#include "common.h"
#include "store.h"
#include "admiral.h"
#include "fleet.h"

class CPlayer;
class CAdmiral;
class CAdmiralList;
class CFleet;
class CFleetList;
class CCouncil;

extern TZone gFortressZone;

class CFortress : public CStore
{
	public:
		enum EStoreFlag
		{
			STORE_LAYER = 0,
			STORE_SECTOR,
			STORE_ORDER,
			STORE_OWNER_ID
		};

	public:
		CFortress();
		virtual ~CFortress();

		virtual const char *table() { return "fortress"; }
		virtual CString& query();

	public:
		int get_layer() { return mLayer; }
		int get_sector() { return mSector; }
		int get_order() { return mOrder; }
		int get_owner_id() { return mOwnerID; }
		CPlayer *get_owner2();
		int get_key() { return (mLayer*100*100 + mSector*100 + mOrder); }
		static int get_key(int aLayer, int aSector, int aOrder)
				{ return (aLayer*100*100 + aSector*100 + aOrder); }

		bool set_layer(int aLayer);
		bool set_sector(int aSector);
		bool set_order(int aOrder);
		bool set_owner_id(int aOwnerID);

		char *get_name() { return (char *)mName; }

		bool add_admiral(CAdmiral *aAdmiral);
		bool remove_admiral(int aAdmiralID);
		CAdmiralList *get_admiral_list() { return &mAdmiralList; }

		bool add_fleet(CFleet *aFleet);
		bool remove_fleet(int aFleetID);
		CFleetList *get_fleet_list() { return &mFleetList; }

		bool is_dead();

		bool regen_empire_fleets();

		bool update_DB();

		bool initialize(int aLayer, int aSector, int aOrder, int aOwnerID);

		bool is_taken_by_council(CCouncil *aCouncil);

		bool is_attackable(CPlayer *aAttacker);

	protected :
		void refresh_name();

	private:
		int
			mLayer,
			mSector,
			mOrder,
			mOwnerID;
		CString
			mName;
		CAdmiralList
			mAdmiralList;
		CFleetList
			mFleetList;

	public:
		friend CStoreCenter &operator<<(CStoreCenter &aStoreCenter,
										CFortress &aFortress);
};

class CFortressList: public CSortedList
{
	public:
		CFortressList();
		virtual ~CFortressList();

		bool add_fortress(CFortress *aFortress);
		bool remove_fortress(int aLayer, int aSector, int aOrder);

		CFortress *get_by_layer_sector_order(int aLayer, int aSector, int aOrder);
		CFortress *get_by_key(int aKey);

		bool update_DB();

		int get_number_of_fortresses_taken_by_council(CCouncil *aCouncil, int aLayer = 0);

	public :
		char *get_fortress_status_html(CPlayer *aPlayer);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

#endif
