#if !defined(__ARCHSPACE_SHIP_H__)
#define __ARCHSPACE_SHIP_H__

#include "store.h"
#include "define.h"
#include "component.h"

class CPlayer;

#define DEFAULT_SHIP_SIZE_NUMBER	2

extern TZone gShipSizeZone;
/**
*/
class CShipSize
{
	private:
		int
			mID,
			mClass,		//class
			// ---Change--- //
			mBaseHP,          // HP
			// ---End Change--- //
			mSpace,		//space
			mWeapon,	//weapon slot
			mSlot,		//space/slot
			mDevice,	//device slot
			mCost,		//building cost
			mUpkeep;	//MP upkeep
		CString
			mName,		//name
			mDescription;

	public:
		CShipSize() {}

		int get_id() { return mID; }
		const char *get_name() { return (char *)mName; }
		int get_class() { return mClass; }
		int get_base_hp() { return mBaseHP; }
		int get_space() { return mSpace; }
		int get_weapon() { return mWeapon; }
		int get_slot() { return mSlot; }
		int get_device() { return mDevice; }
		int get_cost() { return mCost; }
		int get_upkeep() { return mUpkeep; }
		char *get_description() { return (char *)mDescription; }

		void set_id( int aID ) { mID = aID; }
		void set_name( char *aName ) { mName = aName; }
		void set_class( int aClass ) { mClass = aClass; }
		// ---Change--- //
		void set_base_hp( int aBaseHP ) { mBaseHP = aBaseHP; }
		// ---End Change--- //
		void set_space( int aSpace ) { mSpace = aSpace; }
		void set_weapon( int aWeapon ) { mWeapon = aWeapon; }
		void set_slot( int aSlot ) { mSlot = aSlot; }
		void set_device( int aDevice ) { mDevice = aDevice; }
		void set_cost( int aCost ) { mCost = aCost; }
		void set_upkeep( int aUpkeep ) { mUpkeep = aUpkeep; }
		void set_description(char *aDescription) { mDescription = aDescription; }

		const char *html_print_row();
		char *input_weapon_list_html(CPlayer *aPlayer);
		char *input_device_list_html(CPlayer *aPlayer);

	RECYCLE(gShipSizeZone);
};

/**
*/
class CShipSizeList: public CSortedList
{
	public:
		CShipSizeList();
		virtual ~CShipSizeList();

		int add_ship_size(CShipSize *aShip);
		bool remove_ship_size(int aShipID);

		CShipSize *get_by_id(int aShipID);
	protected:
		virtual bool free_item( TSomething aItem );
		virtual int compare( TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key( TSomething aItem, TConstSomething aKey ) const;
		virtual const char *debug_info() const { return "ship size list"; }
};

/**
*/
class CShipSizeTable: public CSortedList
{
	public:
		CShipSizeTable();
		virtual ~CShipSizeTable();

		int add_ship_size(CShipSize *aShip);
		bool remove_ship_size(int aShipID);

		CShipSize* get_by_id( int aShipID );

		char *available_size_list_html(CPlayer *aPlayer);
		char *size_information_html(CPlayer *aPlayer);
	protected:
		virtual bool free_item( TSomething aItem );
		virtual int compare( TSomething aItem1, TSomething aItem2 ) const;
		virtual int compare_key( TSomething aItem, TConstSomething aKey ) const;
		virtual const char *debug_info() const { return "ship size table"; }
};

/**
*/
class CShipScript: public CScript
{
	public:
		virtual ~CShipScript() {}

		bool get( CShipSizeTable *aShipTable );
};

extern TZone gShipDesignZone;
/**
*/
class CShipDesign: public CStore
{
	public:
		CShipDesign();
		CShipDesign( MYSQL_ROW aRow );
		bool is_black_market_design() { return mBlackMarketDesign; }
		void make_black_market_design() { mBlackMarketDesign = true;}
		void remove_black_market_design() {mBlackMarketDesign = false;}
		bool is_empire_design() { return mEmpireDesign; }
		void make_empire_design() { mEmpireDesign = true;}
		void remove_empire_design() {mEmpireDesign = false;}

		virtual ~CShipDesign();
	/* from CStore */
		virtual const char *table() { return "class"; }
		virtual CString& query();

		enum EDeleteDesign {
			USED_IN_FLEET,
			USED_IN_DOCK,
			USED_IN_REPAIR_BAY,
			USED_IN_BUILD_Q,
			DELETE_OK,
			DELETE_PROHIBITED	// telecard 2001/03/05
		};

