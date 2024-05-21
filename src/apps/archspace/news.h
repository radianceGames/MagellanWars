#if !defined(__ARCHSPACE_NEWS_H__)
#define __ARCHSPACE_NEWS_H__

#include <cstdio>
#include "cgi.h"
#include "project.h"

class CKnownTech;
class CPurchasedProject;
class CDiscoveredProject;
class CPlanet;
class CPlayer;
class CAdmiral;

//---------------------------------------------    CNews
/**
*/
class CNews: public CBase
{
	protected:
		CString mNews;

	public:
		CNews();
		virtual ~CNews();
		bool mViewed;

	public:
		const char *get();
		const char *get_query();

	public:
		void set_query(const char *aQuery);

	public:
		void clear();
		inline bool empty();

	protected:
		virtual const char *filter(const char *aNews) = 0;
};

inline bool
CNews::empty()
{
	return (mNews.length() == 0) ? true:false;
}

//--------------------------------------------    CKnownTechNews
/**
*/
class CKnownTechNews: public CNews
{
	public:
		virtual ~CKnownTechNews();

	public:
		void known_tech_news(const char *aName);

	protected:
		virtual const char *filter(const char* aNews);
};

//-------------------------------------------     CNewPlanetNews
/**
*/
class CPlanetNews: public CNews
{
	public:
		virtual ~CPlanetNews();

	public:
		void new_planet_news(const char *aName);

	protected:
		virtual const char *filter(const char* aNews);
};

//-------------------------------------------     CPurchasedProjectNews
/**
*/
class CPurchasedProjectNews: public CNews
{
	public:
		virtual ~CPurchasedProjectNews();

	public:
		void purchased_project_news(const char *aName);

	protected:
		virtual const char *filter(const char* aNews);
};

//-------------------------------------------     CDiscoveredProjectNews
/**
*/
class CDiscoveredProjectNews: public CNews
{
	public:
		virtual ~CDiscoveredProjectNews();

	public:
		void discovered_project_news(const char *aName);

	protected:
		virtual const char *filter(const char* aNews);
};

//-------------------------------------------     CAdmiralNews
/**
*/
class CAdmiralNews: public CNews
{
	public:
		virtual ~CAdmiralNews();

	public:
		void admiral_news(const char *aName);

	protected:
		virtual const char *filter(const char* aNews);
};

//--------------------------------------------    CNewsCenter
class CNewsCenter
{
	private:
		enum
		{
			STORE_TURN = 0,
			STORE_PRODUCTION,
			STORE_RESEARCH,
			STORE_POPULATION,
			STORE_ABILITY,
			STORE_TECH,
			STORE_PLANET,
			STORE_PROJECT,
			STORE_ADMIRAL,
			STORE_TIME_NEWS
		};

	private:
		CKnownTechNews mKnownTechNews;
		CPlanetNews mPlanetNews;
		CPurchasedProjectNews mPurchasedProjectNews;
		CDiscoveredProjectNews mDiscoveredProjectNews;
		CAdmiralNews mAdmiralNews;

		CCommandSet mStoreFlag;
		CPlayer *mOwner;

		int mTurn;
		int mProduction;
		int mResearch;
		int mPopulation;
		CCommandSet mAbility;

		CQueryList mTimeNews;

	public:
		CNewsCenter();
		~CNewsCenter();

		bool set_owner(CPlayer *aPlayer);
		const char *generate();
		const char *time_news();

	public: // get
		inline bool empty();
		const char *get_query();

	public: // set
		inline void set_turn(int aTurn);
		inline int get_turn() { return mTurn; }
		inline void set_production(int aProduction);
		inline void set_research(int aResearch);
		inline void set_population(int aPopulation);
		inline void set_ability(const char *aAbility);
		inline void set_tech(const char *aTech);
		inline void set_planet(const char *aPlanet);
		inline void set_project(const char *aProject);
		inline void set_admiral(const char *aAdmiral);
		void set_time_news(FILE *aNewsFile);

	public:
		void known_tech_news(CKnownTech* aKnownTech);
		void new_planet_news(CPlanet* aPlanet);
		void purchased_project_news(CPurchasedProject* aProject);
		void discovered_project_news(CProject* aProject);
		void admiral_news(CAdmiral* aAdmiral);

