#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../race.h"
#include "../../preference.h"

// temporary removed till special defense plan are working

/*bool
CPageDefensePlan::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("STRING_GENERIC_DEFENSE_PLAN", GETTEXT("Generic Defense Plan"));

	CDefensePlanList *
		DefensePlanList = aPlayer->get_defense_plan_list();

	ITEM("SPECIAL_DEFENSE_PLAN_INFO",
			DefensePlanList->special_defense_plan_info_html());

	ITEM("STRING_RACE", GETTEXT("Race"));

	ITEM("STRING_HUMAN",
			RACE_TABLE->get_name_by_id(CRace::RACE_HUMAN));
	ITEM("STRING_TARGOID",
			RACE_TABLE->get_name_by_id(CRace::RACE_TARGOID));
	ITEM("STRING_BUCKANEER",
			RACE_TABLE->get_name_by_id(CRace::RACE_BUCKANEER));
	ITEM("STRING_TECANOID",
			RACE_TABLE->get_name_by_id(CRace::RACE_TECANOID));
	ITEM("STRING_EVINTOS",
			RACE_TABLE->get_name_by_id(CRace::RACE_EVINTOS));
	ITEM("STRING_AGERUS",
			RACE_TABLE->get_name_by_id(CRace::RACE_AGERUS));
	ITEM("STRING_BOSALIAN",
			RACE_TABLE->get_name_by_id(CRace::RACE_BOSALIAN));
	ITEM("STRING_XELOSS",
			RACE_TABLE->get_name_by_id(CRace::RACE_XELOSS));
	ITEM("STRING_XERUSIAN",
			RACE_TABLE->get_name_by_id(CRace::RACE_XERUSIAN));
	ITEM("STRING_XESPERADOS",
			RACE_TABLE->get_name_by_id(CRace::RACE_XESPERADOS));

	ITEM("STRING_POWER_RATIO", GETTEXT("Power Ratio"));

	ITEM("STRING_MIN_", GETTEXT("Min."));
	ITEM("STRING_MAX_", GETTEXT("Max."));

	ITEM("STRING_ATTACK_TYPE", GETTEXT("Attack Type"));

	ITEM("STRING_SIEGE", GETTEXT("Siege"));
	ITEM("STRING_BLOCKADE", GETTEXT("Blockade"));

	ITEM("MAKE_NEW_PLAN_MESSAGE",
			GETTEXT("Make new special defense plan."));

//	system_log( "end page handler %s", get_name() );

	return output( "war/defense_plan.html" );
}*/

bool
CPageDefensePlan::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

    	// if player doesn't have a preference object yet, make it
    	if (aPlayer->get_preference() == NULL)
        	aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));

	CPreference *
		aPreference = aPlayer->get_preference();

	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	int
		ValidFleets = 0;
	for (int i = 0; i < FleetList->length(); i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		CMission &
			Mission = Fleet->get_mission();
		if ((Fleet->get_status() == CFleet::FLEET_STAND_BY) || (Mission.get_mission() == CMission::MISSION_RETURNING))
		{
			ValidFleets++;
		}
	}
	if (ValidFleets <= 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You do not have any fleets on stand-by.<BR>"
						"Please try again later."));
		return output("war/defense_plan_generic_new_error.html");
	}

	ITEM("PREFERENCE_PREFIX", GETTEXT("Deploy fleets using:"));

	CString HTMLSelectOptions;
	HTMLSelectOptions.clear();
	for (int i=0; i <= CPreference::PR_JAVASCRIPT; i++)
    {
      if (aPreference->has(i))
          HTMLSelectOptions.format("<OPTION VALUE=%d SELECTED>%s</OPTION>\n",i,aPreference->EPreferencetoString((CPreference::EPreference)i));
      else
          HTMLSelectOptions.format("<OPTION VALUE=%d>%s</OPTION>\n",i,aPreference->EPreferencetoString((CPreference::EPreference)i));
    }

	ITEM("SELECT_OPTIONS", (char*)HTMLSelectOptions);

	ITEM("SELECT_FLEET_MESSAGE",
			GETTEXT("Select fleets to deploy(up to 20 fleets)."));

	ITEM("FLEET_LIST", FleetList->deployment_fleet_list_html(aPlayer));

//	system_log( "end page handler %s", get_name() );

	return output( "war/defense_plan_generic_new.html" );
}

