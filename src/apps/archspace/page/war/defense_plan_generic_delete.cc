#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageDefensePlanGenericDelete::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	CDefensePlanList *
		DefensePlanList = aPlayer->get_defense_plan_list();
	CDefensePlan *
		DefensePlan = DefensePlanList->get_generic_plan();

	if (!DefensePlan)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't have generic defense plan now."));

		return output("war/defense_plan_error.html");
	}

	CDefenseFleetList *
		DefenseFleetList = DefensePlan->get_fleet_list();

	int
		InitFleetNumber = DefenseFleetList->length();

	for (int i=InitFleetNumber-1 ; i>=0 ; i--)
	{
		CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)DefenseFleetList->get(i);

		DefenseFleet->type(QUERY_DELETE);
		STORE_CENTER->store(*DefenseFleet);

		DefenseFleetList->remove_defense_fleet(DefenseFleet->get_fleet_id());
	}

	DefensePlan->type(QUERY_DELETE);
	STORE_CENTER->store(*DefensePlan);

	DefensePlanList->remove_defense_plan(DefensePlan->get_id());

	ITEM("RESULT_MESSAGE",
			GETTEXT("Now your generic defense plan is deleted."));

//	system_log( "end page handler %s", get_name() );

	return output( "war/defense_plan_generic_result.html" );
}