		void time_news(const char* aNewsString);

		void save_news();
		void clear_news();
		bool isViewed();

	private:
		const char* get_current_time();
};

inline bool
CNewsCenter::empty()
{
	return mStoreFlag.is_empty();
}

inline void
CNewsCenter::set_turn(int aTurn)
{
	mStoreFlag += STORE_TURN;
	mTurn = aTurn;
}

inline void
CNewsCenter::set_production(int aProduction)
{
	mStoreFlag += STORE_PRODUCTION;
	mProduction = aProduction;
}

inline void
CNewsCenter::set_research(int aResearch)
{
	mStoreFlag += STORE_RESEARCH;
	mResearch = aResearch;
}

inline void
CNewsCenter::set_population(int aPopulation)
{
	mStoreFlag += STORE_POPULATION;
	mPopulation = aPopulation;
}

inline void
CNewsCenter::set_ability(const char *aAbility)
{
	mStoreFlag += STORE_ABILITY;
	mAbility.clear();
	if (aAbility) mAbility.set_string(aAbility);
}

inline void
CNewsCenter::set_tech(const char *aTech)
{
	mStoreFlag += STORE_TECH;
	mKnownTechNews.clear();
	if (aTech) mKnownTechNews.set_query(aTech);
}

inline void
CNewsCenter::set_planet(const char *aPlanet)
{
	mStoreFlag += STORE_PLANET;
	mPlanetNews.clear();
	if (aPlanet) mPlanetNews.set_query(aPlanet);
}

inline void
CNewsCenter::set_project(const char *aProject)
{
	mStoreFlag += STORE_PROJECT;
	mPurchasedProjectNews.clear();
	if (aProject) mPurchasedProjectNews.set_query(aProject);
}

inline void
CNewsCenter::set_admiral(const char *aAdmiral)
{
	mStoreFlag += STORE_ADMIRAL;
	mAdmiralNews.clear();
	if (aAdmiral) mAdmiralNews.set_query(aAdmiral);
}

//-------------------------------------- CPlanetNewsCenter
/**
*/
class CPlanetNewsCenter
{
	private:
		CPlanet* mPlanet;

		enum
		{
			STORE_POPULATION = 0,
			STORE_FACTORY,
			STORE_MILITARY_BASE,
			STORE_RESEARCH_LAB,
			STORE_ATMOSPHERE,
			STORE_TEMPERATURE,
			STORE_RESOURCE,
			STORE_GRAVITY
		};

		CCommandSet mStoreFlag;

		int mPopulation;
		int mFactory;
		int mMilitaryBase;
		int mResearchLab;
		char mAtmosphere[8];
		int mTemperature;
		int mResource;
		double mGravity;

	public:
		CPlanetNewsCenter();
		~CPlanetNewsCenter();

		void init(CPlanet *aPlanet);

		inline void set_planet(CPlanet *aPlanet);
		const char *generate();

	public:
		inline bool empty();
		const char *get_query();

	public:
		int get_population() { return mPopulation; }
		inline void set_population(int aPopulation);

		int get_factory() { return mFactory; }
		inline void set_factory(int aFactory);

		int get_military_base() { return mMilitaryBase; }
		inline void set_military_base(int aMilitaryBase);

		int get_research_lab() { return mResearchLab; }
		inline void set_research_lab(int aResearchLab);
};

inline void
CPlanetNewsCenter::set_planet(CPlanet *aPlanet)
{
	mPlanet = aPlanet;
}

inline bool
CPlanetNewsCenter::empty()
{
	return mStoreFlag.is_empty();
}

inline void
CPlanetNewsCenter::set_population(int aPopulation)
{
	mStoreFlag += STORE_POPULATION;
	mPopulation = aPopulation;
}

inline void
CPlanetNewsCenter::set_factory(int aFactory)
{
	mStoreFlag += STORE_FACTORY;
	mFactory = aFactory;
}

inline void
CPlanetNewsCenter::set_military_base(int aMilitaryBase)
{
	mStoreFlag += STORE_MILITARY_BASE;
	mMilitaryBase = aMilitaryBase;
}

inline void
CPlanetNewsCenter::set_research_lab(int aResearchLab)
{
	mStoreFlag += STORE_RESEARCH_LAB;
	mResearchLab = aResearchLab;
}

#endif
