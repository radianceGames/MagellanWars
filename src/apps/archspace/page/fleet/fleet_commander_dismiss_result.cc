#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageFleetCommanderDismissResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CCommandSet
		AdmiralSet;
	AdmiralSet.clear();

	QUERY("ADMIRAL_SET", AdmiralSetString);
	if (AdmiralSetString == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't select any fleet commanders."));
		return output("fleet/fleet_commander_error.html");
	}

	AdmiralSet.set_string(AdmiralSetString);

	if (AdmiralSet.is_empty())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't select any fleet commanders."));
		return output("fleet/fleet_commander_error.html");
	}

	CAdmiralList *
		Pool = aPlayer->get_admiral_pool();
	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	for (int i=0 ; i<Pool->length() ; i++)
	{
		CAdmiral *
			Admiral = (CAdmiral *)Pool->get(i);
		if (AdmiralSet.has(i) == true)
		{
			if (Admiral->get_fleet_number() != 0)
			{
				CFleet *
					Fleet = FleetList->get_by_id(Admiral->get_fleet_number());
				if (Fleet == NULL)
				{
					Admiral->set_fleet_number(0);

					Admiral->type(QUERY_UPDATE);
					STORE_CENTER->store(*Admiral);

					SLOG((char *)format("WARNING : The admiral %s was assigned to a fleet, but the fleet doesn't exist. So the admiral has been moved to the admiral pool.", Admiral->get_nick()));
				}
				else
				{
					ITEM("ERROR_MESSAGE",
							(char *)format(GETTEXT("The admiral %1$s is assigned to the fleet %2$s, so you can't dismiss the admiral right now."),
											Admiral->get_nick(), Fleet->get_nick()));
					return output("fleet/fleet_commander_error.html");
				}
			}
		}
	}
	static CString
		AdmiralName;
	AdmiralName.clear();

	bool
		Comma = false;
	int
		InitAdmiralNumber = Pool->length();

	for (int i=InitAdmiralNumber-1 ; i>=0 ; i--)
	{
		if (!AdmiralSet.has(i)) continue;

		CAdmiral *
			Admiral = (CAdmiral *)Pool->get(i);
		if (Comma) AdmiralName += ",";
		AdmiralName.format(" %s", Admiral->get_nick());

		Admiral->type(QUERY_DELETE);
		*STORE_CENTER << *Admiral;
		Pool->remove_admiral(Admiral->get_id());

		Comma = true;
	}

	if (Comma == false)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The admiral(s) that you have chosen doesn't/don't exist."));
	}
	else
	{
		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("You have lost %1$s forever."),
								(char *)AdmiralName));
	}

	ITEM("JS_FLEET_COMMANDER_NUMBER",
			aPlayer->get_admiral_pool()->length());

	ITEM("ATTACHED_FLEET_COMMANDER_JAVASCRIPT",
		aPlayer->get_admiral_list()->attached_fleet_commander_info_javascript(aPlayer));

	ITEM("POOLED_FLEET_COMMANDER_JAVASCRIPT",
		aPlayer->get_admiral_pool()->pool_fleet_commander_info_javascript(aPlayer));

	ITEM("STRING_ATTACHED_FLEET_COMMANDER_S_",
			GETTEXT("Attached Fleet Commander(s)"));

	ITEM("ATTACHED_FLEET_COMMANDER_INFO",
			aPlayer->get_admiral_list()->attached_fleet_commander_info_html());

	ITEM("STRING_FLEET_COMMANDER_S__IN_THE_POOL",
			GETTEXT("Fleet Commander(s) in the Pool"));

	ITEM("POOL_FLEET_COMMANDER_INFO",
			aPlayer->get_admiral_pool()->pool_fleet_commander_info_html());

	ITEM("PLAYER_COMMANDER_VIEW_OPTIONS",
			aPlayer->get_preference()->commander_view_html());

//	system_log("end page handler %s", get_name());

	return output("fleet/fleet_commander.html");
}

