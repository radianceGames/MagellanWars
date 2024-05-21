#include "../../pages.h"

bool
CPageTechTree::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

//	system_log("end page handler %s", get_name());
	static CString Aquired;
	Aquired.clear();
	CKnownTechList* techlist = aPlayer->get_tech_list();
	for(int i = 0; i < techlist->length(); i++)
	{
		CKnownTech *
			Tech = (CKnownTech *)techlist->get(i);
		Aquired += (CString)"Aquired[Aquired.length] = new Array('" + Tech->get_tech()->get_name() + (CString)"');\n";
	}
	ITEM("AQUIRED_JS", Aquired.get_data());
	return output("domestic/tech_tree.html");
}
