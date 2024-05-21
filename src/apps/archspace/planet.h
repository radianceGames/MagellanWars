#if !defined(__ARCHSPACE_PLANET_H__)
#define __ARCHSPACE_PLANET_H__

#include "ctrl.model.h"
#include "define.h"
#include "news.h"
#include "store.h"

class CRace;
class CCluster;

extern TZone gResourceZone;
extern TZone gPlanetListZone;

//--------------------------------------------------- CResource
enum EResourceType {
	RESOURCE_BEGIN = 0,
	RESOURCE_PRODUCTION = 0,
	RESOURCE_MILITARY,
	RESOURCE_RESEARCH,
//	RESOURCE_NOGADA,
	RESOURCE_MAX,

	BUILDING_FACTORY = RESOURCE_PRODUCTION,
	BUILDING_MILITARY_BASE = RESOURCE_MILITARY,
	BUILDING_RESEARCH_LAB = RESOURCE_RESEARCH,
//	POPULATION = RESOURCE_NOGADA
};

/**
*/
class CResource
{
	private:
		int
			mResource[RESOURCE_MAX];	// resource > 0
	public:
		CResource();
		CResource(CResource& aResource);
		CResource(int aFactory, int aMilitary, int aResearch);
		~CResource();

		inline void clear();
		bool set(EResourceType aResourceType, int aResourceValue);
		bool change(EResourceType aResourceType, int aResourceValue);
		int get(EResourceType aResourceType) const;
		void set_all(int aProduction, int aMilitary, int aResearch);
		int total() const;
		CResource& operator=(const CResource& aResource);
		CResource& operator+=(const CResource& aResource);

		const char* debug_info();
		const CString &get_query( char *prefix = "" );

		static const char *get_building_name( int aBuilding );

	RECYCLE(gResourceZone);
};

inline void
CResource::clear()
{
	memset(mResource, 0, sizeof(mResource));
}

//-------------------------------------------------------- CPlanet

#define MIN_TEMPERATURE		100
#define MAX_TEMPERATURE		500
#define MIN_GRAVITY		0.2
#define MAX_GRAVITY		2.5
#define MIN_ATMOSPHERE		0
#define MAX_ATMOSPHERE		5
#define SUM_ATMOSPHERE		5

extern TZone gPlanetZone;

class CPlanet: public CStore
{
	friend class CPlanetNewsCenter;
	friend class CPlanetList;

	public:
		enum EPlanetAttribute
		{
			PA_BEGIN = 0,
			PA_LOST_TRABOTULIN_LIBRARY = 0,
			PA_COGNITION_AMPLIFIER,
			PA_MILITARY_STRONGHOLD,
			PA_ANCIENT_RUINS,
			PA_ARTIFACT,
			PA_MASSIVE_ARTIFACT,
			PA_ASTEROID,
			PA_MOON,
			PA_RADIATION,
			PA_SEVERE_RADIATION,
			PA_HOSTILE_MONSTER,
			PA_OBSTINATE_MICROBE,
			PA_BEAUTIFUL_LANDSCAPE,
			PA_BLACK_HOLE,
			PA_NEBULA,
			PA_DARK_NEBULA,
			PA_VOLCANIC_ACTIVITY,
			PA_INTENSE_VOLCANIC_ACTIVITY,
			PA_OCEAN,
			PA_IRREGULAR_CLIMATE,
			PA_MAJOR_SPACE_ROUTE,
			PA_MAJOR_SPACE_CROSSROUTE,
			PA_FRONTIER_AREA,
			PA_GRAVITY_CONTROLED,
			PA_SHIP_YARD,
			PA_MAINTENANCE_CENTER,
			PA_UNDERGROUND_CAVERNS,
			PA_RARE_ORE,
			PA_MOON_CLUSTER,
//			PA_STABLE_WORMHOLE,
//			PA_DIPLOMATIC_OUTPOST,
			PA_END
		};

		enum EGasType
		{
		    GAS_BEGIN = 0,
		    GAS_H2 = 0,
		    GAS_Cl2,
		    GAS_CO2,
		    GAS_O2,
		    GAS_N2,
		    GAS_CH4, // 메탄
		    GAS_H2O,
		    GAS_MAX
		};

