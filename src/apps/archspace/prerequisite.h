#if !defined(__ARCHSPACE_PREREQUISITE_H__)
#define __ARCHSPACE_PREREQUISITE_H__

#include "util.h"
#include <vector>
class CPlayer;

class CPrerequisite
{
	public:
		enum ERequisiteType
		{
			RT_RACE = 0,
			RT_PLANET,
			RT_TECH,
			RT_PROJECT,
			RT_POWER,
			RT_RANK,
			RT_CLUSTER,
			RT_COMMANDER_LEVEL,
			RT_FLEET,
			RT_COUNCIL_SIZE,
			RT_RP,
			RT_EMPIRE_RELATION,
			RT_COUNCIL_PROJECT,
			RT_HAS_SHIP,
			RT_SHIP_POOL,
			RT_POPULATION_INCREASE,
			RT_POPULATION,
			RT_CONCENTRATION_MODE,
			RT_TECH_ALL,
			RT_TITLE,
			RT_COUNCIL_SPEAKER,
			RT_COUNCIL_WAR,
			RT_WAR_IN_COUNCIL,
			RT_HONOR,
			
			CM_ENVIRONMENT,
			CM_GROWTH,
			CM_RESEARCH,
			CM_PRODUCTION,
			CM_MILITARY,
			CM_SPY,
			CM_COMMERCE,
			CM_EFFICIENCY,
			CM_GENIUS,
			CM_DIPLOMACY,
			CM_FACILITY_COST
		};
		
		enum ERequisiteOperator
		{
			RO_GREATER = 0,
			RO_LESS,
			RO_EQUAL,
			RO_GREATER_EQUAL,
			RO_LESS_EQUAL,
			RO_NOT
		};

	private:
		int mRequisiteType;
		int mRequisiteOperator;
		int mRequisiteArgument;
	
	public:
		inline void set(int aRequisiteType, int aRequisiteOperator, int aRequisiteArgument);
		bool evaluate(CPlayer *aPlayer);
		inline int get_type() {return mRequisiteType;}
		inline int get_operator() {return mRequisiteOperator;}
		inline int get_argument() {return mRequisiteArgument;}
};

inline void
CPrerequisite::set(int aRequisiteType, int aRequisiteOperator, int aRequisiteArgument)
{
	mRequisiteType = aRequisiteType;
	mRequisiteOperator = aRequisiteOperator;
	mRequisiteArgument = aRequisiteArgument;
}

class CPrerequisiteList : public std::vector<CPrerequisite>
{
	private:
		bool mSecret;
	public:
		~CPrerequisiteList();
	public:
		void add_prerequisite(int aRequisiteType, int aRequisiteOperator, int aRequisiteArgument);
		bool evaluate(CPlayer *aPlayer);
		const char *tech_description();
		const char *tech_description(CPlayer *aPlayer);
		const char *race_description();
		const char *project_description();
		inline bool is_secret();
		inline void set_secret(bool aBool);
};

inline bool
CPrerequisiteList::is_secret()
{
	return mSecret;
}

inline void
CPrerequisiteList::set_secret(bool aBool)
{
	mSecret = aBool;
}

class PrerequisiteLoader: public CScript
{
	private:
		enum ERequisiteOperator
		{
			RO_GREATER = 0,
			RO_LESS,
			RO_EQUAL,
			RO_GREATER_EQUAL,
			RO_LESS_EQUAL,
			RO_NOT
		};
		
		enum ERequisiteType
		{
			RT_RACE = 0,
			RT_PLANET,
			RT_TECH,
			RT_PROJECT,
			RT_POWER,
			RT_RANK,
			RT_CLUSTER,
			RT_COMMANDER_LEVEL,
			RT_FLEET,
			RT_COUNCIL_SIZE,
			RT_RP,
			RT_EMPIRE_RELATION,
			RT_COUNCIL_PROJECT,
			RT_HAS_SHIP,
			RT_SHIP_POOL,
			RT_POPULATION_INCREASE,
			RT_POPULATION,
			RT_CONCENTRATION_MODE,
			RT_TECH_ALL,
			RT_TITLE,
			RT_COUNCIL_SPEAKER,
			RT_COUNCIL_WAR,
			RT_WAR_IN_COUNCIL,
			RT_HONOR,
			
			CM_ENVIRONMENT,
			CM_GROWTH,
			CM_RESEARCH,
			CM_PRODUCTION,
			CM_MILITARY,
			CM_SPY,
			CM_COMMERCE,
			CM_EFFICIENCY,
			CM_GENIUS,
			CM_DIPLOMACY,
			CM_FACILITY_COST
		};
		
		int get_operator(char *aOperator);
	public:
		void load_prerequisites(TSomething aSomething, CPrerequisiteList *aList);
};

#endif
