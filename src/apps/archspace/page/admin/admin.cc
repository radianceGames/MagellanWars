#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../admin.h"
#include "../../portal.h"

bool
CPageAdmin::handler(CPlayer *aPlayer)
{
    //system_log("start page handler %s", get_name());

    // Default player access check
    CPortalUser *thisAdmin = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
    CString error_msg;
    error_msg.format("%s '%s' %s.", 
                     GETTEXT("Access Level "), 
                     user_level_to_string(thisAdmin->get_user_level()), 
                     GETTEXT("does not grant access"));
    if (!ADMIN_TOOL->has_access(aPlayer))
    {
       ITEM("ERROR_MESSAGE", (char *)error_msg);
       return output("admin/admin_error.html"); 
    }  


    // Check for Admin Portal User changes
	CAdminList *AdminList = ADMIN_TOOL->get_admin_list();
    EDevLevel NewDevLevel = MIN_DEV_LEVEL;
    EUserLevel NewUserLevel = MIN_USER_LEVEL;
    CString log_message;
	for (int i=AdminList->length()-1 ; i>=0 ; i--)
	{
		CPortalUser *
			PortalUser = (CPortalUser *)AdminList->get(i);
		CString
			QueryString;
		QueryString.format("USER_LEVEL%d", i);
		QUERY((char *)QueryString, AdminTypeString);

        // faster but less secure as_atoi instead of string_to_user_level
        // string to user always does a valid user level
        // while as_atoi currently just does WHATEVER integer value is inputed
        // via the form
        if (AdminTypeString != NULL)
		{
            NewUserLevel = EUserLevel(as_atoi(AdminTypeString));
            if (NewUserLevel != PortalUser->get_user_level())
            {
                     if ((int)PortalUser->get_user_level() > (int)thisAdmin->get_user_level())
                     {
                         ITEM("ERROR_MESSAGE", GETTEXT("You cannot change an admins "
                                                       "user level that is higher than "
                                                       "yours!"));
                         return output("admin/admin_error.html"); 
                     }
                     if (PortalUser->get_portal_id() == thisAdmin->get_portal_id())
                     {
                      if ((int)thisAdmin->get_user_level() < (int)NewUserLevel)
                      {
                         ITEM("ERROR_MESSAGE", GETTEXT("You cannot set your own "
                                                       "user level higher than it "
                                                       "already is!"));
                         return output("admin/admin_error.html"); 
                      }                     
                     }
                     log_message.clear();
                     log_message.format("Changed user_level for %s(#%d) from %s to %s.",
                                        PortalUser->get_name(),
                                        PortalUser->get_portal_id(), 
                                        user_level_to_string(PortalUser->get_user_level()), 
                                        user_level_to_string(NewUserLevel));
                     ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)log_message);


			         if (NewUserLevel >= MIN_ADMIN_LEVEL)
                         PortalUser->set_user_level(NewUserLevel);
                     else
                         AdminList->remove_admin(PortalUser);

            }
		}                      

        QueryString.clear();                 
  		QueryString.format("DEV_LEVEL%d", i);
		QUERY((char *)QueryString, DevTypeString);
		
        if (DevTypeString != NULL)
		{
            NewDevLevel = EDevLevel(as_atoi(DevTypeString));
            if (NewDevLevel != PortalUser->get_dev_level())
            {
              if (PortalUser->get_user_level() != UL_DEVELOPER)
              {
                ITEM("ERROR_MESSAGE", GETTEXT("You cannot give non-developers"
                                      " developer privledges!"));
                return output("admin/admin_error.html"); 
              }
              if (thisAdmin->get_user_level() != UL_DEVELOPER && 
                  thisAdmin->get_user_level() != UL_ADMIN)
              {
                ITEM("ERROR_MESSAGE", GETTEXT("You cannot give out "
                                      " developer privledges!"));
                return output("admin/admin_error.html"); 
              }

               log_message.clear();
               log_message.format("Changed dev_level for %s(#%d) from %s to %s.",
                                        PortalUser->get_name(),
                                        PortalUser->get_portal_id(), 
                                        dev_level_to_string(PortalUser->get_dev_level()), 
                                        dev_level_to_string(NewDevLevel));
               ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)log_message);


               PortalUser->set_dev_level(NewDevLevel);
            }
		}                      
        PortalUser->type(QUERY_UPDATE);
   	   STORE_CENTER->store(*PortalUser);           

	}

    // Check for Banned IP removal
	CIPList *BannedIPList = ADMIN_TOOL->get_banned_ip_list();
 
	for (int i=BannedIPList->length()-1 ; i>=0 ; i--)
	{
		CIP *
			BannedIP = (CIP *)BannedIPList->get(i);

		CString
			QueryString;
		QueryString.format("BANNED_IP%d", i);

		QUERY((char *)QueryString, BannedIPString);
		if (BannedIPString != NULL)
		{
			if (!strcasecmp(BannedIPString, "ON"))
			{
                log_message.clear();
                log_message.format("Removed ban on %s",BannedIP->get_name());
                BannedIPList->remove_ip(BannedIP->get_address());
				ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)log_message);
			}
		}
	}
	ADMIN_TOOL->save_banned_ip_list();

    // Admin Portal Users List
	CString
		AdminListHtml;
	AdminListHtml = ADMIN_TOOL->admin_list_html();
	if (AdminListHtml.length() == 0)
	{
		ITEM("ADMIN_LIST", " ");
	}
	else
	{
		ITEM("ADMIN_LIST", (char *)AdminListHtml);
	}

    // Banned Players (by IP) List
	CString
		BannedIPListHTML;
	BannedIPListHTML = ADMIN_TOOL->banned_ip_list_html();
	if (BannedIPListHTML.length() == 0)
	{
		ITEM("BANNED_IP_LIST", " ");
	}
	else
	{
		ITEM("BANNED_IP_LIST", (char *)BannedIPListHTML);
	}
    // Static Admin.as Page Variables
    ITEM("STRING_ADMIN_TITLE", GETTEXT("Administration"));
	ITEM("STRING_ADMIN_SETTING", GETTEXT("Admin Setting"));
	ITEM("STRING_BANNED_IP", GETTEXT("Banned IP"));
	ITEM("STRING_REBOOT", GETTEXT("Reboot"));
	ITEM("STRING_RESET", GETTEXT("Reset"));
	ITEM("STRING_THE_CONQUEROR_S_NAME", GETTEXT("The Conqueror's Name"));
	ITEM("STRING_THE_NAME_OF_THE_CONQUEROR_S_COUNCIL",
         GETTEXT("The Name Of The Conqueror's Council"));
	ITEM("STRING_BROADCAST", GETTEXT("Broadcast"));
	ITEM("STRING_MESSAGE", GETTEXT("Message"));
	ITEM("STRING_MAIL", GETTEXT("Mail"));

    // Generic Admin.as Page Variables
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));

    ITEM("ADMIN_LINK_DES_STRING", GETTEXT("You can manage multiple general administration features"));
    ITEM("INSPECT_LINK_DES_STRING", GETTEXT("You can inspect and manage players, councils, and users from here"));    
    ITEM("BLACK_MARKET_LINK_DES_STRING", GETTEXT("You can manage the black market from here"));
    ITEM("CONFIG_LINK_DES_STRING", GETTEXT("You can manage the server configuration from here"));
    ITEM("RESTRICTION_LINK_DES_STRING", GETTEXT("Manage player and council restrictions here"));            
    //system_log("end page handler %s", get_name());

	return output("admin/admin.html");
}