		enum EStoreFlag
		{
			STORE_ID = 0,
			STORE_CLUSTER,
			STORE_OWNER,
			STORE_ORDER,
			STORE_NAME,
			STORE_ATTRIBUTE,
			STORE_POPULATION,

			STORE_BUILDING_FACTORY,
			STORE_BUILDING_MILITARY_BASE,
			STORE_BUILDING_RESEARCH_LAB,

			STORE_PROGRESS_FACTORY,
			STORE_PROGRESS_MILITARY_BASE,
			STORE_PROGRESS_RESEARCH_LAB,

			STORE_RATIO_FACTORY,
			STORE_RATIO_MILITARY_BASE,
			STORE_RATIO_RESEARCH_LAB,

			STORE_ATMOSPHERE,
			STORE_TEMPERATURE,
			STORE_SIZE,
			STORE_RESOURCE,
			STORE_GRAVITY,

			STORE_INVESTMENT,

			STORE_TERRAFORMING,
			STORE_TERRAFORMING_TIMER,

			STORE_COMMERCE_WITH_1,
			STORE_COMMERCE_WITH_2,
			STORE_COMMERCE_WITH_3,

			STORE_PRIVATEER_TIMER,
			STORE_BLOCKADE_TIMER,

			STORE_NEWS_POPULATION,
			STORE_NEWS_FACTORY,
			STORE_NEWS_MILITARY_BASE,
			STORE_NEWS_RESEARCH_LAB,

			STORE_TURNS_TILL_DESTRUCTION,
			STORE_PLANET_INVEST_POOL
		};

	public:
		enum
		{
			SIZE_TINY = 0,
			SIZE_SMALL,
			SIZE_MEDIUM,
			SIZE_LARGE,
			SIZE_HUGE,
		};

		enum
		{
			RESOURCE_ULTRA_POOR = 0,
			RESOURCE_POOR,
			RESOURCE_NORMAL,
			RESOURCE_RICH,
			RESOURCE_ULTRA_RICH
		};

	private:
		static int mMaxID;

		int mID;
		int mOwnerID;
		int mClusterID;

		CString mName;

		CCluster* mCluster;

		int mOrder;

		CControlModel mControlModel;

		CCommandSet mAttribute;

		int mPopulation;		// 단위 k
		int mMaxPopulation;		// 단위 k

		CResource
			mOldBuilding,
			mBuilding,
			mBuildingProgress,
			mDistributeRatio,
			mNewResource,
			mUpkeep,
			mNewBuilding;

		char mAtmosphere[GAS_MAX];  // 0 - 5

		int mTemperature; // 100 - 500K
		int mSize;		  // 0 - 4
		int mResource;	  // 0 - 4
		double mGravity;  // 0.2G - 2.5G

		int mInvestment;
		int mInvestRate;
		int mWasteRate;

		bool mTerraforming;
		int mTerraformingTimer;

		time_t mPrivateerTimer;
		int mPrivateerAmount;

		time_t mBlockadeTimer;

		int mCommerceWith[MAX_COMMERCE_PLANET];

		CPlanetNewsCenter mPlanetNewsCenter;

		int mDestroyTurns;
		bool mPlanetInvestPool;

	public:
		CPlanet();
		CPlanet(MYSQL_ROW aRow);
		virtual ~CPlanet();

		void initialize(CRace *aRace = NULL);

		void init_planet_news_center();

		virtual const char *table() { return "planet"; }
		virtual CString& query();

		// get
		int get_id() { return mID; }
		CPlayer* get_owner();
		int get_owner_id() { return mOwnerID; }
		CCluster* get_cluster();
		int get_cluster_id();
		inline int get_order();

		char *get_name() { return (char *)mName; }
		inline int get_population();
		char *get_population_with_unit();
		int get_old_population();

		inline int get_max_population() { return mMaxPopulation; }
		char *get_max_population_with_unit();
		bool set_max_population();

		int get_power();

		const char *get_size_description();
		const char *get_resource_description();
		const char *get_atmosphere();
		inline int get_temperature();
		inline int get_size();
		inline int get_resource();
		inline double get_gravity();
		const char* get_order_name();

		int get_pp_per_turn();
		int get_mp_per_turn();
		int get_rp_per_turn();

		int get_production_per_turn();
		int get_research_per_turn();
		int get_military_per_turn();

