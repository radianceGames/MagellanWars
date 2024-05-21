#include "../pages.h"
#include "../archspace.h"
#include <cstdlib>
#include "../banner.h"
#include "../game.h"
#include <ctype.h>

bool
CPageCommon::get_conversion()
{
	if (!mConnection) return false;

	char *
		Country = CONNECTION->cookies().get_value("COUNTRY");
	char *
		TopBanner = BANNER->get_top_banner_by_country_menu(Country);
	char *
		BottomBanner = BANNER->get_bottom_banner_by_country_menu(Country);

	ITEM("ADLINE", TopBanner);
	ITEM("ADLINE_BOTTOM", BottomBanner);

	ITEM("CHAR_SET", GAME->get_charset());
	ITEM("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	return true;
}

void
CPageCommon::message_page(const char *aMessage)
{
	ITEM("MESSAGE", aMessage)

	output("message.html");
}

bool
CPageCommon::handle(CConnection &aConnection)
{
	mConnection = &aConnection;
	if (strstr(allow_method(), aConnection.get_method()) == NULL)
	{
		message_page("Forbidden access");
		return false;
	}

	mConversion.remove_all();
	get_conversion();
	if (!check_referrer())
	    return false;

	mQuery.remove_all();
	mQuery.set(QUERY_STRING, '&');

	return true;
}

bool CPageCommon::check_referrer()
{
#ifndef NOREFFERER
	CString
		CurrentReferer,
		LegalReferer;
	CurrentReferer = mConnection->get_referer();
#ifdef SSL
	LegalReferer.format("https://%s", mConnection->get_host_name());
#endif
#ifndef SSL
	LegalReferer.format("http://%s", mConnection->get_host_name());
#endif

//	SLOG("CURRENT:%s",(char *)CurrentReferer);
//	SLOG("LEGAL:%s",(char *)LegalReferer);

	if ((char *)CurrentReferer == NULL || CurrentReferer.length() < LegalReferer.length())
	{
        SLOG("ALERT: [D.N.E.] Possible Scripting -- Ref:%s UA:%s IP:%s M:%s URI:%s QUERYS:%s", 
            (char *)CurrentReferer, (char *)mConnection->get_agent(), (char *)mConnection->get_connection(), 
            (char *)mConnection->get_method(), (char *)mConnection->get_uri(), QUERY_STRING);
		//message_page("Bugeriffic? Try the bug forums!");
		mOutput = " ";
		mConnection = NULL;
		return false;
	}

	for (int i=0; i < LegalReferer.length(); i++)
	     if (toupper(LegalReferer[i]) != toupper(CurrentReferer[i]))
	     {
	        SLOG("ALERT: [Invalid] Possible Scripting -- Ref:%s UA:%s IP:%s M:%s URI:%s QUERYS:%s", 
                     (char *)CurrentReferer, (char *)mConnection->get_agent(), (char *)mConnection->get_connection(), 
                     (char *)mConnection->get_method(), (char *)mConnection->get_uri(), QUERY_STRING);
    		//message_page("Critical Error. System Shutdown in -%d seconds?", (number(500000)+500000));
    		mOutput = " ";
    		mConnection = NULL;
            return false;
	     }
#endif
    return true;
}
