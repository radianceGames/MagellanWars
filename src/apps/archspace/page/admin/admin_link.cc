#include "../../admin.h"
#include "../../pages.h"

//CPortalUserData CPageAdminLink::mPortal;

bool
CPageAdminLink::get_conversion()
{
	if (!CPageHeadTitle::get_conversion()) 
		return false;

//	ITEM("DOMESTIC_MAIN", GETTEXT("[Domestic Main]"));
//	ITEM("CONCENTRATION_MODE", GETTEXT("[Concentration Mode]"));
//	ITEM("WHITEBOOK", GETTEXT("[Whitebook]"));
//	ITEM("PROJECT", GETTEXT("[Project]"));
//	ITEM("RESEARCH", GETTEXT("[Research]"));
//	ITEM("PLANET_MANAGEMENT", GETTEXT("[Planet Management]"));

	return true;
}
