#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../pages.h"
#include "../relay.h"

void 
CPageCommon::message_page(CConnection& aConnection, const char *aMessage)
{
	CQueryList
		Conversion = get_conversion(aConnection);
	
	Conversion.set_value("MESSAGE", aMessage);

	output("message.html", Conversion);
}

CQueryList& 
CPageCommon::get_conversion(CConnection &aConnection)
{
	static CQueryList
		Conversions;

	CWebConnection
		*Connection = (CWebConnection*)&aConnection;

	CRelay
		*Relay = (CRelay*)Connection->application();

	const char 
		*AdInfoP = Connection->cookies().get_value("AdInfoP");
	const char 
		*AdInfoS = Connection->cookies().get_value("AdInfoS");

	int 
		P = (!AdInfoP) ? -1:atoi(AdInfoP);
	int 
		S = (!AdInfoS) ? -1:atoi(AdInfoS);

	const char
		*Banner = Relay->get_banner_center()->get_banner(P, S);

	Conversions.set_value("ADLINE", 
			(Banner) ? Banner:"Banner code didn't load");
	Conversions.set_value("COOKIE", Connection->get_cookie());
	
	return Conversions;
}
