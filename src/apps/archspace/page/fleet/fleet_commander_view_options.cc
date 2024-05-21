#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageFleetCommanderViewOptions::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());
  	// if player doesn't have a preference object yet, make it
   	if (aPlayer->get_preference() == NULL)
       	aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));

	unsigned int CommanderView = 0x00000000;
	static CString
		Info;
	
	Info.clear();
	for (int index = (CPreference::CS_END - 1); index >= 0 ; index--)
	{
		CommanderView = CommanderView << 1;
		CString QueryVar;
		QueryVar.clear();
		QueryVar.format("STAT_OPTION%d", index);
		QUERY((char *)QueryVar, IndexString);
		if (IndexString)
		{
			if (!strcasecmp(IndexString, "ON"))
			{
				CommanderView = CommanderView | 0x00000001;
			}
		}
	}
	aPlayer->get_preference()->setCommanderView(CommanderView);

	ITEM("RESULT_MESSAGE", GETTEXT("You have successfully changed your commander viewing"));
//	system_log("end page handler %s", get_name());
	return output("fleet/fleet_commander_view_options.html");
}

