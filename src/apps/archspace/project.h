#if !defined(__ARCHSPACE_PROJECT_H__)
#define __ARCHSPACE_PROJECT_H__

#include "effect.h"
#include "ctrl.model.h"
#include "prerequisite.h"

enum EProjectType
{
	TYPE_BEGIN = 0,
	TYPE_PLANET = 0,
	TYPE_FIXED,
	TYPE_COUNCIL,
	TYPE_ENDING,
	TYPE_SECRET,
	TYPE_BM,
	TYPE_MAX
};

extern TZone gProjectZone;

//--------------------------------------------- CProject
/**
*/
class CProject : public CPrerequisiteList
{
	public:
		enum EProjectType
		{
			TYPE_BEGIN = 0,
			TYPE_PLANET = 0,
			TYPE_FIXED,
			TYPE_COUNCIL,
			TYPE_ENDING,
			TYPE_SECRET,
			TYPE_BM,
			TYPE_MAX
		};

	private:
		int
			mID;
		CString
			mName,
			mDescription;
		CControlModel
			mEffect;
		int
			mCost;
		EProjectType
			mType;
		CPlayerEffectList
			mEffectList;

	public:
		CProject();
		~CProject();

	public:		// get
		inline int get_id();
		inline char *get_name();
		inline char *get_description();
		inline const CControlModel& get_effect();
		int get_cost() { return mCost; }
		inline int get_type();
		char *get_type_name();
		static char *get_type_name(int aProjectType);

		CPlayerEffectList *get_effect_list() { return &mEffectList; }

	public:		// set
		inline void set_id(int aID);
		inline void set_name(char *aName);
		inline void set_description(char *aDescription);
		bool set_effect(char *aEffectName, int aValue);
		inline void set_cost(int aCost);
		inline void set_type(int aType);
		bool set_type(char *aType);

		bool add_effect(CPlayerEffect *aEffect);

	public:		// etc
		bool has_effect(int aEffect);
};

inline int
CProject::get_id() 
{ 
	return mID; 
}

inline char *
CProject::get_name() 
{ 
	return (char*)mName; 
}

inline char *
CProject::get_description() 
{ 
	return (char*)mDescription; 
}

inline const CControlModel &
CProject::get_effect() 
{ 
	return mEffect; 
}

inline int
CProject::get_type() 
{ 
	return (int)mType; 
}

inline void
CProject::set_id(int aID) 
{ 
	mID = aID; 
}

inline void
CProject::set_name(char *aName) 
{ 
	mName = aName; 
}

inline void
CProject::set_description(char *aDescription) 
{ 
	mDescription = aDescription; 
}

inline void
CProject::set_cost(int aCost) 
{ 
	mCost = aCost; 
}

inline void
CProject::set_type(int aType)
{
	mType = (EProjectType)aType;
}

//------------------------------------------ CProjectList
/**
*/
class CProjectList: public CSortedList
{
	public:
		CProjectList();
		virtual ~CProjectList();

		int add_project(CProject* Project);
		bool remove_project(int aProjectID);

		CProject* get_by_id(int aProjectID);

		//depreciated:
		char *selected_project_information_html(CCommandSet &aProjectSet);
		char *selected_project_effect_html(CPlayer *aPlayer, CCommandSet &aProjectSet);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "project list"; }
};

//------------------------------------------ CProjectTable
/**
*/
class CProjectTable: public CSortedList
{
	public:
		CProjectTable();
		virtual ~CProjectTable();

		int add_project(CProject *Project);
		bool remove_project(int aProjectID);

		CProject* get_by_id(int aProjectID);

		char *selected_project_information_html(CCommandSet &aProjectSet);
		char *selected_project_effect_html(CPlayer *aPlayer, CCommandSet &aProjectSet);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "project table"; }
};

//--------------------------------------------  CPurchasedProject
extern TZone gPurchasedProjectZone;
/**
*/
class CPurchasedProject: public CStore
{
	private :
		CProject
			*mProject;
		int
			mOwner;
	public:
		CPurchasedProject();
		CPurchasedProject(int aOwner, int aProjectID);
		CPurchasedProject(CProject *aProject);
		virtual ~CPurchasedProject();

		bool initialize(int aOwner, int aProjectID, int aType);

		virtual const char *table() { return "project"; }
		virtual CString& query();

	public: // get
		int get_id() { return mProject->get_id(); }
		const char *get_name() { return mProject->get_name(); }
		int get_owner() { return mOwner; }
		int get_cost() { return mProject->get_cost(); }
		int get_type() { return mProject->get_type(); }
		inline const CControlModel &get_effect();
		inline CPlayerEffectList *get_effect_list();
		inline char *get_description();	//telecard 2001/04/02

	public: // set
		inline void set_owner(int aOwner);

	public: // etc
		const char *html();
		friend CStoreCenter &operator<<(CStoreCenter &aStoreCenter,
										CPurchasedProject &aPurchasedProejct);
	RECYCLE(gPurchasedProjectZone);						
};

inline void 
CPurchasedProject::set_owner(int aOwner) 
{ 
	mOwner = aOwner; 
}

inline const CControlModel&
CPurchasedProject::get_effect()
{
	return mProject->get_effect();
}

inline CPlayerEffectList*
CPurchasedProject::get_effect_list()
{
	return mProject->get_effect_list();
}

//start telecard 2001/04/02
inline char *
CPurchasedProject::get_description() 
{ 
	return mProject->get_description(); 
}
//end telecard 2001/04/02

//----------------------------------------  CPurchasedProjectList
/**
*/
class CPurchasedProjectList: public CSortedList
{
	public:
		CPurchasedProjectList();
		virtual ~CPurchasedProjectList();

		int add_project(CPurchasedProject* aProject);
		bool remove_project(int aProjectID);

		CPurchasedProject* get_by_id(int aProjectID);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "purchased project table"; }
};

#endif
