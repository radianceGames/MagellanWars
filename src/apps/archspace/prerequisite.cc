#include "prerequisite.h"
#include "player.h"
#include "council.h"
#include "archspace.h"
#include "game.h"
#include "race.h"
#include "util.h"
#include "ctrl.model.h"

bool
CPrerequisite::evaluate(CPlayer *aPlayer)
{
	int integer1;
	CFleetList* fleetList;
	CPlayerList* playerList;
	CDock* dock;
	CRepairBay* repairBay;
//	SLOG("evaluating requisite");
	switch(mRequisiteType)
	{
		case RT_RACE :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_race()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_race()!=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_PLANET :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_planet_list()->length()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_planet_list()->length()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_planet_list()->length()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_planet_list()->length()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_planet_list()->length()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_planet_list()->length()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_TECH :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->has_tech(mRequisiteArgument)) return true;
					return false;
				case RO_NOT:
					if (!aPlayer->has_tech(mRequisiteArgument)) return true;
					return false;
			}
			return false;
		case RT_PROJECT :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_purchased_project_list()->get_by_id(mRequisiteArgument)) return true;
					return false;
				case RO_NOT:
					if (!aPlayer->get_purchased_project_list()->get_by_id(mRequisiteArgument)) return true;
					return false;
			}
			return false;
		case RT_POWER :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_power()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_power()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_power()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_power()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_power()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_power()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_RANK :
			return false;
		case RT_CLUSTER :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->cluster_list().length()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->cluster_list().length()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->cluster_list().length()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->cluster_list().length()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->cluster_list().length()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->cluster_list().length()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_COMMANDER_LEVEL :
			return false;
		case RT_FLEET :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_fleet_list()->length()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_fleet_list()->length()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_fleet_list()->length()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_fleet_list()->length()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_fleet_list()->length()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_fleet_list()->length()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_COUNCIL_SIZE :
			integer1 = aPlayer->get_council()->get_number_of_members();
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (integer1==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (integer1!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (integer1>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (integer1<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (integer1>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (integer1<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_RP :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_research()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_research()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_research()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_research()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_research()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_research()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_EMPIRE_RELATION :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_empire_relation()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_empire_relation()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_empire_relation()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_empire_relation()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_empire_relation()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_empire_relation()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_COUNCIL_PROJECT :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_council()->get_purchased_project_list()->get_by_id(mRequisiteArgument)) return true;
					return false;
				case RO_NOT:
					if (!aPlayer->get_council()->get_purchased_project_list()->get_by_id(mRequisiteArgument)) return true;
					return false;
			}
			return false;
		case RT_HAS_SHIP :
			fleetList = aPlayer->get_fleet_list();
			for(int i=0 ; i<fleetList->length() ; i++)
			{
				CFleet*
					fleet = (CFleet*)fleetList->get(i);
				if( fleet->get_size() == mRequisiteArgument )
				{
					if(mRequisiteOperator == RO_EQUAL) return true;
				}
			}
			dock = aPlayer->get_dock();
			for(int i=0 ; i<dock->length() ; i++)
			{
				CDockedShip*
					dockedShip = (CDockedShip*)dock->get(i);
				if( dockedShip->get_size() == mRequisiteArgument )
				{
					if(mRequisiteOperator == RO_EQUAL) return true;
				}
			}
			repairBay = aPlayer->get_repair_bay();
			for(int i=0 ; i<repairBay->length() ; i++)
			{
				CDamagedShip*
					damagedShip = (CDamagedShip*)repairBay->get(i);
				if( damagedShip->get_size() == mRequisiteArgument )
				{
					if(mRequisiteOperator == RO_EQUAL) return true;
				}
			}
			if(mRequisiteOperator == RO_NOT) return true;
			return false;
		case RT_SHIP_POOL :
			integer1 = 0;
			dock = aPlayer->get_dock();
			for(int i=0 ; i<dock->length() ; i++)
			{
				CDockedShip*
					dockedShip = (CDockedShip*)dock->get(i);
				integer1 += dockedShip->get_number();
			}
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (integer1==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (integer1!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (integer1>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (integer1<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (integer1>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (integer1<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_POPULATION_INCREASE :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_last_turn_population_increased()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_last_turn_population_increased()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_last_turn_population_increased()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_last_turn_population_increased()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_last_turn_population_increased()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_last_turn_population_increased()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_POPULATION :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->calc_population()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->calc_population()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->calc_population()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->calc_population()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->calc_population()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->calc_population()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_CONCENTRATION_MODE :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_mode()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_mode()!=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_TECH_ALL :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (!aPlayer->get_available_tech_list()->length()) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_available_tech_list()->length()) return true;
					return false;
			}
			return false;
		case RT_TITLE :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_court_rank()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_court_rank()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_court_rank()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_court_rank()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_court_rank()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_court_rank()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case RT_COUNCIL_SPEAKER :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_council()->get_speaker_id()==aPlayer->get_game_id()) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_council()->get_speaker_id()!=aPlayer->get_game_id()) return true;
					return false;
			}
			return false;
		case RT_COUNCIL_WAR :
			return false;
		case RT_WAR_IN_COUNCIL :
			playerList = aPlayer->get_council()->get_members();
			for(int i=0 ; i<playerList->length() ; i++)
			{
				CPlayer*
					player = (CPlayer*)playerList->get(i);
				CPlayerRelation
					*Relation = aPlayer->get_relation(player);
				if( Relation && (Relation->get_relation() == CRelation::RELATION_WAR || Relation->get_relation() == CRelation::RELATION_TOTAL_WAR))
				{
					if(mRequisiteOperator == RO_NOT) return false;
				}
			}
			if(mRequisiteOperator == RO_NOT) return true;
			return false;
		case RT_HONOR :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_honor()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_honor()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_honor()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_honor()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_honor()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_honor()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_ENVIRONMENT :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_environment()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_environment()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_environment()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_environment()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_environment()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_environment()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_GROWTH :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_growth()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_growth()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_growth()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_growth()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_growth()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_growth()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_RESEARCH :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_research()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_research()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_research()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_research()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_research()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_research()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_PRODUCTION :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_production()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_production()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_production()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_production()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_production()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_production()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_MILITARY :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_military()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_military()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_military()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_military()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_military()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_military()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_SPY :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_spy()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_spy()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_spy()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_spy()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_spy()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_spy()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_COMMERCE :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_commerce()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_commerce()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_commerce()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_commerce()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_commerce()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_commerce()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_EFFICIENCY :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_efficiency()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_efficiency()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_efficiency()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_efficiency()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_efficiency()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_efficiency()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_GENIUS :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_genius()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_genius()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_genius()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_genius()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_genius()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_genius()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_DIPLOMACY :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_diplomacy()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_diplomacy()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_diplomacy()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_diplomacy()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_diplomacy()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_diplomacy()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
		case CM_FACILITY_COST :
			switch(mRequisiteOperator)
			{
				case RO_EQUAL:
					if (aPlayer->get_control_model()->get_facility_cost()==mRequisiteArgument) return true;
					return false;
				case RO_NOT:
					if (aPlayer->get_control_model()->get_facility_cost()!=mRequisiteArgument) return true;
					return false;
				case RO_GREATER:
					if (aPlayer->get_control_model()->get_facility_cost()>mRequisiteArgument) return true;
					return false;
				case RO_LESS:
					if (aPlayer->get_control_model()->get_facility_cost()<mRequisiteArgument) return true;
					return false;
				case RO_GREATER_EQUAL:
					if (aPlayer->get_control_model()->get_facility_cost()>=mRequisiteArgument) return true;
					return false;
				case RO_LESS_EQUAL:
					if (aPlayer->get_control_model()->get_facility_cost()<=mRequisiteArgument) return true;
					return false;
			}
			return false;
	}
	return false;
}

CPrerequisiteList::~CPrerequisiteList()
{
	clear();
}

void
CPrerequisiteList::add_prerequisite(int aRequisiteType, int aRequisiteOperator, int aRequisiteArgument) 
{
	CPrerequisite *aPrerequisite = new CPrerequisite;
	aPrerequisite->set(aRequisiteType, aRequisiteOperator, aRequisiteArgument);
	if ((aRequisiteType != CPrerequisite::RT_RACE)&&
		(aRequisiteType != CPrerequisite::RT_TECH)&&
		(aRequisiteType != CPrerequisite::RT_PROJECT)) set_secret(true);
	
//	assert(*aPrerequisite);
	push_back(*aPrerequisite);
}

bool
CPrerequisiteList::evaluate(CPlayer *aPlayer)
{	
	bool isRace = false;
	bool aRace = false;
	for(std::vector<CPrerequisite>::iterator current = begin(); 
		current < end(); current++)
	{
		if (current->get_type() == CPrerequisite::RT_RACE) 
		{
			if (!isRace) isRace = current->evaluate(aPlayer);
			aRace = true;
		}
		else if (!current->evaluate(aPlayer)) return false;
	}
	if (aRace && !isRace) return false;
	return true;
}

const char*
CPrerequisiteList::tech_description()
{
	char 
		*Path = ARCHSPACE->configuration().get_string(
					"Game", "EncyclopediaWebPath", NULL);
	
	if (!Path)
	{
		SLOG("could not find path of encyclopedia"); 
		return false;
	}
 
	CString
		PrerequisiteName;
  
	PrerequisiteName.clear();
	if (mSecret) 
	{
		PrerequisiteName = "UNKOWN";
		return (char *)PrerequisiteName;
	}
	bool comma = false;
	for(std::vector<CPrerequisite>::iterator current = begin(); 
		current < end(); current++)
	{
		if (current->get_type()==CPrerequisite::RT_TECH)
		{
			CTech *
				Tech = TECH_TABLE->get_by_id(current->get_argument());
			if (comma) PrerequisiteName += ", ";
			if (current->get_operator()==CPrerequisite::RO_EQUAL) {
				PrerequisiteName.format("<A HREF=\"%s/tech/%d.html\">%s</A>",
								Path,Tech->get_id(), Tech->get_name());
			}
			else
			{
				PrerequisiteName.format("<A HREF=\"%s/tech/%d.html\">Cannot have:%s</A>",
								Path, Tech->get_id(), Tech->get_name());
			}
			comma = true;
		}
	}
	if (!PrerequisiteName) return "NONE";
	return (char*)PrerequisiteName;
}

const char*
CPrerequisiteList::tech_description(CPlayer *aPlayer)
{
	char 
		*Path = ARCHSPACE->configuration().get_string(
					"Game", "EncyclopediaWebPath", NULL);
	
	if (!Path)
	{
		SLOG("could not find path of encyclopedia"); 
		return false;
	}

	CString
		PrerequisiteName;
	PrerequisiteName.clear();
	if (mSecret) 
	{
		PrerequisiteName = "UNKOWN";
		return (char *)PrerequisiteName;
	}
	bool comma = false;
	for(std::vector<CPrerequisite>::iterator current = begin(); 
		current < end(); current++)
	{
		if (current->get_type()==CPrerequisite::RT_TECH)
		{
			CTech *
				Tech = TECH_TABLE->get_by_id(current->get_argument());
			if (comma) PrerequisiteName += ", ";
			if (current->get_operator()==CPrerequisite::RO_EQUAL) {
				if (aPlayer->has_tech(current->get_argument()))
				{
					PrerequisiteName.format("<A HREF=\"%s/tech/%d.html\" style=\"color:#0000FF\">%s</A>",
								Path, Tech->get_id(), Tech->get_name());
				}
				else
				{
					PrerequisiteName.format("<A HREF=\"%s/tech/%d.html\" style=\"color:#FF0000\">%s</A>",
								Path, Tech->get_id(), Tech->get_name());
				}
			}
			else
			{
				if (aPlayer->has_tech(current->get_argument()))
				{
					PrerequisiteName.format("<A HREF=\"%s/tech/%d.html\" style=\"color:#0000FF\">Cannot have:%s</A>",
								Path, Tech->get_id(), Tech->get_name());
				}
				else
				{
					PrerequisiteName.format("<A HREF=\"%s/tech/%d.html\" style=\"color:#FF0000\">Cannot have:%s</A>",
								Path, Tech->get_id(), Tech->get_name());
				}
			}
			comma = true;
		}
	}
	if (!PrerequisiteName) PrerequisiteName = "NONE";
	return (char*)PrerequisiteName;
}

const char*
CPrerequisiteList::race_description()
{
	char 
		*Path = ARCHSPACE->configuration().get_string(
					"Game", "EncyclopediaWebPath", NULL);
	
	if (!Path)
	{
		SLOG("could not find path of encyclopedia"); 
		return false;
	}
 

	CString
		PrerequisiteName;
	
	PrerequisiteName.clear();
	if (mSecret) 
	{
		PrerequisiteName = "UNKOWN";
		return (char *)PrerequisiteName;
	}
	bool comma = false;
	for(std::vector<CPrerequisite>::iterator current = begin(); 
		current < end(); current++)
	{
		if (current->get_type()==CPrerequisite::RT_RACE)
		{
			CRace *
				Race = RACE_TABLE->get_by_id(current->get_argument());
			if (comma) PrerequisiteName += ", ";
			if (current->get_operator()==CPrerequisite::RO_EQUAL) {
				PrerequisiteName.format("<A HREF=\"%s/race/%d.html\">%s</A>",
								Path, Race->get_id(), Race->get_name());
			}
			else
			{
				PrerequisiteName.format("<A HREF=\"%s/race/%d.html\">Cannot be:%s</A>",
								Path, Race->get_id(), Race->get_name());
			}
			comma = true;
		}
	}
	if (!PrerequisiteName) PrerequisiteName = "ALL RACES";
	return (char*)PrerequisiteName;
}

const char*
CPrerequisiteList::project_description()
{
	char 
		*Path = ARCHSPACE->configuration().get_string(
					"Game", "EncyclopediaWebPath", NULL);
	
	if (!Path)
	{
		SLOG("could not find path of encyclopedia"); 
		return false;
	}

	CString
		PrerequisiteName;
	
	PrerequisiteName.clear();
	if (mSecret) 
	{
		PrerequisiteName = "UNKOWN";
		return (char *)PrerequisiteName;
	}
	bool comma = false;
	for(std::vector<CPrerequisite>::iterator current = begin(); 
		current < end(); current++)
	{
		if (current->get_type()==CPrerequisite::RT_PROJECT)
		{
			CProject *
				Project = PROJECT_TABLE->get_by_id(current->get_argument());
			if (comma) PrerequisiteName += ", ";
			if (current->get_operator()==CPrerequisite::RO_EQUAL) {
				PrerequisiteName.format("<A HREF=\"%s/project/%d.html\">%s</A>",
								Path, Project->get_id(), Project->get_name());
			}
			else
			{
				PrerequisiteName.format("<A HREF=\"%s/project/%d.html\">Cannot have:%s</A>",
								Path, Project->get_id(), Project->get_name());
			}
			comma = true;
		}
	}
	if (!PrerequisiteName) PrerequisiteName = "NONE";
	return (char*)PrerequisiteName;
}

void
PrerequisiteLoader::load_prerequisites(TSomething aSomething, CPrerequisiteList *aList)
{
	TSomething
		aSection;
	int i = 0;
	aList->set_secret(false);
//	SLOG("getting requisites...");
	while ((aSection = get_array(i++, aSomething)) != NULL)
	{	
//		SLOG("fetching requisite");
		if (!strcasecmp("Control Model", get_name(aSection)))
		{
			TSomething aCM, aOperator;
			if ((aCM = get_section("Growth", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_GROWTH, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("Research", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_RESEARCH, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("Production", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_PRODUCTION, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("Military", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_MILITARY, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("Spy", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_SPY, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("Commerce", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_COMMERCE, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("Efficiency", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_EFFICIENCY, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("Genius", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_GENIUS, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("Diplomacy", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_DIPLOMACY, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			if ((aCM = get_section("FacilityCost", aSection)) != NULL)
			{
				aOperator = get_array(0, aCM);
				aList->add_prerequisite(CM_FACILITY_COST, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
		}
		else if (!strcasecmp("Planet", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_PLANET, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_PLANET, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
			
		} 
		else if (!strcasecmp("Power", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			aList->add_prerequisite(RT_POWER, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
		} 
		else if (!strcasecmp("Rank", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			aList->add_prerequisite(RT_RANK, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
		} 
		else if (!strcasecmp("Cluster", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_CLUSTER, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_CLUSTER, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
			
		}
		else if (!strcasecmp("CommanderLevel", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			aList->add_prerequisite(RT_COMMANDER_LEVEL, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
		} 
		else if (!strcasecmp("Fleet", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_FLEET, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_FLEET, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
		}
		else if (!strcasecmp("CouncilSize", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_COUNCIL_SIZE, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_COUNCIL_SIZE, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
		}
		else if (!strcasecmp("RP", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_RP, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_RP, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
		}
		else if (!strcasecmp("EmpireRelation", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_EMPIRE_RELATION, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_EMPIRE_RELATION, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
		}
		else if (!strcasecmp("HasShip", get_name(aSection)))
		{
			aList->add_prerequisite(RT_HAS_SHIP, RO_EQUAL, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("ShipPool", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_SHIP_POOL, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_SHIP_POOL, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
		}
		else if (!strcasecmp("PopulationIncrease", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_POPULATION_INCREASE, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_POPULATION_INCREASE, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
		}
		else if (!strcasecmp("Population", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			aList->add_prerequisite(RT_POPULATION, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
		}
		else if (!strcasecmp("ConcentrationMode", get_name(aSection)))
		{
			aList->add_prerequisite(RT_CONCENTRATION_MODE, RO_EQUAL, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("CouncilWar", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			aList->add_prerequisite(RT_COUNCIL_WAR, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
		}
		else if (!strcasecmp("Honor", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			aList->add_prerequisite(RT_HONOR, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
		}
		else if (!strcasecmp("Title", get_name(aSection)))
		{
			TSomething aOperator;
			aOperator = get_array(0, aSection);
			if(aOperator)
			{
				aList->add_prerequisite(RT_TITLE, get_operator(get_name(aOperator)), as_atoi(get_data(aOperator)));
			}
			else
			{
				aList->add_prerequisite(RT_TITLE, RO_GREATER_EQUAL, as_atoi(get_data(aSection)));
			}
		}
		else if (!strcasecmp("Race", get_name(aSection)))
		{
			if (strcasecmp("Psi",get_data(aSection)))
			{
				aList->add_prerequisite(RT_RACE, RO_EQUAL, as_atoi(get_data(aSection)));
			}
			else
			{
				CRace *ARace;
				for(int index = 0; index< RACE_TABLE->length(); index++)
				{
					ARace = (CRace *)RACE_TABLE->get(index);
					if (ARace->has_ability(ABILITY_PSI))
					{
						aList->add_prerequisite(RT_RACE, RO_EQUAL, ARace->get_id());
					}
				}
			}
		}
		else if (!strcasecmp("NotRace", get_name(aSection)))
		{
			if (strcasecmp("Psi",get_data(aSection)))
			{
				aList->add_prerequisite(RT_RACE, RO_NOT, as_atoi(get_data(aSection)));
			}
			else
			{
				CRace *ARace;
				for(int index = 0; index< RACE_TABLE->length(); index++)
				{
					ARace = (CRace *)RACE_TABLE->get(index);
					if (ARace->has_ability(ABILITY_PSI))
					{
						aList->add_prerequisite(RT_RACE, RO_NOT, ARace->get_id());
					}
				}
			}
		}
		else if (!strcasecmp("Tech", get_name(aSection)))
		{
			aList->add_prerequisite(RT_TECH, RO_EQUAL, as_atoi(get_data(aSection)));	
		}
		else if (!strcasecmp("NotTech", get_name(aSection)))
		{
			aList->add_prerequisite(RT_TECH, RO_NOT, as_atoi(get_data(aSection)));	
		}
		else if (!strcasecmp("Project", get_name(aSection)))
		{
			aList->add_prerequisite(RT_PROJECT, RO_EQUAL, as_atoi(get_data(aSection)));	
		}
		else if (!strcasecmp("NotProject", get_name(aSection)))
		{
			aList->add_prerequisite(RT_PROJECT, RO_NOT, as_atoi(get_data(aSection)));	
		}
		else if (!strcasecmp("TechAll", get_name(aSection)))
		{
			aList->add_prerequisite(RT_TECH_ALL, RO_EQUAL, as_atoi(get_data(aSection)));	
		}
		else if (!strcasecmp("NotTechAll", get_name(aSection)))
		{
			aList->add_prerequisite(RT_TECH_ALL, RO_NOT, as_atoi(get_data(aSection)));	
		}
		else if (!strcasecmp("CouncilProject", get_name(aSection)))
		{
			aList->add_prerequisite(RT_COUNCIL_PROJECT, RO_EQUAL, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("NotCouncilProject", get_name(aSection)))
		{
			aList->add_prerequisite(RT_COUNCIL_PROJECT, RO_NOT, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("CouncilSpeaker", get_name(aSection)))
		{
			aList->add_prerequisite(RT_COUNCIL_SPEAKER, RO_EQUAL, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("NotCouncilSpeaker", get_name(aSection)))
		{
			aList->add_prerequisite(RT_COUNCIL_SPEAKER, RO_NOT, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("WarInCouncil", get_name(aSection)))
		{
			aList->add_prerequisite(RT_WAR_IN_COUNCIL, RO_EQUAL, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("NoWarInCouncil", get_name(aSection)))
		{
			aList->add_prerequisite(RT_WAR_IN_COUNCIL, RO_NOT, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("PopulationIncreaseLess", get_name(aSection)))
		{
			aList->add_prerequisite(RT_POPULATION_INCREASE, RO_LESS_EQUAL, as_atoi(get_data(aSection)));
		}
		else if (!strcasecmp("Society", get_name(aSection)))
		{
			CRace *ARace;
				for(int index = 0; index< RACE_TABLE->length(); index++)
				{
					ARace = (CRace *)RACE_TABLE->get(index);
					if (!strcasecmp(ARace->get_society_name(),get_data(aSection)))
					{
						aList->add_prerequisite(RT_RACE, RO_EQUAL, ARace->get_id());
					}
				}
		}
		else if (!strcasecmp("NotSociety", get_name(aSection)))
		{
			CRace *ARace;
				for(int index = 0; index< RACE_TABLE->length(); index++)
				{
					ARace = (CRace *)RACE_TABLE->get(index);
					if (!strcasecmp(ARace->get_society_name(),get_data(aSection)))
					{
						aList->add_prerequisite(RT_RACE, RO_NOT, ARace->get_id());
					}
				}
		}
	}
}

int 
PrerequisiteLoader::get_operator(char *aOperator)
{
//	SLOG("looking up requisite operator");
	if (!strcasecmp(aOperator,"greater")) return RO_GREATER;
	if (!strcasecmp(aOperator,"less")) return RO_LESS;
	if (!strcasecmp(aOperator,"equal")) return RO_EQUAL;
	if (!strcasecmp(aOperator,"greaterorequal")) return RO_GREATER_EQUAL;
	if (!strcasecmp(aOperator,"lessorequal")) return RO_LESS_EQUAL;
	if (!strcasecmp(aOperator,"not")) return RO_NOT;
	return -1;
}