	private:
		int
			mDesignID;
		CString
			mName;
		int
			mOwner;
		int
			mBody,
			mArmor,
			mEngine,
			mComputer,
			mShield;
		int
			mWeaponList[WEAPON_MAX_NUMBER],
			mWeaponNumberList[WEAPON_MAX_NUMBER],
			mDeviceList[DEVICE_MAX_NUMBER];
		int
			mBuildCost,
			mBuildTime;
		bool mBlackMarketDesign;
		bool mEmpireDesign;
	public:
		int get_design_id() { return mDesignID; }
		virtual const char* get_name();
		const char *get_nick();
		int get_owner() { return mOwner; }
		int get_body() { return mBody; }
		int get_size() { return mBody - 4000; }
		int get_armor() { return mArmor; }
		int get_engine() { return mEngine; }
		int get_computer() { return mComputer; }
		int get_shield() { return mShield; }

		int get_max_hp_without_effect();

		int get_weapon( int index ) { return mWeaponList[index]; }
		int get_weapon_number( int index ) { return mWeaponNumberList[index]; }
		int get_device( int index ) { return mDeviceList[index]; }

		int get_build_time() { return mBuildTime; }
		int get_build_cost() { return mBuildCost; }

		void set_design_id( int aDesignID ) { mDesignID = aDesignID; }
		virtual void set_name( char* aName ) { mName = aName; }
		void set_owner(int aOwnerID) { mOwner = aOwnerID; }
		void set_body( int aBody ) { mBody = aBody; }
		void set_armor( int aArmor ) { mArmor = aArmor; }
		void set_engine( int aEngine ) { mEngine = aEngine; }
		void set_computer( int aComputer ) { mComputer = aComputer; }
		void set_shield( int aShield ) { mShield = aShield; }

		void set_weapon( int index, int aWeapon ) { mWeaponList[index] = aWeapon; }
		void set_weapon_number( int index, int aNumber ) { mWeaponNumberList[index] = aNumber ; }
		void set_device( int index, int aDevice ) { mDeviceList[index] =aDevice; }

		void set_build_time( int aTime ) { mBuildTime = aTime; }
		void set_build_cost( int aCost ) { mBuildCost = aCost; }

		char *print_html(CPlayer *aPlayer, int aPP = -1);
		char *class_info_for_building_html(CPlayer *aPlayer);

		int can_delete(CPlayer *aPlayer);
		bool check_legality();
		bool check_component_legality(CComponent *aComponent);
		friend CStoreCenter& operator<<(CStoreCenter& aStoreCenter, CShipDesign& aDesign);

	RECYCLE(gShipDesignZone);
};

/**
*/
class CShipDesignList: public CSortedList
{
	public:
		CShipDesignList() {};
		virtual ~CShipDesignList() { remove_all(); }

		int add_ship_design(CShipDesign *aDesign);
		bool remove_ship_design(int aDesignID);
		bool remove_without_free_ship_design(int aDesignID);

		CShipDesign* get_by_id( int aDesignID );
		int max_design_id();
		const char* print_html(CPlayer *aPlayer, int aPP = -1);
		char *building_class_select_html(CPlayer *aPlayer);

	protected:
		virtual bool free_item( TSomething aItem );
		virtual int compare( TSomething aItem1, TSomething aItem2 ) const;
		virtual int compare_key( TSomething aItem, TConstSomething aKey ) const;
		virtual const char *debug_info() const { return "ship design list"; }
};


extern TZone gShipToBuildZone;
/**
*/
class CShipToBuild : public CShipDesign
{
	public :
		enum EStoreFlag
		{
			STORE_OWNER = 0,
			STORE_DESIGN_ID,
			STORE_NUMBER,
			STORE_TIME_ORDER
		};

	private:
		int
			mNumber;
		time_t
			mOrder;
	public:

		CShipToBuild() { mNumber = 0; mOrder = 0; }
		CShipToBuild( int aOwner, CShipDesign *aDesign, int aNumber = 0, int aOrder = 0 );

		virtual ~CShipToBuild();

		virtual const char *table() { return "ship_building_q"; }
		virtual CString& query();

		int change_number( int aNumber );
		int get_number() { return mNumber; }

		time_t get_order() { return mOrder; }
		void set_order() { mOrder = time(0); }

		const char* print_html(CPlayer *aPlayer, int aPP = -1);
		char *ship_to_build_info_html(int aIndex);

