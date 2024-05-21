#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageFleetCommanderDismissConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	static CString
		Message;
	Message.clear();

	CCommandSet
		AdmiralSet;
	AdmiralSet.clear();

	CAdmiralList *
		Pool = aPlayer->get_admiral_pool();
	for (int i=0 ; i<Pool->length() ; i++)
	{
		char
			QueryVar[11] = "ADMIRAL000";

		if (i < 10)
		{
			QueryVar[7] = i + '0';
			QueryVar[8] = '\0';
		}
		else if (i < 100)
		{
			QueryVar[7] = i/10 + '0';
			QueryVar[8] = i%10 + '0';
			QueryVar[9] = '\0';
		}
		else if (i < 1000)
		{
			QueryVar[7] = i/100 + '0';
			QueryVar[8] = (i%100)/10 + '0';
			QueryVar[9] = i%10 + '0';
			QueryVar[10] = '\0';
		}
		else
		{
			message_page("Strange admiral number");
			return true;
		}

		QUERY(QueryVar, AdmiralIndexString);
		if (AdmiralIndexString)
		{
			if (!strcasecmp(AdmiralIndexString, "ON"))
			{
				AdmiralSet += i;
			}
		}
	}

	if (AdmiralSet.is_empty())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't select any fleet commanders."));
		return output("fleet/fleet_commander_error.html");
	}

	Message = GETTEXT("If you dismiss your commander(s), you will lose the commander(s)"
						" permanently.");
	Message += "<BR><BR>\n";

	bool
		Comma = false;
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

			if (Comma) Message += ", ";
			Message += Admiral->get_nick();
			Comma = true;
		}
	}
	Message += "<BR><BR>\n";
	
	if(Comma) {
		Message += GETTEXT("Are you sure you want to dismiss them?");
	} else {
		Message += GETTEXT("Are you sure you want to dismiss the commander?");
	}

	ITEM("CONFIRM_MESSAGE", (char *)Message);

	ITEM("ADMIRAL_SET", AdmiralSet.get_string());

//	system_log("end page handler %s", get_name());

	return output("fleet/fleet_commander_dismiss_confirm.html");
}

