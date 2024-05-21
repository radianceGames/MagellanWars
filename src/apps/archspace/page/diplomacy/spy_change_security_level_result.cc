#include <libintl.h>
#include "../../pages.h"

bool
CPageSpyChangeSecurityLevelResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("SECURITY_LEVEL", SecurityLevelString);
	int
		SecurityLevel = as_atoi(SecurityLevelString);
	if (SecurityLevel < 1 || SecurityLevel > 5)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You selected wrong security level. Please try again."));

		return output("diplomacy/spy_error.html");
	}

	int
		OldSecurityLevel = aPlayer->get_security_level();

	if (SecurityLevel == OldSecurityLevel)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("Your security level hasn't changed."));

		return output("diplomacy/spy_change_security_level_result.html");
	}

	aPlayer->set_security_level(SecurityLevel);

	static CString
		Result;
	Result.clear();

	Result.format(GETTEXT("Your security level has changed from \"%1$s\" to \"%2$s\"."),
							CSpy::get_security_level_description(OldSecurityLevel),
							CSpy::get_security_level_description(SecurityLevel));

	ITEM("RESULT_MESSAGE", (char *)Result);

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/spy_change_security_level_result.html" );
}
