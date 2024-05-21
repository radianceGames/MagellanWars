#if !defined(__ARCHSPACE_BLACKMARKET_H__)
#define __ARCHSPACE_BLACKMARKET_H__

#include "store.h"

class CSortedPlanetList;
class CShipDesignList;
class CAdmiralList;
class CFleetList;
class CTech;
class CFleet;
class CShipDesign;
class CAdmiral;
class CProject;
class CPlanet;
class CPlayer;

// These values are in minutes
#define TECH_EXPIRE_TIME	60 * 1	// 1 hour
#define FLEET_EXPIRE_TIME	60 * 1	// 1 hour
#define ADMIRAL_EXPIRE_TIME	60 * 1	// 1 hour
#define PROJECT_EXPIRE_TIME	60 * 1	// 1 hour
#define PLANET_EXPIRE_TIME	60 * 1	// 1 hour

// These define the max tech level/race/ship class
#define MAX_TECH_LEVEL 6
#define MAX_SHIP_CLASS 10

// Special Event Switches - 0 = off, 1 = on
#define SPECIAL_EVENT_ENABLED 1

class CBid : public CStore
{
	public:
		enum
		{
			ITEM_TECH,
			ITEM_FLEET,
			ITEM_ADMIRAL,
			ITEM_PROJECT,
			ITEM_PLANET
		};

	private:
		int
			mID;
		int
			mType,
			mItem;
		int
			mWinner;	// current winner
		int
			mPrice;		// current price
		int
			mOpend,		// opend time
			mExpire,	// current expire time
			mClosed;	// closed time, if not closed, 0.
		int
			mNumberOfPlanet;

	public:
		CBid();
		CBid(MYSQL_ROW aRow);
		virtual ~CBid() {};

		int get_id() { return mID; }
		int get_type() { return mType; }
		bool is_type(int aType) { return (mType == aType); }
		int get_item() { return mItem; }
		int get_winner_id() { return mWinner; }
		int get_open_time() { return mOpend; }
		int get_expire_time() { return mExpire; }
		int get_price() { return mPrice; }
		bool is_closed() { return(mClosed != 0); }
		int get_number_of_planet() { return mNumberOfPlanet; }

		void set_id(int aID) { mID = aID; }
		void set_type(int aType) { mType = aType; }
		void set_item(int aItemID) { mItem = aItemID; }
		void set_winner_id(int aWinner) { mWinner = aWinner; }
		void set_open_time(int aTime) { mOpend = aTime; }
		//void set_expire_time(int aTime) { mExpire = aTime; }
		void set_expire_time(int aTime);
		bool set_price(int aPrice);
		void set_closed_time(int aTime) { mClosed = aTime; }
		void set_number_of_planet(int aNumberOfPlanet)
				{ mNumberOfPlanet = aNumberOfPlanet; }

		virtual const char *table() { return "blackmarket"; }
		virtual CString& query();
		friend CStoreCenter& operator<<(CStoreCenter& aStoreCenter, CBid& aBid);
};

class CBidList : public CSortedList
{
	public:
		CBidList();
		virtual ~CBidList();

		int add_bid(CBid *aBid);
		bool remove_bid(int aBidID);

		CBid* get_by_id(int aBidID);

		int count_by_type(int aType);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key( TSomething aItem, TConstSomething aKey) const;
};

class CBlackMarket
{
	private:
		CBidList *mBidList;

		CSortedPlanetList *
			mPlanetList;
		CShipDesignList *
			mShipDesignList;
		CAdmiralList *
			mAdmiralList;
		CFleetList *
			mFleetList;

	public :
		static int
			mBlackMarketItemRegen,
			mBidExpireTime;

	public:
		CBlackMarket();
		~CBlackMarket();

		CSortedPlanetList *get_planet_list() { return mPlanetList; }
		CShipDesignList *get_ship_design_list() { return mShipDesignList; }
		CAdmiralList *get_admiral_list() { return mAdmiralList; }
		CFleetList *get_fleet_list() { return mFleetList; }

		void load( CMySQL *aMySQL );
		void add_bid(CBid *aBid);
		bool remove_bid(CBid *aBid);
		int get_new_bid_id();

		bool add_new_item(CTech *aTech);
		bool add_new_item(CFleet *aFleet, CShipDesign *aDesign);
		bool add_new_item(CAdmiral *aAdmiral);
		bool add_new_item(CProject *aProejct);
		bool add_new_item(CPlanet *aPlanet);

		void create_new_tech();
		void create_new_fleet();
		void create_new_admiral();
		void create_new_project();
		void create_new_planet();

		void add_item( CShipDesign* aDesign );
		void add_item( CFleet* aFleet );
		void add_item( CAdmiral* aAdmiral );
		void add_item( CPlanet* aPlanet );

		CBidList *get_bid_list() { return mBidList; }
		void expire( CBid* bid );
		void expire_all();

		void bid(int aBidID, int aPlayerID, int price);

		bool initialize(CMySQL *aMySQL);

	public:
		void add_planet( CPlanet *planet );
		void add_fleet( CFleet *fleet );
		void remove_by_fleet_id( int id );
};

class CBlackMarketHTML
{
	public:
		const char* get_tech_list_html(CPlayer *aPlayer);
		const char* get_fleet_list_html(CPlayer *aPlayer);
		const char* get_admiral_list_html(CPlayer *aPlayer);
		const char* get_project_list_html(CPlayer *aPlayer);
		const char* get_planet_list_html(CPlayer *aPlayer);
};

#endif
