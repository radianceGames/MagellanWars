#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"
#include "../player.h"
#include "../preference.h"

bool
CPagePreference::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

    // if player doesn't have a preference object yet, make it
    if (aPlayer->get_preference() == NULL)
        aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));
    CPreference *aPreference = aPlayer->get_preference();
    ITEM("PREFERENCE_TITLE", GETTEXT("Preferences"));
    
    QUERY("CHANGE_PREF", ChangePrefString);
    if (ChangePrefString)
    {
        int ChangePref = as_atoi(ChangePrefString);
       ITEM("RESULT_MESSAGE", aPreference->handle(ChangePref));
       return output("preference_result.html");
    }
  
    ITEM("CURRENT", (char*)aPreference->HTMLTable());
    ITEM("PREFERENCE_PREFIX", GETTEXT("Prefer:"));
    ITEM("SELECT_OPTIONS", (char*)aPreference->HTMLSelectOptions());
//	system_log("end page handler %s", get_name());

	return output("preference.html");
}