		int get_pure_production_per_turn();
		int get_pure_commerce_per_turn();

		int get_upkeep_per_turn(int aResourceType = -1);

		inline int get_invest_max_production();
		inline int get_investment();

		inline int get_waste_rate();

		int get_commerce_with(int Index);
		void clear_commerce_with(int aPlanetID);
		void clear_commerce_all();
		char *all_commercial_planet_name();

		bool is_paralyzed();

		bool get_privateer();
		time_t get_privateer_timer();
		int get_privateer_amount();

		bool get_blockade();
		time_t get_blockade_timer();
		void start_blockade();

		void set_destroy_timer(int turns);
		int get_destroy_turns();
		bool update_destroy_timer();
		char *get_destroy_time();

		CControlModel *get_control_model() { return &mControlModel; }

		inline bool get_planet_invest_pool() { return mPlanetInvestPool; }
		inline void set_planet_invest_pool(bool status) { mPlanetInvestPool = status; mStoreFlag += STORE_PLANET_INVEST_POOL; }

		static int mRatioUltraPoor;
		static int mRatioPoor;
		static int mRatioNormal;
		static int mRatioRich;
		static int mRatioUltraRich;
		static int mCommerceMultiplier;

	public: // set
		inline void set_id(int aPlanetID);
		void set_owner(CPlayer *aOwner);
		void set_owner_id(int aID) { mOwnerID = aID; }
		void set_cluster(CCluster *aCluster);
		inline void set_order(int aOrder);

		bool set_atmosphere(EGasType aGasType, int aGas);
		bool set_atmosphere(const char *aAtmosphere);
		bool set_temperature(int aTemperature);
		bool set_gravity(double aGravity);
		bool set_size(int aSize);
		bool set_resource(int aResource);

		inline void set_cluster_id(int aID);
		inline void set_name(const char *aName);
		bool change_population(int aPopulation);
		void reset_old_population();

		void set_privateer_timer(time_t aTimer);
		void set_privateer_amount(int aMount);

		bool change_atmosphere(EGasType aDecreasing, EGasType aIncreasing);
		void change_temperature(int aTemperature);
		void change_gravity(double aGravity);
		void change_size(int aSize);
		void change_resource(int aResource);

		bool change_investment(int aAmount);

		void set_commerce_with(int Index, int aPlanetID);

		void normalize();
		const char *get_planet_image_url();

	public: // etc
		void start_terraforming();
		void start_privateer();
		int calc_environment_control();
		int calc_environment_control(CPlayer *aPlayer);	// telecard 2001/03/08
		inline void load_attribute(const char *aAttributeString);
		void update_turn();
		const char* html_management_record();
		const char* news();

		const char *get_atmosphere_html();
		const char *get_attribute_html();

		bool set_attribute(EPlanetAttribute aPlanetAttribute);
		bool remove_attribute(EPlanetAttribute aPlanetAttribute);
		bool has_attribute(EPlanetAttribute aPlanetAttribute);
		bool has_attribute(int aPlanetAttribute);

		int calc_commerce();
		int get_diplomacy_modifier_to_commerce();

		void build_control_model(CPlayer *aOwner);

		int calc_invest_rate();
        static const char *get_gas_description(EGasType aGasType);
		void refresh_resource_per_turn();
		char *get_attribute_description();

	public: // resource
		CResource &get_building() { return mBuilding; }
		CResource &get_distribute_ratio() { return mDistributeRatio; }
		CResource &get_new_resource() { return mNewResource; }
		CResource &get_upkeep() { return mUpkeep; }
		CResource &get_new_building() { return mNewBuilding; }

	public :
		static bool set_commerce_between(int aPlanetID1, int aPlanetID2);
		static bool clear_commerce_between(int aPlanetID1, int aPlanetID2);

	protected:
        const char *get_attribute_description(EPlanetAttribute aAttribute);
		int compute_nogada_point();
		int compute_upkeep_and_output(int aNogadaPoint, CResource *aNewResource, CResource *aUpkeep);
		void build_new_building(int aNogadaPoint);
		void compute_max_population();
		void process_population_growth();
		int calc_waste_rate();

		inline void process_privateer(time_t aNowGameTime);

		void process_terraforming();
		bool terraforming_atmosphere();
		bool terraforming_temperature();
		bool terraforming_gravity();
		bool change_planet_attribute();

