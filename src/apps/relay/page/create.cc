#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../pages.h"
#include "../relay.h"

const char*
CPageCreate::race_html(CConnection &aConnection)
{	
	static CString
		Buffer;

	Buffer.clear();

	CWebConnection
		*Connection = (CWebConnection*)&aConnection;
	CRelay
		*Relay = (CRelay*)Connection->application();
	CRaceList
		*RaceList = Relay->get_race_list();

	Buffer = "<TABLE border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n";
	Buffer += "<TH class=\"maintext\">Name</TH>"
			  "<TH class=\"maintext\">Description</TH>";
	for(int i=0; i<RaceList->length(); i++)
	{
		CRace
			*RaceData = (CRace*)(*RaceList)[i];

		Buffer += "<TR bgcolor=\"#0A0013\">\n";
	
		if (!i)
			Buffer.format("<TD class=\"maintext\">"
					"<INPUT TYPE=\"radio\""
					" NAME=\"RACE\" VALUE=\"%d\" CHECKED>%s"
					"</TD>\n", 
					RaceData->get_id(), RaceData->get_name());
		else
			Buffer.format("<TD class=\"maintext\">"
					"<INPUT TYPE=\"radio\""
					" NAME=\"RACE\" VALUE=\"%d\">%s"
					"</TD>\n", 
					RaceData->get_id(), RaceData->get_name());

		Buffer.format("<TD class=\"maintext\">%s</TD>\n", 
				RaceData->get_description());
		Buffer += "</TR>\n";
	}
	Buffer += "</TABLE>\n";

	return (char*)Buffer;
}

bool
CPageCreate::handle(CConnection &aConnection)
{
	CQueryList
		Conversion = get_conversion(aConnection);

	Conversion.set_value("RACE_SELECT", race_html(aConnection));

	if (!mHTMLStation->get_html(*this, "create.html", &Conversion))
		return false;
	
	return true;
}
