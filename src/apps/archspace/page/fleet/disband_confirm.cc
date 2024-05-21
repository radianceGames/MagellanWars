#include <libintl.h>
#include "../../pages.h"

bool
CPageDisbandConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	CCommandSet
		FleetSet;
	FleetSet.clear();

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CString
			QueryVar;
		QueryVar.clear();
		QueryVar.format("FLEET%d", i);

		QUERY((char *)QueryVar, FleetIndexString);

		if (FleetIndexString)
		{
			if (!strcasecmp(FleetIndexString, "ON"))
			{
			FleetSet += i;
			}
		}
	}

	if(FleetSet.is_empty())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
		return output("fleet/disband_error.html");
	}

	ITEM("FLEETS", FleetList->disband_confirm_fleet_list_html(aPlayer, &FleetSet));

	ITEM("DISBAND_FLEET",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET_SET VALUE=%s>",
							FleetSet.get_string()));

	Message = GETTEXT("If you disband selected fleet(s), you will lose all the exp.");
	Message += "<BR>\n";
	Message += GETTEXT("Are you sure you want to disband them?");
	Message += "<BR>\n";

	ITEM("CONFIRM_MESSAGE", (char *)Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/disband_confirm.html");
}

