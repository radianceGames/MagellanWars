#include "../pages.h"
#include "../archspace.h"

bool
CPageLogout::handle()
{
//	system_log("start page handler %s", get_name());

	CONNECTION->id_string().remove_all();
	CONNECTION->cookies().replace_value("ID_STRING", "");
	CONNECTION->cookies().replace_value("ARCHSPACE_HOST", "");
	CONNECTION->cookies().replace_value("ARCHSPACE_PORT", "");

//	system_log("end page handler %s", get_name());

	message_page("Logout");
	
	return true;
}
