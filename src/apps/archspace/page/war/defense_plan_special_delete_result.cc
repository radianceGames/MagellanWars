#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageDefensePlanSpecialDeleteResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("PLAN_ID", PlanIDString);
	int
		PlanID = as_atoi(PlanIDString);

	CDefensePlanList *
		DefensePlanList = aPlayer->get_defense_plan_list();
	CDefensePlan *
		DefensePlan = DefensePlanList->get_by_id(PlanID);

	if (!DefensePlan)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have a special plan with ID %1$s."),
								dec2unit(PlanID)));
		return output("war/defense_plan_special_error.html");
	}

	CDefenseFleetList *
		DefenseFleetList = DefensePlan->get_fleet_list();
	int
		FleetNumber = DefenseFleetList->length();

	for (int i=FleetNumber-1 ; i>=0 ; i--)
	{
		CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)DefenseFleetList->get(i);

		DefenseFleet->type(QUERY_DELETE);
		STORE_CENTER->store(*DefenseFleet);

		DefenseFleetList->remove_defense_fleet(DefenseFleet->get_fleet_id());
	}

	DefensePlan->type(QUERY_DELETE);
	STORE_CENTER->store(*DefensePlan);

	DefensePlanList->remove_defense_plan(PlanID);

	ITEM("RESULT_MESSAGE",
			GETTEXT("You have successfully deleted the special defense plan."));

//	system_log( "end page handler %s", get_name() );

	return output("war/defense_plan_special_delete_result.html");
}

