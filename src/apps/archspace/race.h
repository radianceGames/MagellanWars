#if !defined(__ARCHSPACE_RACE_H__)
#define __ARCHSPACE_RACE_H__

#include "ctrl.model.h"
#include "planet.h"

extern TZone gRaceZone;
extern TZone gRaceTableZone;

//--------------------------------------------------- CRace
// ---Change--- //
#define MAX_RACE 10
// ---End Change--- //
/**
*/
class CRace: public CBase
{
	class CInnateTechList: public CList
	{
		public :
			virtual ~CInnateTechList();
		protected :
			virtual bool free_item(TSomething aItem);
			virtual const char *debug_info() const { return "race innate tech"; }
	};

	public:
		enum ESociety 
		{
			SOCIETY_BEGIN = 0,
			SOCIETY_TOTALISM = 0,
			SOCIETY_CLASSISM,
			SOCIETY_PERSONALISM,
			// ---Change--- //
			SOCIETY_UNKNOWN,
			// ---End Change--- //
			SOCIETY_MAX
		};

		enum ERaceIndex
		{
			RACE_BEGIN = 1,
			RACE_HUMAN = 1,
			RACE_TARGOID,
			RACE_BUCKANEER,
			RACE_TECANOID,
			RACE_EVINTOS,
			RACE_AGERUS,
			RACE_BOSALIAN,
			RACE_XELOSS,
			RACE_XERUSIAN,
			RACE_XESPERADOS,
			// ---Change--- //
			RACE_TRABOTULIN,
			// ---End Change--- //
			RACE_MAX
		};

	private:
		int mID;
		int mSociety;
		int mInitialEmpireRelation;
		int
			mSurvivalModifier,
			mMoraleModifier,
			mBerserkModifier;

		CString mName;
		CString mDescription;
		CControlModel mControlModel;
		CCommandSet mAbility;
		CCommandSet mAttribute;
		CInnateTechList mInnateTechList;

		char mAtmosphere[CPlanet::GAS_MAX];
		int mTemperature;
		double mGravity;

	public:
		CRace();
		virtual ~CRace();

	public: // get
		inline int get_id();
		inline int get_society();

		const char *get_society_name();
		char *get_name() { return (char *)mName; }

		inline const char *get_description();

		inline const CControlModel &get_control_model();
		inline const CCommandSet &get_ability();

		inline int get_innate_tech_count();
		inline int get_innate_tech(int aIndex);

		const char *get_atmosphere();
		inline int get_temperature();
		inline double get_gravity();
		
		inline int get_morale_modifier();
		inline int get_berserk_modifier();
		inline int get_survival_modifier();

	public: // set
		inline void set_id(int aRaceID);
		bool set_society_by_name(char *aSociety);
		void set_society(int aSociety);
		inline void set_name(char *aName);
		inline void set_description(char *aDesc);
		bool set_control(int aType, int aAmount);
		void set_ability(int aAbility);
		bool set_ability(char *aAbility);
		inline void set_innate_tech(int aTechID);
		inline void set_control_model(CControlModel &Model);

		bool set_atmosphere(const char *aAtmosphere);
		bool set_temperature(int aTemperature);
		bool set_gravity(double aGravity);

		inline void set_morale_modifier( int aMorale );
		inline void set_berserk_modifier( int aBerserk );
		inline void set_survival_modifier( int aSurvival );

		inline void set_empire_relation( int aRelation );
		inline int get_empire_relation();

	public: // etc
		inline bool has_attribute(int aAttribute);
		inline bool has_ability(int aAbility);

		const char *html_print_row();

		char *race_control_model_information_html();

	RECYCLE(gRaceZone);
};


inline int 
CRace::get_id() 
{ 
	return mID; 
}

inline int 
CRace::get_society() 
{ 
	return mSociety;
}

inline const char *
CRace::get_description() 
{ 
	return (char*)mDescription; 
}

inline const CControlModel&
CRace::get_control_model() 
{ 
	return mControlModel; 
}

inline const CCommandSet&
CRace::get_ability() 
{ 
	return mAbility; 
}

inline int 
CRace::get_innate_tech_count() 
{ 
	return mInnateTechList.length(); 
}

inline int 
CRace::get_innate_tech(int aIndex)
{ 
	return (int)mInnateTechList.get(aIndex); 
}

inline bool 
CRace::has_attribute(int aAttribute) 
{ 
	return mAttribute.has(aAttribute); 
}

inline bool 
CRace::has_ability(int aAbility) 
{ 
	return mAbility.has(aAbility); 
}

inline void 
CRace::set_id(int aRaceID) 
{ 
	mID = aRaceID; 
}

inline void 
CRace::set_name(char *aName) 
{ 
	mName = aName; 
}

inline void 
CRace::set_description(char *aDesc) 
{ 
	mDescription = aDesc; 
}
	
inline void 
CRace::set_innate_tech(int aTechID) 
{ 
	mInnateTechList.push((void *)aTechID); 
}

inline void 
CRace::set_control_model(CControlModel &Model)
{ 
	mControlModel = Model; 
}

inline int 
CRace::get_temperature()
{
	return mTemperature;
}

inline double 
CRace::get_gravity()
{
	return mGravity;
}

inline int
CRace::get_morale_modifier()
{
	return mMoraleModifier;
}

inline int
CRace::get_berserk_modifier()
{
	return mBerserkModifier;
}

inline int
CRace::get_survival_modifier()
{
	return mSurvivalModifier;
}

inline void
CRace::set_morale_modifier( int aMorale )
{
	mMoraleModifier = aMorale;
}

inline void
CRace::set_berserk_modifier( int aBerserk )
{
	mBerserkModifier = aBerserk;
}

inline void
CRace::set_survival_modifier( int aSurvival )
{
	mSurvivalModifier = aSurvival;
}

inline void
CRace::set_empire_relation( int aRelation )
{
	mInitialEmpireRelation = aRelation;
}

inline int
CRace::get_empire_relation()
{
	return mInitialEmpireRelation;
}

//--------------------------------------------------- CRaceTable
/**
*/
class CRaceTable: public CSortedList
{
	public:
		CRaceTable();
		virtual ~CRaceTable();

		int add_race(CRace *aRace);
		bool remove_race(int aRaceID);

		CRace* get_by_id(int aRaceID);
		const char *get_name_by_id(int aRaceID);
		int get_id_by_name(char *aName);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "race table"; }

	RECYCLE(gRaceTableZone);
};

#endif