	RECYCLE(gPlanetZone);
};

const char *get_size_description(int aSize);
const char *get_resource_description(int aResource);
const char *get_attribute_description(CPlanet::EPlanetAttribute aAttribute);

inline int
CPlanet::get_order()
{
	return mOrder;
}

inline int
CPlanet::get_population()
{
	return mPopulation;
}

inline void
CPlanet::set_id(int aPlanetID)
{
	mID = aPlanetID;
}

inline void
CPlanet::set_cluster_id(int aID)
{
	mClusterID = aID;
	mCluster = NULL;
}

inline void
CPlanet::set_name(const char* aName)
{
	mName = aName;
	mStoreFlag += STORE_NAME;
}

inline void
CPlanet::load_attribute(const char *aAttributeString)
{
	mAttribute.set_string(aAttributeString);
}

inline void
CPlanet::set_order(int aOrder)
{
	mOrder = aOrder;
	mStoreFlag += STORE_ORDER;
}

inline int
CPlanet::get_temperature()
{
	return mTemperature;
}

inline int
CPlanet::get_size()
{
	return mSize;
}

inline int
CPlanet::get_resource()
{
	return mResource;
}

inline double
CPlanet::get_gravity()
{
	return mGravity;
}

inline int
CPlanet::get_invest_max_production()
{
	return (int)((((double)mPopulation)*((double)mMaxPopulation))/10000000.0);
}

inline int
CPlanet::get_investment()
{
	return mInvestment;
}

inline int
CPlanet::get_waste_rate()
{
	return mWasteRate;
}

//--------------------------------------------------- CPlanetList
/**
*/
class CPlanetList: public CList
{
	private:
		int
			mPower;
	public:
		CPlanetList();
		virtual ~CPlanetList();

	public:
		int add_planet(CPlanet* aPlanet);
		bool remove_planet(int aPlanetID);
		bool remove_without_free_planet(int aPlanetID);

		CPlanet *get_by_id(int aPlanet);
		CPlanet* get_by_order(int aOrder);

		int total_population();

		const char *generate_news();
		const char *html_select();

		int count_planet_from_cluster( int aClusterID );
		int count_planet_with_attribute( int aAttribute );

		int get_power() { return mPower; }
		bool refresh_power();

		char *siege_planet_border_planet_select_html();
		char *blockade_border_planet_select_html();
		char *raid_border_planet_select_html();

		char *whitebook_planet_list_html();
		void get_info();

		CPlanetList& operator= (CPlanetList& aPlanet);

		bool update_DB();

		void add_destroy_planet(int ID);
		void cancel_destroy_planet(int ID);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual const char *debug_info() const { return "planet list"; }

	RECYCLE(gPlanetListZone);
};

//--------------------------------------------------- CSortedPlanetList
extern TZone gSortedPlanetListZone;
/**
*/
class CSortedPlanetList: public CSortedList
{
	public:
		CSortedPlanetList();
		virtual ~CSortedPlanetList();

	public:
		int add_planet(CPlanet* aPlanet);
		bool remove_planet(int aPlanetID);

		CPlanet *get_by_id(int aPlanetID);

		bool update_DB();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "sorted planet list"; }
	RECYCLE(gSortedPlanetListZone);
};

//------------------------------------------------------- CPlanetTable
/**
*/
class CPlanetTable : public CSortedList
{
	class CIDIndex: public CList
	{
		public:
			CIDIndex();
			virtual ~CIDIndex();

		protected:
			virtual bool free_item(TSomething aItem);
			virtual const char *debug_info() const { return "planet table id index"; }
	};

	public:
		CPlanetTable();
		virtual ~CPlanetTable();

	public:
		bool load(CMySQL &aMysql);

		int add_planet_when_loading(CPlanet* aPlanet);
		int add_planet(CPlanet* aPlanet);
		bool remove_planet(int aPlanetID);

		inline CPlanet *get_by_id(int aPlanetID);

		CPlanet *create_new_planet( CRace *aRace );

	protected:
		CIDIndex
			mIDIndex;

		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "planet table"; }
};

inline CPlanet*
CPlanetTable::get_by_id(int aPlanetID)
{
	return (CPlanet*)mIDIndex.get(aPlanetID);
}

#endif