		friend CStoreCenter& operator<<(CStoreCenter& aStoreCenter, CShipToBuild& aShip);

	RECYCLE(gShipToBuildZone);
};

extern TZone gShipBuildQZone;

/**
*/
class CShipBuildQ: public CSortedList
{
	public:
		CShipBuildQ() {};
		virtual ~CShipBuildQ() { remove_all(); }

		int add_ship_to_build(CShipToBuild *aShip);
		bool remove_ship_to_build(CShipToBuild *aShip);

		const char* print_html(CPlayer *aPlayer, int aPP = -1);
		char *building_queue_info_html();

	protected:
		virtual bool free_item( TSomething aItem );
		virtual int compare( TSomething aItem1, TSomething aItem2 ) const;
		virtual int compare_key( TSomething aItem, TConstSomething aKey ) const;
		virtual const char *debug_info() const { return "ship build q"; }
};

extern TZone gDockedShipZone;
/**
*/
class CDockedShip: public CShipDesign
{
	public :
		enum EStoreFlag
		{
			STORE_OWNER = 0,
			STORE_DESIGN_ID,
			STORE_NUMBER
		};

	public:
		CDockedShip() { mNumber = 0; }
		CDockedShip( CShipDesign *aClass, int aNumber = 0 );
		virtual ~CDockedShip();

		virtual const char *table() { return "docked_ship"; }
		virtual CString& query();

		int change_number( int aNumber );
		int get_number() { return mNumber; }
		int calc_upkeep();

		int get_power();

		friend CStoreCenter& operator<<(CStoreCenter& aStoreCenter, CShipToBuild& aShip);

		const char *print_html();
		char *docked_ship_info_html(int aIndex);

	private:
		int mNumber;

	RECYCLE(gDockedShipZone);
};

/**
*/
class CDock: public CSortedList
{
	private:
		int
			mPower;
	public:
		CDock();
		virtual ~CDock();

		int add_docked_ship(CDockedShip *aShip);
		bool remove_docked_ship(CDockedShip *aShip);
		bool change_ship( CShipDesign *aClass, int aNumber );

		CDockedShip *get_by_id( int aDesignID );

		int count_ship( int aDesignID );
		int get_ship_number();

		int get_power() { return mPower; }
		bool refresh_power();

		const char *print_html();
		char *dock_info_html();
		char *radio_ship_reassignment_html(int designID);
		char *print_html_select();
		char *print_javascript_select();
	protected:
		virtual bool free_item( TSomething aItem );
		virtual int compare( TSomething aItem1, TSomething aItem2 ) const;
		virtual int compare_key( TSomething aItem, TConstSomething aKey ) const;
		virtual const char *debug_info() const { return "dock"; }
};

extern TZone gDamagedShipZone;
/**
*/
class CDamagedShip: public CShipDesign
{
	public:
		enum EStoreFlag
		{
			STORE_OWNER,
			STORE_ID,
			STORE_DESIGN_ID,
			STORE_HP
		};

	public:
		CDamagedShip() { mID = mHP = 0; }
		CDamagedShip( CShipDesign *aClass, int aHP, int aID = -1 );
		virtual ~CDamagedShip();

		virtual const char *table() { return "damaged_ship"; }
		virtual CString& query();

		int get_hp() { return mHP; }
		int change_hp( int aHP );
		int get_max_hp() { return get_max_hp_without_effect(); }

		int repair( int aPercent );
		bool is_fully_repaired();
		int get_repair_ratio();

		int get_id() { return mID; }
		void set_id( int aID ) { mID = aID; }

		friend CStoreCenter& operator<<(CStoreCenter& aStoreCenter, CShipToBuild& aShip);

		const char *print_html();
	private:
		int
			mID,
			mHP;

	RECYCLE(gDamagedShipZone);
};

/**
*/
class CRepairBay: public CSortedList
{
	public:
		CRepairBay() {};
		virtual ~CRepairBay() { remove_all(); }

		int add_damaged_ship(CDamagedShip *aShip);
		bool remove_damaged_ship(CDamagedShip *aShip);

		CDamagedShip *get_by_id( int aID );

		const char *print_html();
		char *repairing_bay_info_html();

	protected:
		virtual bool free_item( TSomething aItem );
		virtual int compare( TSomething aItem1, TSomething aItem2 ) const;
		virtual int compare_key( TSomething aItem, TConstSomething aKey ) const;
		virtual const char *debug_info() const { return "repair bay"; }
};

#endif

/* end of file ship.h */
