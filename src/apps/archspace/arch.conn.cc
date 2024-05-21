#include "common.h"
#include "util.h"
#include "archspace.h"
#include <cstdlib>
#include "define.h"
#include "banner.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libintl.h>
#include "player.h"
#include "game.h"
#include "council.h"
#include "admin.h"

TZone gArchspaceConnectionZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CArchspaceConnection),
	0,
	0,
	NULL,
	"Zone CArchspaceConnection"
};

CArchspaceConnection::CArchspaceConnection(
				CApplication *aApplication):
	CCryptConnection()
{
	mPortalID = -1;
	mGameID = -1;
}

CArchspaceConnection::~CArchspaceConnection()
{
	close();
}

bool decrypt_success(const char *aString, int aLength)
{
	const static char 
		Ascii[] = "1234567890=&%;,.+-_ "
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";

	if( aString == NULL ) return false;
	for(int i=0; i<aLength; i++)
		if (!strchr(Ascii, aString[i]))
			return false;
	return true;
}

CString&
CArchspaceConnection::decrypt_id_string(const char *aString)
{
	static CString 
		Buffer;

	int 
		Length = (strlen(aString)-16)/2;

	Buffer.clear();
	decrypt(CRYPT_CURRENT_KEY, aString, Buffer);

	if (Buffer.length() != 0)
	{
		if (!decrypt_success((char*)Buffer, Length)) 
		{
			Buffer.clear();
			decrypt(CRYPT_OLD_KEY, aString, Buffer);
			if (!decrypt_success((char*)Buffer, Length))
			{
				Buffer.clear();
				decrypt(CRYPT_FUTURE_KEY, aString, Buffer);
				if (!decrypt_success((char*)Buffer, Length))
				{
					Buffer.clear();
				}
			} 
		}
	}

	return Buffer;	
}

void
CArchspaceConnection::message_page(const char *aMessage)
{
	CQueryList
		Conversion;
	Conversion.set_value("MESSAGE", aMessage);

	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(mCookies.get_value("COUNTRY"));
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(mCookies.get_value("COUNTRY"));

	if (TopBanner == NULL || BANNER->is_top_banner_available() == false)
	{
		Conversion.set_value("ADLINE", " ");
	}
	else
	{
		Conversion.set_value("ADLINE", TopBanner);
	}

	if (BottomBanner == NULL || BANNER->is_bottom_banner_available() == false)
	{
		Conversion.set_value("ADLINE_BOTTOM", " ");
	}
	else
	{
		Conversion.set_value("ADLINE_BOTTOM", BottomBanner);
	}

	CString
		Output;
	CPage::get_html_station()->get_html(Output, "message.html", &Conversion);
	set_content(Output);
	send_terminate();
}

void
CArchspaceConnection::portal_login_message_page(const char *aMessage)
{
	CQueryList
		Conversion;
	Conversion.set_value("MESSAGE", aMessage);

	CString
		Output;
	CPage::get_html_station()->get_html(Output, "login_again.html", &Conversion);
	set_content(Output);
	send_terminate();
}

void
CArchspaceConnection::page(const char *aMessage)
{
	CString
		Output = aMessage;

	set_content(Output);
	send_terminate();
}

bool
CArchspaceConnection::make_page()
{
	static time_t t = time(0);
	static int count = 0;

	if( t <= time(0)-60 ){
		system_log( "page handler call %d during %d seconds", count, time(0)-t );
		count = 0;
		t = time(0);
	}

	count++;

	// ip ban
	if (ADMIN_TOOL->get_banned_ip_list()->has(inet_addr(get_connection())))
	{
		SLOG("Ban IP %s", get_connection());
		message_page("Your IP is not allowed");
		return true;
	}

	mCookies.set(mCookie, ';');
//	SLOG("Total Cookie:%d", mCookies.length());
/*	for(int i=0; i<mCookies.length(); i++)
	{
		CQuery *
			Query = (CQuery *)mCookies.get(i);
		SLOG("Cookie %d:[%s]/[%s]", i, Query->get_name(), Query->get_value());
	}
*/

    	char * PHPSessionID = mCookies.get_value("phpbb2mysql_sid");
	if (!PHPSessionID)
	{
		portal_login_message_page(GETTEXT("First of all, you must log in."));
		return true;
	}


 	CMySQL
		*MySQL=NULL;
    	while (!(MySQL = MYSQL_POOL->get_connection())) 
         	;           
 
 	CString
		Query;	
    	Query.clear();           
	Query.format( "SELECT "
			"session_user_id, "
			"session_time "
			"FROM asbb_sessions where session_id='%s' LIMIT 1", PHPSessionID);
	MySQL->query( (char*)Query );
	MySQL->use_result();
	Query.clear();

 	if ( MySQL->has_result() && MySQL->fetch_row())
	{
        	MYSQL_ROW aRow = MySQL->row();
        	char *aPortalID = aRow[0];
        	char *aSessionTime = aRow[1];

        	if (!aPortalID || atoi(aPortalID) <= 0)
	    	{
		   portal_login_message_page(GETTEXT("First of all, you must log in."));
			if (MySQL->has_result())
				MySQL->free_result();

		   MYSQL_POOL->release_connection(MySQL);
		   return true;
	    	}
	    	mPortalID = atoi(aPortalID);

        	if (time(0) - atoi(aSessionTime) > 60)
        	{
			if (MySQL->has_result())
				MySQL->free_result();

           		Query.format( "UPDATE asbb_sessions SET "
			             "session_time = '%d' "
			             "WHERE session_id='%s'", time(0), PHPSessionID);
           		MySQL->query( (char*)Query );
	       		MySQL->use_result();
			if (MySQL->has_result())
				MySQL->free_result();
			Query.clear();

           		Query.format( "UPDATE asbb_users SET "
			             "user_session_time = '%d' "
			             "WHERE user_id='%d'", time(0), mPortalID);
           		MySQL->query( (char*)Query );
	       		MySQL->use_result();
			if (MySQL->has_result())
				MySQL->free_result();
	       		Query.clear();       
        	}
        	else
        	{
			if (MySQL->has_result())
				MySQL->free_result();
        	}
	}
	else
	{
	   portal_login_message_page(GETTEXT("First of all, you must log in."));
			if (MySQL->has_result())
				MySQL->free_result();

           MYSQL_POOL->release_connection(MySQL);
           return true;
    	}
			if (MySQL->has_result())
				MySQL->free_result();
    
    MYSQL_POOL->release_connection(MySQL);	

	CPlayer* Player = PLAYER_TABLE->get_by_portal_id(mPortalID);

    if(Player) mGameID = Player->get_game_id();

	CString *
		Page = mPageStation->get_page(*this);

	if (Page && Page->length() > 0)
	{
		CString
			TString;

		set_content(*Page);
		send_terminate();
	}
	else
	{
		SLOG("ERROR : The requested page was not found!");
		message_page(GETTEXT("The requested page was not found. Please ask Archspace Development Team."));
		return true;
	}

	return true;
}
