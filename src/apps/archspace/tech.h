#if !defined(__ARCHSPACE_TECH_H__)
#define __ARCHSPACE_TECH_H__

#include "common.h"
#include "util.h"
#include "ctrl.model.h"
#include "store.h"
#include "prerequisite.h"
extern TZone gKnownTechListZone;

//---------------------------------------------------- CTech
/**
*/
class CTech: public CPrerequisiteList
{
	private:
		CString
			mName,
			mDescription;

		int
			mID;
		int
			mType,
			mAttribute,
			mLevel;

		CControlModel
			mEffect;

		int
			mProject,
			mSpy;

		CIntegerList
			mComponentList;

	public:
		enum ETechType
		{
			BEGIN = 0,
			TYPE_INFORMATION = 0,
			TYPE_LIFE,
			TYPE_MATTER_ENERGY,
			TYPE_SOCIAL,
			TYPE_MAX
		};

		enum ETechAttribute
		{
			ATTR_BASIC,
			ATTR_NORMAL,
			ATTR_INNATE,
			ATTR_MAX
		};

	public:
		CTech();
		~CTech();

	public: // get
		inline const char *get_name();
		char *get_name_with_level();
		inline const char *get_description();
		inline int get_id();
		inline int get_level();
		inline int get_type();
		char *get_type_description();
		static char *get_type_description(int aType);
		inline int get_attribute() { return mAttribute; }
		inline const CControlModel &get_effect();
		bool set_effect(char *aEffectName, int aValue);
		inline int get_project();
		inline int get_spy() { return mSpy; }
		inline CIntegerList &get_component_list();

	public: // set
		inline void set_id(int aID);
		inline void set_name(char *aName);
		inline void set_description(char *aDescription);
		void set_type(char *aTypeName);
		inline void set_level(int aLevel);
		void set_attribute(char *aAttribute);
		inline void set_project(int aProjectID);
		inline void set_spy(int aSpyID) { mSpy = aSpyID; }
		inline void add_component( int aComponentID );

	public: // etc
		inline bool is_attribute(ETechAttribute aAttribute);
		CTech* get_prereq(CTech* top, CPlayer* aPlayer);
		// something should be in the argument
		void load();

		const char *html();
};

inline const char*
CTech::get_name()
{
	return (char *)mName;
}

inline const char*
CTech::get_description()
{
	return (char *)mDescription;
}

inline int
CTech::get_id()
{
	return mID;
}

inline int
CTech::get_level()
{
	return mLevel;
}

inline int
CTech::get_type()
{
	return mType;
}

inline int
CTech::get_project()
{
	return mProject;
}

inline CIntegerList&
CTech::get_component_list()
{
	return mComponentList;
}

inline void
CTech::set_id(int aID)
{
	mID = aID;
}

inline void
CTech::set_name(char *aName)
{
	mName = aName;
}

inline void
CTech::set_description(char *aDescription)
{
	mDescription = aDescription;
}

inline void
CTech::set_level(int aLevel)
{
	mLevel = aLevel;
}

inline const CControlModel&
CTech::get_effect()
{
	return mEffect;
}

inline bool
CTech::is_attribute(ETechAttribute aAttribute)
{
	return aAttribute == mAttribute;
}

inline void
CTech::set_project(int aProjectID)
{
	mProject = aProjectID;
}

inline void
CTech::add_component(int aComponentID)
{
	mComponentList.insert_sorted((void*)aComponentID);
}

//----------------------------------------------- CTechList
/**
*/
class CTechList: public CSortedList
{
	public:
		CTechList();
		virtual ~CTechList();

		int add_tech(CTech *aTech);
		bool remove_tech(int aTechID);

		CTech *get_by_id(int aTechID);

		CTech *get_by_level( int aLevel );
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

//----------------------------------------------- CTechTable
/**
*/
class CTechTable: public CSortedList
{
	public:
		CTechTable();
		virtual ~CTechTable();

		int add_tech(CTech *aTech);
		bool remove_tech(int aTechID);

		CTech* get_by_id(int aTechID);
		CTech* get_by_name(const char* name);
		// start telecard 2001/02/16
		CTechList* get_by_level(int aLevel);
		// end telecard 2001/02/16
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "tech table"; }
};

//----------------------------------------------- CKnownTech

extern TZone gKnownTechZone;
/**
*/
class CKnownTech
{
	private :
		CTech
			*mTech;
		int
			mOwner,
			mAttribute;

	public:
		CKnownTech();
		CKnownTech(int aOwner, int aTechID, int aAttribute);
		CKnownTech(CTech *aTech);
		~CKnownTech();

		bool initialize(int aOwner, int aTechID, int aAttr);

	public: // get
		inline int get_id();
		inline const char *get_name();
		char *get_name_with_level();
		inline int get_level();
		inline int get_owner();
		inline int get_type();
		inline int get_attribute();
		inline int get_project();
		inline CIntegerList& get_component_list();
		inline const CControlModel& get_effect();
		inline CTech* get_tech() { return mTech; }

	public: // set
		inline void set_owner(int aOwner);
		inline void set_attribute(int aAttribute);

	public: // etc
		int get_power();

		const char *html();

	RECYCLE(gKnownTechZone);
};

inline int
CKnownTech::get_id()
{
	return mTech->get_id();
}

inline const char *
CKnownTech::get_name()
{
	return mTech->get_name();
}

inline int
CKnownTech::get_level()
{
	return mTech->get_level();
}

inline int
CKnownTech::get_owner()
{
	return mOwner;
}

inline int
CKnownTech::get_type()
{
	return mTech->get_type();
}

inline int
CKnownTech::get_attribute()
{
	return mAttribute;
}

inline int
CKnownTech::get_project()
{
	return mTech->get_project();
}

inline CIntegerList&
CKnownTech::get_component_list()
{
	return mTech->get_component_list();
}

inline const CControlModel&
CKnownTech::get_effect()
{
	return mTech->get_effect();
}

inline void
CKnownTech::set_owner(int aOwner)
{
	mOwner = aOwner;
}

inline void
CKnownTech::set_attribute(int aAttribute)
{
	mAttribute = aAttribute;
}

//--------------------------------------------------- CKnownTechList
/**
*/
class CKnownTechList: public CSortedList, public CStore
{
	public:
		enum EStoreFlag
		{
			STORE_INFO,
			STORE_LIFE,
			STORE_MATTER,
			STORE_SOCIAL
		};

	public:
		CKnownTechList();
		virtual ~CKnownTechList();

		bool initialize(MYSQL_ROW aRow);
		inline void set_owner(int aOwner) { mOwner = aOwner; }

		int add_known_tech(CKnownTech *aTech);
		bool remove_known_tech(int aTechID);

		int count_by_category( int aType );

		CKnownTech* get_by_id(int aTechID);

		inline int get_power() { return mPower; }
		bool refresh_power();

		virtual const char *table() { return "tech"; }
		virtual CString &query();

		inline CCommandSet& get_information() { return mInfo; }
		inline CCommandSet& get_life() { return mLife; }
		inline CCommandSet& get_matter_energy() { return mMatter; }
		inline CCommandSet& get_social() { return mSocial; }
	private:
		int
			mOwner,
			mPower;

		CCommandSet
			mInfo,
			mLife,
			mMatter,
			mSocial;

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "known tech list"; }

	RECYCLE(gKnownTechListZone);
};

#endif
